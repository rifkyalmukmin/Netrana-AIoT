#ifndef DUMMY_H
#define DUMMY_H

#include <Arduino.h>

// ========== DEVELOPMENT MODE FLAG ==========
// Set to true untuk development tanpa hardware
// Set to false untuk production dengan UART sesungguhnya
#define USE_DUMMY_DATA false
// ===========================================

struct DummyData {
    String kind;        // "BTN", "NONE", "LEGACY"
    String btnName;     // "LEFT", "RIGHT", "UP", "DOWN", "EMERGENCY"
    float distanceFront;
    float distanceBottom;
};

// Generate random dummy data
DummyData generateDummyData();

// Generate specific dummy events
String generateDummyBTN();
String generateDummyNONE();
String generateDummyLegacy();
String generateDummyBracket();

#endif
