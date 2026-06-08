#pragma once

#include <esp_log.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"


#include "lcd.h"
#include "xbm.h"
#include "input.h"

typedef enum {
  UI_IDLE,
  UI_MSG,
  UI_REQUEST,
  UI_SYS_INFO_POPUP
} ui_state_t;

typedef struct {
  ui_state_t state;
  char msg[32];
} ui_msg_t;

typedef struct {
    size_t length;
    const char** names;
    const bool* breakAfterClick;
    void **args;
    void (**callbacks)(void*);
} ui_menu_struct_t;

extern QueueHandle_t uiQueue;

void ui_setup(void);

void ui_menu_invoke(void* ui_menu_struct);

void ui_invoke_sys_info_popup(void* args);
