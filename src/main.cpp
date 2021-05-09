
// dep tools lib
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <TaskScheduler.h>
#include <PubSubClient.h>
#include <asyncHTTPrequest.h>

// customized headers
#include "config.h"
#include "getinfo.h"
#include "ota.h"
#include "led.h"

// #if LWIP_FEATURES && !LWIP_IPV6

#define HAVE_NETDUMP 0

#ifndef STASSID
#define STASSID "e-LyLg"
#define STAPSK  ""
#endif

#include <ESP8266WiFi.h>
#include <lwip/napt.h>
#include <lwip/dns.h>
#include <LwipDhcpServer.h>

#define NAPT 1000
#define NAPT_PORT 10

// #if HAVE_NETDUMP

// #include <NetDump.h>

// void dump(int netif_idx, const char* data, size_t len, int out, int success) {
//   (void)success;
//   Serial.print(out ? F("out ") : F(" in "));
//   Serial.printf("%d ", netif_idx);

//   // optional filter example: if (netDump_is_ARP(data))
//   {
//     netDump(Serial, data, len);
//     //netDumpHex(Serial, data, len);
//   }
// }
// #endif

const char *ssid = "e-LyLg";
const char *password = "";

String token = "bd909515be366a0d5cace96e49eb4007938c190a";

// // is socket ?
// #define CONFIG_SOCKET

// declare functions
void mqtt_callback(char *topic, byte *payload, unsigned int length);
void job_callback();
void auth_giwifi();
void send_status(unsigned int msg_id);
void dp_handler();

String action_name = "socket"; //socket

// #if defined(CONFIG_SOCKET)
// String action_name = "socket";
// #endif

Led myLed;     //Reverse
Led light(12); //Reverse

// hw set
String deviceSN = getChipId(); // Must be unique on the MQTT network

// mqtt set
String pubTopic = setTopic(deviceSN, "msg");
String subTopic = setTopic(deviceSN, "event");

WiFiClient client;
asyncHTTPrequest request;
PubSubClient mqttclient(CONFIG_MQTT_HOST, CONFIG_MQTT_PORT, &mqtt_callback, client);

Task job_task(0, TASK_FOREVER, &job_callback);
Task auth_task(3  * TASK_MINUTE, TASK_FOREVER, &auth_giwifi);
Scheduler runner;

void job_callback()
{
    if (!mqttclient.connected())
    {
        Serial.print(F("MQTT State: "));
        Serial.println(mqttclient.state());
        if (mqttclient.connect(deviceSN.c_str(), CONFIG_MQTT_USER, CONFIG_MQTT_PASS))
        {
            Serial.print(F("MQTT Connected."));
            Serial.println("Client id = " + deviceSN);
            mqttclient.subscribe(subTopic.c_str());
            Serial.println("Sub Topic = " + subTopic);
            Serial.println("Pub Topic = " + pubTopic);
        }
    }
    mqttclient.loop();
}

void send_status(unsigned int msg_id, String cmd)
{
    DynamicJsonDocument msg(1024);

    String type = "status";

    // create the job json data
    msg["type"] = type;
    msg["msg_id"] = msg_id;
    msg["event_cmd"] = cmd;

    if (light.getStatus())
    {
        msg["data"][action_name] = "off";
    }
    else
    {
        msg["data"][action_name] = "on";
    }

    if (myLed.getStatus())
    {
        msg["data"]["led"] = "off";
    }
    else
    {
        msg["data"]["led"] = "on";
    }

    String gateway = WiFi.gatewayIP().toString();
    String ssid =  WiFi.SSID().c_str();

    msg["wifi"]["ssid"] = ssid; 
    msg["wifi"]["gateway"] = gateway;
    //msg["wifi"]["token"] = token.c_str();
    msg["sn"] = deviceSN;
    msg["model"] = CONFIG_DEVICE_MODEL + action_name;
    msg["hw_ver"] = CONFIG_DEVICE_HW;
    msg["fw_ver"] = CONFIG_DEVICE_FW;
    //msg["data"]["full_ver"] = ESP.getFullVersion();

    char buffer[1024];
    serializeJson(msg, buffer);
    mqttclient.publish(pubTopic.c_str(), buffer);
}

