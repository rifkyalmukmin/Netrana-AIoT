// === Tombol Navigasi dan Darurat ===
// Catatan: A6 tidak bisa sebagai digital input di Nano
#define BTN_UP        A1
#define BTN_RIGHT     A2
#define BTN_LEFT      A3
#define BTN_DOWN      A5
#define BTN_OK        A4
// #define BTN_EMERGENCY 7   // Gunakan D7 jika ingin tombol terpisah

// === Sensor depan & bawah ===
#define TRIG_PIN_FRONT   4
#define ECHO_PIN_FRONT   5
#define TRIG_PIN_BOTTOM  2
#define ECHO_PIN_BOTTOM  3

#define MOTOR_PIN 6   // Motor getar tunggal (PWM)

// === Konstanta ===
const unsigned long DEBOUNCE_MS  = 30;
const unsigned long HEARTBEAT_MS = 1000;

// Getaran
const float   VIB_THRESHOLD_CM      = 150.0;  // ambang jarak depan
const uint8_t VIB_PWM_PROXIMITY     = 200;    // kekuatan getar saat dekat
const uint8_t VIB_PWM_EMERGENCY     = 255;    // kekuatan getar saat emergency
const unsigned long VIB_EMERGENCY_MS = 1500;  // durasi getar emergency

// === Struktur tombol ===
struct Btn {
  uint8_t pin;
  const char* name;
  bool lastStable;
  bool lastRead;
  unsigned long lastChangeMs;
};

Btn buttons[] = {
  {BTN_UP,   "OK",        true, true, 0},
  {BTN_LEFT, "LEFT",      true, true, 0},
  {BTN_DOWN, "DOWN",      true, true, 0},
  {BTN_RIGHT,"RIGHT",     true, true, 0},
  {BTN_OK,   "EMERGENCY", true, true, 0},
  // {BTN_EMERGENCY, "EMERGENCY", true, true, 0},
};

float distanceFront = 0.0f, distanceBottom = 0.0f;
unsigned long lastHeartbeat = 0;
unsigned long lastPressMs   = 0;

// Timer getar emergency
unsigned long vibEmergencyUntil = 0;

// === Fungsi utilitas ===
float randomFloat02dec(float minV, float maxV) {
  long scaled = random((long)(minV * 100), (long)(maxV * 100) + 1);
  return scaled / 100.0f;
}

float readDistance(uint8_t trigPin, uint8_t echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long duration = pulseIn(echoPin, HIGH, 30000UL); // timeout 30 ms
  if (duration == 0) return -1.0f; // timeout
  return (duration * 0.0343f) / 2.0f; // cm
}

// === Setup ===
void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(A0));

  for (auto &b : buttons) {
    pinMode(b.pin, INPUT_PULLUP);
    bool lvl = digitalRead(b.pin);
    b.lastStable = lvl;
    b.lastRead = lvl;
    b.lastChangeMs = millis();
  }

  pinMode(TRIG_PIN_FRONT, OUTPUT);
  pinMode(ECHO_PIN_FRONT, INPUT);
  pinMode(TRIG_PIN_BOTTOM, OUTPUT);
  pinMode(ECHO_PIN_BOTTOM, INPUT);
  pinMode(MOTOR_PIN, OUTPUT);

  // pastikan motor mati di awal
  analogWrite(MOTOR_PIN, 0);

  lastHeartbeat = millis();
  lastPressMs   = 0;

  Serial.println("SENDER READY");
}

// === Loop utama ===
void loop() {
  unsigned long now = millis();

  // Baca sensor ultrasonic
  distanceFront  = readDistance(TRIG_PIN_FRONT,  ECHO_PIN_FRONT);
  distanceBottom = readDistance(TRIG_PIN_BOTTOM, ECHO_PIN_BOTTOM);

  // Kontrol motor: proximity + emergency
  bool proximityOn = (distanceFront > 0 && distanceFront <= VIB_THRESHOLD_CM);
  bool emergencyOn = (now < vibEmergencyUntil);

  uint8_t level = 0;
  if (emergencyOn) {
    level = VIB_PWM_EMERGENCY;
    // Atur pola kedip jika ingin
    // level = ((now / 50) % 2) ? VIB_PWM_EMERGENCY : 0;
  } else if (proximityOn) {
    level = VIB_PWM_PROXIMITY;
  } else {
    level = 0;
  }
  analogWrite(MOTOR_PIN, level);

  // Deteksi tombol ditekan, kirim segera
  for (auto &b : buttons) {
    bool lvl = digitalRead(b.pin);
    if (lvl != b.lastRead) {
      b.lastRead = lvl;
      b.lastChangeMs = now;
    }
    if ((now - b.lastChangeMs) >= DEBOUNCE_MS) {
      if (b.lastStable != b.lastRead) {
        b.lastStable = b.lastRead;

        if (b.lastStable == LOW) { // ditekan
          Serial.print("BTN, ");
          Serial.print(b.name);
          Serial.print(", ");
          Serial.print(distanceFront, 2);
          Serial.print(", ");
          Serial.println(distanceBottom, 2);

          // Jika EMERGENCY, aktifkan getar darurat
          if (String(b.name) == "EMERGENCY") {
            vibEmergencyUntil = now + VIB_EMERGENCY_MS;
          }

          lastPressMs = now;
        }
      }
    }
  }

  // Heartbeat tiap 1 detik jika tidak ada tombol dalam 1 detik terakhir
  if (now - lastHeartbeat >= HEARTBEAT_MS) {
    lastHeartbeat += HEARTBEAT_MS;

    if (now - lastPressMs >= HEARTBEAT_MS) {
      Serial.print("NONE, ");
      Serial.print(distanceFront, 2);
      Serial.print(", ");
      Serial.println(distanceBottom, 2);
    }
  }

  delay(50); // frekuensi pembacaan sensor
}
