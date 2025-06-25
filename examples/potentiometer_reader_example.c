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
  analog_t myPot = analog_create(ADC_CHANNEL_4, ADC_UNIT_1);

  while(1) {
    analog_read(&myPot);
    if(myPot.voltage >= 3000) {
      printf("exceeding 3 volts! \n");
    }
    vTaskDelay(pdsMS_TO_TICKS(100));
    //if you want the raw uncalibrated result of the analog, use "myPot.raw" if you ever need it
  }
}
