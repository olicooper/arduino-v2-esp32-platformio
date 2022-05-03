#ifndef _LORA_H
#define _LORA_H

#include <Arduino.h>
#include <SPI.h>
#include <lmic.h>
#include <hal/hal.h>
#include <Arduino_LoRaWAN_network.h>

#if defined(ARDUINO_LOLIN32_LITE)
#define LMIC_SS_PIN   5  //SS
#define LMIC_RST_PIN  4  //RST_LoRa
#define LMIC_DIO0_PIN 2  //DIO0
#define LMIC_DIO1_PIN 15 //DIO1
#define LMIC_DIO2_PIN 13 //DIO2
#elif defined(ARDUINO_HELTEC_WIRELESS_STICK)
#define LMIC_SS_PIN   18 //SS
#define LMIC_RST_PIN  14 //RST_LoRa
#define LMIC_DIO0_PIN 26 //DIO0
#define LMIC_DIO1_PIN 35 //DIO1
#define LMIC_DIO2_PIN 34 //DIO2
#endif
#define STATE_PUBLISH_INTERVAL_S 300

static const uint8_t PROGMEM APPEUI[8] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
static const uint8_t PROGMEM DEVEUI[8] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
static const uint8_t PROGMEM APPKEY[16] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

// Forward declarations
void processLMICCallback(osjob_t * j);


class Lora : 
    protected Arduino_LoRaWAN_ttn
{
public:
    static const lmic_pinmap pinMap;
    static osjob_t basicStatePublishJob;

    void setup();
    void loop();
    void publishBasicState(bool reschedule = false);

protected:
    // Called after LMIC reset
    virtual void NetBeginRegionInit() override;
    virtual void NetJoin() override;
    virtual void NetTxComplete() override;
    virtual bool GetOtaaProvisioningInfo(Arduino_LoRaWAN::OtaaProvisioningInfo* pProvisioningInfo) override;

};

#endif //_LORA_H
