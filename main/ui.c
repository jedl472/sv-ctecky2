#include "ui.h"

static const char *TAG = "ui";

QueueHandle_t uiQueue;


void ui_update_task(void *args) {
  ui_msg_t ui_msg;

  while(1) {
    if(xQueueReceive(uiQueue, &ui_msg, portMAX_DELAY)) {
      u8g2_ClearBuffer(&lcd);

      switch(ui_msg.state){
        case UI_IDLE:
          u8g2_SetFont(&lcd, u8g2_font_5x8_tf);
          // u8g2_DrawStr(&lcd, 15,30,"tady nekde sv logo");
          u8g2_DrawXBM(&lcd, 0, 0, 128, 64, xbm_strela_vlna_logo);
          break;

        case UI_MSG:
          u8g2_SetFont(&lcd, u8g2_font_5x8_tf);
          u8g2_DrawXBM(&lcd, 0, -4, 128, 64, xbm_strela_vlna_logo);

          u8g2_DrawStr(&lcd, 0,60, ui_msg.msg);
          break;

        case UI_REQUEST:
          u8g2_SetFont(&lcd, u8g2_font_6x10_tf);
          u8g2_DrawFrame(&lcd, -1, 0, 130, 12);
          u8g2_DrawStr(&lcd, 10, 10, "Nastavte parametry");

          u8g2_SetFont(&lcd, u8g2_font_5x8_tf);
          u8g2_DrawStr(&lcd, 0, 21, "Nazev tymu: ");
          u8g2_DrawStr(&lcd, 64, 21, "<nazev tymu>");
          u8g2_DrawStr(&lcd, 0, 30, "Stav uctu: ");
          u8g2_DrawStr(&lcd, 64, 30, "<stav uctu>");
          u8g2_DrawStr(&lcd, 0, 37, "-------------------------");
          u8g2_DrawStr(&lcd, 5, 44, "Typ ulohy: ");
          u8g2_DrawStr(&lcd, 64, 44, "<typ ulohy>");
          u8g2_DrawStr(&lcd, 5, 54, "Typ akce:  ");

          u8g2_SetDrawColor(&lcd, 0);
          u8g2_DrawBox(&lcd, 64, 47, 64, 10);
          u8g2_SetDrawColor(&lcd, 1);

          u8g2_DrawStr(&lcd, 64, 54, "<akce>");


          break;

        case UI_SYS_INFO_POPUP:
            // u8g2.drawXBM(64, 25, 80, 40, xbm_strela_vlna_logo_small);
            char ip_str[16]; //format ip data into buffers
            char gw_str[16];

            snprintf(ip_str,
                     sizeof(ip_str),
                     IPSTR,
                     IP2STR(&ip_info.ip));
            snprintf(gw_str,
                     sizeof(gw_str),
                     IPSTR,
                     IP2STR(&ip_info.gw));
            
            u8g2_SetFont(&lcd, u8g2_font_profont10_tf );

            u8g2_SetDrawColor(&lcd, 0);
            u8g2_DrawBox(&lcd, 0, 0, 128, 64);
            u8g2_SetDrawColor(&lcd, 1);

            u8g2_DrawStr(&lcd, 0, 10, "verze: ");
            u8g2_DrawStr(&lcd, 35, 10, CONFIG_SV_SOFTWARE_VERSION);


            u8g2_DrawStr(&lcd, 100, 10, "ID:");
            u8g2_DrawStr(&lcd, 117, 10, CONFIG_SV_DEVICE_ID);

            u8g2_DrawStr(&lcd, 0, 25, "IP:");
            u8g2_DrawStr(&lcd, 0, 35, "GW:");
            u8g2_DrawStr(&lcd, 0, 50, "server:");
    
            u8g2_DrawStr(&lcd, 20, 25, ip_str);
            u8g2_DrawStr(&lcd, 20, 35, gw_str);
            u8g2_DrawStr(&lcd, 0, 60, "<server>");

            break;
      }


      u8g2_SendBuffer(&lcd);
    }
  }
}

void ui_setup(void) {
  uiQueue = xQueueCreate(10, sizeof(ui_msg_t));

  xTaskCreate(ui_update_task, "ui_update_task", 4096, NULL, 1, NULL);


  ESP_LOGI(TAG, "ui setup finished");
}

void _ui_menu_draw(ui_menu_struct_t* ui_menu_struct, uint8_t cursorPos) {
    u8g2_ClearBuffer(&lcd); 
    u8g2_SetFont(&lcd, u8g2_font_5x8_tf);

    for(int i = 0; i < ui_menu_struct->length; i++) {
        u8g2_DrawStr(&lcd, 10, (10*i)+8, ui_menu_struct->names[i]);
    }
    
    u8g2_DrawStr(&lcd, 0, (10*cursorPos) + 8, ">");
    u8g2_SendBuffer(&lcd);
}

void ui_menu_invoke(void* ui_menu_struct) {
    ui_menu_struct_t *menu = ui_menu_struct;

    button_event_t button_event;
    size_t cursorPos = 0;    
    bool menuActive = true;

    _ui_menu_draw(menu, cursorPos); 
    

    while(menuActive) {
        if(xQueueReceive(buttonEventQueue, &button_event, 50/portTICK_PERIOD_MS)) {
            if(button_event.action == ACTION_BTN_PRESS) {
                switch (button_event.id) {
                    case BTN_ESC:
                        menuActive = false;
                        break;
                    case BTN_DOWN:
                        if(cursorPos < (menu->length - 1)) cursorPos++;
                        break;
                    case BTN_UP:
                        if(cursorPos > 0) cursorPos--;
                        break;
                    case BTN_ENT:
                        menu->callbacks[cursorPos](menu->args[cursorPos]);
                        if(menu->breakAfterClick[cursorPos]) menuActive = false;
                        break;
                    default:
                        break;
                }
            }

            if(menuActive) _ui_menu_draw((ui_menu_struct_t*)ui_menu_struct, cursorPos);
            // printf("cursorPos: %d\n", cursorPos);
        }
    }
}


void ui_invoke_sys_info_popup(void *args) {
    ui_msg_t ui_msg;
    button_event_t button_event;
    
    ui_msg.state = UI_SYS_INFO_POPUP;
    xQueueSend(uiQueue, &ui_msg, portMAX_DELAY);

    while(true) {
        if(xQueueReceive(buttonEventQueue, &button_event, 50/portTICK_PERIOD_MS)) {
            if(button_event.action == ACTION_BTN_PRESS && button_event.id == BTN_ESC) {
                break;
            }
        }
    }
}
