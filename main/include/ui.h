/* 
 * Zde se řeší všechno vykreslování a ui logika (menu).
 */

#pragma once

#include <esp_log.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_netif.h"

#include "lcd.h"
#include "xbm.h"
#include "input.h"
#include "wifi.h"

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
    char** names;
    bool* breakAfterClick;
    void **args;
    void (**callbacks)(void*);
} ui_menu_struct_t;

//metoda pro volání ui_update_tasku
extern QueueHandle_t uiQueue;

void ui_setup(void);

//blokující callback metoda pro spuštění menu (patří k ní ui_menu_struct_t)
void ui_menu_invoke(void* ui_menu_struct);

//blokující metoda co zobrazí popup s informacemi o systému
void ui_invoke_sys_info_popup(void* args);