void send_feedback(unsigned int msg_id, String cmd)
{
    DynamicJsonDocument msg(1024);

    String type = "feedback";
    // create the job json data
    msg["type"] = type;
    msg["msg_id"] = msg_id;
    msg["event_cmd"] = cmd;

    msg["sn"] = deviceSN;
    msg["model"] = CONFIG_DEVICE_MODEL + action_name;
    msg["hw_ver"] = CONFIG_DEVICE_HW;
    msg["fw_ver"] = CONFIG_DEVICE_FW;

    char buffer[256];
    serializeJson(msg, buffer);
    mqttclient.publish(pubTopic.c_str(), buffer);
}

void auth_giwifi()
{
//   HTTPClient http;

  String gateway = WiFi.gatewayIP().toString();

  String url = "http://" + gateway;
  url = url + ":8060";
  url = url + "/wifidog/auth?token=";
  url = url + token;
  url = url + "&info=";
  Serial.println(url);
//   // Your IP address with path or Domain name with URL path
//   http.begin(client,url);

//   // Send HTTP POST request
//   int httpResponseCode = http.GET();

//   if (httpResponseCode > 0)
//   {
//     Serial.print("HTTP Response code: ");
//     Serial.println(httpResponseCode);
//     // payload = http.getString();
//     // Serial.print(payload);
//   }
//   else
//   {
//     Serial.print("Error code: ");
//     Serial.println(httpResponseCode);
//   }

//   // Free resources
//   http.end();
    if(request.readyState() == 0 || request.readyState() == 4){
        request.open("GET", url.c_str());
        request.send();
    }
    Serial.println(request.responseHTTPcode());
  return;
}


// void ota_update()
// {
//     ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
//     ESPhttpUpdate.onStart(update_started);
//     ESPhttpUpdate.onEnd(update_finished);
//     ESPhttpUpdate.onProgress(update_progress);
//     ESPhttpUpdate.onError(update_error);

//     t_httpUpdate_return ret = ESPhttpUpdate.update(client,"http://sz.icepie.net:6689/.pio/build/nodemcuv2/firmware.bin");

//     switch (ret)
//     {
//     case HTTP_UPDATE_FAILED:
//         Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
//         break;

//     case HTTP_UPDATE_NO_UPDATES:
//         Serial.println("HTTP_UPDATE_NO_UPDATES");
//         break;

//     case HTTP_UPDATE_OK:
//         Serial.println("HTTP_UPDATE_OK");
//         break;
//     }
// }

// mqtt callback func
void mqtt_callback(char *topic, byte *payload, unsigned int length)
{
    byte *end = payload + length;
    for (byte *p = payload; p < end; ++p)
    {
        Serial.print(*((const char *)p)); // message from the topic
    }
    Serial.println("");

    StaticJsonDocument<256> doc;
    deserializeJson(doc, payload, length);
    JsonObject event = doc.as<JsonObject>();

    String cmd = event["cmd"];
    String data = event["data"];
    unsigned int msg_id = event["msg_id"];

    if (msg_id == 0)
    {
        return;
    }

    if (cmd == "dp")
    {
        ESP.deepSleep(3e6);
        // runner.addTask(deepsleep_task);
        // deepsleep_task.enable();
    }
    else if (cmd == "led")
    {
        send_feedback(msg_id, cmd);
        if (data == "off")
            myLed.on();
        else if (data == "on")
            myLed.off();
    }
    else if (cmd == "status")
        send_status(msg_id, cmd);
    else if (cmd == action_name)
    {
        send_feedback(msg_id, cmd);
        if (data == "on")
            light.off();
        else if (data == "off")
            light.on();
    }
    else if (cmd == "reset")
    {
        send_feedback(msg_id, cmd);
        if (data == "factory")
        {
            WiFi.disconnect(false);
            ESP.eraseConfig();
        }
        ESP.reset();
    }
    else{
        send_feedback(msg_id, "unknown");
    }
}

/*
 *  Determine if the device is automatically connected to wifi via cache
 *  Return a Boolean value
 */
