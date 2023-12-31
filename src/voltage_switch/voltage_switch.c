#include "../avr_common/uart.h"
#include <avr/io.h>

/**
 * Enables a switch-controlled interrupt on pin 12.
 * When the switch is pressed, pin 13 will toggle.
 * This is useful for testing components.
 * The switch should be connected between pin 13 and ground.
 */
void setup_switch_interrupt(void) {
    DDRB &= ~_BV(PB4); // set pin 12 to input
    PORTB |= _BV(PB4); // enable pull-up resistor on pin 12
    PCMSK0 |= _BV(PCINT4); // enable pin change interrupt on pin 12
    PCICR |= _BV(PCIE0); // enable pin change interrupt 0

    DDRB |= _BV(PB5); // set pin 13 to output
    PORTB &= ~_BV(PB5); // set pin 13 to low

    DDRB |= _BV(PB3); // set pin 11 to output
    PORTB &= ~_BV(PB3); // set pin 11 to low
}

void toggle_voltage(void) {
    PORTB ^= _BV(PB5);
    PORTB ^= _BV(PB3);
}