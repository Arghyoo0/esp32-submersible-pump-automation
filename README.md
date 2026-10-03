# Smart Water Tank & Submersible Pump Automation

![Platform](https://img.shields.io/badge/Platform-ESP32-blue?logo=espressif)
![Framework](https://img.shields.io/badge/Framework-Arduino%20C%2B%2B-00979D?logo=arduino)
![Architecture](https://img.shields.io/badge/Architecture-Dual--Redundant%20Hybrid-purple)
![Protocol](https://img.shields.io/badge/Protocol-ESP--NOW%20%7C%20Blynk%202.0-FF6F00)
![License](https://img.shields.io/badge/License-MIT-green)

An industrial-grade IoT automated fluid management system engineered for high-power (1HP) residential KSB submersible pumps. It features a **zero-downtime, dual-network redundant architecture**: real-time local machine-to-machine telemetry over point-to-point **ESP-NOW radio**, coupled with **Blynk 2.0 Cloud** for global mobile monitoring and remote override[cite: 1, 3, 4].

> **Formal Documentation:** A complete PDF manual and LaTeX specification are included in this repository:
> - **[Download Documentation PDF (docs/PROJECT_DOCUMENTATION.pdf)](./docs/PROJECT_DOCUMENTATION.pdf)**
> - **[LaTeX Source File (docs/PROJECT_DOCUMENTATION.tex)](./docs/PROJECT_DOCUMENTATION.tex)**

---

## 🌟 Core Innovation: Dual-Network Redundancy Engine

Most DIY home automation systems fail when the internet or home router disconnects, stranding the pump in an unknown state. This system separates **local control execution** from **cloud telemetry reporting** to guarantee zero downtime:

```text
                     ┌───────────────────────────────────────────────┐
                     │            DUAL-LAYER NETWORK TOPOLOGY        │
                     └───────────────────────────────────────────────┘
                                             │
                   ┌─────────────────────────┴─────────────────────────┐
                   ▼                                                   ▼
     [LOCAL TIER: ESP-NOW (2.4GHz)]                      [CLOUD TIER: Wi-Fi 802.11 b/g/n]
   - Fully decentralized point-to-point                - Internet-dependent reporting
   - Zero dependence on router / DHCP                  - Real-time Blynk 2.0 mobile dashboards
   - Latency: < 5 ms                                   - Historical charts & remote overrides
   - Self-healing RF Channel 8 Fallback                - Non-blocking async sync
                   │                                                   │
                   ▼                                                   ▼
       ┌────────────────────────┐                         ┌────────────────────────┐
       │   AUTONOMOUS CONTROL   │                         │  TELEMETRY & VISIBILITY│
       │ Pump automates safely  │                         │ Mobile alerts, manual  │
       │ even if router dies!   │                         │ override from anywhere │
       └────────────────────────┘                         └────────────────────────┘
