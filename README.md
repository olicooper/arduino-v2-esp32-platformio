### Current status

Simple project builds and runs Arduino v2.0.0 (Serial output confirms ESP-IDF v4.4 is running) on a `lolin 32 Lite`. 
However, the MQTT client fails to disconnect gracefully. The same error occurs on the [`mem-optimized` branch](https://github.com/espressif/arduino-esp32/tree/mem-optimized).

### Build log

```log
PLATFORM: Espressif 32 (3.3.1+sha.3784198) > WEMOS LOLIN32 Lite
HARDWARE: ESP32 240MHz, 320KB RAM, 4MB Flash
PACKAGES:
 - framework-arduinoespressif32 0.0.0+sha.44c1198
 - tool-esptoolpy 1.30100.210531 (3.1.0)
 - tool-mkspiffs 2.230.0 (2.30)
 - toolchain-xtensa-esp32 8.4.0+2021r1
Dependency Graph
|-- <WiFi> 2.0.0
RAM:   [=         ]  10.1% (used 32948 bytes from 327680 bytes)
Flash: [=======   ]  69.1% (used 905993 bytes from 1310720 bytes)
```

### Debug log

```log
========= BOOT INFO =========

MAC: FF:FF:FF:FF:FF:FF
BootPart: offset=0x10000 size=0x140000
IDF version: v4.4-dev-2313-gc69f0ec32
Free memory: 222856 bytes

=============================

mqtt null 0
WIFI CONNECTED
wifconn mem: f=136636
mqtt 0
mqttconn mem: f=130124
connend mem: f=130124
MQTTEV MQTT_EVENTS, event_id=7
Other event id:7
MQTTEV MQTT_EVENTS, event_id=1
MQTT_EVENT_CONNECTED
mem: 109092 bytes
mqtt stop 0
```