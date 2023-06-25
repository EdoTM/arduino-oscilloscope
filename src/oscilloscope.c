#include "avr_common/uart.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include "./adc/adc.h"
#include "./voltage_switch/voltage_switch.h"
#include "./square_wave/square_wave.h"

volatile uint8_t sqwv_period_mode = 0;

ISR(ADC_vect) {
    print_analog_value_to_serial();
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

ISR(USART_RX_vect) {
    uint8_t data = UDR0;
    uint8_t pin = data - '0';
    if (pin <= 5)
        set_adc_pin(pin);
}

int main(void) {
    printf_init();

    setup_analog_to_digital_conversion();
    set_adc_pin(0);

    setup_switch_interrupt();

    setup_square_wave_generator();
    set_square_wave_period_mode(sqwv_period_mode);

    sei(); // enable interrupts

    while (1) {
        start_analog_digital_conversion();
        _delay_ms(17);
    }
}
