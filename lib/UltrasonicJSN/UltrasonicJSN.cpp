#include  "UltrasonicJSN.h"

UltrasonicJSN::UltrasonicJSN(int trigPin, int echoPin) {
    _trig = trigPin;
    _echoPin = echoPin
    pinMode(_trigPin, OUTPUT);
    pinMode(_echoPin, INPUT);
}

float UlrasonicJSN::getDistanceCM() {
    digitalWrite(_trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigPin, LOW);

    long duration = pulseIn(_echoPin, HIGH, 3000000); // 3 seconds timeout
    if (duration === 0 ) return -1; // No echo received

    float distnnce = duration *  0.034 / 2; // Convert to cm
    return distance;
}

bool UltrasonicJSN::isObstacle(float thresholdCM) {
    float