// bool autoConfig()
// {
//     WiFi.begin();
//     if (WiFi.SSID().length() == 0)
//     {
//         return false;
//     }

//     for (int i = 0; i < CONFIG_WIFI_RETRY_TIME; i++)
//     {
//         int wstatus = WiFi.status();
//         if (wstatus == WL_CONNECTED)
//         {
//             Serial.println("AutoConfig Success");
//             //get_wifi_info();
//             WiFi.printDiag(Serial);
//             return true;
//             //break;
//         }
//         else
//         {
//             Serial.println("AutoConfig Waiting......");
//             Serial.println("WIFI STATUS: " + wstatus);
//             delay(1000);
//         }
//     }
//     Serial.println("AutoConfig Faild!");
//     return false;
//     WiFi.printDiag(Serial);
// }

// /*
//  *  SmartConfig function
//  *  Return void
//  */
// void smartConfig()
// {
//     WiFi.mode(WIFI_STA);
//     Serial.println("\r\nWait for Smartconfig");
//     WiFi.beginSmartConfig();
//     while (1)
//     {
//         Serial.printf(".");
//         myLed.off();
//         delay(500);
//         if (WiFi.smartConfigDone())
//         {
//             Serial.println("SmartConfig Success\n");
//             //get_wifi_info();
//             WiFi.setAutoConnect(true); // set auto connect when device boot
//             ESP.reset();
//             //break;
//         }
//         myLed.on();
//         delay(500);
//     }
// }

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;

    randomSeed(analogRead(0));


//   Serial.begin(115200);
  Serial.printf("\n\nNAPT Range extender\n");
  Serial.printf("Heap on start: %d\n", ESP.getFreeHeap());


// #if HAVE_NETDUMP
//   phy_capture = dump;
// #endif

  // first, connect to STA so we can get a proper local DNS server
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.printf("\nSTA: %s (dns: %s / %s)\n",
                WiFi.localIP().toString().c_str(),
                WiFi.dnsIP(0).toString().c_str(),
                WiFi.dnsIP(1).toString().c_str());

  // give DNS servers to AP side
  dhcpSoftAP.dhcps_set_dns(0, WiFi.dnsIP(0));
  dhcpSoftAP.dhcps_set_dns(1, WiFi.dnsIP(1));

  WiFi.softAPConfig(  // enable AP, with android-compatible google domain
    IPAddress(192,168, 9, 1),
    IPAddress(192, 168, 9, 1),
    IPAddress(255, 255, 255, 0));
  WiFi.softAP("ICEPIE-IOT-"+deviceSN, STAPSK);
  Serial.printf("AP: %s\n", WiFi.softAPIP().toString().c_str());

  Serial.printf("Heap before: %d\n", ESP.getFreeHeap());
  err_t ret = ip_napt_init(NAPT, NAPT_PORT);
  Serial.printf("ip_napt_init(%d,%d): ret=%d (OK=%d)\n", NAPT, NAPT_PORT, (int)ret, (int)ERR_OK);
  if (ret == ERR_OK) {
    ret = ip_napt_enable_no(SOFTAP_IF, 1);
    Serial.printf("ip_napt_enable_no(SOFTAP_IF): ret=%d (OK=%d)\n", (int)ret, (int)ERR_OK);
    if (ret == ERR_OK) {
      Serial.printf("WiFi Network '%s' with same password is now NATed behind '%s'\n", STASSID "extender", STASSID);
    }
  }
  Serial.printf("Heap after napt init: %d\n", ESP.getFreeHeap());
  if (ret != ERR_OK) {
    Serial.printf("NAPT initialization failed\n");
  }

    // // wifi config
    // WiFi.begin(ssid, password);
    // if (!autoConfig())
    // {
    //     Serial.println("Start module");
    //      WiFi.begin(ssid, password);
    // }

    // led on
    myLed.off();

    // print chip info
    get_device_info();

    // runner setting
    runner.init();
    runner.addTask(job_task);
    runner.addTask(auth_task);

    job_task.enable();
    auth_task.enable();
}

void loop()
{
    //mqttclient.loop();
    runner.execute();
}
