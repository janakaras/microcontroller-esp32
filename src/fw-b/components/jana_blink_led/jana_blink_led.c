#include "jana_blink_led.h"

#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include <unistd.h>

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (9) // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_FREQUENCY          (5000) // Frequency in Hertz. Set frequency at 5 kHz

static void example_ledc_init(void)
{
  // Prepare and then apply the LEDC PWM timer configuration
  ledc_timer_config_t ledc_timer = {
      .speed_mode       = LEDC_MODE,
      .timer_num        = LEDC_TIMER,
      .duty_resolution  = LEDC_DUTY_RES,
      .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
      .clk_cfg          = LEDC_AUTO_CLK
  };
  ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

  // Prepare and then apply the LEDC PWM channel configuration
  ledc_channel_config_t ledc_channel = {
      .speed_mode     = LEDC_MODE,
      .channel        = LEDC_CHANNEL,
      .timer_sel      = LEDC_TIMER,
      .intr_type      = LEDC_INTR_DISABLE,
      .gpio_num       = LEDC_OUTPUT_IO,
      .duty           = 0, // Set duty to 0%
      .hpoint         = 0
  };
  ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

void jana_init_led(void){
  example_ledc_init();
  printf("LED initalized\n");
}

void jana_led_low(void)
{
  ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 8000));
  // Update duty to apply the new value
  ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}

void jana_led_bright(void)
{
  ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0));
  // Update duty to apply the new value
  ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}