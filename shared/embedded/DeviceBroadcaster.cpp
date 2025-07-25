//
// Created by valentinw on 17.07.25.
//

#include "DeviceBroadcaster.h"
#include <esp_wifi.h>

namespace IOT
{
    DeviceBroadcaster::DeviceBroadcaster(String purpose)
    {
        purpose_ = purpose;
    }

    void DeviceBroadcaster::setup()
    {
        while (WiFi.status() != WL_CONNECTED) { delay(500);}

        uint8_t baseMac[6];
        esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);

        if (ret == ESP_OK)
        {
            sniprintf(macAddr, 18,"%02X:%02X:%02X:%02X:%02X:%02X",
                           baseMac[0], baseMac[1], baseMac[2],
                           baseMac[3], baseMac[4], baseMac[5]);
        }

        udp_.begin(udpPort_);
    }

    void DeviceBroadcaster::sendBroadcast()
    {
        //wifi not connected
        if (WiFi.status() != WL_CONNECTED) {return;}

        String message = "{";
        message += "\"ip\": \"" + WiFi.localIP().toString() + "\", ";
        message += "\"mac\": \""+ String(macAddr) +"\", ";
        message += "\"purpose\": \""+ purpose_ +"\"";
        message += "}";
        udp_.beginPacket(broadcastIp_, udpPort_);
        udp_.write((const uint8_t*)message.c_str(), message.length());
        udp_.endPacket();
    }

    void DeviceBroadcaster::sendBroadcast(const unsigned long msInterval)
    {
        unsigned long now = millis();
        if (now - lastBroadcast_ >= msInterval)
        {
            lastBroadcast_ = now;
            sendBroadcast();
        }
    }
}
#include "DeviceBroadcaster.h"
