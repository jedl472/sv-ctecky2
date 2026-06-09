/* 
 * Zde se inicializuje u8g2 a hal, všechno vykreslování se řeší v ui.c  
 */

#ifndef LCD_H
#define LCD_H 

#include <esp_log.h>

#include "u8g2_esp32_hal.h"

#define PIN_LCD_CLK 18
#define PIN_LCD_MOSI 23
#define PIN_LCD_CS 5

extern u8g2_t lcd;

void lcd_setup();


#endif 
