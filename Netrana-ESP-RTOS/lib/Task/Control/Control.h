#ifndef CONTROL_H
#define CONTROL_H

#include <Arduino.h>

#include <UART/UART.h>

void parserControlInput(String &line, String &kind, String &name, float &distanceFront, float &distanceBottom);

#endif