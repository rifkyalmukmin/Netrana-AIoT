#pragma once
#include <Arduino.h>

class UltrasonicJSN {
 public:
    UltrasonicJSN(uint8_t trig, uint8_t echo);
    void begin();
    //Mengembalikan jarak dalam cm (float). Jika out of range, → NAN.
    float readDistanceCM(uint32_t timeout_us = 30000); 
    private:
    uint8_t trig_, echo_;
};

