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
#include "wifi.h"

typedef enum {
  DEV_BOOT,
  DEV_IDLE,
} dev_mode_t;

dev_mode_t dev_mode = DEV_BOOT;

static const char* TAG = "main";



void cb_request_return_last(void *args) {
    printf("invoked return last action\n");
}



void cb_soft_reboot(void *args) {
    ESP_LOGI(TAG, "invoking soft reboot callback");
    esp_restart();
}




void device_task(void *args) {
  ESP_LOGI(TAG, "boot finished");
  dev_mode = DEV_IDLE;

  ui_msg_t ui_msg;
  nfc_event_t nfc_event;
  button_event_t button_event;

  while (true) {
    switch (dev_mode) {
      case DEV_IDLE:
        if(xQueueReceive(nfcEventQueue, &nfc_event, 50/portTICK_PERIOD_MS)) { // MAGIC_VALUE: task pusti kazdych 50 ms
          ui_msg.state = UI_MSG;
          strcpy(ui_msg.msg, "tag ready");
          xQueueSend(uiQueue, &ui_msg, portMAX_DELAY);

          vTaskDelay(2000/portTICK_PERIOD_MS);

          ui_msg.state = UI_IDLE;

          xQueueSend(uiQueue, &ui_msg, portMAX_DELAY);
        }

        if(xQueueReceive(buttonEventQueue, &button_event, 50/portTICK_PERIOD_MS)) {
            if(button_event.action == ACTION_BTN_PRESS && button_event.id == BTN_ENT) {
                
                ui_menu_struct_t menu_system = {
                    .length = 3,
                    .names = (const char *[]){"System info", "Wifi", "Reboot"},
                    .breakAfterClick = (const bool[]){true, true, true},
                    .callbacks = (void (*[])(void *)){ui_invoke_sys_info_popup, NULL, cb_soft_reboot},
                    .args = (void *[]){NULL, NULL, NULL, NULL},
                };

                ui_menu_struct_t menu = {
                    .length = 2,
                    .names = (const char *[]){"Vratit posledni akci", "System"},
                    .breakAfterClick = (const bool[]){true, false},
                    .callbacks = (void (*[])(void *)){cb_request_return_last, ui_menu_invoke},
                    .args = (void *[]){NULL, (void*)&menu_system},
                };
                
                ui_menu_invoke((void*)&menu);
                
                ui_msg.state = UI_IDLE;
                xQueueSend(uiQueue, &ui_msg, portMAX_DELAY);
            }
        }

        break;
      default:
        break;
    }

  }
}




void app_main(void) {
  printf("APP_MAIN\n");
  input_setup();
  lcd_setup();
  ui_setup();
  nfc_setup();

  ui_msg_t ui_msg = {
    .state = UI_MSG,
    .msg = "wifi testing"
  };

  xQueueSend(uiQueue, &ui_msg, portMAX_DELAY);


  // ESP_ERROR_CHECK(app_wifi_init());
  // ESP_ERROR_CHECK(app_wifi_connect(CONFIG_ESP_WIFI_SSID, CONFIG_ESP_WIFI_PASSWORD));  
  //
  // ESP_ERROR_CHECK(app_wifi_disconnect());
  ui_msg.state = UI_IDLE;
  xQueueSend(uiQueue, &ui_msg, portMAX_DELAY);

  xTaskCreate(device_task, "device_task", 4096, NULL, 2, NULL);
}



