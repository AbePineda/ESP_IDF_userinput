#include "ANALOG_INPUT.h"
#include <stdlib.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "esp_adc/adc_continuous.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"

#include "driver/ledc.h"

#include "hal/adc_types.h"
#include "DIGITAL_INPUT.h"


static bool adc_initialized = false;
static adc_oneshot_unit_handle_t shared_adc_handle = NULL;
static adc_cali_handle_t shared_cali_handle = NULL;



void ADC_CONFIG(analog_t* analog) {
    if (!adc_initialized) {
        adc_oneshot_unit_init_cfg_t unit_cfg = {
            .unit_id = analog->unit_id,
            .ulp_mode = ADC_ULP_MODE_DISABLE
        };
        ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_cfg, &shared_adc_handle));

        adc_cali_curve_fitting_config_t cali_cfg = {
            .unit_id = analog->unit_id,
            .atten = ADC_ATTEN_DB_12,
            .bitwidth = ADC_BITWIDTH_12
        };
        ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_cfg, &shared_cali_handle));

        adc_initialized = true;
    }

    analog->adc_handle = shared_adc_handle;
    analog->cali_handle = shared_cali_handle;

    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_12
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(analog->adc_handle, analog->pin, &chan_cfg));
}

void analog_read(analog_t* analog) {
    ESP_ERROR_CHECK(adc_oneshot_read(analog->adc_handle, analog->pin, &analog->raw));
    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(analog->cali_handle, analog->raw, &analog->voltage));
}

analog_t analog_create(adc_channel_t pin, adc_unit_t unit) {
    analog_t a = { .pin = pin, .unit_id = unit };
    ADC_CONFIG(&a);
    return a;
}

void PWM_CONFIG(uint32_t freq, ledc_channel_t channel, gpio_num_t pin) {
    ledc_timer_config_t timer_cfg = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT, // 0–1023
        .timer_num = LEDC_TIMER_0,
        .freq_hz = freq,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_cfg));

    ledc_channel_config_t channel_cfg = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = channel,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = pin,
        .duty = 0,
        .hpoint = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&channel_cfg));
}

void pwm_set_duty(ledc_channel_t channel, uint32_t duty) {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel);
}

void joystick_update(joystick_t* joy, analog_t* joystickX, analog_t* joystickY, const int sensitivity, const int maximum, const int center) {
    analog_read(joystickX);
    analog_read(joystickY);

    int x = joy->rotate ? joystickY->voltage : joystickX->voltage;
    int y = joy->rotate ? joystickX->voltage : joystickY->voltage;

    joy->x_raw_value = x;
    joy->y_raw_value = y;

    int increment_to_max = (maximum - (center + sensitivity)) / 100;
    int increment_to_min = (center - sensitivity) / 100;


    if(x >= (center+sensitivity)) {
        joy->UP = true;
        if(joy->invert_x) {
            joy->UP = false;
            joy->DOWN = true;
        }
    } else if(x <= (center-sensitivity)) {
        joy->DOWN = true;
        if(joy->invert_x) {
            joy->UP = true;
            joy->DOWN = false;
        }
    } else {
        joy->DOWN = false;
        joy->UP = false;
    }

    if(y >= (center+sensitivity)) {
        joy->RIGHT = true;
        if(joy->invert_y) {
            joy->RIGHT = false;
            joy->LEFT = true;
        }
    } else if(y <= (center-sensitivity)) {
        joy->LEFT = true;
        if(joy->invert_y) {
            joy->RIGHT = true;
            joy->LEFT = false;
        }
    } else {
        joy->RIGHT = false;
        joy->LEFT = false;
    }

   
    if(joy->UP) {
        joy->x_percent = abs((x - (center+sensitivity)) / increment_to_max);
    } else if(joy->DOWN) {
        joy->x_percent = abs(((center-sensitivity) - x) / increment_to_min);
    } else {
        joy->x_percent = 0;
    }

    if(joy->RIGHT) {
        joy->y_percent = abs((y - (center+sensitivity)) / increment_to_max);
    } else if(joy->LEFT) {
        joy->y_percent = abs(((center-sensitivity) - y) / increment_to_min);
    } else {
        joy->y_percent = 0;
    }




}


