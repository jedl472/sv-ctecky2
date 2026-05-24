#ifndef INPUT_H
#define INPUT_H

#include <esp_log.h>
#include <driver/gpio.h>
#include <esp_timer.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include <stdio.h>



extern QueueHandle_t isrButtonQueue;
extern QueueHandle_t buttonEventQueue;

void input_setup(void);



#endif // !INPUT_H
