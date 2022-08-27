#include "jana_read_adc_new.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* void jana_read_adc_new(void) {
    printf("Hello World from the ADC reader\n");
}*/

#define ln(x) log(x)

// ADC Channels
#if CONFIG_IDF_TARGET_ESP32
#define ADC1_EXAMPLE_CHAN0 ADC1_CHANNEL_6
#define ADC2_EXAMPLE_CHAN0 ADC2_CHANNEL_0
static const char *TAG_CH[2][10] = {{"ADC1_CH6"}, {"ADC2_CH0"}};
#else
#define ADC1_EXAMPLE_CHAN3 ADC1_CHANNEL_3
static const char *TAG_CH[2][10] = {{"ADC1_CH3"}};
#endif

// ADC Attenuation
#define ADC_EXAMPLE_ATTEN ADC_ATTEN_DB_11

// ADC Calibration
#if CONFIG_IDF_TARGET_ESP32
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_VREF
#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32C3
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32S3
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP_FIT
#endif

static bool cali;
static int adc_raw[2][10];
static const char *TAG = "ADC SINGLE";

static esp_adc_cal_characteristics_t adc1_chars;

static void adc_calibration_init(void) {
  esp_err_t ret;
  bool cali_enable = false;

  ret = esp_adc_cal_check_efuse(ADC_EXAMPLE_CALI_SCHEME);
  if (ret == ESP_ERR_NOT_SUPPORTED) {
    ESP_LOGW(TAG,
             "Calibration scheme not supported, skip software calibration");
  } else if (ret == ESP_ERR_INVALID_VERSION) {
    ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
  } else if (ret == ESP_OK) {
    cali_enable = true;
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_EXAMPLE_ATTEN,
                             ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
  } else {
    ESP_LOGE(TAG, "Invalid arg");
  }

  cali = cali_enable;
}

void jana_init_adc(void){
  adc_calibration_init();
}

double jana_read_adc_new(void) {
  esp_err_t ret = ESP_OK;
  uint32_t voltage = 0;

  // ADC1 config
  //ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
  //ESP_ERROR_CHECK(
  //    adc1_config_channel_atten(ADC1_EXAMPLE_CHAN3, ADC_EXAMPLE_ATTEN));

  adc1_config_width(ADC_WIDTH_BIT_DEFAULT);
  adc1_config_channel_atten(ADC1_EXAMPLE_CHAN3, ADC_EXAMPLE_ATTEN);

  double res_ntc;
  double volt_raw;
  double temp;

    volt_raw = 0;
    res_ntc = 0;

    //printf("\n");

    // raw voltage
    adc_raw[0][0] = adc1_get_raw(ADC1_EXAMPLE_CHAN3);
    volt_raw = (double)esp_adc_cal_raw_to_voltage(adc_raw[0][0], &adc1_chars);
    //printf("voltage input: %f mV \n", volt_raw);

    // resistance of NTC
    res_ntc = (82000.0 * (3300.0 - volt_raw)) / volt_raw;
    //printf("resistance of NTC: %f ohm \n", res_ntc);

    vTaskDelay(pdMS_TO_TICKS(2000));

    // temperature
    temp = 1.0 / ((1.0 / 298.0) - ((ln(100000.0) - ln(res_ntc)) / 4075.0));

    //printf("temp: %f kelvin \n", temp);
    //printf("temp: %f celsius \n", temp - 273.15);

    printf("______________________________________ \n");

    return temp - 273.15; 
  
}