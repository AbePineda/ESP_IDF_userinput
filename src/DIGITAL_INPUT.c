#include "DIGITAL_INPUT.h"
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"

//----------------------------------------------SOLO BUTTON STUFF

static button_t buttons[MAX_BUTTONS];
static size_t num_buttons = 0;

void PIN_CONFIG(gpio_num_t* pins, size_t count, bool input, bool input_pullup, bool input_pulldown) {
    if(count > MAX_BUTTONS) {
        ESP_LOGE("PIN_CONFIG", "too many buttons, max is %d", MAX_BUTTONS);
        return;
    }

    num_buttons = count;

    gpio_mode_t mode = input ? GPIO_MODE_INPUT : GPIO_MODE_OUTPUT;
    gpio_pull_mode_t pull;
    bool use_pull = false;

    if(input) {
        if(input_pullup) {
            pull = GPIO_PULLUP_ONLY;
            use_pull = true;
        } else if(input_pulldown) {
            pull = GPIO_PULLDOWN_ONLY;
            use_pull = true;
        }
    }

    for (size_t i = 0; i < count; i++) {
        buttons[i].pin = pins[i];
        gpio_set_direction(pins[i], mode);
        if (input && use_pull) {
            gpio_set_pull_mode(pins[i], pull);
        }

        buttons[i].internal.current_level = gpio_get_level(pins[i]);
        buttons[i].internal.last_level = buttons[i].internal.current_level;
        buttons[i].internal.last_change_time_us = esp_timer_get_time();
        buttons[i].internal.state = BUTTON_IDLE;
        buttons[i].internal.last_state = BUTTON_IDLE;
    }

}

void PIN_UPDATE() {
    for(size_t i = 0; i < num_buttons; i++) {
        bool level = gpio_get_level(buttons[i].pin);
        button_internal* b = &buttons[i].internal;

        b->last_state = b->state;

        if(level != b->last_level) {
            b->last_change_time_us = esp_timer_get_time();
            b->current_level = level;
            b->state = level ? BUTTON_PRESSED : BUTTON_RELEASED;
        } else {
            b->state = level ? BUTTON_HELD : BUTTON_IDLE;
        }

        b->last_level = b->current_level;
    }
}

button_state get_button_state(size_t index) {
    if(index >= num_buttons) return BUTTON_IDLE;
    return buttons[index].internal.state;
}

button_state last_button_state(size_t index) {
    if(index >= num_buttons) return BUTTON_IDLE;
    return buttons[index].internal.last_state;
}

bool combo_flag(const size_t* internal_pins, const button_state* target_state, size_t count) {
    for (size_t i = 0; i < count; i++) {
        /*
        if (buttons[internal_pins[i]].internal.state != target_state[i]) {
            return false;
        }
            */

        if(get_button_state(i) != target_state[i]) {
            return false;
        }
    }
    return true;
}

//----------------------------------------------MATRIX BUTTON STUFF

void MATRIX_CONFIG(button_matrix_t* matrix, gpio_num_t* matrix_inputs, size_t col_count, gpio_num_t* matrix_outputs, size_t row_count) {
    matrix->rows = matrix_outputs;
    matrix->cols = matrix_inputs;
    matrix->row_count = row_count;
    matrix->col_count = col_count;
    
    matrix->states = malloc(matrix->row_count * sizeof(button_internal*));
    for(size_t i = 0; i < matrix->row_count; i++) {
        matrix->states[i] = calloc(matrix->col_count, sizeof(button_internal));
    }

    for(size_t i = 0; i < col_count; i++) {
        gpio_set_direction(matrix_inputs[i], GPIO_MODE_INPUT);
        gpio_set_pull_mode(matrix_inputs[i], GPIO_PULLDOWN_ONLY);
    }

    for(size_t i = 0; i < row_count; i++) {
        gpio_set_direction(matrix_outputs[i], GPIO_MODE_OUTPUT);
    }
}

void MATRIX_UPDATE(button_matrix_t* matrix) {
    for(size_t row = 0; row < matrix->row_count; row++) {
        for(size_t r = 0; r < matrix->row_count; r++) {
            gpio_set_level(matrix->rows[r], 0); //set all outputs/rows as low
        }

        gpio_set_level(matrix->rows[row], 1); //set one row as high and wait a sec
        vTaskDelay(pdMS_TO_TICKS(1));

        for(size_t col = 0; col < matrix->col_count; col++) {
            bool level = gpio_get_level(matrix->cols[col]);
            button_internal* btn = &matrix->states[row][col];
            
            btn->last_state = btn->state;

            if(level != btn->last_level) {
                btn->last_change_time_us = esp_timer_get_time();
                btn->current_level = level;
                btn->state = level ? BUTTON_PRESSED : BUTTON_RELEASED;
            } else {
                btn->state = level ? BUTTON_HELD : BUTTON_IDLE;
            }
            btn->last_level = btn->current_level;
        }
    }
}

button_state matrix_get_state(button_matrix_t* matrix, size_t row, size_t col) {
    return matrix->states[row][col].state;
}

