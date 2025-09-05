#pragma once
#include <Arduino.h>


// ======================
// Pinout — SESUAIKAN!
// ======================
// Dua sensor ultrasonik JSN‑SR04 (Trigger/Echo)
// Sensor BAWAH (deteksi lubang/halangan bawah tongkat)
#define US1_TRIG 8
#define  US1 ECHO 9
// Sensor ATAS TENGAH (deteksi tiang/halangan depan setinggi badan)
#define US2_TRIG 10
#define US2_ECHO 11

// Vibration motor (via transistor + diode). Gunakan PWM (LEDC)
#define VIBRATION_PIN 5

// Tombol - Gunakan INPUT_PULLUP (aktif LOW)
#define BUTTON_PANIC 2
#define BUTTON_NAV_UP 3
#define BUTTON_NAV_DOWN 4
#define BUTTON_NAV_LEFT 6
#define BUTTON_NAV_RIGHT 7

// ==== CAM PIN MAP (contoh template; PERIKSA modul DFRobot Anda) ====
// Jika memakai pin preset dari modul tertentu, ganti sesuai dokumen resmi.
// Di bawah ini contoh pola umum; TIDAK menjamin cocok out‑of‑the‑box.

#define CAM_PIN_PWDN -1 // Power down (gunakan -1 jika tidak ada)
#define CAM_PIN_RESET -1 // Reset (gunakan -1 jika tidak ada)
#define CAM_PIN_XCLK 15
#define CAM_PIN_SIOD 17
#define CAM_PIN_SIOC 18
#define CAM_PIN_D7 16
#define CAM_PIN_D6 37
#define CAM_PIN_D5 14
#define CAM_PIN_D4 13
#define CAM_PIN_D3 12
#define CAM_PIN_D2 11
#define CAM_PIN_D1 48
#define CAM_PIN_D0 47
#define CAM_PIN_VSYNC 38
#define CAM_PIN_HREF 40
#define CAM_PIN_PCLK 39

//XCLK frequency
#define CAM_XCLK_FREQ 20000000 // 20 MHz umum untuk S3 + PSRAM