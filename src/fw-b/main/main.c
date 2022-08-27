#include "foo.h"
#include "shared_foo.h"
#include "jana_blink_led.h"
#include "jana_spi_slave_receive.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

int calc_frequency(double temp){
  if (temp > 40) {
    return 200;
  }
  if (temp < 0) {
    return 2000;
  }
  int frequency = 2000 - (int)(temp * 18 * 2.5);
  return frequency;
}
static float temp;
static int frequency = 0;

void spi_handler() {
  jana_init_slave();
  while(1){
    temp = jana_get_data_slave(frequency);
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void led_handler() {
  jana_init_led();
  while(1){
    frequency = calc_frequency((double) temp);

    // blink LED
    jana_led_bright();
    vTaskDelay(pdMS_TO_TICKS(frequency));
    jana_led_low();
    vTaskDelay(pdMS_TO_TICKS(frequency));
  }

}

void app_main() {
  xTaskCreate(&spi_handler, "spi_handler", 3000, NULL, 5, NULL);
  xTaskCreate(&led_handler, "led_handler", 2000, NULL, 5, NULL);

}


