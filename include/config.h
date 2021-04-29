
// Default
#define CONFIG_PROJECT_CODE "giwifi"
#define CONFIG_RELEASE 0
#define CONFIG_DEVICE_MODEL "lamp-"
#define CONFIG_DEVICE_FW 0.2
#define CONFIG_DEVICE_HW "ESP8266-REV01"
#define CONFIG_WIFI_RETRY_TIME 10
#define CONFIG_OPEN_DOOR_TIMEOUT 8

// MQTT
#define CONFIG_MQTT_HOST ""
#define CONFIG_MQTT_PORT 1883
#define CONFIG_MQTT_USER ""
#define CONFIG_MQTT_PASS ""

// OTA
#define CONFIG_OTA_HOST ""
#define CONFIG_OTA_PORT 6689
#define CONFIG_OTA_PATH "/.pio/build/nodemcuv2/firmware.bin"

/*
 *  Set a topic use a unique value and topic type, just like "project/deviceid/msgtype"
 *  Return a String value
 */
String setTopic(String unique, String type)
{
    String topic = CONFIG_PROJECT_CODE;
    topic += "/";
    topic += unique;
    topic += "/";
    topic += type;

    return topic;
}
