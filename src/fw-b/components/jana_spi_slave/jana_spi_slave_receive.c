#include "jana_spi_slave_receive.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

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
#include "driver/spi_slave.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include <unistd.h>

#define GPIO_HANDSHAKE 8
#define GPIO_MOSI 7
#define GPIO_MISO 2
#define GPIO_SCLK 6
#define GPIO_CS 10

#ifdef CONFIG_IDF_TARGET_ESP32
#define RCV_HOST    HSPI_HOST

#else
#define RCV_HOST    SPI2_HOST

#endif

void my_post_setup_cb(spi_slave_transaction_t *trans) {
  WRITE_PERI_REG(GPIO_OUT_W1TC_REG, (1 << GPIO_HANDSHAKE));
}

//Called after transaction is sent/received. We use this to set the handshake line low.
void my_post_trans_cb(spi_slave_transaction_t *trans) {
  WRITE_PERI_REG(GPIO_OUT_W1TS_REG, (1 << GPIO_HANDSHAKE));
}

static int n = 0;
static esp_err_t ret;
static WORD_ALIGNED_ATTR char sendbuf[129]="";
static WORD_ALIGNED_ATTR char recvbuf[129]="";
static spi_slave_transaction_t t;

void jana_init_slave(void) {

  //Configuration for the SPI bus
  spi_bus_config_t buscfg={
      .mosi_io_num=GPIO_MOSI,
      .miso_io_num=GPIO_MISO,
      .sclk_io_num=GPIO_SCLK,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
  };

  //Configuration for the SPI slave interface
  spi_slave_interface_config_t slvcfg={
      .mode=0,
      .spics_io_num=GPIO_CS,
      .queue_size=3,
      .flags=0,
      .post_setup_cb=my_post_setup_cb,
      .post_trans_cb=my_post_trans_cb
  };

  //Configuration for the handshake line
  gpio_config_t io_conf={
      .intr_type=GPIO_INTR_DISABLE,
      .mode=GPIO_MODE_OUTPUT,
      .pull_up_en=GPIO_PULLDOWN_ENABLE,
      .pin_bit_mask=(1<<GPIO_HANDSHAKE)
  };

  //Configure handshake line as output
  gpio_config(&io_conf);
  //Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
  gpio_set_pull_mode(GPIO_MOSI, GPIO_PULLUP_ONLY);
  gpio_set_pull_mode(GPIO_SCLK, GPIO_PULLUP_ONLY);
  gpio_set_pull_mode(GPIO_CS, GPIO_PULLUP_ONLY);

  //Initialize SPI slave interface
  ret=spi_slave_initialize(RCV_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);
  assert(ret==ESP_OK);

  memset(recvbuf, 0, 33);
  memset(&t, 0, sizeof(t));
}

float jana_get_data_slave(int frequency){
  //Clear receive buffer, set send buffer to something sane
  memset(recvbuf, 0xA5, 129);
  sprintf(sendbuf, "%d", frequency);

  //Set up a transaction of 128 bytes to send/receive
  t.length=128*8;
  t.tx_buffer=sendbuf;
  t.rx_buffer=recvbuf;
  /* This call enables the SPI slave interface to send/receive to the sendbuf and recvbuf. The transaction is
  initialized by the SPI master, however, so it will not actually happen until the master starts a hardware transaction
  by pulling CS low and pulsing the clock etc. In this specific example, we use the handshake line, pulled up by the
  .post_setup_cb callback that is called as soon as a transaction is ready, to let the master know it is free to transfer
  data.
  */
  ret=spi_slave_transmit(RCV_HOST, &t, portMAX_DELAY);

  //spi_slave_transmit does not return until the master has done a transmission, so by here we have sent our data and
  //received data from the master. Print it.
  //printf("Received--: %s\n", recvbuf);
  n++;
  //return atof(recvbuf);
  float temp = atof(recvbuf);
  printf("Temperature received: %f\n", temp);
  return temp;
}