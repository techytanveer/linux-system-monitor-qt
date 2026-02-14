# Linux System Monitor (Qt6) CPP CI
![Build Status]([![C++ Qt6 CI](https://github.com/techytanveer/linux-system-monitor-qt/actions/workflows/ci.yml/badge.svg)](https://github.com/techytanveer/linux-system-monitor-qt/actions/workflows/ci.yml))

This project uses GitHub Actions to build and test C++ code on Ubuntu 24.04.

1. The GitHub README.md
This is designed to be professional, scannable, and include all the technical details a recruiter or collaborator would look for.

A lightweight, real-time system monitoring utility built with C++ and Qt6. This tool parses the Linux `/proc` filesystem to provide high-fidelity data on CPU usage, RAM consumption, Network throughput, and process-level resource allocation.

## 🚀 Features
* **Live CPU Tracking:** Real-time percentage calculation via `/proc/stat`.
* **Memory Dashboard:** Visualizes used vs. available system RAM.
* **Network Monitor:** Tracks delta speeds for Upload/Download interfaces.
* **Top 5 Processes:** Dynamically identifies and lists the 5 most memory-intensive processes.
* **Dark Mode UI:** Optimized for high-contrast readability with white-on-dark styling.

## 🛠 Prerequisites
To build this project from source, you will need:
* **Compiler:** GCC/G++ (version 9+)
* **Build System:** CMake (3.16+)
* **Framework:** Qt6 (Core, Widgets, Gui)

## 📂 Project Structure
```
SystemMonitor/
├── CMakeLists.txt
├── include/
│   ├── SystemInfo.h       # Logic for data retrieval
│   └── MainWindow.h       # UI definitions
└── src/
    ├── main.cpp
    ├── SystemInfo.cpp
    └── MainWindow.cpp```
```

## 📦 Installation & Build

### 1. Clone the repository
```
git clone [https://github.com/yourusername/SystemMonitor.git](https://github.com/yourusername/SystemMonitor.git)
cd SystemMonitor
```
### 2. Build via CMake
```
mkdir build && cd build
cmake ..
make
```
### 3. Run

---
