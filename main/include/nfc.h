/* 
 * Tento soubor se stará o setup a obhospodařování knihovny pn532.
 */

#pragma once

#include <esp_log.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "pn532.h"
#include "pn532_driver_i2c.h"


#define PN532_MODE_I2C 1
#define PN532_MODE_HSU 0
#define PN532_MODE_SPI 0

#define NFC_SCL_PIN    GPIO_NUM_22
#define NFC_SDA_PIN    GPIO_NUM_21
#define NFC_RESET_PIN  GPIO_NUM_NC
#define NFC_IRQ_PIN    GPIO_NUM_NC


typedef struct {
  uint8_t uid[7];
  uint8_t uid_len;
} nfc_event_t;

// sem jdou data ze čtečny NTAG a také jí vyčítá device_task
extern QueueHandle_t nfcEventQueue;

void nfc_setup(void);
