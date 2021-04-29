#include <Esp.h>

String getChipId()
{
    String id = "0x";
    id += String(ESP.getChipId(), HEX);

    return id;
}

// /*
//  *  Get the wifi information
//  *  Return void
//  */
// void get_wifi_info()
// {
//     Serial.println("WiFi Connected.");
//     Serial.printf("SSID: %s\n", WiFi.SSID().c_str());
//     Serial.printf("Password: %s\n", WiFi.psk().c_str());
//     Serial.print("IP Address: ");
//     Serial.println(WiFi.localIP());
// }

/*
 *  Get the device information
 *  Return void
 */
void get_device_info()
{
    Serial.printf("ChipID: %08X\n", ESP.getChipId());

    Serial.printf("Flash Chip id:   %08X\n", ESP.getFlashChipId());
    Serial.printf("Flash Speed: %dMHz\n", ESP.getFlashChipSpeed() / 1000000);
    Serial.printf("Flash Chip Size: %d\n", ESP.getFlashChipSize());
    Serial.printf("Remaining heap size is: %d\n", ESP.getFreeHeap());

    //get the supply voltage
    Serial.printf("The supply voltage: %dv\n", ESP.getVcc() / 1000);

    /*
    //SPIFFS
    Serial.println("Mounting FS...");
    if (!SPIFFS.begin())
    {
        Serial.println("Failed to mount file system");
        return;
    }
    FSInfo fsinfo;
    SPIFFS.info(fsinfo);
    Serial.printf("Total: %u, Used: %u, remain: %u\n", fsinfo.totalBytes, fsinfo.usedBytes, fsinfo.totalBytes - fsinfo.usedBytes);
    */
}