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

void cb_connect_wifi(void *args) {
//     int index = (int)(intptr_t)args;
//     wifi_config_t cfg = {0};
//
// strlcpy((char *)cfg.sta.ssid,
//         wifi_networks[index].ssid,
//         sizeof(cfg.sta.ssid));
//
// strlcpy((char *)cfg.sta.password,
//         wifi_networks[index].psk,
//         sizeof(cfg.sta.password));
//
// esp_err_t err = esp_wifi_disconnect();
//
// if (err != ESP_OK &&
//     err != ESP_ERR_WIFI_NOT_STARTED &&
//     err != ESP_ERR_WIFI_NOT_INIT)
// {
//     ESP_ERROR_CHECK(err);
// }
//
// ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &cfg));
//
// wifi_mode_t mode;
// ESP_ERROR_CHECK(esp_wifi_get_mode(&mode));
//
// if (mode == WIFI_MODE_NULL) {
//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
// }
//
// ESP_ERROR_CHECK(esp_wifi_start()); 
// ESP_ERROR_CHECK(esp_wifi_connect());
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
          ui_msg.state = UI_REQUEST;
          strcpy(ui_msg.msg, "tag ready");
          xQueueSend(uiQueue, &ui_msg, portMAX_DELAY);

          // vTaskDelay(2000/portTICK_PERIOD_MS);
          //
          // ui_msg.state = UI_IDLE;
          //
          // xQueueSend(uiQueue, &ui_msg, portMAX_DELAY);
        }

        if(xQueueReceive(buttonEventQueue, &button_event, 50/portTICK_PERIOD_MS)) {
            if(button_event.action == ACTION_BTN_PRESS && button_event.id == BTN_ENT) {


                ui_menu_struct_t menu_wifi = {
                    .length = wifi_network_count,
                    .names = calloc(wifi_network_count, sizeof(char *)),
                    .breakAfterClick = calloc(wifi_network_count, sizeof(bool)),
                    .args = calloc(wifi_network_count, sizeof(void *)),
                    .callbacks = calloc(wifi_network_count, sizeof(void (*)(void *))),
                };
                for(int i = 0; i < wifi_network_count; i++) {
                    menu_wifi.names[i] = wifi_networks[i].ssid;
                    menu_wifi.breakAfterClick[i] = false;
                    menu_wifi.callbacks[i] = (void *)cb_connect_wifi;
                    menu_wifi.args[i] = (void *)(intptr_t)i;
                }   

                ui_menu_struct_t menu_system = {
                    .length = 3,
                    .names = (char *[]){"System info", "Connect wifi", "Reboot"},
                    .breakAfterClick = (bool[]){true, true, true},
                    .callbacks = (void (*[])(void *)){ui_invoke_sys_info_popup, ui_menu_invoke, cb_soft_reboot},
                    .args = (void *[]){NULL, (void*)&menu_wifi, NULL},
                };

                ui_menu_struct_t menu = {
                    .length = 2,
                    .names = (char *[]){"Vratit posledni akci", "System"},
                    .breakAfterClick = (bool[]){true, false},
                    .callbacks = (void (*[])(void *)){cb_request_return_last, ui_menu_invoke},
                    .args = (void *[]){NULL, (void*)&menu_system},
                };
                
                ui_menu_invoke((void*)&menu);

                free(menu_wifi.names); //TODO: zbavit se tady to dost zavazi
                free(menu_wifi.breakAfterClick);
                free(menu_wifi.args);
                free(menu_wifi.callbacks);

                free(args);
                
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
  

  ui_msg_t ui_msg = {
    .state = UI_MSG,
    .msg = "booting ..."
  };

  xQueueSend(uiQueue, &ui_msg, portMAX_DELAY);

  nfc_setup();
  ESP_ERROR_CHECK(app_wifi_init());
  
  vTaskDelay(1000/portTICK_PERIOD_MS);

  
  ui_msg.state = UI_IDLE;
  xQueueSend(uiQueue, &ui_msg, portMAX_DELAY);

  xTaskCreate(device_task, "device_task", 4096, NULL, 2, NULL);
}



