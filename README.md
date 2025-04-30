# iot-smart-home

A smart home IoT project that integrates various devices for automation, monitoring, and remote control.

## Overview

This project consists of two main components:

1. **ESP32 Smart Home Main Controller**: Manages the central control of the smart home system, including Wi-Fi connectivity, HTTP server, and TCP server for communication with other devices.
2. **TempHumLightStation**: A peripheral device that monitors temperature, humidity, and light levels, and communicates with the main controller.

## Features

### ESP32 Smart Home Main Controller
- Wi-Fi initialization and event handling.
- HTTP server for user interaction and setpoint updates.
- TCP server for communication with peripheral devices.
- Logging and debugging support using ESP32's logging library.
- Sends HTTP commands to control appliances via Shelly Smart Plug S.

### TempHumLightStation
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

## Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/) installed in your development environment.
- A compatible ESP32 board for the main controller.
- A compatible microcontroller (e.g., Arduino Uno) for the TempHumLightStation.
- Shelly Smart Plug S for appliance control.

### Building and Flashing

1. Navigate to the desired project directory (`ESP32_SMART_HOME_MAIN_CONTROLLER` or `TempHumLightStation`).
2. Open the project in Visual Studio Code with the PlatformIO extension.
3. Build the project using the PlatformIO build system.
4. Flash the firmware to the respective device.

### Configuration

- Update Wi-Fi credentials in the `TempHumLightStation` EEPROM or use the default credentials.
- Modify setpoints and automation logic as needed in the source code.

## Usage

- Access the HTTP server hosted by the ESP32 Smart Home Main Controller to monitor and update setpoints.
- The TempHumLightStation will automatically send sensor data to the main controller and respond to setpoint changes.
- Use the web dashboard to view real-time sensor readings and control automation settings.

## Communication

The system uses multiple communication protocols:
- **Arduino ↔ ESP8266 (UART)**: Sends AT commands for Wi-Fi connectivity and communicates with the ESP32 over TCP.
- **Arduino ↔ ESP32 (TCP WiFi)**: Sends sensor data and receives setpoint updates.
- **ESP32 ↔ Shelly Plug S (HTTP)**: Sends commands to control the heater and humidifier.
- **ESP32 ↔ Web Dashboard (HTTP Server)**: Displays real-time sensor values and allows remote setpoint updates.

## Additional Information

For full details about the project, including design decisions, implementation details, and testing results, please refer to the accompanying report: **Smart Home System.pdf**.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request with your changes.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.