#include <stdio.h>
#include "../avr_common/uart.h"
#include <avr/io.h>

/**
 * Sets up the analog to digital converter.
 */
void setup_analog_to_digital_conversion(void) {
    // set ADC reference voltage to AVCC
    ADMUX |= _BV(REFS0);
    ADMUX &= ~_BV(REFS1);
    ADMUX &= ~_BV(ADLAR); // set ADC to 10-bit conversion
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // set ADC prescaler to 128

    ADCSRA |= _BV(ADIE); // enable interrupts

    ADCSRA |= _BV(ADEN); // enable ADC
}

/**
 * Sets the ADC pin. The pin must be between 0 and 5 (Arduino UNO).
 * @param pin: the pin to set. 0 \<= pin \<= 5.
 */
void set_adc_pin(uint8_t pin) {
    ADMUX &= 0xF0; // clear last 4 bits of ADMUX
    ADMUX |= pin;
}

uint8_t get_adc_pin(void) {
    return ADMUX & 0x0F;
}

void start_analog_digital_conversion(void) {
    ADCSRA |= _BV(ADSC);
}

void print_analog_value_to_serial(void) {
    uint16_t adc_value = ADC;
    printf("%u\n", adc_value);
}