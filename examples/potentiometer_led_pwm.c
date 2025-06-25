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
  PWM_CONFIG(5000, LEDC_CHANNEL_0, GPIO_NUM_12); //5kHz PWM on Gpio 12

  while(1) {
    analog_read(&myPot);
    uint32_t duty = (myPot.voltage * 1023) / 3300;
    if (duty > 1023) duty = 1023;
    pwm_set_duty(LEDC_CHANNEL_0, duty);
    vTaskDelay(pdsMS_TO_TICKS(10)); //make sure the delay is small, create a seperate task tbh because theres a noticeable choppiness even at 100 ms
  }
}
