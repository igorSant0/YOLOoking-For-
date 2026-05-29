# 🎮 YOLOoking For? — An AI & IoT Interactive Guessing Game

An interactive, physical-digital guessing game that combines **Computer Vision (YOLO)**, **IoT (ESP32)**, and a **Web Interface** to bring the classic "Hot or Cold" game into the modern era.

---

## 📝 Project Overview

The dynamic of the game is simple yet engaging:

1. **The Setup:** A player selects a physical object and shows it to the camera. The system locks this object as the "hidden target".
2. **The Hunt:** The target object is hidden. Other players start showing random objects to the camera to guess what the target is.
3. **The Feedback:** Every time an incorrect object is presented, the system provides a textual hint on the web interface and triggers physical feedback (LEDs/sounds) via the ESP32. The loop continues until the correct object is found!

---

## 🏗️ System Architecture & Data Flow

To ensure high performance and low latency, the project uses a **centralized state-machine architecture** hosted on a PC. Since YOLO cannot run directly on microcontrollers, the ESP32 acts strictly as an I/O peripheral.

                         ┌──────────────┐
                         │  Smartphone  │ (IP Camera Stream by Wi-Fi)
                         └──────┬───────┘
                                │ MJPEG / RTSP
                                ▼
      ┌───────────┐     ┌──────────────┐     ┌───────────┐
      │  Web UI   │ ◄───┤  Central PC  ├────►│   ESP32   │
      └───────────┘     │  Python/YOLO │     └───────────┘
      WebSockets Hint   └──────────────┘     Hardware Alerts/
      & Scoreboard                           Motors/LEDs/Display

1. **Video Capture:** A smartphone acts as a wireless IP Camera, streaming live frames to the central server via local Wi-Fi.
2. **AI Processing:** The Central PC runs a **Python** backend equipped with **OpenCV** and the **YOLO** object detection model to infer what object is currently being shown.
3. **Game Logic & UI:** The Python server manages the game states and uses **WebSockets** to instantly push hints, countdowns, and game status to a responsive **Web Interface**.
4. **Physical Actuation:** The server sends lightweight commands to the **ESP32** (via HTTP or WebSockets) to trigger physical responses, such as blinking error/success LEDs, playing buzzer sounds, or activating servo motors to open a prize box.

---

## 🛠️ Tech Stack

- **Artificial Intelligence:** Python, YOLO (Ultralytics), OpenCV
- **Backend:** FastAPI / Flask (Python) with WebSockets
- **Frontend:** HTML5, CSS3, JavaScript (or React)
- **Hardware/IoT:** ESP32 (configured in Station Mode)
- **Connectivity:** Local Wi-Fi Network

---

## 🚀 Key Features

- **Real-time Object Detection:** Sub-second inference using lightweight YOLO Nano models.
- **Hardware-Software Integration:** Smooth communication loop bridging Web UIs and physical microcontrollers.
- **Smart Device Synergy:** Repurposes standard smartphones into high-quality IoT sensors without custom app deployment.
