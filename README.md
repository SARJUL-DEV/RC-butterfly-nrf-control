# 🦋 IoT WiFi-to-RF Ornithopter Bridge

![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)
![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

> **A Novel Architecture for RC Avian Robotics using Dual-Phase Synced Servos over a Custom WiFi-to-nRF Gateway.**

## 🌟 Overview
This project introduces a completely new and innovative approach to controlling a robotic butterfly (ornithopter). Instead of traditional bulky RC transmitters, it utilizes a custom **"WiFi-to-RF Bridge"**. 

A smartphone connects to an ESP8266 acting as a local Web Server with a virtual joystick UI. This ESP8266 seamlessly translates the WiFi inputs into RF signals via an nRF24L01 module. The receiver, an ESP32, uses advanced master-synced trigonometric logic (Cosine waves) to drive two independent servos, creating a perfect, out-of-sync-proof 180° flapping motion.

### 💡 The Invention
This unique architecture—combining a smartphone Web-UI, a wireless bridging protocol, and a dynamic phase-synced motor algorithm—was conceptualized and developed by **Sarjul**. It eliminates the need for expensive physical joysticks and mechanical crankshafts, offering a fully digital, highly customizable flight control system.

## ✨ Key Features
* **Virtual Remote Control:** HTML/JS-based dual-joystick interface accessible from any mobile browser.
* **Bridge Technology:** ESP8266 acts as a bridge, extending the standard smartphone WiFi range by converting it to high-range nRF24L01 2.4GHz signals.
* **Master-Synced Flapping Math:** Custom global-phase algorithm ensures both servos never fall out of sync, regardless of speed variations.
* **Digital Steering:** Dynamic amplitude reduction for turning, mimicking real avian flight without breaking synchronization.
* **Lightweight Setup:** Completely removes heavy mechanical gears on the receiver side.

## 🛠️ Hardware Requirements

### Transmitter (The Bridge)
* 1x NodeMCU ESP8266
* 1x nRF24L01 Wireless Module

### Receiver (The Ornithopter)
* 1x ESP32 Development Board
* 1x nRF24L01 Wireless Module
* 2x 180° Servo Motors (e.g., SG90)
* Lightweight Battery (LiPo 3.7V / 7.4V depending on power setup)

## 🔌 Wiring & Connections

### Transmitter (ESP8266 + nRF24L01)
| nRF24L01 Pin | ESP8266 Pin | Notes |
| :--- | :--- | :--- |
| VCC | 3.3V | Strictly 3.3V (Use adapter if needed) |
| GND | GND | |
| CE | D2 (GPIO 4) | |
| CSN | D1 (GPIO 5) | |
| SCK | D5 (GPIO 14) | |
| MOSI | D7 (GPIO 13) | |
| MISO | D6 (GPIO 12) | |

### Receiver (ESP32 + nRF24L01 + Servos)
| Module Pin | ESP32 Pin | Notes |
| :--- | :--- | :--- |
| **nRF24L01** | | |
| VCC | 3.3V | |
| GND | GND | |
| CE | GPIO 4 | |
| CSN | GPIO 5 | |
| SCK | GPIO 18 | VSPI SCK |
| MOSI | GPIO 23 | VSPI MOSI |
| MISO | GPIO 19 | VSPI MISO |
| **Servos** | | |
| Left Servo Signal | GPIO 25 | |
| Right Servo Signal| GPIO 26 | |
| Servos VCC / GND | External 5V | Do not power from ESP32 pins directly |

## 🚀 Installation & Setup
1. Clone this repository to your local machine.
2. Open the Transmitter code and upload it to your ESP8266 via Arduino IDE.
3. Open the Receiver code and upload it to your ESP32. Ensure the `ESP32Servo` and `RF24` libraries are installed via the Library Manager.
4. Power up both devices. 
5. Connect your smartphone to the WiFi network **"ESP-REMOTE"** (Password: `12345678`).
6. Open your mobile browser and navigate to `http://192.168.4.1` to access the virtual remote and fly!

## 👨‍💻 Author & Credits
**Created and Invented by Sarjul**  
*A passion project pushing the boundaries of DIY RC, IoT integration, and embedded math logic.*

---
*Open Source Initiative: If you use, adapt, or get inspired by this architecture, please provide attribution to the original creator.*
