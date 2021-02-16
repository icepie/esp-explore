
// dep lib
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <aWOT.h>

// customized headers
#include "config.hpp"

WiFiClient client;
WiFiServer server(80);
Application app;

/*
 *  Determine if the device is automatically connected to wifi via cache
 *  Return a Boolean value
 */
bool autoConfig()
{
    WiFi.begin();
    for (int i = 0; i < CONFIG_WIFI_RETRY_TIME; i++)
    {
        int wstatus = WiFi.status();
        if (wstatus == WL_CONNECTED)
        {
            Serial.println("AutoConfig Success");
            // wifi info
            Serial.println("WiFi Connected.");
            Serial.printf("SSID: %s\n", WiFi.SSID().c_str());
            Serial.printf("Password: %s\n", WiFi.psk().c_str());
            Serial.print("IP Address: ");
            Serial.println(WiFi.localIP());
            WiFi.printDiag(Serial);
            return true;
            //break;
        }
        else
        {
            Serial.println("AutoConfig Waiting......");
            Serial.println("WIFI STATUS: " + wstatus);
            delay(1000);
        }
    }
    Serial.println("AutoConfig Faild!");
    return false;
    WiFi.printDiag(Serial);
}

/*
 *  SmartConfig function
 *  Return void
 */
void smartConfig()
{
    WiFi.mode(WIFI_STA);
    Serial.println("\r\nWait for Smartconfig");
    WiFi.beginSmartConfig();
    while (1)
    {
        Serial.print(".");
        if (WiFi.smartConfigDone())
        {
            Serial.println("SmartConfig Success\n");
            WiFi.setAutoConnect(true); // set auto connect when device boot
            break;
        }
        delay(500); // It's important to add delay to this place, otherwise it's extremely easy to crash and restart.
    }
}

void index(Request &req, Response &res)
{
    res.print("Hello World!");
}

void sysInfo(Request &req, Response &res)
{
    res.set("Content-Type", "application/json");

    char buffer[256];
    StaticJsonDocument<200> data;

    data["code"] = 200;

    String id = "0x";
    id += String(ESP.getChipId(), HEX);

    data["data"]["sn"] = id;
    data["data"]["fw"] = CONFIG_PROJECT_CODE;
    data["data"]["ver"] = CONFIG_DEVICE_FW;
    data["data"]["network"] = WiFi.SSID();
    data["data"]["ip"] = WiFi.localIP().toString();

    data["msg"] = "ok";

    serializeJson(data, buffer);

    res.print(buffer);

    res.end();
}

void router()
{
    app.get("/", &index);
    app.get("/sysinfo", &sysInfo);
    server.begin();
}

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;

    // wifi config
    if (!autoConfig())
    {
        Serial.println("Start module");
        smartConfig();
    }

    // start router
    router();
}

void loop()
{
    client = server.available();
    if (client.connected())
    {
        app.process(&client);
    }
}