#include <stdio.h>
#include "avr_common/uart.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/**
 * Sets up the analog to digital converter.
 */
void setup_analog_to_digital_conversion() {
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

void start_analog_digital_conversion() {
    ADCSRA |= _BV(ADSC);
}

ISR(ADC_vect) {
    uint16_t adc_value = ADC;
    printf("%u\n", adc_value);
}

/**
 * Enables a switch-controlled interrupt on pin 12.
 * When the switch is pressed, pin 13 will toggle.
 * This is useful for testing components.
 * The switch should be connected between pin 13 and ground.
 */
void setup_switch_interrupt() {
    DDRB &= ~_BV(PB4); // set pin 12 to input
    PORTB |= _BV(PB4); // enable pull-up resistor on pin 12
    PCMSK0 |= _BV(PCINT4); // enable pin change interrupt on pin 12
    PCICR |= _BV(PCIE0); // enable pin change interrupt 0

    DDRB |= _BV(PB5); // set pin 13 to output
    PORTB &= ~_BV(PB5); // set pin 13 to low
}

ISR(PCINT0_vect) {
    // if pin is high, toggle pin 12
    if (PINB & _BV(PB4)) {
        PORTB ^= _BV(PB5);
    }
}

int main() {
    printf_init();
    setup_analog_to_digital_conversion();
    setup_switch_interrupt();

    sei(); // enable interrupts

    while (1) {
        start_analog_digital_conversion();
        _delay_ms(55);
    }
}
