#include <Arduino.h>

int VM = 22;

void setup() {
  pinMode (VM,OUTPUT);
  Serial.begin(9600);
}

void loop() {
digitalWrite(VM,1);
delay(1000);
digitalWrite(VM,0);
delay(1000);
}