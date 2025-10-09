#include "esp_camera.h"
#include <WiFi.h>
#include <Preferences.h>
#include "time.h"
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// ================== Kamera & IO ==================
#define PWDN_GPIO_NUM -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 5
#define Y9_GPIO_NUM 4
#define Y8_GPIO_NUM 6
#define Y7_GPIO_NUM 7
#define Y6_GPIO_NUM 14
#define Y5_GPIO_NUM 17
#define Y4_GPIO_NUM 21
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 16
#define VSYNC_GPIO_NUM 1
#define HREF_GPIO_NUM 2
#define PCLK_GPIO_NUM 15
#define SIOD_GPIO_NUM 8
#define SIOC_GPIO_NUM 9
#define LED_GPIO_NUM 47

// UART ke Arduino Nano
#define RX_PIN 44
#define TX_PIN 43

// ================== Jaringan ==================
Preferences preferences;
const char *ssid = "Mangga ditampi";
const char *password = "Blade#12";

// Server upload frame
const char *serverHost = "azurenetrana-964169919294.us-central1.run.app";
const int   serverPort = 443;
const char *serverPath = "/";
const char *controlPath = "/control";

// Endpoint darurat
const char *emergencyHost = "https://netranabackend-964169919294.asia-southeast1.run.app/api/notification/send";

// ================== Konfigurasi Aplikasi ==================
const float CAPTURE_THRESHOLD_CM = 150.0f;
const uint32_t SERIAL_READ_TIMEOUT_MS = 150;

// ================== Util ==================
static inline String trimCopy(const String &s) { String t = s; t.trim(); return t; }

bool splitCSV(const String &line, String out[], int maxTokens, char sep = ',') {
  int count = 0, start = 0;
  while (count < maxTokens) {
    int idx = line.indexOf(sep, start);
    if (idx < 0) { out[count++] = trimCopy(line.substring(start)); break; }
    out[count++] = trimCopy(line.substring(start, idx));
    start = idx + 1;
  }
  return count > 0;
}

bool toFloatSafe(const String &s, float &val) {
  if (s.length() == 0) return false;
  val = s.toFloat();
  if (val == 0.0f && s != "0" && s != "0.0" && s != "0.00") {
    String t = s; t.replace("cm", ""); t.trim();
    if (t.length() == 0) return false;
    val = t.toFloat();
    if (val == 0.0f && t != "0" && t != "0.0" && t != "0.00") return false;
  }
  return true;
}

// Parser format bracket: "[BTN] name=RIGHT front-=392.61 bottom-72.39"
bool parseBracketBTN(const String &line, String &name, float &front, float &bottom) {
  if (!line.startsWith("[BTN]")) return false;
  String s = line; s.replace("-=", "="); s.replace("[BTN]", ""); s.trim();

  int p1 = s.indexOf("name="); if (p1 < 0) return false;
  int p1e = s.indexOf(' ', p1);
  String namePart = p1e > 0 ? s.substring(p1 + 5, p1e) : s.substring(p1 + 5); namePart.trim();

  int p2 = s.indexOf("front=");
  int p2e = s.indexOf(' ', p2);
  String frontPart = (p2 >= 0) ? (p2e > 0 ? s.substring(p2 + 6, p2e) : s.substring(p2 + 6)) : "";

  int p3 = s.indexOf("bottom=");
  int p3e = s.indexOf(' ', p3);
  String bottomPart = (p3 >= 0) ? (p3e > 0 ? s.substring(p3 + 7, p3e) : s.substring(p3 + 7)) : "";

  name = namePart;
  front = frontPart.toFloat();
  bottom = bottomPart.toFloat();
  return name.length() > 0;
}

// ================== Waktu, WiFi, Kamera ==================
void setupTime() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) { Serial.println("⚠️ Failed to obtain time"); return; }
  Serial.print("✅ Time synced: ");
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void connectToWiFi(const char *ssid, const char *password) {
  if (WiFi.status() == WL_CONNECTED) return;
  WiFi.begin(ssid, password);
  Serial.printf("Connecting to WiFi: %s\n", ssid);
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 40) { delay(250); Serial.print("."); retries++; }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi connected!");
    Serial.print("IP address: "); Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ Failed to connect to WiFi.");
  }
}

void initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM; 
  config.pin_d1 = Y3_GPIO_NUM; 
  config.pin_d2 = Y4_GPIO_NUM; 
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM; 
  config.pin_d5 = Y7_GPIO_NUM; 
  config.pin_d6 = Y8_GPIO_NUM; 
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM; 
  config.pin_pclk = PCLK_GPIO_NUM; 
  config.pin_vsync = VSYNC_GPIO_NUM; 
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM; 
  config.pin_sccb_scl = SIOC_GPIO_NUM; 
  config.pin_pwdn = PWDN_GPIO_NUM; 
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_QQVGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 25;
  config.fb_count = 1;

  if (psramFound()) { config.jpeg_quality = 25; config.fb_count = 2; config.grab_mode = CAMERA_GRAB_LATEST; }
  else { config.frame_size = FRAMESIZE_QQVGA; config.fb_location = CAMERA_FB_IN_DRAM; }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) { Serial.printf("Camera init failed with error 0x%x\n", err); }
}

void sendFrameToServer() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) { Serial.println("Camera capture failed"); return; }

  WiFiClientSecure client; client.setInsecure();
  if (!client.connect(serverHost, serverPort)) { Serial.println("❌ Connection to server failed!"); esp_camera_fb_return(fb); return; }

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

