#ifndef UART_H
#define UART_H

#include <Arduino.h>

#define RX_PIN 44
#define TX_PIN 43

extern const uint32_t SERIAL_READ_TIMEOUT_MS;

static inline String trimCopy(const String &s) { String t = s; t.trim(); return t; }
    
void initUART();
bool splitCSV(const String &line, String out[], int maxTokens, char sep = ',');
bool toFloatSafe(const String &s, float &val);
bool parseBracketBTN(const String &line, String &name, float &front, float &bottom);
void readLineFromUART(String &line);

#endif