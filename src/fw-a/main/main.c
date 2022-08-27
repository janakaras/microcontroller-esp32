#include "foo.h"
#include "shared_foo.h"
#include "jana_read_adc_new.h"
#include "jana_spi_master_send.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdio.h>

void app_main() {
  jana_spi_master_init();
  jana_init_adc();

  while (1) {
    double temp = jana_read_adc_new(); 
    printf("Temperature in Celsius: %f\n", temp);
    float dutyValue = jana_spi_master_send(temp);
    if(dutyValue > 8190 || dutyValue < 0) {
      printf("Error, please check status of LED!\n");
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
  
}
