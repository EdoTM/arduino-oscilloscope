#include "avr_common/uart.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include "./adc/adc.h"
#include "./voltage_switch/voltage_switch.h"

int main(void) {
    printf_init();
    setup_analog_to_digital_conversion();
    setup_switch_interrupt();

    sei(); // enable interrupts

    while (1) {
        start_analog_digital_conversion();
        _delay_ms(17);
    }
}
