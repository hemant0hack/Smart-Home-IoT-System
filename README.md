# Smart Home IoT System

An ESP32-based smart home automation system using MQTT and ThingsBoard for real-time control, monitoring, and dashboard integration.

## 🚀 Overview

This project is a complete smart home solution built with ESP32 microcontroller, integrating:
- IR remote control
- MQTT communication via ThingsBoard
- Scheduled and automated relay control
- Motion detection and gas monitoring
- Real-time dashboard updates

## 🧠 Features

- ✅ **8-Relay Control** via IR remote and ThingsBoard RPC
- 📊 **Dashboard Integration** for live control and monitoring
- 🌡️ **DHT22** for temperature & humidity sensing
- 🔥 **MQ2 Sensor** for gas level monitoring (analog + digital)
- 🚶‍♂️ **PIR Sensor** automation for motion-based relay control
- 🕒 **Scheduled Relays** (e.g., auto ON/OFF based on time)
- 🔁 **IR and ThingsBoard Sync** to reflect real-time state across systems

## ⚙️ Technologies Used

- **ESP32** (Wi-Fi-enabled microcontroller)
- **MQTT** (Message Queuing Telemetry Transport)
- **ThingsBoard** (Open-source IoT platform)
- **Arduino IDE** for firmware development
- **C++** for ESP32 programming

## 🔌 Hardware Components

| Component      | Quantity |
|----------------|----------|
| ESP32 Dev Board | 1 |
| 8-Channel Relay Module | 1 |
| IR Receiver (e.g., TSOP38238) | 1 |
| DHT22 Temperature Sensor | 1 |
| MQ2 Gas Sensor | 1 |
| PIR Motion Sensor | 1 |
| Resistors, Jumper Wires, Breadboard | As needed |
| 5V Power Supply | 1 |

## 🧾 System Architecture

