// dep lib
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// customized headers
#include "config.h"

#include <Crypto.h>
#include <SHA256.h>

#define HASH_SIZE 32

WiFiClient client;
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

char *btoh(char *dest, uint8_t *src, int len)
{
  char *d = dest;
  while (len--)
    sprintf(d, "%02x", (unsigned char)*src++), d += 2;
  return dest;
}

char *crypto_password(char *str)
{
  char hex[256];
  char *msg = str;

  uint8_t result[HASH_SIZE];
  SHA256 sha256Hash;

  sha256Hash.reset();
  sha256Hash.update(msg, strlen(msg));
  sha256Hash.finalize(result, HASH_SIZE);

  return btoh(hex, result, HASH_SIZE);
}

StaticJsonDocument<200> loginData;

String litLogin(char *user, char *psw)
{
  char buffer[256];

  loginData["cardNo"] = user;
  loginData["password"] = crypto_password(psw);

  serializeJson(loginData, buffer);

  http.begin(client, CONFIG_LIT_ENDPOINT_LOGIN); //Specify request destination
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(buffer); //Send the request

  if (httpCode > 0)
  { //Check the returning code

    String payload = http.getString(); //Get the request response payload
    Serial.println(payload);           //Print the response payload

    DynamicJsonDocument doc(2048);
    deserializeJson(doc, payload);

    if (doc["code"].as<int>() == 200)
    {
      return doc["data"]["token"];
    }
  }

  http.end(); //Close connection

  return String();
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

  //
  char litUser[] = "USER";
  char litPWD[] = "PWD";

  // try to login
  String token = litLogin(litUser, litPWD);
  // check status
  if (token.isEmpty())
    Serial.println("ERR");
  else
    Serial.println("OK");
}

void loop()
{
  delay(0);
}
