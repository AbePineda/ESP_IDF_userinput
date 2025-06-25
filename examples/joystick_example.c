#include <stdio.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "esp_adc/adc_continuous.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "hal/adc_types.h"

#include "driver/ledc.h"

#include "DIGITAL_INPUT.h"
#include "ANALOG_INPUT.h"

void app_main() {
  joystick_t myJoy;
  myJoy.invert_x = true;
  myJoy.invert_y = true;
  myJoy.rotate = true;

  analog_t joystick_x = analog_create(ADC_CHANNEL_3, ADC_UNIT_1); //this function basically just initializes the struct and its purely for aesthetic reasons
  analog_t joystick_y = analog_create(ADC_CHANNEL_4, ADC_UNIT_1); //be sure to look through your ESP's datasheet to confirm its channel macro and unit

  while(1) {
    //this funtion requires the object of the whole joystick, and its components of the joystick (x and y)
    //50 is the general sensativity you want
    //3100 is the maximum voltage it could read, I recommend starting off with 3300 because it translates to 3.3V which is the maximum output the ESP32 can give
    //use "myJoy.x_raw_value"/"myJoy.y_raw_value" to debug and get a better configuration details based off of your actually joystick you're working with
    //1600 is the center of the joystick, the overall dead zone of this is "center +/- sensativity"
    joystick_update(&myJoy, &joystick_x, &joystick_y, 50, 3100, 1600);

    printf("UP: %d, DOWN: %d, LEFT: %d, RIGHT: %d\n", myJoy.UP, myJoy.DOWN, myJoy.LEFT, myJoy.RIGHT);
    prinf("X percentage: %d, Y percentage: %d \n", myJoy.x_percent, myJoy.y_percent);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
