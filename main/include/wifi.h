#pragma once

#include <inttypes.h>
#include <string.h>

#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/event_groups.h"

#include "esp_netif.h"
#include "nvs_flash.h"
#include "esp_wifi.h"

esp_err_t app_wifi_init(void);

esp_err_t app_wifi_connect(char* wifi_ssid, char* wifi_password);

esp_err_t app_wifi_disconnect(void);

esp_err_t app_wifi_deinit(void);

