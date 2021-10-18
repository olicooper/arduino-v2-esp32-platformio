#include "lora.h"
#include <HardwareSerial.h>

const lmic_pinmap Lora::pinMap = {
    .nss = LMIC_SS_PIN,
    .rxtx = lmic_pinmap::LMIC_UNUSED_PIN,
    .rst = LMIC_RST_PIN,
    .dio = { LMIC_DIO0_PIN, LMIC_DIO1_PIN, LMIC_DIO2_PIN },
};
osjob_t Lora::basicStatePublishJob = {};

void Lora::setup() {

    SetReceiveBufferBufferCb(
        [](void *pCtx, uint8_t uPort, const uint8_t *pBuffer, size_t nBuffer) {
            auto const pLora = ((Lora *)pCtx);

            Serial.printf("RX msg: port=%u rxSize=%u\n", uPort, nBuffer);
        });

    // Generic event listener which can be used for debugging etc.
    RegisterListener(
        // use a lambda so we're "inside" the cMyLoRaWAN from public/private perspective
        [](void *pCtx, uint32_t event) -> void {
            auto const pLora = (Lora *)pCtx;

            switch(event)
            {
                case EV_TXSTART:
                {
                    Serial.printf("EV_TXSTART: Ch=%u\n", (uint8_t)LMIC.txChnl);
                }
                break;
                
                // Class B beacon events
                case EV_BEACON_FOUND:
                case EV_SCAN_TIMEOUT:
                case EV_BEACON_TRACKED:
                case EV_BEACON_MISSED:
                case EV_LOST_TSYNC:
                    Serial.printf("Beacon ev=%u\n", event);
                    break;
                
                default:
                    break;
            }
        },
        (void *) this
    );

    begin(&pinMap);

    // Start publish job - this will automatically perform a network join
    os_setCallback(&basicStatePublishJob, processLMICCallback);
}

void Lora::loop() {
    Super::loop();
}

void Lora::NetBeginRegionInit() {
    this->Super::NetBeginRegionInit();
    Serial.println("NetBeginRegionInit");
}

void Lora::NetJoin() {
    this->Super::NetJoin();
    Serial.println("NetJoin");

    SetLinkCheckMode(true);

    LMIC_setPingable(4);
}

void Lora::NetTxComplete() {
    this->Super::NetTxComplete();

    if (LMIC.txrxFlags & TXRX_ACK) {
        Serial.println("NetTxComplete, received ack");
    }
    else {
        Serial.println("NetTxComplete");
    }
}

bool Lora::GetOtaaProvisioningInfo(OtaaProvisioningInfo *pInfo) {
    if (pInfo != nullptr) {
        memcpy_P(pInfo->AppKey, APPKEY, sizeof(pInfo->AppKey));
        memcpy_P(pInfo->DevEUI, DEVEUI, sizeof(pInfo->DevEUI));
        memcpy_P(pInfo->AppEUI, APPEUI, sizeof(pInfo->AppEUI));
    }

    return true;
}

void Lora::publishBasicState(bool reschedule) {
    // Test data
    size_t dataLen = 2;
    uint8_t data[2] = { 0x01, 0x02 };

    // Schedule the next reading
    if (reschedule) {
        os_setTimedCallback(&basicStatePublishJob, 
            os_getTime() + sec2osticks(STATE_PUBLISH_INTERVAL_S), processLMICCallback);
    }

    // Prepare upstream data transmission at the next possible time
    if (SendBuffer(data, dataLen, nullptr, nullptr, 0, 1)) {
        Serial.println("Publishing state");
    } else {
        Serial.println("Publish failed");
    }
}
