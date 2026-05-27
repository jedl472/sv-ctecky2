# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES esp_wifi nvs_flash esp_event esp_netif esp_timer)
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "main.c" "input.c" "lcd.c" "ui.c" "nfc.c" "wifi.c")
set(COMPONENT_ADD_INCLUDEDIRS "." "include")

register_component()
