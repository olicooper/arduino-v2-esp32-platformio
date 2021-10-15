#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "sdkconfig.h"
#include <Arduino.h>

void setup() {
    // Serial.begin(115200);
    delay(100);
}

void loop() {
    // Serial.println("Hello");
    ESP_LOGI("Hello");
    delay(5000);
}
