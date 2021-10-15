// #include <stdio.h>
// #include <freertos/FreeRTOS.h>
// #include <freertos/task.h>
// #include "sdkconfig.h"
extern "C" {
#include "esp_ota_ops.h"
}
#include <Arduino.h>

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
}

void loop() {
    delay(5000);
}
