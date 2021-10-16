// #include <stdio.h>
// #include <freertos/FreeRTOS.h>
// #include <freertos/task.h>
// #include "sdkconfig.h"
extern "C" {
#include <esp_ota_ops.h>
#include <esp_err.h>
#include <esp_wifi.h>
#include <http_parser.h>
}
#include <string>
#include <Arduino.h>
#include <WiFi.h>

void onWifiConnected(wifi_event_sta_connected_t info);
void onWifiDisconnected(wifi_event_sta_disconnected_t info);
void onWifiGotIp(ip_event_got_ip_t info);

bool enabled = true;
bool wifiConnected = false;

void setup() {
    Serial.begin(115200);
    delay(100);

    Serial.println("\n========= BOOT INFO =========\n");
    union {
        uint64_t val64;
        uint8_t val8[8];
    } mac = {};
    mac.val64 = ESP.getEfuseMac();
    Serial.printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", 
        mac.val8[0], mac.val8[1], mac.val8[2], 
        mac.val8[3], mac.val8[4], mac.val8[5]);
    const esp_partition_t *part = esp_ota_get_running_partition();
    if (part) {
        Serial.printf("BootPart: offset=0x%X size=0x%X\n", part->address, part->size);
    }
    Serial.printf("IDF version: %s\n", esp_get_idf_version());
    Serial.printf("Free memory: %d bytes\n", esp_get_free_heap_size());
    Serial.println("\n=============================\n");


    // WIFI CONFIGURATION
    // =============================
    WiFi.onEvent((WiFiEventFuncCb)[&](arduino_event_id_t event, arduino_event_info_t info) {
        Serial.printf("WiFi onEvent: %d\n", event);

        switch (event)
        {
            case ARDUINO_EVENT_WIFI_STA_CONNECTED:
                onWifiConnected(info.wifi_sta_connected);
                break;
            case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
                onWifiDisconnected(info.wifi_sta_disconnected);
                break;
            case ARDUINO_EVENT_WIFI_STA_GOT_IP:
                onWifiGotIp(info.got_ip);
                break;
            default:
                break;
        }
    });

    WiFi.mode(WIFI_MODE_STA); //WIFI_OFF|WIFI_MODE_NULL

    // Storing the values in 'Storage' class, so disable this
    WiFi.persistent(false);
    // Fetch details from Storage class...
    auto ssidStr = std::string("MySsid");
    auto passStr = std::string("MyPassword");

    WiFi.setAutoReconnect(false); // default=true

    auto wlStatus = WiFi.begin(ssidStr.c_str(), passStr.c_str(), 0, NULL, false);

    if (wlStatus == WL_CONNECT_FAILED) {
        enabled = false;
        WiFi.mode(WIFI_OFF); //WIFI_MODE_NULL
        Serial.println("WiFi failed to start, WiFi disabled");
    }
    else {
        // This does NOT connect here! WiFi 'connected' callback will be called later
        auto reason = esp_wifi_connect();
        if (reason == ESP_OK) {
            Serial.println("WiFi connecting");
        }
        // likely a credentials issue or AP not in range
        else {
            enabled = false;
            Serial.println("WiFi failed to connect");
        }
    }
}

void loop() {
    delay(5000);
}


void onWifiConnected(wifi_event_sta_connected_t info) {
    Serial.println("onWifiConnected");
}

void onWifiDisconnected(wifi_event_sta_disconnected_t info) {
    Serial.println("onWifiDisconnected");
    if((info.reason == WIFI_REASON_AUTH_EXPIRE) ||
        (info.reason >= WIFI_REASON_BEACON_TIMEOUT && info.reason != WIFI_REASON_AUTH_FAIL))
    {
        // AP not in range, extend the timer interval
        if (info.reason == WIFI_REASON_NO_AP_FOUND) {
            //connectInterval = WIFI_CONN_INTERVAL * 10U;
        } else {
            //connectInterval = WIFI_CONN_INTERVAL;
        }
    } 
    // permanent failure (passphrase/encryption wrong, wifi broken etc.)
    else {
        enabled = false;
        WiFi.disconnect(true, true);
        WiFi.mode(WIFI_OFF);
    }
}

void onWifiGotIp(ip_event_got_ip_t info) {
    Serial.println("onWifiGotIp");

    wifiConnected = WiFi.status() == WL_CONNECTED;
    
    auto ip = new uint8_t[4];
    memcpy(ip, &info.ip_info.ip.addr, 4);

    Serial.printf(
        "IP " IPSTR " con=%u chg=%u\n",
        ip[0], ip[1], ip[2], ip[3],
        // IP2STR(info.ip_info.ip),
        wifiConnected,
        info.ip_changed);

    delete[] ip;
}
