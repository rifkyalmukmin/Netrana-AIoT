# Netrana-AIoT

![Project Banner](https://via.placeholder.com/1200x300.png?text=Netrana-AIoT+Project)

A comprehensive AIoT project featuring a modular control system with a haptic feedback remote for intuitive interaction with robots, vehicles, or other IoT devices.

---

## Table of Contents

- [About The Project](#about-the-project)
- [System Architecture](#system-architecture)
- [Key Features](#key-features)
- [Hardware & Software](#hardware--software)
  - [Hardware Components](#hardware-components)
  - [Software Firmware](#software-firmware)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Hardware Setup](#hardware-setup)
  - [Firmware Installation](#firmware-installation)
- [How It Works](#how-it-works)
- [Roadmap](#roadmap)
- [Contributing](#contributing)
- [License](#license)

---

## About The Project

Netrana-AIoT is not just a simple IoT project; it's a complete ecosystem for controlling a device (like a rover or a drone) while receiving real-time environmental feedback directly in your hands. The system is split into two main parts:

1.  **The Brain (`Netrana-ESP`)**: A powerful **DFRobot FireBeetle 2 ESP32-S3** microcontroller that serves as the central processing unit. It runs a web server for video streaming, connects to Wi-Fi, and listens for commands from the remote.
2.  **The Remote (`Netrana-Nano`)**: A custom-built handheld controller based on an Arduino Nano. This remote has navigation buttons, reads data from ultrasonic sensors, and most importantly, features a **vibration motor for haptic feedback**, alerting the user to obstacles detected by the sensors.

This project is an excellent platform for experimenting with advanced robotics, human-computer interaction, and applied AIoT concepts.

## System Architecture

The data flows through the system as follows, creating a closed-loop control and feedback mechanism.

```
+------+   Buttons & Haptic Feedback   +-------------------------+
| User | <---------------------------> |   Netrana-Nano Remote   |
+------+                               | (Arduino, Sensors, Vibe)|
                                       +-------------------------+
                                                    |
                                                    | Serial Communication
                                                    | (CSV Data Format)
                                                    v
+--------------------------------------------------------------------------+
|                               Netrana-ESP Brain                          |
| (DFRobot FireBeetle 2 ESP32-S3, Web Server, Camera, Wi-Fi, Temp/Humidity) |
+--------------------------------------------------------------------------+
      |
      | Control Signals (PWM, etc.)
      v
+------------------+
| Robot / Vehicle  |
| (Motors, Servos) |
+------------------+
```

## Key Features

- **Haptic Feedback**: Feel obstacles before you see them through vibrations in the remote.
- **Real-time Video Streaming**: On-board camera on the ESP32 streams video to a web interface.
- **Dual-Sensor Input**: The remote is equipped with front-facing and bottom-facing ultrasonic sensors for robust obstacle detection.
- **Modular Design**: The brain and remote are separate modules, communicating via a simple serial protocol, allowing for easy upgrades and maintenance.
- **Web-Based Control Interface**: The ESP32 hosts a web page for video and potentially for control and data visualization.
- **Multi-Sensor Platform**: Includes temperature/humidity sensing on the main board.
- **JSON Data Handling**: Ready for integration with web services or MQTT brokers thanks to the ArduinoJson library.

## Hardware & Software

### Hardware Components

- **Main Brain**:
  - DFRobot FireBeetle 2 ESP32-S3
  - A compatible camera module
  - DFRobot SHT3x Temperature & Humidity Sensor
- **Haptic Remote**:
  - Arduino Nano (or compatible)
  - 2x HC-SR04 Ultrasonic Distance Sensors
  - 5x Push Buttons (for UP, DOWN, LEFT, RIGHT, OK/EMERGENCY)
  - 1x Vibration Motor
  - Pull-up/Pull-down resistors as needed
- **General**:
  - Jumper wires
  - Power source (e.g., LiPo battery)

### Software Firmware

1.  **`Netrana-ESP`**:

    - **IDE**: PlatformIO
    - **Framework**: Arduino
    - **Board**: `dfrobot_firebeetle2_esp32s3`
    - **Key Libraries**: `DFRobot_SHT3x`, `ArduinoJson`
    - **Functionality**: Runs a web server, processes serial data from the remote, streams video, and reads the local SHT3x sensor.

2.  **`Netrana-Nano`**:
    - **IDE**: Arduino IDE or PlatformIO
    - **Framework**: Arduino
    - **Board**: Arduino Nano
    - **Functionality**: Reads button presses and ultrasonic sensors, controls the vibration motor, and sends formatted data over the serial port.

---

## Getting Started

### Prerequisites

- [Visual Studio Code](https://code.visualstudio.com/) with the [PlatformIO IDE Extension](https://platformio.org/install/ide?install=vscode)
- [Arduino IDE](https://www.arduino.cc/en/software) (optional, for the Nano)
- Basic knowledge of electronics and soldering.

### Hardware Setup

1.  **Assemble the Remote**: Connect the buttons, ultrasonic sensors, and vibration motor to the Arduino Nano as defined by the pins in `Netrana-Nano/Netrana_sensor/Netrana_sensor.ino`.
2.  **Connect Remote to Brain**: Connect the `TX` pin of the Arduino Nano to the `RX` pin of the ESP32-S3, and the `RX` pin of the Nano to the `TX` of the ESP32-S3. **Ensure a common ground (GND)** between the two boards.

### Firmware Installation

#### 1. Netrana-ESP (The Brain)

1.  Open the `Netrana-AIoT` project folder in Visual Studio Code.
2.  PlatformIO should automatically recognize the `Netrana-ESP` project.
3.  Navigate to the `Netrana-ESP/platformio.ini` file. You may need to configure Wi-Fi credentials or other settings here.
4.  Connect the DFRobot FireBeetle 2 ESP32-S3 board to your computer.
5.  Use the PlatformIO controls to **Build** and then **Upload** the firmware.

#### 2. Netrana-Nano (The Remote)

1.  Open the `Netrana-Nano/Netrana_sensor/Netrana_sensor.ino` sketch in the Arduino IDE.
2.  Select "Arduino Nano" as the board, the correct processor, and the COM port.
3.  Click **Upload** to flash the firmware to the remote.

## How It Works

1.  The **Nano Remote** constantly scans for button presses and measures distances using its two ultrasonic sensors.
2.  If a button is pressed, it immediately sends a message like `"BTN, <button_name>, <front_dist>, <bottom_dist>"` over serial.
3.  If no button is pressed for a second, it sends a heartbeat message: `"NONE, <front_dist>, <bottom_dist>"`.
4.  If the front sensor detects an object closer than `150 cm`, it activates the **vibration motor** to provide haptic feedback. The vibration is stronger if the "EMERGENCY" button is pressed.
5.  The **ESP32 Brain** listens for these serial messages, parses them, and can then translate them into actions (e.g., moving a rover).
6.  Simultaneously, the ESP32 runs a web server, streaming video from its camera and potentially displaying sensor data received from the remote.

## Roadmap

- [ ] **Implement Rover Control**: Write the logic on the ESP32 to control motors based on commands from the remote.
- [ ] **Enhance Web UI**: Improve the web interface to display sensor data (from both remote and brain) alongside the video feed.
- [ ] **MQTT Integration**: Use the `ArduinoJson` library to send structured data (sensor readings, commands) to an MQTT broker for cloud-based logging and control.
- [ ] **AI-Powered Object Detection**: Leverage the ESP32-S3's capabilities to run a small AI model on the camera feed for object detection or lane following.
- [ ] **Power Management**: Implement battery monitoring and low-power modes.

---

## Contributing

Contributions are what make the open-source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

1.  Fork the Project
2.  Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3.  Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4.  Push to the Branch (`git push origin feature/AmazingFeature`)
5.  Open a Pull Request

## License

Distributed under the MIT License. See `LICENSE` file for more information.
