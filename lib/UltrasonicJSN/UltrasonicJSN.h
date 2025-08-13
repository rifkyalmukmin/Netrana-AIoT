#ifndef ULTRASONICJSN_H
#define ULTRASONICJSN_H

#include <Arduino.h>

class UltrasonicJSN {
public:
    UltrasonicJSN(int trigPin, int echoPin);
    float getDistanceCM();
    bool isObstacle(float thresholdCM);

private:
    int _trigPin;
    int _echoPin;
};

#endif
