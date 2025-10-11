#include "Dummy.h"

// Daftar nama button untuk random
const char* buttonNames[] = {"LEFT", "RIGHT", "UP", "DOWN", "SELECT", "EMERGENCY"};
const int buttonCount = 6;

DummyData generateDummyData() {
    DummyData data;
    
    int eventType = random(0, 4); // 0=BTN, 1=NONE, 2=LEGACY, 3=BRACKET
    
    switch (eventType) {
        case 0: // BTN
            data.kind = "BTN";
            data.btnName = buttonNames[random(0, buttonCount)];
            data.distanceFront = random(50, 400) + (random(0, 100) / 100.0f);
            data.distanceBottom = random(30, 200) + (random(0, 100) / 100.0f);
            break;
            
        case 1: // NONE
            data.kind = "NONE";
            data.btnName = "";
            data.distanceFront = random(50, 400) + (random(0, 100) / 100.0f);
            data.distanceBottom = random(30, 200) + (random(0, 100) / 100.0f);
            break;
            
        case 2: // LEGACY
            data.kind = "LEGACY";
            data.btnName = "";
            data.distanceFront = random(50, 400) + (random(0, 100) / 100.0f);
            data.distanceBottom = -1.0f;
            break;
            
        case 3: // BRACKET
            data.kind = "BRACKET";
            data.btnName = buttonNames[random(0, buttonCount)];
            data.distanceFront = random(50, 400) + (random(0, 100) / 100.0f);
            data.distanceBottom = random(30, 200) + (random(0, 100) / 100.0f);
            break;
    }
    
    return data;
}

String generateDummyBTN() {
    const char* btn = buttonNames[random(0, buttonCount)];
    float front = random(50, 400) + (random(0, 100) / 100.0f);
    float bottom = random(30, 200) + (random(0, 100) / 100.0f);
    
    return String("BTN, ") + btn + ", " + String(front, 2) + ", " + String(bottom, 2);
}

String generateDummyNONE() {
    float front = random(50, 400) + (random(0, 100) / 100.0f);
    float bottom = random(30, 200) + (random(0, 100) / 100.0f);
    
    return String("NONE, ") + String(front, 2) + ", " + String(bottom, 2);
}

String generateDummyLegacy() {
    float front = random(50, 400) + (random(0, 100) / 100.0f);
    return String(front, 2) + "cm";
}

String generateDummyBracket() {
    const char* btn = buttonNames[random(0, buttonCount)];
    float front = random(50, 400) + (random(0, 100) / 100.0f);
    float bottom = random(30, 200) + (random(0, 100) / 100.0f);
    
    return String("[BTN] name=") + btn + " front=" + String(front, 2) + " bottom=" + String(bottom, 2);
}
