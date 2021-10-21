extern "C" {
#include "esp_ota_ops.h"
#include "esp_log.h"
}
#include <Arduino.h>
#include <WiFi.h>
#include "mqtt_client.h"

// #define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE // doesnt work :(
#define LOG_MQTT_LOCAL_LEVEL ESP_LOG_VERBOSE

#define BUTTON_PIN 17

//extern const uint8_t server_cert_pem_start[] asm("_binary_cert_pem_start");
//extern const uint8_t server_cert_pem_end[] asm("_binary_cert_pem_end");

esp_mqtt_client_handle_t mqttClient;
bool wifiStarted = false;
bool mqttStarted = false;
bool mqttConnected = false;

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        Serial.printf("Last error %s: 0x%x\n", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    Serial.printf("MQTTEV %s, event_id=%d\n", base, event_id);
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id = 0;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        mqttConnected = true;
        Serial.printf("MQTT_EVENT_CONNECTED\n");
        Serial.printf("mem: %d bytes\n", esp_get_free_heap_size());
        // msg_id = esp_mqtt_client_publish(client, "topic/qos1", "data_3", 0, 1, 0);
        // Serial.printf("sent publish successful, msg_id=%d\n", msg_id);

        // msg_id = esp_mqtt_client_subscribe(client, "topic/qos0", 0);
        // Serial.printf("sent subscribe successful, msg_id=%d\n", msg_id);

        // msg_id = esp_mqtt_client_subscribe(client, "topic/qos1", 1);
        // Serial.printf("sent subscribe successful, msg_id=%d\n", msg_id);

        // msg_id = esp_mqtt_client_unsubscribe(client, "topic/qos1");
        // Serial.printf("sent unsubscribe successful, msg_id=%d\n", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        mqttConnected = false;
        Serial.printf("MQTT_EVENT_DISCONNECTED\n");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        Serial.printf("MQTT_EVENT_SUBSCRIBED, msg_id=%d\n", event->msg_id);
        // msg_id = esp_mqtt_client_publish(client, "topic/qos0", "data", 0, 0, 0);
        // Serial.printf("sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        Serial.printf("MQTT_EVENT_UNSUBSCRIBED, msg_id=%d\n", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        Serial.printf("MQTT_EVENT_PUBLISHED, msg_id=%d\n", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        Serial.printf("MQTT_EVENT_DATA\n");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        Serial.printf("MQTT_EVENT_ERROR\n");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls\n", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack\n", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno\n",  event->error_handle->esp_transport_sock_errno);
            Serial.printf("Last errno string (%s)\n", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        Serial.printf("Other event id:%d\n", event->event_id);
        break;
    }
}

void setup() {
    Serial.begin(115200);
    delay(100);

    Serial.println("\n========= BOOT INFO =========");
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
    Serial.println("\n=============================");

    pinMode(BUTTON_PIN, INPUT_PULLUP);

    WiFi.mode(WIFI_MODE_STA);

    esp_log_level_set("MQTT_CLIENT", LOG_MQTT_LOCAL_LEVEL);
    esp_log_level_set("TRANS_SSL", LOG_MQTT_LOCAL_LEVEL);
    esp_log_level_set("TRANSPORT_WS", LOG_MQTT_LOCAL_LEVEL);
    esp_log_level_set("TRANSPORT_BASE", LOG_MQTT_LOCAL_LEVEL);
    esp_log_level_set("esp-tls", LOG_MQTT_LOCAL_LEVEL);
    esp_log_level_set("TRANSPORT", LOG_MQTT_LOCAL_LEVEL);
    esp_log_level_set("OUTBOX", LOG_MQTT_LOCAL_LEVEL);

    esp_mqtt_client_config_t mqtt_cfg = {};
    
    // WSS
    // mqtt_cfg.uri = "wss://host/ws";
    // mqtt_cfg.cert_pem = (const char *)server_cert_pem_start;
    
    // WS
    mqtt_cfg.uri = "ws://host/ws";
    
    // MQTT
    // mqtt_cfg.uri = "mqtt://host";
    
    mqtt_cfg.client_id = "clientid";
    mqtt_cfg.buffer_size = 256;
    mqtt_cfg.network_timeout_ms = 5000;
    mqtt_cfg.keepalive = 60;

    mqtt_cfg.lwt_topic = "lwt/clientid";
    mqtt_cfg.lwt_msg = "offline";
    mqtt_cfg.lwt_msg_len = 7;
    mqtt_cfg.lwt_retain = false;
    mqtt_cfg.lwt_qos = 0;
    
    mqttClient = esp_mqtt_client_init(&mqtt_cfg);
    Serial.printf("mqtt null %u\n", mqttClient == NULL);
    
    if (mqttClient != NULL) {
        esp_mqtt_client_register_event(mqttClient, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    }

    WiFi.begin("ssid", "password");
}

void loop() {
    if (!wifiStarted) {
        while (!WiFi.isConnected()) {
            delay(500);
        }

        {
            wifiStarted = true;
            Serial.printf("WIFI CONNECTED\n");
            Serial.printf("wifconn mem: f=%d\n", esp_get_free_heap_size());
            delay(1000);

            if (mqttClient != NULL) {
                auto success = esp_mqtt_client_start(mqttClient);
                if (success == ESP_OK) {
                    mqttStarted = true;
                }
                Serial.printf("mqtt %d\n", success);
                Serial.printf("mqttconn mem: f=%d\n", esp_get_free_heap_size());
            }
        }
        Serial.printf("connend mem: f=%d\n", esp_get_free_heap_size());
        return;
    }

    delay(2000);

    if (mqttClient != NULL) {
        if (digitalRead(BUTTON_PIN) == LOW) {
            // source code: https://github.com/espressif/esp-mqtt/blob/36de30e46df5c04b2d633ceede6505905183489f/mqtt_client.c#L1579
            esp_err_t success = esp_mqtt_client_disconnect(mqttClient);
            Serial.printf("mqtt disconnnect %d\n", success);
        }

        if (mqttStarted && !mqttConnected) {
            esp_err_t success;

            // source code: https://github.com/espressif/esp-mqtt/blob/36de30e46df5c04b2d633ceede6505905183489f/mqtt_client.c#L1620
            success = esp_mqtt_client_stop(mqttClient);
            Serial.printf("mqtt stop %d\n", success);

            success = esp_mqtt_client_destroy(mqttClient);
            Serial.printf("mqtt destroy %d\n", success);
            
            mqttStarted = false;
            mqttClient = NULL;

            delay(3000);

        }
    }
}
