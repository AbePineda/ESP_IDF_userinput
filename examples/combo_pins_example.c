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

gpio_num_t myPins[] = {GPIO_NUM_6, GPIO_NUM_7};


const size_t combo_pins[] = {GPIO_NUM_6, GPIO_NUM_7};
button_state target_state[] = {BUTTON_HELD, BUTTON_PRESSED}; //so when gpio 6 is held and gpio 7 is pressed the combo action is activated

void app_main() {
  PIN_CONFIG(myPins, 2, true, false, true);
  while(1) {
    PIN_UPDATE();
    //the combo pins, the target state, and how may elements of pins are there, i didn't do any form of precaution (for most functions) so just make sure they match up
    if(combo_flag(combo_pins, target_state, 2)) {
      printf("combo matched! \n");
    }
    vTaskDelay(pdsMS_TO_TICKS(100));
  }
}
