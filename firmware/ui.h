#ifndef UI_H
#define UI_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <config.h>
#include <weather.h>

class UIManager
{
public:
    UIManager();
    void begin();
    void clearScreen();

    void drawMessage(const WeatherData &data, const String& timeStr,int batteryLevel, bool isWifiConnected);

    void drawWifiQR(const String& ssid);
    void drawChargingScreen(int frame, int batteryPct);

    void drawMessage(const String& title, const String& subtitle,uint16_t color = COLOR_WHITE);

private:
    Adafruit_SSD1351 display;
    void drawBatteryIcon(int x,int y, int pct);
    void drawWeatherIcon(int x,int y, bool online);
    void drawWeatherIcon(int x, int y , int weatherCode, int size = 1);
    String getWMOWeatherText(int code);
};

#endifww