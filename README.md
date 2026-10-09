# Motion-Triggered Privacy Alert (MakeNJIT Hackathon 2025)

Team Cherry Blossom's hackathon project: two ESP32 boards that talk peer to peer over **ESP-NOW**. When a motion sensor near the door detects someone coming, your PC gets an instant heads-up and switches to a "privacy" screen.

## How it works
```
[PIR sensor] → ESP32 Master ──ESP-NOW broadcast──▶ ESP32 Slave ──USB serial──▶ Node.js listener ──▶ PC action
```
1. **Master** (`ESP_NOW_Broadcast_Master/`) reads a PIR motion sensor and broadcasts a message over ESP-NOW when motion is detected. An LED shows the sensor state.
2. **Slave** (`ESP_NOW_Broadcast_Slave/`) sits by the PC, receives the broadcast, and prints `[MOTION]` over serial.
3. **PC listener** (`PC_Command/PC_Command.js`) watches the serial port and, on `[MOTION]`:
   - shows a desktop notification
   - runs `PC_Command.ahk`, which mutes the volume and opens a "look busy" page (course notes or a lecture video)

ESP-NOW needs no Wi-Fi router or pairing, so the link is fast and works anywhere.

## Hardware
- 2× ESP32 dev boards
- PIR motion sensor (GPIO 4 on the master)
- LED (GPIO 17 on the master)

## Setup
**Firmware:** flash each sketch with the Arduino IDE and the ESP32 board package.

**PC side (Windows):**
```bash
cd PC_Command
npm install
node PC_Command.js
```
Before running, edit `COM_PORT` in `PC_Command.js` to match the slave board, and update the file paths in `PC_Command.js` and `PC_Command.ahk`. [AutoHotkey v1](https://www.autohotkey.com/) must be installed.

## Tech
ESP32 · ESP-NOW · Arduino C++ · Node.js (serialport, node-notifier) · AutoHotkey
