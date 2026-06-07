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

        case UI_MENU:
          //WIP
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
