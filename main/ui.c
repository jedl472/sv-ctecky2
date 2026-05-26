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
          u8g2_DrawStr(&lcd, 15,30,"tady nekde sv logo");
          break;

        case UI_MSG:
          u8g2_SetFont(&lcd, u8g2_font_5x8_tf);
          u8g2_DrawStr(&lcd, 15,25,"tady nekde sv logo");

          u8g2_DrawStr(&lcd, 0,60, ui_msg.msg);
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
