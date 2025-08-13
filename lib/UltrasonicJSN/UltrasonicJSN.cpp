#include "UltrasonicJSN.h"

UltrasonicJSN::UltrasonicJSN(int trigPin, int echoPin){
    _trigPin = trigPin;
    _echoPin = echoPin;
    pinMode(_trigPin, OUTPUT);
    pinMode(_echoPin, INPUT);
}

float UltrasonicJSN::getDistanceCM() {
    digitalWrite(_trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigPin, LOW);

    long duration = pulseIn(_echoPin, HIGH);
    if (duration == 0) return -1; // No echo received
    
    float distance = duration * 0.034 / 2; // Convert to cm
    return distance;
}
bool UltrasonicJSN::isObstacle(float thresholdCM) {
    float distance = getDistanceCM();
    return (distance > 0 && distance < thresholdCM);
}