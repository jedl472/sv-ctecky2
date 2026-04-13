#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <driver/gpio.h>
#include <esp_timer.h>

#include "sdkconfig.h" //asi nemusi byt

#define DEBOUNCE_MS 100

QueueHandle_t buttonQueue;

typedef enum {
  BTN_RIGHT,
  BTN_LEFT,
  BTN_UP,
  BTN_DOWN,
  BTN_ENT,
  BTN_ESC,
  BTN_COUNT
} button_id_t;

const gpio_num_t button_pins[BTN_COUNT] = {
  GPIO_NUM_27,
  GPIO_NUM_26,
  GPIO_NUM_14,
  GPIO_NUM_33,
  GPIO_NUM_32,
  GPIO_NUM_25
};

typedef struct {
  button_id_t id;
  uint32_t time;
} button_event_t;

static void IRAM_ATTR gpio_isr_handler(void *args) {
  button_id_t button = (button_id_t)args;

  button_event_t event = {
    .id = button,
    .time = xTaskGetTickCountFromISR()
  };

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xQueueSendFromISR(buttonQueue, &event, &xHigherPriorityTaskWoken);
}




void button_control_task(void *args) {
  button_event_t event;
  uint32_t last_press_time[BTN_COUNT] = {0};

  for(;;) {
    if(xQueueReceive(buttonQueue, &event, portMAX_DELAY)) {
      if (event.time - last_press_time[event.id] < pdMS_TO_TICKS(DEBOUNCE_MS))
        continue;

      last_press_time[event.id] = event.time;

      uint8_t level = gpio_get_level(button_pins[event.id]);
      
      if (level == 0)  {// pouze pokud jsou tlacitka pripojena na pull up
        printf("btn %d pressed\n", event.id);
      } else {
        printf("btn %d released\n", event.id);
      }
    }
  }
}

void app_main(void) {
  buttonQueue = xQueueCreate(10, sizeof(button_event_t));

  
  gpio_install_isr_service(0);
  
  for(int i = 0; i < BTN_COUNT; i++) {
    esp_rom_gpio_pad_select_gpio(button_pins[i]);
    gpio_set_direction(button_pins[i], GPIO_MODE_INPUT);
    gpio_pullup_en(button_pins[i]);
    gpio_pulldown_dis(button_pins[i]);
    gpio_set_intr_type(button_pins[i], GPIO_INTR_ANYEDGE);

    gpio_isr_handler_add(button_pins[i], gpio_isr_handler, (void*)(button_pins[i]));
  }
  
  xTaskCreate(button_control_task, "button_control_task", 4096, NULL, 1, NULL);
}
