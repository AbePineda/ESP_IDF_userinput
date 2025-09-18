#pragma once

#include "driver/gpio.h"
#include "driver/ledc.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAX_BUTTONS
#define MAX_BUTTONS 16
#endif

typedef enum {
    BUTTON_IDLE,
    BUTTON_PRESSED,
    BUTTON_HELD,
    BUTTON_RELEASED
} button_state;

typedef struct {
    bool current_level;
    bool last_level;
    uint16_t last_change_time_us;
    button_state last_state;
    button_state state;
} button_internal;

typedef struct {
    gpio_num_t pin;
    button_internal internal;
} button_t;

typedef struct {
    gpio_num_t* rows;
    gpio_num_t* cols;
    size_t row_count;
    size_t col_count;
    button_internal** states;
} button_matrix_t;


void PIN_CONFIG(gpio_num_t* pins, size_t count, bool input, bool input_pullup, bool input_pulldown);
void PIN_UPDATE(void);

button_state get_button_state(size_t index);
button_state last_button_state(size_t index);
bool button_just_pressed(size_t index);
bool button_just_released(size_t index);

bool combo_flag(const size_t* internal_pins, const button_state* target_state, size_t count);

void PWM_CONFIG(uint32_t freq, ledc_channel_t channel, gpio_num_t pin);
void pwm_set_duty(ledc_channel_t channel, uint32_t duty);

void MATRIX_CONFIG(button_matrix_t* matrix, gpio_num_t* matrix_inputs, size_t col_count, gpio_num_t* matrix_outputs, size_t row_count);




#ifdef __cplusplus
}
#endif
