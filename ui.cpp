#include "ui.h"
#include "config.h"
#include <qrcode.h>

UIManager::UIMananger()
    : display(OLED_CS,OLED_DC,OLED_MOSI,OLED_SCK,OLED_RST){}

void UIManager::begin(){
    display.begin();
    display.fillScreen(COLOR_BLACK);
}

void UIManager() ::clearScreen()
{
    display.fillScreen(COLOR_BLACK);
}

void UIManager::drawMessage(const String& title, const String& subtitle, uint16_t color)
{
    display.fillScreen(COLOR_BLACK);

    display.setTextColor(color);
    display.setTextSize(1);

    int16_t x1,y1;
    uint16_t w,h;
    
    display.getTextBounds(title.c_str(),0,0,&x1,&y1,&w,&h);
    displaay.setCursor(SCREEN_WIDTH/2 - w/2, 45);
    display.print(title);

    display.getTextColor(COLOR_GREY);
    display.getTextBounds(subtitle.c_str(),0,0,&x1,&y1,&w,&h);
    display.setCursor(SCREEN_WIDTH/2 - w/2,70);
    display.print(subtitle);
}

void UIManager::drawDashboard(const WeatherData& data, const String& timeStr,int batteryLevel,bool isWifiConnected){

    display.fillScreen(COLOR_BLACK));

    drawWifiIcon(8,3,isWifiConnected);

    display.setTextColor(COLOR_WHITE);
    display.setTextSize(1);
    display.setCursor(48,3);
    display.print(timeStr);

    drawBatteryIcon(104,3,batteryLevel);
    display.drawFastHLine(0,15,SCREEN_WIDTH,COLOR_GREY);
    if(!data.isDataValid)
    {
        drawMessage("No Weather Data","Connecting...",COLOR_YELLOW);
        return;
    }

    display.setTextColor(COLOR_CYAN);
    display.setTextSize(1);
    int16_t x1,y1;
    uint16_t w,h;
    display.getTextBounds(data.cityName.c_str(),0,0,&x1,&y1,&w,&h);
    int cityX = (SCREEN_WIDTH - w)/2;
    if(cityX<5) cityX = 5;
    display.setCursor(cityX,19);
    display.print(data.cityName);

    drawWeatherIcon(16,30,data.weatherCode,2);

    display.setTextColor(COLOR_WHITE);
    display.setTextSize(2);
    display.setCursor(70,32);
    display.print((int)data.temperature);

    display.setTextSize(1);
    display.print("o");
    display.setTextSize(2);
    display.print("C");

    display.setTextSize(1);
    display.setTextColor(COLOR_GREY);
    



}
    

