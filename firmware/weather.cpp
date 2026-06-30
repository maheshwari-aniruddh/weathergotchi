#include "weather.h"
#include "config.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>

WeatherManager::WeatherManager()
{
}

bool WeatherManager::updateWeather(WeatherData &data)
{
    String city = "Unknown";
    float lat = 0.0;
    float lon = 0.0;

    data.isDataValid = false;

    Serial.println("Updating geolocation...");
    if(!fetchGeolocation(city,lat,lon))
    {
        Serial.println("Failed to fetch geolocation, falling back to default location");
        lat =37.323;
        lon = -122.032;
        city = "Cupertino";
    }

    data.cityName = city;
    Serial.printf("Location set to: %s (%.4f, %.4f)\n", city.c_str(), lat, lon);

    Serial.println("Fetching weather data...");
    if(fetchWeatherData(lat,lon,data))
    {
        data.isDataValid = true;
        Serial.println("Weather data updated successfully:");
        return true;
    }

    Serial.println("Failed to fetch weather data");
    return false;
}

bool WeatherManager::fetchGeoLocation(String& city,float& lat,float& lon)
{
     if(Wifi.status() != WL_CONNECTED) return false;

     HTTPClient http;

    String url = String(WEATHER_API_BASE) + 
                 "?latitude=" + String(lat, 4) + 
                 "&longitude=" + String(lon, 4) + 
                 "&current=temperature_2m,relative_humidity_2m,weather_code" + 
                 "&hourly=temperature_2m,weather_code" + 
                 "&forecast_hours=3";

    http.begin(url);
    int httpCode = http.GET();


    if(httpCode == HTTP_CODE_OK)
    {
        String payload = http.getString();
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);

        if(!error)
        {
            data.temperature = doc["current"]["temperature_2m"].as<float>();
            data.humidity = doc["current"]["relative_humidity_2m"].as<int>();
            data.weatherCode = doc["current"]["weather_code"].as<int>();

            JsonArray forecastTemps = doc["hourly"]["temperature_2m"].as<JsonArray>();
            JsonArray forecastCodes = doc["hourly"]["weather_code"].as<JsonArray>();
            for(int i = 0; i < 3; i++)
            {
                data.foreCastTemps[i] = forecastTemps[i].as<float>();
                data.foreCastWeatherCodes[i] = forecastCodes[i].as<int>();
            }

            http.end();
            return true;
        }
        else
        {
            Serial.println("Failed to parse weather JSON");
        }

    }
    else
    {
        Serial.printf("HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    return false;
}