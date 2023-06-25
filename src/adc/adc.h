#pragma once

void enable_adc(void);

void disable_adc(void);

void setup_analog_to_digital_conversion(void);

void start_analog_digital_conversion(void);

void print_analog_value_to_serial(void);

void set_adc_pin(uint8_t pin);

uint8_t get_adc_pin(void);