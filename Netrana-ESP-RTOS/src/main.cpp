#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Modules
#include <Camera/Camera.h>
#include <UART/UART.h>
#include <Connection/Connection.h>
#include <Dummy/Dummy.h>  // Dummy data untuk development

// Tasks
#include <SendData/SendData.h>
#include <Control/Control.h>

const float CAPTURE_THRESHOLD_CM = 150.0f;

// ================== Task Handles ==================
TaskHandle_t handleTaskFrame = NULL;
TaskHandle_t handleTaskControl = NULL;
TaskHandle_t handleTaskEmergency = NULL;
TaskHandle_t handleTaskReader = NULL;

// ================== Event Handlers ==================
void handleButtonEvent(const String &name, float distanceFront) {
    Serial.print("[BTN] name="); Serial.print(name);
    Serial.print(" front="); Serial.println(distanceFront, 2);
    
    if (name.equalsIgnoreCase("EMERGENCY")) {
        queueEmergencyEvent();
    } else {
        queueControlEvent(name);
    }
    
    if (distanceFront > 0 && distanceFront <= CAPTURE_THRESHOLD_CM) {
        Serial.println("Front <= threshold -> capture");
        queueFrameCapture();
    }
}

void handleNoneEvent(float distanceFront, float distanceBottom) {
    Serial.print("[NONE] front="); Serial.print(distanceFront, 2);
    Serial.print(" bottom="); Serial.println(distanceBottom, 2);
    
    if (distanceFront > 0 && distanceFront <= CAPTURE_THRESHOLD_CM) {
        Serial.println("Front <= threshold via heartbeat -> capture");
        queueFrameCapture();
    }
}

void handleLegacyFormat(const String &line) {
    int cmIdx = line.indexOf("cm");
    if (cmIdx > 0) {
        float f = line.substring(0, cmIdx).toFloat();
        Serial.print("[LEGACY] front="); Serial.println(f, 2);
        if (f > 0 && f <= CAPTURE_THRESHOLD_CM) {
            queueFrameCapture();
        }
    } else {
        Serial.print("[WARN] Unrecognized line: "); 
        Serial.println(line);
    }
}

// ================== UART Reader Task (Support Dummy Mode) ==================
void taskUARTReader(void *parameter) {
    for (;;) {
        // Auto-reconnect WiFi
        if (!isWiFiConnected()) {
            Serial.println("⚠️  WiFi disconnected, attempting reconnect...");
            connectToWiFi();
        }
        
        String line;
        
        #if USE_DUMMY_DATA
            // ========== DUMMY MODE: Generate random data ==========
            int randType = random(0, 4);
            switch (randType) {
                case 0: line = generateDummyBTN(); break;
                case 1: line = generateDummyNONE(); break;
                case 2: line = generateDummyLegacy(); break;
                case 3: line = generateDummyBracket(); break;
            }
            Serial.print("[DUMMY] Generated: ");
            Serial.println(line);
            
            // Delay lebih lama untuk dummy mode agar mudah di-monitor
            vTaskDelay(3000 / portTICK_PERIOD_MS);
        #else
            // ========== PRODUCTION MODE: Read from UART ==========
            readLineFromUART(line);
            
            if (line.length() == 0) {
                vTaskDelay(10 / portTICK_PERIOD_MS);
                continue;
            }
        #endif
        
        // Parse control input (sama untuk dummy dan real)
        String kind, name;
        float distanceFront, distanceBottom;
        parserControlInput(line, kind, name, distanceFront, distanceBottom);
        
        // Handle events
        if (kind.equalsIgnoreCase("BTN")) {
            handleButtonEvent(name, distanceFront);
        } 
        else if (kind.equalsIgnoreCase("NONE")) {
            handleNoneEvent(distanceFront, distanceBottom);
        } 
        else if (kind.length() == 0) {
            handleLegacyFormat(line);
        }
        
        #if !USE_DUMMY_DATA
            vTaskDelay(10 / portTICK_PERIOD_MS);
        #endif
    }
}

void setup() {
    Serial.begin(115200);
    delay(100);
    
    #if USE_DUMMY_DATA
        Serial.println("==========================================");
        Serial.println("⚠️  DUMMY MODE ENABLED - DEVELOPMENT ONLY");
        Serial.println("==========================================");
        randomSeed(analogRead(0));
    #else
        Serial.println("==========================================");
        Serial.println("✅ PRODUCTION MODE - Using Real UART");
        Serial.println("==========================================");
        initUART();
    #endif
    
    // Init modules
    initCamera();
    
    // Init connection
    initWiFi();
    connectToWiFi();
    setupTime();
    
    // Create queues
    queueFrame = xQueueCreate(5, sizeof(uint8_t));
    queueControl = xQueueCreate(10, sizeof(ControlData));
    queueEmergency = xQueueCreate(3, sizeof(uint8_t));
    
    if (queueFrame == NULL || queueControl == NULL || queueEmergency == NULL) {
        Serial.println("❌ Failed to create queues!");
        while (1) { delay(1000); }
    }
    
    // Create tasks
    xTaskCreatePinnedToCore(
        taskSendFrame,
        "SendFrame",
        8192,
        NULL,
        2,
        &handleTaskFrame,
        0
    );
    
    xTaskCreatePinnedToCore(
        taskSendControl,
        "SendControl",
        8192,
        NULL,
        2,
        &handleTaskControl,
        0
    );
    
    xTaskCreatePinnedToCore(
        taskSendEmergency,
        "SendEmergency",
        8192,
        NULL,
        3,
        &handleTaskEmergency,
        1
    );
    
    xTaskCreatePinnedToCore(
        taskUARTReader,
        "UARTReader",
        4096,
        NULL,
        1,
        &handleTaskReader,
        1
    );
    
    Serial.println("RECEIVER READY (RTOS Mode)");
    Serial.println("Tasks created:");
    Serial.println("   - taskSendFrame (Core 0, Priority 2)");
    Serial.println("   - taskSendControl (Core 0, Priority 2)");
    Serial.println("   - taskSendEmergency (Core 1, Priority 3)");
    Serial.println("   - taskUARTReader (Core 1, Priority 1)");
    
    #if USE_DUMMY_DATA
        Serial.println("\n⚠️  Remember to set USE_DUMMY_DATA = false for production!");
    #endif
}

void loop() {
    // Empty - semua logic ada di RTOS tasks
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
