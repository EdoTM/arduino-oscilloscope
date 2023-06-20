#include "avr_common/uart.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include "./adc/adc.h"
#include "./voltage_switch/voltage_switch.h"


ISR(ADC_vect) {
    print_analog_value_to_serial();
}

ISR(PCINT0_vect) {
    uint8_t pin_12_value = PINB & _BV(PB4);
    if (pin_12_value) {
        toggle_voltage();
    }
}

int main(void) {
    printf_init();
    setup_analog_to_digital_conversion();
    setup_switch_interrupt();

    set_adc_pin(0);

    sei(); // enable interrupts

    while (1) {
        start_analog_digital_conversion();
        _delay_ms(17);
    }
}
