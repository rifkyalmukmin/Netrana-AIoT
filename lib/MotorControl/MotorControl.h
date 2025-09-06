#pragma once
#include <Arduino.h>

class MotorControl {
    public:
    void begin(uint8_t pin, uint8_t channel = 0, uint32_t freq = 2000, uint8_t resolution = 8);
    void setDuty(uint8_t duty); // 0..255
    void pulse(uint8_t duty, uint16_t on_ms, uint16_t off_ms, uint8_t repeat);
    private:
    uint8_t chan_ = 0;
};