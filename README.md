# Smart Water Tank & Submersible Pump Automation

![Platform](https://img.shields.io/badge/Platform-ESP32-blue?logo=espressif)
![Framework](https://img.shields.io/badge/Framework-Arduino%20C%2B%2B-00979D?logo=arduino)
![Architecture](https://img.shields.io/badge/Architecture-Dual--Redundant%20Hybrid-purple)
![Protocol](https://img.shields.io/badge/Protocol-ESP--NOW%20%7C%20Blynk%202.0-FF6F00)
![License](https://img.shields.io/badge/License-MIT-green)

An industrial-grade IoT automated fluid management system engineered for high-power (1HP) residential KSB submersible pumps. It features a **zero-downtime, dual-network redundant architecture**: real-time local machine-to-machine telemetry over point-to-point **ESP-NOW radio**, coupled with **Blynk 2.0 Cloud** for global mobile monitoring and remote override.

> **Formal Documentation:** A complete PDF manual and LaTeX specification are included in this repository:
> - **[Download Documentation PDF (docs/PROJECT_DOCUMENTATION.pdf)](./docs/PROJECT_DOCUMENTATION.pdf)**
> - **[LaTeX Source File (docs/PROJECT_DOCUMENTATION.tex)](./docs/PROJECT_DOCUMENTATION.tex)**

---

## 📑 Table of Contents

- [Core Innovation: Dual-Network Redundancy Engine](#-core-innovation-dual-network-redundancy-engine)
- [Operational Modes Under Network Conditions](#-operational-modes-under-network-conditions)
- [Dynamic Channel Synchronization & Fallback Logic](#-dynamic-channel-synchronization--fallback-logic)
- [System Architecture](#-system-architecture)
- [Hardware Specifications & Bill of Materials](#-hardware-specifications--bill-of-materials)
- [Electrical Wiring & Starter Box Integration](#-electrical-wiring--starter-box-integration)
- [Build & Hardware Gallery](#-build--hardware-gallery)
- [Software Architecture & Industrial Logic](#-software-architecture--industrial-logic)
- [Emergency Operating Procedure](#-emergency-operating-procedure-eop)

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
```

---

## 🌐 Operational Modes Under Network Conditions

| System State | Local ESP-NOW Link | Wi-Fi / Router Status | Automation Status | Mobile App Visibility |
| :--- | :--- | :--- | :--- | :--- |
| **Normal Mode** | Active (Channel Synced) | Connected to Home SSID | 100% Fully Automated | Live gauges, full remote control |
| **Internet Outage** | Active (Channel Synced) | Connected to LAN, No WAN | 100% Fully Automated | App offline; local logic intact |
| **Router Crash / Blackout** | Active (Channel 8 Fallback) | Disconnected / Router Dead | 100% Fully Automated | App offline; physical pump runs safe |

---

## 📡 Dynamic Channel Synchronization & Fallback Logic

- **Dynamic Frequency Locking:** The Roof Node scans the 2.4GHz spectrum on boot (`getWiFiChannel(ssid)`) to detect the current operating channel of the home router.
- **Channel Aligning:** Both nodes lock their ESP-NOW radio channels to match the router's frequency without dropping Wi-Fi packets.
- **Hardcoded Channel 8 Fallback:** If the home router completely loses power or resets, the Roof Node automatically switches to Channel 8. The Pump Node initializes on Channel 8 by default, ensuring telemetry packets continue uninterrupted across floors without missing a single pulse.

---

## 🏗️ System Architecture

```text
       ┌────────────────────────────────────────────────────────┐
       │                   ROOF SENSOR NODE                     │
       │  [AJ-SR04M Sensor] ──> [ESP32 Telemetry Unit]          │
       └───────────────────────────┬────────────────────────────┘
                                   │
                                   │ ESP-NOW (2.4GHz Point-to-Point)
                                   │ Dynamic Scan / Fallback Channel 8
                                   ▼
       ┌────────────────────────────────────────────────────────┐
       │                   PUMP MASTER CONTROLLER               │
       │                   [ESP32 Logic Engine]                 │
       └──────────────┬──────────────────────────┬──────────────┘
                      │                          │
        Wi-Fi (Blynk 2.0 Cloud)            GPIO 19 & 18 Logic Pulses
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
```

### Node Responsibilities

- **Roof Node:** Measures water distance using an AJ-SR04M sensor every 2 seconds, maps it into tank volume percentage, scans for channel parity, and transmits the payload over ESP-NOW.
- **Pump Node:** Intercepts incoming telemetry packets, verifies triple-reading depth confidence thresholds, executes capacitor-safe relay pulses, and pipes status to Blynk Cloud.
- **Blynk Cloud:** Hosts live gauges, historical metrics, and non-blocking manual start/stop override buttons.

---

## 🧰 Hardware Specifications & Bill of Materials

| Component | Qty | Model / Rating | Role in System |
| :--- | :---: | :--- | :--- |
| Microcontrollers | 2 | ESP32 NodeMCU (Dual-Core 240MHz) | Roof Telemetry Node & Pump Controller Node |
| Telemetry Sensor | 1 | AJ-SR04M Waterproof Ultrasonic | Acoustic distance measurement inside sealed tank |
| Relay Interface | 1 | 5V 2-Channel Optocoupled Module | Opto-isolated dry contacts for KSB contactor triggering |
| Power Supplies | 2 | Itel L25ZIIE (5V DC, 500mA) | Isolated linear power blocks for microcontrollers |
| Enclosures | 2 | IP65 PVC Weatherproof Boxes (8x6") | Dielectric, dust, and moisture isolation |
| Power Cabling | ~15m | 1.0 sq mm 2-Core Outdoor Copper | High-voltage line feeding control units |
| Emergency Switch | 1 | 16A Single-Pole Wall Switch | Immediate system-wide circuit isolation |
| Adhesive / Sealants | N/A | M-Seal Epoxy & 3M VHB Tape | Structural bonding, IP65 cable entry potting |

> 💰 **Cost Efficiency:** Total project build cost was approximately **₹1,500 – ₹2,000 INR**, delivering a **~75% cost reduction** compared to commercial pump automation controllers.

---

## ⚡ Electrical Wiring & Starter Box Integration

### Pump Node (ESP32 → Dual Relay Module)

| ESP32 Pin | Relay Module Pin | Wire Color | Operational Description |
| :--- | :--- | :--- | :--- |
| `GPIO 19` | `IN1` (Relay G) | Blue | Fires Green **START** button simulation (3000 ms pulse) |
| `GPIO 18` | `IN2` (Relay R) | White | Fires Red **STOP** button simulation (1000 ms pulse) |
| `VIN (5V)` | `VCC` | Green | Provides 5V excitation current for relay coils |
| `GND` | `GND` | Brown | Common logic reference ground |

### KSB Starter Panel Interfacing

The KSB starter utilizes a contactor with a continuous running capacitor and a high-surge starting capacitor (120/150 MFD).

- **Green Button Emulation (Relay G):** The relay output terminals (`COM` and `NO`) are wired **in parallel** across the manual Green Start Button terminals. Closing this relay for exactly 3 seconds energizes the contactor coil and pulls in the starting capacitor.
- **Red Button Emulation (Relay R):** The relay output terminals (`COM` and `NC`) are wired **in series** with the manual Red Stop Button circuit. Opening this relay for 1 second breaks the magnetic holding loop of the contactor, dropping motor power cleanly.

```text
[KSB Starter - Green Button] ──┬──────────────┐
                               │              │
                               └──[Relay G: NO]──[Relay G: COM]
                                  (Parallel Connection)

[KSB Starter - Red Button] ───[Relay R: COM]──[Relay R: NC]─── (Series Line)
```

### Roof Node (ESP32 → AJ-SR04M Sensor)

| ESP32 Pin | AJ-SR04M Board Pin | Functional Purpose |
| :--- | :--- | :--- |
| `5V / VIN` | `5V / VCC` | Operating power (30mA nominal) |
| `GND` | `GND` | Common ground reference |
| `GPIO 27` | `TRIG` | Generates ultrasonic trigger bursts |
| `GPIO 26` | `ECHO` | High-accuracy return pulse capture |

> ⚠️ **Safety Warning:** This project interfaces with 220V AC mains. Work only with power isolated, keep high-voltage and low-voltage zones physically separated, and have the installation checked by a qualified electrician if you are unsure.

---

## 📸 Build & Hardware Gallery

### Contactor & Starter Panel Wiring

![KSB Starter Panel Wiring](docs/images/enclosure_isolation.jpg)

*Figure 1: High-Voltage Contactor & Relay Module Interfacing.*
Integration of the 2-channel opto-isolated relay board with the 220V KSB starter panel. Relay G (top, blue/white wiring) is connected across the Normally Open (NO) and Common (COM) terminals in parallel with the physical Green Start button. Relay R (bottom, red wiring) is connected across the Normally Closed (NC) and Common (COM) terminals in series with the physical Red Stop button. Wires are stripped to 6mm to prevent plastic insulation clamping inside the blue terminal blocks, with zero exposed live copper strands to eliminate 220V AC flashover hazards.

### Enclosure Layout & High/Low Voltage Isolation

![Enclosure Internal Isolation](docs/images/enclosure_isolation.jpg)

*Figure 2: Internal Enclosure Zoning & Dielectric Isolation.*
Internal layout of the IP65 junction box demonstrating strict safety segregation between high-voltage (220V AC) mains lines and low-voltage (3.3V/5V DC) logic. The left zone houses incoming AC mains and relay output screw terminals, while the right zone isolates the ESP32 microcontroller and USB step-down converter. A rigid, non-conductive PVC divider barrier is bonded between the sections to physically block dislodged wires from contacting logic traces. Boards are raised on 2mm 3M VHB high-density acrylic foam tape to prevent sharp through-hole solder pins from piercing the casing.

### Roof Node & Waterproof Transducer Mounting

![AJ-SR04M Sensor Lid Mount](docs/images/sensor_lid_mount.jpg)

*Figure 3: AJ-SR04M Transducer Mounting & Environmental Potting.*
Weatherproof ultrasonic sensor installation on the concrete overhead tank lid. The AJ-SR04M probe is mounted perpendicularly to the water plane and permanently sealed using fast-curing M-Seal epoxy putty. The epoxy perimeter provides a gas-tight, waterproof seal that shields sensitive electronics against severe heat, rainwater ingress, and persistent internal humidity/condensation. The 4-core signal bundle is twisted to reject RF noise and features a downward gravity drip loop before entering the enclosure.

### Field Telemetry & RF Penetration Test

![Serial Monitor Field Test](docs/images/serial_monitor_testing.jpg)

*Figure 4: Ground-Floor ESP-NOW RF Penetration & Logic Validation.*
Real-time field diagnostic session over a 115200 baud serial monitor from the ground-floor command position. Telemetry verifies 2.4GHz ESP-NOW packets successfully penetrating two reinforced concrete floor slabs and navigating past the dense water tank. The log captures the "Software Armor" wave debounce filter in action: sequential level updates (14% → 21% → 23% → 34% → 81% → 98%) requiring 3 consecutive confirmations before executing a clean 1-second pulse on GPIO 18 to disengage the KSB motor starter cleanly.

---

## 🧠 Software Architecture & Industrial Logic

### 1. The 3-Second Latching Rule

Submersible motors cannot sustain prolonged contact with starting capacitors without overheating the capacitor bank. The Pump Node firmware manages pulse durations strictly:

- **Starting:** Energizes `RELAY_START` (GPIO 19) for **3000 ms**, latching the internal KSB contactor coil, then immediately releases the line.
- **Stopping:** Energizes `RELAY_STOP` (GPIO 18) for **1000 ms**, breaking the contactor's holding circuit.

### 2. The "Wave Filter" (Debounce Armor)

Reservoirs experience continuous water sloshing, surface ripples, and wind distortion during filling. A single outlier reading must never trigger the pump.

- Requires **three consecutive** confirmed readings **below 20%** before initiating motor activation (`lowConfidenceCounter >= 3`).
- Requires **three consecutive** confirmed readings **above 80%** before initiating motor shutdown (`highConfidenceCounter >= 3`).
- Any reading within the safe mid-band (**21% to 79%**) immediately resets both counters to zero.

### 3. Sensor Calibration Limits

| Measured Distance | Tank Level |
| :---: | :--- |
| **85 cm** | 0% (Tank Empty) |
| **30 cm** | 100% (Tank Full / Safe Top Margin) |

---

## 🚨 Emergency Operating Procedure (EOP)

If a component fails, Wi-Fi drops, or maintenance is required, the system can be completely bypassed back to 100% manual control:

1. **Cut Smart Grid Power:** Flip the 6A Master Switch on the 1st-floor bathroom switchboard to the **OFF** position.
2. **Relay Failsafe:** Cutting power de-energizes all relays to their unpowered default states (Relay G open, Relay R closed via NC contact).
3. **Manual Operation:** Walk to the KSB starter panel.
   - Press the physical **Green Button** for 3 seconds to start the pump.
   - Press the physical **Red Button** to stop the pump.

Because the relays are integrated into the control line rather than switching the pump's heavy motor current directly, the physical starter panel remains fully functional even during a complete IoT system outage.

---

## 📄 License

This project is licensed under the [MIT License](./LICENSE).
