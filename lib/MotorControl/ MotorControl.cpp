#include "MotorControl.h"

MotorControl::MotorControl(int motorPin) {
    _motorPin = motorPin;
    pinMode(_motorPin, OUTPUT);
    digitalWrite(_motorPin, LOW);
}

void MotorControl::vibrate(int durationMs) {
    digitalWrite(_motorPin, HIGH);
    delay(durationMs);
    digitalWrite(_motorPin, LOW);
}

void MotorControl::stop() {
    digitalWrite(_motorPin, LOW);
}


