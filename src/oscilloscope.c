#include <stdio.h>
#include "avr_common/uart.h"
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <assert.h>
#include "./adc/adc.h"
#include "./voltage_switch/voltage_switch.h"
#include "./square_wave/square_wave.h"

volatile uint8_t sqwv_period_mode = 0;
volatile uint16_t sample_rate_ms = 17;
volatile uint16_t ms_since_last_conversion = 0;

volatile uint8_t sample_rate_changed = 0;

ISR(ADC_vect) {
    print_analog_value_to_serial();
    disable_adc();
}

ISR(PCINT0_vect) {
    uint8_t pin_10_value = PINB & _BV(PB2);
    if (!pin_10_value) {
        sqwv_period_mode = (sqwv_period_mode + 1) & 0x03;
        set_square_wave_period_mode(sqwv_period_mode);
        return;
    }

    uint8_t pin_12_value = PINB & _BV(PB4);
    if (!pin_12_value) {
        toggle_voltage();
        return;
    }
}

ISR(TIMER1_COMPA_vect) {
    toggle_square_wave_voltage();
}

volatile uint8_t rx_buf[8];
volatile uint8_t rx_buf_index = 0;

ISR(USART_RX_vect) {
    uint8_t rx_byte = UDR0;
    if (rx_byte != '\n' && rx_byte != '\r') {
        rx_buf[rx_buf_index++] = rx_byte;
        return;
    }
    rx_buf[rx_buf_index] = '\0';
    rx_buf_index = 0;

    unsigned char command = rx_buf[0];
    const char* cmd_buf = (const char*) rx_buf + 1;

    if (command == 'a') {
        uint8_t adc_pin = strtoul(cmd_buf, NULL, 10);
        if (adc_pin >= 0 && adc_pin <= 5)
            set_adc_pin(adc_pin);
    }
    else if (rx_buf[0] == 'f') {
        uint16_t new_sample_rate_ms = strtoul(cmd_buf, NULL, 10);
        if (new_sample_rate_ms >= 1 && new_sample_rate_ms <= 1000) {
            sample_rate_ms = new_sample_rate_ms;
            sample_rate_changed = 1;
        }
    }
}

void setup_timer_for_adc_sampling(void) {
    TCCR0A |= _BV(WGM01); // set CTC mode
    TCCR0B |= _BV(CS01) | _BV(CS00); // set prescaler to 64
    OCR0A = 249; // tick every 1 ms
    TIMSK0 |= _BV(OCIE0A); // enable compare interrupt
}

ISR(TIMER0_COMPA_vect) {
    if (sample_rate_changed) {
        sample_rate_changed = 0;
        ms_since_last_conversion = 0;
        printf("f\n");
        return;
    }

    ms_since_last_conversion++;

    if (ms_since_last_conversion >= sample_rate_ms) {
        ms_since_last_conversion = 0;
        enable_adc();
        start_analog_digital_conversion();
    }
}

int main(void) {
    cli(); // disable interrupts
    printf_init();

    setup_analog_to_digital_conversion();
    set_adc_pin(0);

    setup_switch_interrupt();

    setup_square_wave_generator();
    set_square_wave_period_mode(sqwv_period_mode);

    setup_timer_for_adc_sampling();

    printf("\n");
    printf("start\n");

    set_sleep_mode(SLEEP_MODE_IDLE);
    sei(); // enable interrupts

    while (1) {
        sleep_mode();
    }
}