// ============== Control: tombol biasa ke /control ==============
bool sendControlCommand(const String &name) {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi(ssid, password);
    if (WiFi.status() != WL_CONNECTED) return false;
  }

  WiFiClientSecure client; client.setInsecure();
  if (!client.connect(serverHost, serverPort)) { Serial.println("❌ Control connect failed"); return false; }

  String body = String("{\"control\":\"") + name + "\"}";

  client.print(String("POST ") + controlPath + " HTTP/1.1\r\n");
  client.print(String("Host: ") + serverHost + "\r\n");
  client.print("Content-Type: application/json\r\n");
  client.print("Content-Length: " + String(body.length()) + "\r\n");
  client.print("Connection: close\r\n\r\n");
  client.print(body);

  while (client.connected()) { String line = client.readStringUntil('\n'); if (line == "\r") break; }
  String resp = client.readString();
  Serial.print("Control response: "); Serial.println(resp);

  client.stop();
  return true;
}

// ============== Emergency: body statis ke emergencyHost ==============
bool sendEmergency() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi(ssid, password);
    if (WiFi.status() != WL_CONNECTED) return false;
  }

  WiFiClientSecure client; client.setInsecure();
  HTTPClient http;

  if (!http.begin(client, emergencyHost)) {
    Serial.println("❌ Emergency begin failed");
    return false;
  }

  http.addHeader("Content-Type", "application/json");
  // Body statis sesuai permintaan
  String body = "{\"title\":\"Aa Agam Dalam Bahaya!\","
                "\"body\":\"Sistem mendeteksi bahwa Aa Agam mungkin sedang dalam situasi darurat. Mohon segera hubungi atau kunjungi keberadaannya.\","
                "\"userId\":\"YayZ1wVhMIMAF25QkAIL87js0Mv1\"}";

  int code = http.POST(body);
  Serial.print("Emergency HTTP code: "); Serial.println(code);
  String resp = http.getString();
  Serial.print("Emergency response: "); Serial.println(resp);

  http.end();
  return code > 0 && code < 400;
}

// ================== Setup & Loop ==================
void setup() {
  Serial.begin(115200);
  delay(100);

  Serial2.setTimeout(SERIAL_READ_TIMEOUT_MS);
  Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);

  initCamera();
  connectToWiFi(ssid, password);
  setupTime();

  Serial.println("RECEIVER READY");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) connectToWiFi(ssid, password);

  if (Serial2.available()) {
    String line = Serial2.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) return;

    // Parser format bracket
    if (line.startsWith("[BTN]")) {
      String btnName; float f = -1, b = -1;
      if (parseBracketBTN(line, btnName, f, b)) {
        Serial.print("[BTN-BRACKET] name="); Serial.print(btnName);
        Serial.print(" front="); Serial.print(f, 2);
        Serial.print(" bottom="); Serial.println(b, 2);

        if (btnName.equalsIgnoreCase("EMERGENCY")) {
          sendEmergency();
        } else {
          sendControlCommand(btnName);
        }

        if (f > 0 && f <= CAPTURE_THRESHOLD_CM) {
          Serial.println("Front <= threshold via bracket -> capture");
          sendFrameToServer();
        }
        return;
      }
    }

    // Parser CSV: "BTN, NAME, front, bottom" atau "NONE, front, bottom"
    String tok[4];
    if (!splitCSV(line, tok, 4, ',')) return;

    String kind = tok[0]; kind.trim();
    float distanceFront = -1.0f, distanceBottom = -1.0f;

    if (kind.equalsIgnoreCase("BTN")) {
      String btnName = tok[1]; btnName.trim();
      bool okF = toFloatSafe(tok[2], distanceFront);
      bool okB = toFloatSafe(tok[3], distanceBottom);

      Serial.print("[BTN] name="); Serial.print(btnName);
      Serial.print(" front="); Serial.print(okF ? distanceFront : -1.0f, 2);
      Serial.print(" bottom="); Serial.println(okB ? distanceBottom : -1.0f, 2);

      if (btnName.equalsIgnoreCase("EMERGENCY")) {
        sendEmergency();
      } else {
        sendControlCommand(btnName);
      }

      if (okF && distanceFront > 0 && distanceFront <= CAPTURE_THRESHOLD_CM) {
        Serial.println("Front <= threshold via BTN event -> capture");
        sendFrameToServer();
      }

    } else if (kind.equalsIgnoreCase("NONE")) {
      bool okF = toFloatSafe(tok[1], distanceFront);
      bool okB = toFloatSafe(tok[2], distanceBottom);

      Serial.print("[NONE] front="); Serial.print(okF ? distanceFront : -1.0f, 2);
      Serial.print(" bottom="); Serial.println(okB ? distanceBottom : -1.0f, 2);

      if (okF && distanceFront > 0 && distanceFront <= CAPTURE_THRESHOLD_CM) {
        Serial.println("Front <= threshold via heartbeat -> capture");
        sendFrameToServer();
      }

    } else {
      int cmIdx = line.indexOf("cm");
      if (cmIdx > 0) {
        float f = line.substring(0, cmIdx).toFloat();
        Serial.print("[LEGACY] front="); Serial.println(f, 2);
        if (f > 0 && f <= CAPTURE_THRESHOLD_CM) sendFrameToServer();
      } else {
        Serial.print("[WARN] Unrecognized line: "); Serial.println(line);
      }
    }
  }

  delay(10);
}
