# iot-smart-home

[📄 Full Report: Smart Home System.pdf](Smart%20Home%20System.pdf)

A smart home IoT project that integrates various devices for automation, monitoring, and remote control.

## Overview

This project consists of two main components:

1. **ESP32 Smart Home Main Controller**: Manages the central control of the smart home system, including Wi-Fi connectivity, HTTP server, and TCP server for communication with other devices.
2. **TempHumLightStation**: A peripheral device that monitors temperature, humidity, and light levels, and communicates with the main controller.

## Hardware and Features

This project integrates various hardware components and features to create a smart home system for automation, monitoring, and remote control. The system consists of two main components:

### ESP32 Smart Home Main Controller
- **Hardware**:
  - **ESP32 Development Board**: Acts as the central controller for the smart home system.
  - **Shelly Smart Plug S**: Used to control appliances such as heaters and dehumidifiers via HTTP commands.
- **Features**:
  - Wi-Fi initialization and event handling.
  - HTTP server for user interaction and setpoint updates.
  - TCP server for communication with peripheral devices.
  - Logging and debugging support using ESP32's logging library.
  - Sends HTTP commands to control appliances via Shelly Smart Plug S.

### TempHumLightStation
- **Hardware**:
  - **Arduino Uno**: Serves as the microcontroller for the peripheral station.
  - **ESP8266 Wi-Fi Module**: Provides Wi-Fi connectivity for communication with the ESP32.
  - **SparkFun Weather Shield**: Includes sensors for temperature, humidity, and light intensity.
    - **Si7021 Sensor**: Measures temperature and humidity.
    - **Light Sensor**: Measures ambient light intensity.
- **Features**:
  - Reads temperature, humidity, and light sensor data using the SparkFun Weather Shield.
  - Stores and retrieves configuration settings from EEPROM.
  - Handles TCP communication with the main controller.
  - Automates actions based on sensor data and setpoints.


## Project Structure

The project is organized as follows:

```
iot-smart-home/
├── ESP32_SMART_HOME_MAIN_CONTROLLER/
│   ├── src/
│   │   ├── automation.c          # Automation logic for heater and dehumidifier
│   │   ├── globals.c             # Global variables and logging functions
│   │   ├── handshake.c           # Handshake logic with the Arduino
│   │   ├── http_server.c         # HTTP server implementation
│   │   ├── json_parser.c         # JSON parsing for sensor data and commands
│   │   ├── main.c                # Main entry point for the ESP32 controller
│   │   ├── tcp_server.c          # TCP server implementation
│   │   ├── wifi.c                # Wi-Fi initialization and event handling
│   ├── platformio.ini            # PlatformIO configuration for ESP32
│   └── README.md                 # Documentation for the ESP32 controller
├── TempHumLightStation/
│   ├── src/
│   │   ├── automation.cpp        # Automation logic for temperature and humidity
│   │   ├── eeprom.cpp            # EEPROM read/write functions
│   │   ├── globals.cpp           # Global variables for the station
│   │   ├── helpers.cpp           # Helper functions for serial and data handling
│   │   ├── i2c.cpp               # I2C communication functions
│   │   ├── main.cpp              # Main entry point for the station
│   │   ├── sensor.cpp            # Sensor reading functions (temperature, humidity, light)
│   │   ├── wifi_commands.cpp     # Wi-Fi command handling for ESP8266
│   │   ├── wifi_handshake.cpp    # Handshake logic with the ESP32
│   │   ├── wifi_tcp.cpp          # Wi-Fi and TCP connection logic
│   ├── platformio.ini            # PlatformIO configuration for the station
│   └── README.md                 # Documentation for the station
├── LICENSE                       # License file (e.g., MIT)
└── README.md                     # Main project documentation
```

## Communication

The system uses multiple communication protocols:
- **Arduino ↔ ESP8266 (UART)**: Sends AT commands for Wi-Fi connectivity and communicates with the ESP32 over TCP.
- **Arduino ↔ ESP32 (TCP WiFi)**: Sends sensor data and receives setpoint updates.
- **ESP32 ↔ Shelly Plug S (HTTP)**: Sends commands to control the heater and humidifier.
- **ESP32 ↔ Web Dashboard (HTTP Server)**: Displays real-time sensor values and allows remote setpoint updates.
