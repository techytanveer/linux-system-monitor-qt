cmake_minimum_required(VERSION 3.16)
project(SystemMonitor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt's automatic processing
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Enable CTest
enable_testing()

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(SystemMonitor
    src/main.cpp
    src/MainWindow.cpp
    src/SystemInfo.cpp
    src/AnomalyDetector.cpp
    include/MainWindow.h
    include/SystemInfo.h
    include/AnomalyDetector.h
)

# Create a test executable
add_executable(detector_test 
    tests/test_anomaly.cpp 
    include/SystemInfo.h
    src/AnomalyDetector.cpp # We link the source file directly
)

target_include_directories(SystemMonitor PRIVATE include)
target_link_libraries(SystemMonitor PRIVATE Qt6::Widgets)

# Link Qt (since AnomalyDetector uses QString and QList)
target_link_libraries(detector_test Qt6::Core)

# Register the test
add_test(NAME AnomalyLogicTest COMMAND detector_test)

# ----------- Installation Rules --------------
# This tells CMake where to put the binary when "installed"
install(TARGETS SystemMonitor DESTINATION bin)

# Install the .desktop file
install(FILES system-monitor-qt.desktop DESTINATION share/applications)  

# 3. (Optional) Install an icon if you have a .png file
install(FILES resources/SMicon.png DESTINATION share/icons/hicolor/64x64/apps)

# --- Packaging Logic (CPack) ---
set(CPACK_PACKAGE_NAME "system-monitor-qt")
set(CPACK_PACKAGE_VENDOR "Tanveer")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A simple Qt-based Linux System Monitor")
set(CPACK_PACKAGE_CONTACT "tanvahme@gmail.com")

# Specifics for Debian (.deb)
set(CPACK_GENERATOR "DEB;RPM")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Tanveer")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")

# This automatically detects Qt6 dependencies so the user doesn't have to install them manually
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)
