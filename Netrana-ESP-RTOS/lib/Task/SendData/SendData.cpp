#include "SendData.h"

// Queue handles
QueueHandle_t queueFrame = NULL;
QueueHandle_t queueControl = NULL;
QueueHandle_t queueEmergency = NULL;

void sendFrameToServer() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) { Serial.println("Camera capture failed"); return; }

  WiFiClientSecure client; client.setInsecure();
  if (!client.connect(serverHost, serverPort)) { 
    Serial.println("Connection to server failed!"); 
    esp_camera_fb_return(fb); 
    return; 
  }

  String boundary = "----ESP32Boundary";
  String head = "--" + boundary + "\r\n"
                "Content-Disposition: form-data; name=\"file\"; filename=\"esp32.jpg\"\r\n"
                "Content-Type: image/jpeg\r\n\r\n";
  String tail = "\r\n--" + boundary + "--\r\n";
  int contentLength = head.length() + fb->len + tail.length();

  client.print(String("POST ") + serverPath + " HTTP/1.1\r\n");
  client.print(String("Host: ") + serverHost + "\r\n");
  client.print("Content-Type: multipart/form-data; boundary=" + boundary + "\r\n");
  client.print("Content-Length: " + String(contentLength) + "\r\n");
  client.print("Connection: close\r\n\r\n");

  client.print(head);
  client.write(fb->buf, fb->len);
  client.print(tail);

  while (client.connected()) { String line = client.readStringUntil('\n'); if (line == "\r") break; }
  String response = client.readString();
  Serial.println("Server response:"); Serial.println(response);

  client.stop();
  esp_camera_fb_return(fb);
}

bool sendControlCommand(const String &name) {
    if (!isWiFiConnected()) {
        Serial.println("⚠️  WiFi not connected, attempting reconnect...");
        connectToWiFi();
        if (!isWiFiConnected()) {
            Serial.println("❌ WiFi reconnect failed");
            return false;
        }
    }
    
    WiFiClientSecure client;
    client.setInsecure();
    
    if (!client.connect(serverHost, serverPort)) {
        Serial.println("Control connect failed");
        return false;
    }
    
    String body = String("{\"control\":\"") + name + "\"}";
    
    client.print(String("POST ") + controlPath + " HTTP/1.1\r\n");
    client.print(String("Host: ") + serverHost + "\r\n");
    client.print("Content-Type: application/json\r\n");
    client.print("Content-Length: " + String(body.length()) + "\r\n");
    client.print("Connection: close\r\n\r\n");
    client.print(body);
    
    String statusLine = client.readStringUntil('\n');
    Serial.print("Status: ");
    Serial.println(statusLine);
    
    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line.length() <= 2) break;
    }
    
    String resp = client.readString();
    Serial.print("Control response: ");
    Serial.println(resp);
    
    client.stop();
    
    if (statusLine.indexOf("200") == -1) {
        Serial.println("Server error detected");
        return false;
    }
    
    return true;
}

bool sendEmergency() {
    if (!isWiFiConnected()) {
        Serial.println("⚠️  WiFi not connected, attempting reconnect...");
        connectToWiFi();
        if (!isWiFiConnected()) {
            Serial.println("❌ WiFi reconnect failed");
            return false;
        }
    }
    
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
    
    // emergencyHost harus full URL dengan https://
    if (!http.begin(client, emergencyHost)) {
        Serial.println("Emergency begin failed");
        return false;
    }
    
    http.addHeader("Content-Type", "application/json");

    String body = "{\"title\":\"Aa Agam Dalam Bahaya!\","
                  "\"body\":\"Sistem mendeteksi bahwa Aa Agam mungkin sedang dalam situasi darurat. Mohon segera hubungi atau kunjungi keberadaannya.\","
                  "\"userId\":\"YayZ1wVhMIMAF25QkAIL87js0Mv1\"}";
    
    int code = http.POST(body);
    Serial.print("Emergency HTTP code: ");
    Serial.println(code);
    String resp = http.getString();
    Serial.print("Emergency response: ");
    Serial.println(resp);
    
    http.end();
    
    return code >= 200 && code < 300;
}

// ================== RTOS Task Implementations ==================

void taskSendFrame(void *parameter) {
    uint8_t trigger;
    
    for (;;) {
        if (xQueueReceive(queueFrame, &trigger, portMAX_DELAY) == pdTRUE) {
            Serial.println("[TASK-FRAME] Sending frame to server...");
            sendFrameToServer();
            Serial.println("[TASK-FRAME] Frame sent!");
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void taskSendControl(void *parameter) {
    ControlData data;
    
    for (;;) {
        if (xQueueReceive(queueControl, &data, portMAX_DELAY) == pdTRUE) {
            Serial.print("[TASK-CONTROL] Sending control: ");
            Serial.println(data.name);
            
            String controlName = String(data.name);
            sendControlCommand(controlName);
            
            Serial.println("[TASK-CONTROL] Control sent!");
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void taskSendEmergency(void *parameter) {
    uint8_t trigger;
    
    for (;;) {
        if (xQueueReceive(queueEmergency, &trigger, portMAX_DELAY) == pdTRUE) {
            Serial.println("[TASK-EMERGENCY] Sending emergency notification...");
            sendEmergency();
            Serial.println("[TASK-EMERGENCY] Emergency sent!");
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// ================== Queue Push Functions ==================

void queueFrameCapture() {
    uint8_t trigger = 1;
    if (xQueueSend(queueFrame, &trigger, 0) != pdTRUE) {
        Serial.println("[WARN] Frame queue full!");
    }
}

void queueControlEvent(const String &name) {
    ControlData data;
    strncpy(data.name, name.c_str(), sizeof(data.name) - 1);
    data.name[sizeof(data.name) - 1] = '\0';
    
    if (xQueueSend(queueControl, &data, 0) != pdTRUE) {
        Serial.println("[WARN] Control queue full!");
    }
}

void queueEmergencyEvent() {
    uint8_t trigger = 1;
    if (xQueueSend(queueEmergency, &trigger, 0) != pdTRUE) {
        Serial.println("[WARN] Emergency queue full!");
    }
}