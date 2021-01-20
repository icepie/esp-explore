// dep lib
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// customized headers
#include "config.h"

HTTPClient http; //Declare an object of class HTTPClient

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

void litLogin()
{

  http.begin(CONFIG_LIT_ENDPOINT_LOGIN); //Specify request destination
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST("{\"cardNo\":\"tPmAT5Ab3j7F9\",\"password\":\"49.54\"}");                                 //Send the request

  if (httpCode > 0)
  { //Check the returning code

    String payload = http.getString(); //Get the request response payload
    Serial.println(payload);           //Print the response payload
  }

  http.end(); //Close connection
}

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  randomSeed(analogRead(0));

  // wifi config
  if (!autoConfig())
  {
    Serial.println("Start module");
    smartConfig();
  }

  // try to login
  litLogin();
}

void loop()
{

}
