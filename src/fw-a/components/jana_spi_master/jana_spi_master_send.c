#include "jana_spi_master_send.h"
#include <stdio.h>

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/igmp.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "soc/rtc_periph.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_spi_flash.h"

#include "driver/gpio.h"
#include "esp_intr_alloc.h"
#include <unistd.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define GPIO_HANDSHAKE 8
#define GPIO_MOSI 7
#define GPIO_MISO 2
#define GPIO_SCLK 6
#define GPIO_CS 10

#ifdef CONFIG_IDF_TARGET_ESP32
#define SENDER_HOST HSPI_HOST

#else
#define SENDER_HOST SPI2_HOST

#endif

//The semaphore indicating the slave is ready to receive stuff.
    static xQueueHandle rdySem;

/*
This ISR is called when the handshake line goes high.
*/
static void IRAM_ATTR gpio_handshake_isr_handler(void* arg)
{
  //Sometimes due to interference or ringing or something, we get two irqs after eachother. This is solved by
  //looking at the time between interrupts and refusing any interrupt too close to another one.
  //static uint32_t lasthandshaketime;
  //uint32_t currtime=esp_cpu_get_ccount();
  //uint32_t diff=currtime-lasthandshaketime;
  //if (diff<240000) return; //ignore everything <1ms after an earlier irq
  //lasthandshaketime=currtime;

  //Give the semaphore.
  BaseType_t mustYield=false;
  xSemaphoreGiveFromISR(rdySem, &mustYield);
  if (mustYield) portYIELD_FROM_ISR();
}

static esp_err_t ret;
static spi_device_handle_t handle;
static char sendbuf[128] = {0};
static char recvbuf[128] = {0};
static spi_transaction_t t;

void jana_spi_master_init(void){
  //Configuration for the SPI bus
  spi_bus_config_t buscfg={
      .mosi_io_num=GPIO_MOSI,
      .miso_io_num=GPIO_MISO,
      .sclk_io_num=GPIO_SCLK,
      .quadwp_io_num=-1,
      .quadhd_io_num=-1
  };

  //Configuration for the SPI device on the other side of the bus
  spi_device_interface_config_t devcfg={
      .command_bits=0,
      .address_bits=0,
      .dummy_bits=0,
      .clock_speed_hz=5000000,
      .duty_cycle_pos=128,        //50% duty cycle
      .mode=0,
      .spics_io_num=GPIO_CS,
      .cs_ena_posttrans=3,        //Keep the CS low 3 cycles after transaction, to stop slave from missing the last bit when CS has less propagation delay than CLK
      .queue_size=3
  };

  //GPIO config for the handshake line.
  gpio_config_t io_conf={
      .intr_type=GPIO_INTR_NEGEDGE,
      .mode=GPIO_MODE_INPUT,
      .pull_up_en=1,
      .pin_bit_mask=(1<<GPIO_HANDSHAKE)
  };



  memset(&t, 0, sizeof(t));

  //Create the semaphore.
  rdySem=xSemaphoreCreateBinary();

  //Set up handshake line interrupt.
  gpio_config(&io_conf);
  gpio_install_isr_service(0);
  gpio_set_intr_type(GPIO_HANDSHAKE, GPIO_INTR_POSEDGE);
  gpio_isr_handler_add(GPIO_HANDSHAKE, gpio_handshake_isr_handler, NULL);

  //Initialize the SPI bus and add the device we want to send stuff to.
  ret=spi_bus_initialize(SENDER_HOST, &buscfg, SPI_DMA_CH_AUTO);
  assert(ret==ESP_OK);
  ret=spi_bus_add_device(SENDER_HOST, &devcfg, &handle);
  assert(ret==ESP_OK);

  //Assume the slave is ready for the first transmission: if the slave started up before us, we will not detect
  //positive edge on the handshake line.
  xSemaphoreGive(rdySem);
}

float jana_spi_master_send(double temp){

  int res = snprintf(sendbuf, sizeof(sendbuf),
                     "%f", temp);
  if (res >= sizeof(sendbuf)) {
    printf("Data truncated\n");
  }
  t.length=sizeof(sendbuf)*8;
  t.tx_buffer=sendbuf;
  t.rx_buffer=recvbuf;
  //Wait for slave to be ready for next byte before sending
  xSemaphoreTake(rdySem, portMAX_DELAY); //Wait until slave is ready
  ret=spi_device_transmit(handle, &t);
  ESP_ERROR_CHECK(ret);
  printf("Received: Frequency of LED: %s ms\n", recvbuf);
  printf("Master sent data\n");
  float dutyValue = atof(recvbuf);
  return dutyValue;

}

