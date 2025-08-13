#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <Arduino.h>

class MotorControl {
    public:
        MotorControl(int motorPin);
        void vibrate(int durationMs);
        void stop();

private:
 int _motorPin;

};
#endif 
