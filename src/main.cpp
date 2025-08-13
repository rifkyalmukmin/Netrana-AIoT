#include <Arduino.h>
#include "pins.h"
#include "UltrasonicJSN.h"
#include "MotorControl.h"

UltrasonicJSN ultrasonic(TRIG_PIN, ECHO_PIN);
MotorControl motor(MOTOR_PIN);

void setup() {
    Serial.begin(115200);
}

void loop() {
    float distance = ultrasonic.getDistanceCM();

    if (distance < 0) {
        Serial.println("Tidak ada objek / di luar jangkauan");
        motor.stop();
    } else {
        Serial.print("Jarak: ");
        Serial.print(distance);
        Serial.println(" cm");

        if (ultrasonic.isObstacle(50)) { // threshold 50 cm
            Serial.println("⚠️ Halangan terdeteksi! Mengaktifkan motor getar...");
            motor.vibrate(200); // getar 200ms
        } else {
            motor.stop();
        }
    }

    delay(300);
}
