#pragma once
#include <Arduino.h>
#include "secrets.h"
// ============= Identitas perangkat =============
#ifdef DEVICE_ID
    #define DEVICE_ID "netrana-IoT"
#endif

// ================ WiFi =================
#ifndef Wifi_SSID
    #define Wifi_SSID "your-ssid"
#endif
#ifndef WIFI_PASSS
   #define WIFI_PASSS "your-password"
#endif


// ================ MQTT =================
#ifndef MQQT_HOS
    #define MQQT_BROKER "your-mqtt-broker"
#endif
#ifndef MQQT_PORT
    #define MQQT_PORT 1883
#endif
#ifdef MQQT_USER
    #define MQQT_USER "your-mqtt-user"
#endif
#ifdef MQQT_PASS
    #define MQQT_PASS "your-mqtt-password"
#endif

//Topik Konversi
#define T_BASE String("smartcane/") + DEVICE_ID
#define T_TELEMETRY (T_BASE + "/telemetry") // publish JSON jarak, status
#define T_ALERT (T_BASE + "/alert") // publish panic/halangan
#define T_IMAGE_META (T_BASE + "/image/meta") // publish info upload
#define T_BUTTONS (T_BASE + "/buttons") // publish event tombol
#define T_TTS (T_BASE + "/tts") // (opsional) kirim teks untuk dibacakan di app
#define T_CMD (T_BASE + "/cmd/#") // subscribe command

// ================ YOLO SERVER =================
#ifndef YOLO_URL
    #define YOLO_URL "http://your-yolo-server/detect"
#endif

// ================ Log & Interval =================
#define ENABLE_LOGGING 1 // Set ke 0 untuk menonaktifkan log serial
#define TELEMETRY_INTERVAL_MS 10000UL
#define CAMERA_PERIOD_MS    2000UL
#define ULTRA_SONIC_INTERVAL_MS 80UL 
#define BUTTON_SCAN_MS      20UL

// ===== Threshold & Batas =====
#define MIN_RANGE_CM 20 // JSN-SR04 rekomendasi rentang efektif
#define MAX_RANGE_CM 600
#define OBSTACLE_NEAR_CM 120 // <120 cm dianggap halangan dekat
#define OBSTACLE_CRITICAL_CM 60 // <60 cm kritis → prioritas tinggi

// ===== Motor Getar =====
#define VIBE_DEFAULT_DUTY 200 // 0..255 (PWM 8-bit) — sesuaikan
#define VIBE_STRONG_DUTY 255
#define VIBE_WEAK_DUTY 120

// ===== RTOS Priorities (semakin besar semakin prioritas) =====
#define PRIO_ULTRA 5 // deteksi halangan
#define PRIO_MOTOR 4
#define PRIO_BUTTONS 3
#define PRIO_MQTT 3
#define PRIO_CAMERA 2
#define PRIO_IDLE 1

// ===== Queue Sizes =====
#define Q_BUTTONS_LEN 10
#define Q_ULTRA_LEN 10
#define Q_ALERT_LEN 5

// Helper log makro
#if ENABLE_LOG
    #define LOGI(tag, fmt, ...) Serial.printf("[I] %s: " fmt "\n", tag, ##__VA_ARGS__)
    #define LOGE(tag, fmt, ...) Serial.printf("[E] %s: " fmt "\n", tag, ##__VA_ARGS__)
#else
#define LOGI(...)
#define LOGE(...)
#endif// Interval pengiriman data telemetry