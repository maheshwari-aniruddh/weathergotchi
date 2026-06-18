#ifndef WEATHER_H
#define WEATHER_H

#include <Arduino.h>
#include <WifiClient.h>

struct WeatherData
{
    float temperature;
    int humidity;

    int weatherCode;
    String cityName;
    bool isDataValid;

    float foreCastTemps[3];
    int foreCastWeatherCodes[3];
};

class WeatherManager
{
public:
    WeatherManager();
    bool updateWeather(WeatherData &data);

private:
bool fetchGeolocation(float &latitude, float &longitude, String &cityName);
    bool fetchWeatherData(float latitude, float longitude, WeatherData &data);
    String getWMOWeatherText(int code);
};

#endif // WEATHER_H