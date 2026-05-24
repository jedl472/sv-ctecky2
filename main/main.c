#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>
#include <u8g2.h>

#include "sdkconfig.h"

#include "ui.h"
#include "input.h"
#include "nfc.h"

void app_main(void) {
  printf("APP_MAIN\n");
  input_setup();
  lcd_setup();
  ui_setup();
  nfc_setup();

  ui_msg_t ui_msg = {
    .state = UI_MSG,
    .msg = "xQueueGeneric"
  };

  xQueueSend(uiQueue, &ui_msg, portMAX_DELAY);

  vTaskDelay(2000/portTICK_PERIOD_MS);

  ui_msg.state = UI_IDLE;
  xQueueSend(uiQueue, &ui_msg, portMAX_DELAY);
}
