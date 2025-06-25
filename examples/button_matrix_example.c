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

//2x2 matrix
gpio_num_t myrow[] = {GPIO_NUM_6, GPIO_NUM_7};
gpio_num_t mycolumn[] = {GPIO_NUM_18, GPIO_NUM_8};

button_matrix_t mymatrix;

void app_main() {
  MATRIX_CONFIG(&mymatrix, myrow, 2, mycolumn, 2); // row/columns and # of elements on each
  while(1) {
    MATRIX_UPDATE(&mymatrix);
    if(matrix_get_state(&mymatrix, 0, 0) == BUTTON_PRESSED) {
      printf("Button at row 0, col 0 pressed \n"); 
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
