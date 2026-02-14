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

```SystemMonitor/
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
```bash
git clone [https://github.com/yourusername/SystemMonitor.git](https://github.com/yourusername/SystemMonitor.git)
cd SystemMonitor
2. Build via CMake
Bash
mkdir build && cd build
cmake ..
make
3. Run
Bash
./SystemMonitor
📂 Project Structure
/include - Header files (SystemInfo.h, MainWindow.h)

/src - Logic implementation and UI handling

/resources - Icons and stylesheets


---

## 2. User Guide: Distribution & Installation

Since you are targeting Linux, distributing as `.deb` (Debian/Ubuntu) or `.rpm` (Fedora/RHEL) is the standard way to handle dependencies.

### How to Create a `.deb` Package (Ubuntu/Debian)
The easiest way to do this is using the `CPack` tool, which is integrated with CMake.

1. **Add to your `CMakeLists.txt`:**
```cmake
set(CPACK_PACKAGE_NAME "system-monitor")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_CONTACT "Your Name <you@example.com>")
set(CPACK_GENERATOR "DEB")
include(CPack)
Generate the Package:

Bash
cd build
cmake ..
make package
Install it:

Bash
sudo dpkg -i system-monitor-1.0.0.deb
How to Create an .rpm Package (Fedora/CentOS)
The process is nearly identical, thanks to CMake's abstraction.

Change the Generator: In your CMakeLists.txt or via terminal, set the generator to RPM:

Bash
cpack -G RPM
Install it:

Bash
sudo rpm -ivh system-monitor-1.0.0.rpm
3. Usage Instructions for the App
Viewing Data
CPU/RAM: The top gauges update every 1000ms (1 second).

Process Table: The list automatically sorts itself. The "heavy hitters" (top 5) will shift position in real-time if their memory usage changes.

Troubleshooting
Empty Table: Ensure your user has read permissions for /proc. (Standard users usually do, but some hardened kernels may restrict access).

Network Speed at 0: If you have multiple interfaces (Wi-Fi and Ethernet), the utility currently tracks the primary active interface found in /proc/net/dev.
