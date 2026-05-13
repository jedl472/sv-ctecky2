#include "lcd.h"

u8g2_t lcd;

void lcd_setup() {
  printf("entering display setup\n");
  
  u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
	u8g2_esp32_hal.bus.spi.clk   = PIN_LCD_CLK;
	u8g2_esp32_hal.bus.spi.mosi  = PIN_LCD_MOSI;
	u8g2_esp32_hal.bus.spi.cs    = PIN_LCD_CS;
	u8g2_esp32_hal.dc    = GPIO_NUM_NC;
	u8g2_esp32_hal.reset = GPIO_NUM_NC;
	u8g2_esp32_hal_init(u8g2_esp32_hal);
	
  u8g2_Setup_st7920_s_128x64_f(
		&lcd,
		U8G2_R0,
		u8x8_byte_esp32_sw_spi,       
		u8g2_esp32_gpio_and_delay_cb);  // init u8g2 structure

	u8g2_InitDisplay(&lcd); // send init sequence to the display, display is in sleep mode after this,

	u8g2_SetPowerSave(&lcd, 0); // wake up display
	u8g2_ClearBuffer(&lcd);
	// u8g2_SetFont(&display_lcd, u8g2_font_5x8_tf);
	// u8g2_DrawStr(&display_lcd, 0,15,"sv-ctecky2");
	u8g2_SendBuffer(&lcd);
} // lcd_setup


