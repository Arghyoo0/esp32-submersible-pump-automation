# Smart Water Tank & Submersible Pump Automation

![Platform](https://img.shields.io/badge/Platform-ESP32-blue?logo=espressif)
![Framework](https://img.shields.io/badge/Framework-Arduino%20C%2B%2B-00979D?logo=arduino)
![Protocol](https://img.shields.io/badge/Protocol-ESP--NOW%20%7C%20Blynk%202.0-FF6F00)
![License](https://img.shields.io/badge/License-MIT-green)

A dual-node, industrial-grade IoT automated fluid management system designed for high-power (1HP) residential submersible pumps. This project replaces manual pump operation with millimetre-accurate ultrasonic telemetry, point-to-point radio fault tolerance, wave-debounced relay pulse control, and cloud-synced remote monitoring.

> **Formal Documentation Redundancy:** A full engineering specification sheet is also compiled as a standalone document. You can download or review it directly in this repository:
> - **[Download Project Documentation PDF (docs/PROJECT_DOCUMENTATION.pdf)](./docs/PROJECT_DOCUMENTATION.pdf)**
> - **[LaTeX Source File (docs/PROJECT_DOCUMENTATION.tex)](./docs/PROJECT_DOCUMENTATION.tex)**

---

## 1. Executive Summary

Operating high-power induction submersible pumps manually presents multiple failure modes: tanks overflowing, running the motor dry, or destroying high-capacitance starting components by holding the starter button down too long. 

This project bridges a heavy-duty single-phase KSB pump starter panel with modern dual-node IoT logic:
- **Telemetry Layer:** Measures fluid levels using a high-frequency waterproof ultrasonic transceiver positioned on the roof tank.
- **RF Transmission Layer:** Transmits distance and percentage data locally across concrete slabs using 2.4GHz ESP-NOW point-to-point wireless without requiring an active internet connection.
- **Contactor Actuation Layer:** Replicates physical human interaction with the starter panel using opto-isolated magnetic relays firing millisecond-timed pulses.
- **Cloud Dashboard:** Streams live water gauges, reservoir health metrics, and emergency override switches to the Blynk 2.0 mobile application.

---

## 2. System Architecture

```text
       ┌────────────────────────────────────────────────────────┐
       │                   ROOF SENSOR NODE                     │
       │  [AJ-SR04M Ultrasonic] ──> [ESP32 Telemetry Unit]      │
       └───────────────────────────┬────────────────────────────┘
                                   │
                                   │ ESP-NOW (2.4GHz Point-to-Point)
                                   │ Auto-Channel Scan (Fallback: Ch 1 / 8)
                                   ▼
       ┌────────────────────────────────────────────────────────┐
       │                   PUMP MASTER CONTROLLER               │
       │                   [ESP32 Logic Engine]                 │
       └──────────────┬──────────────────────────┬──────────────┘
                      │                          │
        Wi-Fi (Blynk 2.0 IoT Cloud)       GPIO 19 & 18 Logic Pulses
                      │                          │
                      ▼                          ▼
       ┌───────────────────────┐  ┌─────────────────────────────┐
       │  BLYNK MOBILE APP     │  │  2-CHANNEL RELAY MODULE     │
       │  - Live Water Gauge   │  │  - Relay 1: 3000ms Pulse    │
       │  - Manual Override    │  │  - Relay 2: 1000ms Pulse    │
       │  - Tank Fill Trends   │  └──────────────┬──────────────┘
       └───────────────────────┘                 │
                                                 ▼
                                  ┌─────────────────────────────┐
                                  │   KSB STARTER PANEL (220V)  │
                                  │   - Parallel to Green (NO)  │
                                  │   - Series with Red (NC)    │
                                  └──────────────┬──────────────┘
                                                 │
                                                 ▼
                                  ┌─────────────────────────────┐
                                  │   1HP SUBMERSIBLE PUMP      │
                                  └─────────────────────────────┘
