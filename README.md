### Current status

Tested on a `Heltec Wireless Stick`. Runs Arduino v2.0.0, but `MCCI Arduino LoRaWAN Library` is not working (see [Log output](#log-output) below).

### Log output

* Build Log
    ```log
    PACKAGES:
    - framework-arduinoespressif32 0.0.0+sha.44c1198
    - tool-esptoolpy 1.30100.210531 (3.1.0)
    - toolchain-xtensa-esp32 8.4.0+2021r1
    Dependency Graph
    |-- <MCCI Arduino LoRaWAN Library> 0.9.1
    |   |-- <MCCI LoRaWAN LMIC library> 4.1.0+sha.26326e4
    |   |   |-- <SPI> 2.0.0
    |   |-- <MCCI Arduino Development Kit ADK> 0.2.2+sha.97a5943
    |-- <MCCI LoRaWAN LMIC library> 4.1.0+sha.26326e4
    |   |-- <SPI> 2.0.0
    |-- <SPI> 2.0.0
    RAM:   [          ]   4.3% (used 14140 bytes from 327680 bytes)
    Flash: [==        ]  20.3% (used 266037 bytes from 1310720 bytes)
    ```

* Debug Log on v2.0.0 `main` branch
    ```log
    ========= BOOT INFO =========

    MAC: FF:FF:FF:FF:FF:FF
    BootPart: offset=0x10000 size=0x140000
    IDF version: v4.4-dev-2313-gc69f0ec32
    Free memory: 241704 bytes

    =============================

    NetBeginRegionInit
    Publishing state
    Guru Meditation Error: Core  1 panic'ed (LoadStoreError). Exception was unhandled.

    Core  1 register dump:
    PC      : 0x400d5e97  PS      : 0x00060b30  A0      : 0x800d5f17  A1      : 0x3ffb25b0
    A2      : 0x40013b1e  A3      : 0x00000000  A4      : 0x00000000  A5      : 0x00002954
    A6      : 0xfffffffc  A7      : 0x00000003  A8      : 0x00000000  A9      : 0x3ffc0f94
    A10     : 0x00000010  A11     : 0x00000016  A12     : 0x00000000  A13     : 0x00040df7
    A14     : 0x63ffffff  A15     : 0x00000003  SAR     : 0x0000001b  EXCCAUSE: 0x00000003
    EXCVADDR: 0x40013b1e  LBEG    : 0x400d5e48  LEND    : 0x400d5ea6  LCOUNT  : 0x00000004  


    Backtrace:0x400d5e94:0x3ffb25b00x400d5f14:0x3ffb25e0 0x400d5aeb:0x3ffb26e0 0x400d5bbe:0x3ffb2700 0x400d5c00:0x3ffb2720 0x400d1855:0x3ffb2740 0x400d2d89:0x3ffb2760 0x400d3086:0x3ffb2780 0x400d40bc:0x3ffb27a0 0x400d5d3b:0x3ffb27c0 0x400d14a9:0x3ffb27e0 0x400d15fe:0x3ffb2800 0x400d7900:0x3ffb2820 
    #0  0x400d5e94:0x3ffb25b00 in Arduino_LoRaWAN::SessionChannelMask_EU_like<16u, 4u>::setFrequency(unsigned char (&) [48], unsigned int, unsigned int) at .pio/libdeps/heltec_wireless_stick/MCCI Arduino LoRaWAN Library/src/Arduino_LoRaWAN.h:369
        (inlined by) Arduino_LoRaWAN::SessionChannelMask_EU_like<16u, 4u>::setFrequency(unsigned char (&) [48], unsigned int, unsigned int) at .pio/libdeps/heltec_wireless_stick/MCCI Arduino LoRaWAN Library/src/Arduino_LoRaWAN.h:359
        (inlined by) Arduino_LoRaWAN::BuildSessionState(Arduino_LoRaWAN::SessionState_u&) const at .pio/libdeps/heltec_wireless_stick/MCCI Arduino LoRaWAN Library/src/lib/arduino_lorawan_sessionstate.cpp:114
    ```
