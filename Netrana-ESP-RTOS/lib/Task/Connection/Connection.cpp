#include "Connection.h"

Preferences preferences;

// Use String to safely store WiFi credentials
String wifiSSID = "Mangga ditampi";
String wifiPassword = "Blade#12";

const char *serverHost = "azurenetrana-964169919294.us-central1.run.app";
const int   serverPort = 443;
const char *serverPath = "/";
const char *controlPath = "/control";
const char *emergencyHost = "https://netranabackend-964169919294.asia-southeast1.run.app/api/notification/send";

void initWiFi() {
  preferences.begin("wifi", false);
  String storedSSID = preferences.getString("ssid", "");
  String storedPassword = preferences.getString("password", "");
  preferences.end();

  // Safely assign to String variables
  if (storedSSID.length() > 0) {
    wifiSSID = storedSSID;
    Serial.printf("✅ Loaded SSID from preferences: %s\n", wifiSSID.c_str());
  } else {
    Serial.printf("ℹ️  Using default SSID: %s\n", wifiSSID.c_str());
  }
  
  if (storedPassword.length() > 0) {
    wifiPassword = storedPassword;
    Serial.println("✅ Loaded password from preferences");
  } else {
    Serial.println("ℹ️  Using default password");
  }
}

void connectToWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✅ Already connected to WiFi");
    return;
  }
  
  Serial.println("==========================================");
  Serial.printf("Connecting to WiFi: %s\n", wifiSSID.c_str());
  Serial.println("==========================================");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    Serial.print(".");
    attempts++;
    
    if (attempts % 10 == 0) {
      Serial.println();
    }
  }
  
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("==========================================");
    Serial.println("✅ Connected to WiFi!");
    Serial.print("📡 IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("📶 Signal Strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    Serial.println("==========================================");
  } else {
    Serial.println("==========================================");
    Serial.println("❌ Failed to connect to WiFi");
    Serial.printf("SSID: %s\n", wifiSSID.c_str());
    Serial.println("Please check:");
    Serial.println("  1. SSID is correct");
    Serial.println("  2. Password is correct");
    Serial.println("  3. WiFi is 2.4GHz (not 5GHz)");
    Serial.println("  4. Router is in range");
    Serial.println("==========================================");
  }
}

void setupTime() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.print("Time synced: ");
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

bool isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}