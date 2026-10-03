# esp32-submersible-pump-automation
Dual-node ESP32 automation for a 1HP KSB submersible pump using AJ-SR04M ultrasonic sensing, ESP-NOW wireless telemetry, and Blynk IoT cloud monitoring.

# Smart Water Tank & Submersible Pump Automation

![Platform](https://img.shields.io/badge/Platform-ESP32-blue)
![Framework](https://img.shields.io/badge/Framework-Arduino%20C%2B%2B-00979D)
![Protocol](https://img.shields.io/badge/Protocol-ESP--NOW%20%7C%20Blynk%20IoT-orange)
![License](https://img.shields.io/badge/License-MIT-green)

An industrial-grade, dual-node IoT fluid management system that automates a 1HP KSB submersible water pump using ultrasonic telemetry, fault-tolerant ESP-NOW local radio, and Blynk 2.0 cloud monitoring.

📸 **[View Full Project Build & Wiring Gallery](https://photos.google.com/album/AF1QipMFQYj98AnXiH7HmgCI8IgpLRfNTb7Vkz9QgQO4)**  
📄 **Formal Specification:** A compiled PDF manual and LaTeX source are available in [`/docs`](./docs).

---

## System Architecture

```text
[ ROOF NODE ]
ESP32 + AJ-SR04M Sensor
  │
  │ (ESP-NOW 2.4GHz - Local RF via Channel 1/8)
  ▼
[ PUMP NODE ]
ESP32 Master Controller
  ├── Dual-Channel Optocoupled Relay Module
  │     ├── Relay 1 (GPIO 19) ──> [KSB Starter: Green Button (Parallel)]
  │     └── Relay 2 (GPIO 18) ──> [KSB Starter: Red Button (Series)]
  └── Wi-Fi (2.4GHz) ───────────> [Blynk 2.0 Cloud Dashboard]
