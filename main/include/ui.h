#pragma once

#include <esp_log.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"


#include "lcd.h"
#include "xbm.h"

typedef enum {
  UI_IDLE,
  UI_MSG,
  UI_REQUEST,
  UI_MENU
} ui_state_t;

typedef struct {
  ui_state_t state;
  char msg[32];
} ui_msg_t;


extern QueueHandle_t uiQueue;

void ui_setup(void);
