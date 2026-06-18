#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128     

#define OLED_MOSI 15
#define OLED_SCK 13
#define OLED_CS 2
#define OLED_DC 16
#define OLED_RST 3

#define CHARGE_DETECT_PIN A0
#define CHARGE_THERESHOLD 1500

#define GEOLOCATION_URL "http://ip-api.com/json"
#define WEATHER_API_BASE "http://api.open-meteo.com/v1/forecast"

#define WEATHER_REFRESH_INTERVAL 3600000 // 1 hour in milliseconds
#define WIFI_AP_SSID "YourWiFiSSID"

#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF
#define COLOR_RED 0xF800
#define COLOR_GREEN 0x07E0
#define COLOR_BLUE 0x001F
#define COLOR_CYAN 0x07FF
#define COLOR_YELLOW 0xFFE0
#define COLOR_GREY 0x7BEF


#endif // CONFIG_H