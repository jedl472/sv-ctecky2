#ifndef INPUT_H
#define INPUT_H

#include <esp_log.h>
#include <driver/gpio.h>
#include <esp_timer.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include <stdio.h>


typedef enum {
  BTN_RIGHT,
  BTN_LEFT,
  BTN_UP,
  BTN_DOWN,
  BTN_ENT,
  BTN_ESC,
  BTN_COUNT
} button_id_t;

typedef enum {
  ACTION_BTN_PRESS/*,
  ACTION_BTN_RELEASE*/
} button_action_type_t;

typedef struct {
  button_id_t id;
  button_action_type_t action;
  uint32_t time;
} button_event_t;

extern QueueHandle_t isrButtonQueue;
extern QueueHandle_t buttonEventQueue;

void input_setup(void);



#endif // !INPUT_H
