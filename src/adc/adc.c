#include <stdio.h>
#include "../avr_common/uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/**
 * Sets up the analog to digital converter.
 */
void setup_analog_to_digital_conversion(void) {
    // set ADC reference voltage to AVCC
    ADMUX |= _BV(REFS0);
    ADMUX &= ~_BV(REFS1);
    ADMUX &= ~_BV(ADLAR); // set ADC to 10-bit conversion
    // set input channel to A0
    ADMUX &= ~(_BV(MUX0) | _BV(MUX1) | _BV(MUX2) | _BV(MUX3));
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // set ADC prescaler to 128

    ADCSRA |= _BV(ADIE); // enable interrupts

    ADCSRA |= _BV(ADEN); // enable ADC
}

void start_analog_digital_conversion(void) {
    ADCSRA |= _BV(ADSC);
}

ISR(ADC_vect) {
        uint16_t adc_value = ADC;
        printf("%u\n", adc_value);
}