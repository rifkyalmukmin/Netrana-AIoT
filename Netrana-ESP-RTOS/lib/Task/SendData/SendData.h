#ifndef SEND_DATA_H
#define SEND_DATA_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <Connection/Connection.h>
#include <Camera/Camera.h>

// Struktur data untuk queue control
struct ControlData {
    char name[48];
};

// Queue handles
extern QueueHandle_t queueFrame;
extern QueueHandle_t queueControl;
extern QueueHandle_t queueEmergency;

// Task functions
void taskSendFrame(void *parameter);
void taskSendControl(void *parameter);
void taskSendEmergency(void *parameter);

// Helper functions
void sendFrameToServer();
bool sendControlCommand(const String &name);
bool sendEmergency();

// Queue push functions
void queueFrameCapture();
void queueControlEvent(const String &name);
void queueEmergencyEvent();

#endif