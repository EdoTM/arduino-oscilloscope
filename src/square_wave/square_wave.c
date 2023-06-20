#include "../avr_common/uart.h"
#include <avr/io.h>
#include "square_wave.h"


/**
 * Enables a switch-controlled interrupt on pin 10.
 * When the switch is pressed, the square wave period will change.
 */
void setup_switch_interrupt_on_pin_10(void) {
    DDRB &= ~_BV(PB2); // set pin 10 as input
    PORTB |= _BV(PB2); // enable pull-up resistor on pin 10
    PCMSK0 |= _BV(PCINT2); // enable pin change interrupt on pin 10
    PCICR |= _BV(PCIE0); // enable pin change interrupt 0
}

/**
 * Enables two LEDs to indicate the current square wave period mode.
 * The LEDs are connected to pins 6 and 7.
 */
void setup_square_wave_mode_indicators(void) {
    DDRD |= _BV(PD7) | _BV(PD6); // set pin 7 and 6 to output
    PORTD &= ~_BV(PD7); // set pin 7 to low
    PORTD &= ~_BV(PD6); // set pin 6 to low
}

void setup_square_wave_generator(void) {
    DDRB |= _BV(PB0); // set pin 8 to output
    PORTB &= ~_BV(PB0); // set pin 8 to low

    TCCR1A = 0;
    TCCR1B |= _BV(WGM12); // set CTC mode
    TCCR1B |= _BV(CS12) | _BV(CS10); // set prescaler to 1024
    OCR1A = 0; // set compare match register to 0

    TIMSK1 |= _BV(OCIE1A); // enable timer compare interrupt

    setup_switch_interrupt_on_pin_10();
    setup_square_wave_mode_indicators();
}

void toggle_square_wave_voltage(void) {
    PORTB ^= _BV(PB0);
}

void update_square_wave_indicators(uint8_t mode) {
    uint8_t pin_6_active = mode & 0x01;
    uint8_t pin_7_active = mode & 0x02;

    if (pin_6_active)
        PORTD |= _BV(PD6);
    else
        PORTD &= ~_BV(PD6);

    if (pin_7_active)
        PORTD |= _BV(PD7);
    else
        PORTD &= ~_BV(PD7);
}

/**
 * 0 0 = 0.5 seconds
 * 0 1 = 1 second
 * 1 0 = 2 seconds
 * 1 1 = 3 seconds
 * @param mode 0, 1, 2 or 3.
 */
void set_square_wave_period_mode(uint8_t mode) {
    uint16_t compare_match_value;
    switch (mode) {
        case 1:  // 1 second
            compare_match_value = 15624;
            break;
        case 2:  // 2 seconds
            compare_match_value = 31248;
            break;
        case 3:  // 3 seconds
            compare_match_value = 46872;
            break;
        default:  // half second
            compare_match_value = 7812;
            break;
    }
    OCR1A = compare_match_value;
    update_square_wave_indicators(mode);
}