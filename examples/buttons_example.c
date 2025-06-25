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


#define MAX_BUTTONS 3 //make sure to define max_buttons before declaring the header file or else it will automatically be set to 16 or whatever i had it set to
#include "DIGITAL_INPUT.h"
#include "ANALOG_INPUT.h"

gpio_num_t = myButtons[] = {GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_15}; //create a container with desired pin inputs

void app_main() {
  //"sizeof(myButtons) / sizeof(myButtons[0])" is basically the number of elements, you could do this or manually count them, i was too lazy here
  //the first bool is wether or not the pin will be in INPUT or OUTPUT, the other is if you want pull-up or pull-down (only works with inputs)
  PIN_CONFIG(myButtons, sizeof(myButtons) / sizeof(myButtons[0]), true, false, true);

  while (1) {
    PIN_UPDATE(); //updates the pins
    for(size_t i = 0; i < sizeof(myButtons)/sizeof(myButtons[0]); i++) {
      if (get_button_state(i) == BUTTON_PRESSED) {
        printf("button %d was pressed \n", myButtons[i]);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
  //looking at this again now, idk how i feel about this general set up, this may be improved on later in the future
  //maybe this is just a bad example, in your project you could just start writing conditions right off the bat instead of making another for-loop, like "PIN_UPDATE(); if(get_button_state(1) == BUTTON_PRESSED)..."
}
