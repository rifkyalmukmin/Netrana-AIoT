#include "Capture.h"

void captureFrame() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) { Serial.println("Camera capture failed"); return; }
}