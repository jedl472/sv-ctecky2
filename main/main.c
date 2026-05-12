#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>
#include <u8g2.h>

#include "sdkconfig.h"

#include "display.h"
#include "input.h"

void app_main(void) {
  printf("setup started\n");
  //input_setup();
  display_lcd_setup();

  printf("setup finished\n");
}
