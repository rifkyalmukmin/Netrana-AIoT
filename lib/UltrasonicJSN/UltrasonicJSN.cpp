#include "UltrasonicJSN.h"
#include "config.h"

UltrasonicJSN::UltrasonicJSN(uint8_t trig, uint8_t echo) : trig_(trig), echo_(echo) {}
void UltrasonicJSN::begin() {
        pinMode(trig_, OUTPUT);
        pinMode(echo_, INPUT);
        digitalWrite(trig_, LOW);
}

float UltrasonicJSN::readDistanceCM(uint32_t timeout_us){
    //Trigger 10us
    digitalWrite(trig_, LOW);
    delayMicroseconds(2);
    digitalWrite(trig_, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_, LOW);

    //Baca pulse echo
    unsigned long duration = pulseIn(echo_, HIGH, timeout_us);
    if (duration == 0) return NAN; //timeout

   // Kecepatan suara ~343 m/s → 29.1 us per cm (PPM).
    float cm = (duration / 58.2f); // rumus umum HC-SR04/JSN-SR04
    if (cm < MIN_RANGE_CM || cm > MAX_RANGE_CM) return NAN;
    return cm;
    
}