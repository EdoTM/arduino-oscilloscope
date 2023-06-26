#pragma once

struct Adc {
    uint16_t sample_rate_ms;
    uint16_t ms_since_last_conversion;
    uint8_t sample_rate_changed;
};

void setup_analog_to_digital_conversion(struct Adc* adc, uint16_t sample_rate_ms);

void handle_adc_timer_tick(struct Adc* adc);

void change_adc_sample_rate(struct Adc* adc, uint16_t new_sample_rate_ms);

void disable_adc(void);

void print_analog_value_to_serial(void);

void set_adc_pin(uint8_t pin);