#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_adc/adc_continuous.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "hal/adc_types.h"
#include <stdlib.h>



typedef struct {
    adc_channel_t pin;
    adc_unit_t unit_id;
    adc_oneshot_unit_handle_t adc_handle;
    adc_cali_handle_t cali_handle;
    int raw;
    int voltage;
} analog_t;

typedef struct {
    bool UP;
    bool DOWN;
    bool LEFT;
    bool RIGHT;
    int x_percent;
    int y_percent;
    int x_raw_value;
    int y_raw_value;
    bool invert_x;
    bool invert_y;
    bool rotate;
} joystick_t;


void ADC_CONFIG(analog_t* analog);
void analog_read(analog_t* analog);
analog_t analog_create(adc_channel_t pin, adc_unit_t unit);
void joystick_update(joystick_t* joy, analog_t* joystickX, analog_t* joystickY, const int sensitivity, const int maximum, const int center);




#ifdef __cplusplus
}
#endif
