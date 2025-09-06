#include "MotorControl.h"

void MotorControl::begin(uint8_t pin, uint8_t channel, uint32_t freq, uint8_t resolution) {
    chan_ = channel;
    ledcSetup(channel, freq, resolution);
    ledcAttachPin(pin, channel);
    setDuty(0);
}

void MotorControl::setDuty(uint8_t duty) {
    ledcWrite(chan_, duty);
}

void MotorControl::pulse(uint8_t duty, uint16_t on_ms, uint16_t off_ms, uint8_t repeat) {
    for (uint8_t i = 0; i < repeat; i++) {
        setDuty(duty);
        vTaskDelay(pdMS_TO_TICKS(on_ms));
        setDuty(0);
        vTaskDelay(pdMS_TO_TICKS(off_ms));
    }
}
