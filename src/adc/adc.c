#include <stdio.h>
#include "../avr_common/uart.h"
#include <avr/io.h>
#include "adc.h"

void setup_timer_for_adc_sampling(void) {
    TCCR0A |= _BV(WGM01); // set CTC mode
    TCCR0B |= _BV(CS01) | _BV(CS00); // set prescaler to 64
    OCR0A = 249; // tick every 1 ms
    TIMSK0 |= _BV(OCIE0A); // enable compare interrupt
}

/**
 * Sets up the analog to digital converter and the timer for sampling (uses timer 0).
 */
void setup_analog_to_digital_conversion(struct Adc* adc, uint16_t sample_rate_ms) {
    // set ADC reference voltage to AVCC
    ADMUX |= _BV(REFS0);
    ADMUX &= ~_BV(REFS1);
    ADMUX &= ~_BV(ADLAR); // set ADC to 10-bit conversion
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // set ADC prescaler to 128

    ADCSRA |= _BV(ADIE); // enable interrupts

    setup_timer_for_adc_sampling();

    adc->sample_rate_ms = sample_rate_ms;
    adc->ms_since_last_conversion = 0;
    adc->sample_rate_changed = 0;
}

void enable_adc(void) {
    ADCSRA |= _BV(ADEN);
}

/**
 * Disables the ADC. This function should be called in the ISR(ADC_vect) function,
 * after the ADC conversion is complete.
 */
void disable_adc(void) {
    ADCSRA &= ~_BV(ADEN);
}

void start_analog_digital_conversion(void) {
    ADCSRA |= _BV(ADSC);
}

/**
 * Handles the ADC timer tick. This function should be called in the ISR(TIMER0_COMPA_vect) function.
 * @param adc - the ADC struct
 */
void handle_adc_timer_tick(struct Adc* adc) {
    if (adc->sample_rate_changed) {
        adc->sample_rate_changed = 0;
        adc->ms_since_last_conversion = 0;
        printf("f\n");
        return;
    }

    adc->ms_since_last_conversion++;

    if (adc->ms_since_last_conversion >= adc->sample_rate_ms) {
        adc->ms_since_last_conversion = 0;
        enable_adc();
        start_analog_digital_conversion();
    }
}

/**
 * Changes the ADC sample rate.
 * @param adc - the ADC struct
 * @param new_sample_rate_ms - the new sample rate in milliseconds
 */
void change_adc_sample_rate(struct Adc* adc, uint16_t new_sample_rate_ms) {
    adc->sample_rate_ms = new_sample_rate_ms;
    adc->sample_rate_changed = 1;
}

/**
 * Sets the ADC pin. The pin must be between 0 and 5 (Arduino UNO).
 * @param pin: the pin to set. 0 \<= pin \<= 5.
 */
void set_adc_pin(uint8_t pin) {
    ADMUX &= 0xF0; // clear last 4 bits of ADMUX
    ADMUX |= pin;
}

void print_analog_value_to_serial(void) {
    uint16_t adc_value = ADC;
    printf("%u\n", adc_value);
}