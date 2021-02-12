// dep lib
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// customized headers
#include "config.h"
#include "litncov.h"

#include <Crypto.h>
#include <SHA256.h>

#define HASH_SIZE 32

WiFiClient client;
HTTPClient http; //Declare an object of class HTTPClient

WiFiUDP ntpUDP;

// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
NTPClient timeClient(ntpUDP, "ntp1.aliyun.com", 3600 * 8); //UTC+8

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

String nowDateTime()
{
  String now;

  for (size_t i = 0; now.isEmpty() && i < 3; i++)
  {
    http.begin(client, "http://quan.suning.com/getSysTime.do");
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.GET();
    String payload = http.getString();

    http.end(); //Close connection

    if (httpCode > 0)
    {
      DynamicJsonDocument doc(2048);
      deserializeJson(doc, payload);

      now = doc["sysTime2"].as<char *>();
    }
  }

  return now;
}

/*
 *  litFirstRecord function
 *  
 *  Param float, float, float
 *  temperature -> default is last
 *  temperatureTwo -> default is last
 *  temperatureThree -> default is last
 * 
 *  Return int
 *  0 -> work fine
 *  -1 -> connect error
 *  1 -> login error
 *  2 -> fail to get the last record info
 *  3 -> report error
 *  4 -> get the date time error
 *  5 -> today is reported
 */
int litFirstRecord(char *user, char *psw, float temperature = 0.00, float temperatureTwo = 0.00, float temperatureThree = 0.00)
{

  char buffer[256];
  StaticJsonDocument<200> loginData;

  loginData["cardNo"] = user;
  loginData["password"] = crypto_password(psw);

  serializeJson(loginData, buffer);

  http.begin(client, CONFIG_LIT_ENDPOINT_LOGIN); //Specify request destination
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(buffer);  //Send the request to login
  String payload = http.getString(); //Get the request response payload

  Serial.println(payload); //Print the response payload

  http.end(); //Close connection

  // connect error
  if (httpCode <= 0)
  {
    return -1;
  }

  // login return
  DynamicJsonDocument loginRte(2048);
  deserializeJson(loginRte, payload);

  // if login fail
  if (loginRte["code"].as<int>() != 200)
  {
    return 1;
  }

  String token = loginRte["data"]["token"].as<String>();

  // Create the urk for get last record info
  String eurl = CONFIG_LIT_ENDPOINT_LASTRECORD;
  eurl += "?teamId=";
  eurl += loginRte["data"]["teamId"].as<String>();
  eurl += "&userId=";
  eurl += loginRte["data"]["userId"].as<String>();

  http.begin(client, eurl);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("token", token);

  httpCode = http.GET();
  payload = http.getString();

  Serial.println(payload);

  if (httpCode <= 0)
  { //Check the returning code
    return -1;
  }

  DynamicJsonDocument lastRecordRte(2048);
  deserializeJson(lastRecordRte, payload);

  Serial.println(payload);

  http.end();

  // if get info fail
  if (lastRecordRte["code"].as<int>() != 200)
  {
    return 2;
  }

  char bigBuffer[1500];

  DynamicJsonDocument recordData(2048);

  // most info from lastRecordRte
  deserializeJson(recordData, lastRecordRte["data"].as<String>());

  String now = nowDateTime();
  if (now.isEmpty())
  {
    return 4;
  }

  if (now == lastRecordRte["data"]["reportDate"].as<String>())
  {
    return 5;
  }

  recordData["reportDate"] = now.substring(0, 10);

  recordData["mobile"] = loginRte["data"]["mobile"];
  recordData["nativePlaceProvince"] = loginRte["data"]["nativePlaceProvince"];
  recordData["nativePlaceCity"] = loginRte["data"]["nativePlaceCity"];
  recordData["nativePlaceDistrict"] = loginRte["data"]["nativePlaceDistrict"];
  recordData["nativePlaceAddress"] = loginRte["data"]["nativePlaceAddress"];
  recordData["localAddress"] = loginRte["data"]["localAddress"];

  recordData["userId"] = loginRte["data"]["userId"];
  recordData["teamId"] = loginRte["data"]["teamId"];

  recordData["isTrip"] = lastRecordRte["data"]["isAbroad"];

  if (temperature != 0.00)
  {
    recordData["temperature"] = temperature;
  }

  if (temperatureTwo != 0.00)
  {
    recordData["temperatureTwo"] = temperatureTwo;
  }

  if (temperatureThree != 0.00)
  {
    recordData["temperatureThree"] = temperatureThree;
  }

  serializeJson(recordData, bigBuffer);

  Serial.println(bigBuffer);

  http.begin(client, CONFIG_LIT_ENDPOINT_ADDRECORD); //Specify request destination
  http.addHeader("Content-Type", "application/json;charset=UTF-8");
  http.addHeader("token", token);
  http.addHeader("User-Agent", "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.111 Safari/537.36");

  httpCode = http.POST(bigBuffer); //Send the request to login
  payload = http.getString();       //Get the request response payload

  if (httpCode <= 0)
  { //Check the returning code
    return -1;
  }

  DynamicJsonDocument firstRecordRte(2048);
  deserializeJson(firstRecordRte, payload);

  Serial.println(payload);

  http.end();

  // if report info fail
  if (firstRecordRte["code"].as<int>() != 200)
  {
    return 2;
  }

  return 0;
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
  char litUser[] = "";
  char litPWD[] = "";

  int r = litFirstRecord(litUser, litPWD);
  Serial.println(r);
  // Serial.println(timeClient.getDay());
}

void loop()
{
  timeClient.update();
  // Serial.println(timeClient.getFormattedTime());

  // Serial.println(nowDateTime());
  delay(1000);
}
