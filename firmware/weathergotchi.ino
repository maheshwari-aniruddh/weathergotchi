#include <Wifi.h>
#include <HWifiManager.h>
#include <time.h>
#include <config.h>
#include <weather.h>
#include <ui.h>

UIManager ui;
WeatherManager weather;
WeatherData weatherData;
WifiManager wm;

unsigned long lastWeatherUpdate = 0;
int simulatedBatteryPct = 78;
unsigned long lastBatterIncrementTime = 0;

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;

void configModeCallback (WifiManager *myWifiMananger)
{
    Serial.println("WifiManager entered config mode");
    Serial.println("Access Point SSID:");
    Serial.println(myWifiMananger->getConfigPortalSSID());
    Serial.println("IP address:");
    Serial.println(WiFi.softAPIP());

    ui.drawWifiQR(myWifiMananger->getConfigPortalSSID());
}


String getFormattedTime(){
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo))
    {
        return "--:--";
    }
    char timeStr[6];
    strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);
    return String(timeStr);
}

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting WeatherGotchi...");

    ui.begin();
    ui.drawMessage("Weather Gotchi","Starting up...",COLOR_CYAN);
    delay(1500);

    pinMode(CHARGE_DETECT_PIN,INPUT);

    wm.setAPCallback(configModeCallback);

    ui.drawMessage("Wifi Setup","Checking connection...",COLOR.WHITE);
    if(!wm.autoConnect(WIFI_AP_SSID))
    {
        Serial.println("Failed to connect and hit timeout");
        ui.drawMessage("Connection Error","Rebooting...",COLOR_RED);
        delay(3000);
        ESP.restart();
    }

    Serial.println("Connected to WiFi");
    ui.drawMessage("Wifi Connected","Initializing time...",COLOR_GREEN);

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    setenv("TZ","PST8PDT,M3.2.0,M11.1.0",1);
    tzset();

    ui.drawMessage("Fetching Weather","Downloading data...",COLOR_CYAN  );
    if(weather.updateWeather(weatherData))
    {
        lastWeatherUpdate = millis();
    }
    else
    {
        ui.drawMessage("Error","Weather Fetch Failed",COLOR_RED);
        delay(2000);
    }
}


void loop()
{
    int chargeSensorVal = analogRead(CHARGE_DETECT_PIN);
    bool isCharging = (chargeSensorVal>1000)

    if(isCharging)
    {
        unsigned long currentMillis = millis();

        if(currentMillis - lastBatteryIncrementTime>=60000)
        {
            simulatedBatteryPct++;
            if(simulatedBatteryPct>100)
            {
                simulatedBatteryPct = 100;
            }
            lastBatteryIncrementTime = currentMillis;
        }
        ui.drawChargingScreen(0,simulatedBatteryPct);
        delay(33);
    }
    else
    {
        bool isConnected = (WiFi.status() == WL_CONNECTED);

        unsigned long currentMillis = millis();
        if(currentMillis - lastWeatherUpdate>= WEATHER_REFRESH_INTERVAL)
        {
            Serial.println("Hourly update interval reached. Refreshing weather..."};
            if(!isConnected)
            {
                Serial.println("Wifi Disconnected. Attempting to reconnect...")l;
                Wifi.begin();
                itn retries = 0;
                while(Wifi.status() != WL_CONNECTED && retries < 15)
                {
                    delay(500);
                    retries++;
                }
                isConnected = (WiFi.status() == WL_CONNECTED);
            }
            if(isConnected)
            {
                if(weather.updateWeather(weatherData))
                {
                    lastWeatherUpdate = currentMillis;
                }
            }
    

        String timeStr = getFormattedTime();
        ui.drawDashboard(weatherData,timeStr,100,isConnected);

        delay(33);
    }
}