#include "input.h"

#define DEBOUNCE_MS 250

// TODO: tady by melo byt extreme jednoduche dodelat detekci toho, jestli bylo tlacitko zmacknuto nebo pusteno.
// (pak by mohl byt isr nastaven normalne jako GPIO_INTR_ANYEDGE). Problem je, ze cteni hodnoty pinu gpio_get_level()
// kvuli bouncovani hazi random hodnoty (i v isr). Necht je toto hlavolam na pozdeji.


QueueHandle_t isrButtonQueue;
QueueHandle_t buttonEventQueue;


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
} isr_button_event_t;



typedef enum {
  ACTION_BTN_PRESS/*,
  ACTION_BTN_RELEASE*/
} button_action_type_t;

typedef struct {
  button_id_t id;
  button_action_type_t action;
  uint32_t time;
} button_event_t;




static void IRAM_ATTR gpio_isr_handler(void *args) {
  button_id_t button = (button_id_t)args;

  isr_button_event_t event = {
    .id = button,
    .time = xTaskGetTickCountFromISR(),
  };

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xQueueSendFromISR(isrButtonQueue, &event, &xHigherPriorityTaskWoken);
}

void button_control_task(void *args) {
  isr_button_event_t isr_event;
  uint32_t last_press_time[BTN_COUNT] = {0};

  for(;;) {
    if(xQueueReceive(isrButtonQueue, &isr_event, portMAX_DELAY)) {
      if (isr_event.time - last_press_time[isr_event.id] < pdMS_TO_TICKS(DEBOUNCE_MS))
        continue;

      last_press_time[isr_event.id] = isr_event.time;

      printf("btn %d pressed\n", isr_event.id);  
      
      button_event_t event = {
        .id = isr_event.id,
        .action = ACTION_BTN_PRESS,
        .time = isr_event.time
      };

      xQueueSend(buttonEventQueue, &event, 0);
    }
  }
}

void input_setup(void) {
  isrButtonQueue = xQueueCreate(10, sizeof(isr_button_event_t));
  buttonEventQueue = xQueueCreate(10, sizeof(button_event_t));
  
  gpio_install_isr_service(0);
  
  for(int i = 0; i < BTN_COUNT; i++) {
    esp_rom_gpio_pad_select_gpio(button_pins[i]);
    gpio_set_direction(button_pins[i], GPIO_MODE_INPUT);
    gpio_pullup_en(button_pins[i]);
    gpio_pulldown_dis(button_pins[i]);
    gpio_set_intr_type(button_pins[i], GPIO_INTR_NEGEDGE);

    gpio_isr_handler_add(button_pins[i], gpio_isr_handler, (void*)(i));
  }
  
  xTaskCreate(button_control_task, "button_control_task", 4096, NULL, 1, NULL);
}
