# WiFi2JJY [[中文版]](README.md) 

WiFi2JJY is an open-source firmware project that converts Wi-Fi network time (NTP) into a standard-compliant JJY long-wave time signal, allowing JJY radio-controlled clocks to synchronize accurately without relying on real long-wave radio reception.

Based on the ESP32-C3 Super Mini development board, the firmware connects to a Wi-Fi network, synchronizes time from configurable NTP servers, and generates precise JJY time code waveforms via GPIO output. The project is designed with low power consumption in mind and supports deep sleep operation, making it suitable for long-term, standalone deployment.

WiFi2JJY is intended for:
- JJY radio clock testing and calibration
- Indoor or shielded environments where JJY reception is unavailable
- DIY radio time signal transmitters
- Open-source time synchronization experiments

## Project Overview

JJY is Japan's national standard time signal, operated by the National Institute of Information and Communications Technology (NICT), and used for broadcasting highly accurate standard time. This project leverages the ESP32-C3 Super Mini board to implement JJY signal generation and transmission with the following features:

- Automatic Wi-Fi connection and configuration (via a web interface)  
- NTP time synchronization (supports multiple NTP servers)  
- Generation of JJY-formatted time signals  
- Efficient power management (deep sleep mode)  
- Web configuration interface (for Wi-Fi setup)

## Hardware Requirements

- **Main Controller**: ESP32-C3 Super Mini development board  
- **Power Supply**: 5V or 3.3V power source

### Pin Assignments

Based on the project’s `IOPin.h` file, the following GPIO pins are used:

| Pin Name | GPIO Number | Function Description            |
|----------|--------------|----------------------------------|
| PIN_DA   | GPIO_NUM_3   | JJY signal output pin           |
| PIN_PON  | GPIO_NUM_4   | Power control / wake-up pin     |

## Software Dependencies

- **Arduino IDE**: for code compilation and flashing  
- **ESP32 Arduino Core**: provides ESP32-C3 board support  
- **FreeRTOS**: for task scheduling and multi-threading  
- **WiFi Library**: for Wi-Fi connectivity  
- **WebServer Library**: for the web configuration interface  
- **Preferences Library**: for storing Wi-Fi configuration in non-volatile storage (NVS)

## Features

### 1. Wi-Fi Connection Management
- Automatically enters AP mode on first boot and provides a web configuration interface  
- Saves Wi-Fi credentials to non-volatile storage (NVS)  
- Automatically reconnects to the previously configured Wi-Fi network  
- Switches back to AP mode if connection times out

### 2. NTP Time Synchronization
- Supports multiple NTP server configurations  
- Uses a FreeRTOS task for asynchronous time synchronization  
- Updates system time upon successful synchronization  
- Supports time zone configuration (Tokyo time zone by default)

### 3. JJY Signal Transmission
- Generates time signals compliant with the JJY standard  
- Uses PWM to emulate AM modulation for the JJY signal  
- High-priority task ensures precise signal timing  
- Automatically enters deep sleep after signal transmission

### 4. Power Management
- Supports deep sleep mode to minimize power consumption  
- Wake-up triggered via GPIO pin  
- Automatically enters deep sleep after completing JJY transmission

### 5. Web Configuration Interface
- Scans and lists available Wi-Fi networks  
- Allows SSID and password configuration  
- Displays real-time device status  
- Provides reboot and factory reset functions

## Usage Instructions

### 1. Initial Setup
1. Power the ESP32-C3 Super Mini board correctly  
2. After boot-up, search for and connect to the Wi-Fi network named **“MYNOVA-CLOCK”**  
3. Open a browser and navigate to `http://192.168.1.1` to access the configuration page  
4. Select your Wi-Fi network, enter the password **`MYNOVA123`**, and click “Connect”  
5. The device will automatically connect to your configured Wi-Fi. Once successful, you can view its status in the browser

### 2. Normal Operation
1. On boot, the device automatically connects to the saved Wi-Fi network  
2. Upon successful Wi-Fi connection, it starts the NTP time synchronization task  
3. After time synchronization completes, it begins transmitting the JJY signal  
4. Once transmission finishes, the device enters deep sleep  
5. The device can be woken up via the PON pin to restart the cycle

### 3. Reconfiguration
To reconfigure Wi-Fi settings:
1. Power off the device  
2. Hold the PON pin (if connected to a physical button) or send a reset command via serial  
3. The device will clear saved settings and re-enter AP mode  
4. Repeat the initial setup steps

## Code Structure

The project follows a modular design with the following key files:

```
WiFi2JJY/
├── WiFi2JJY.ino          # Main program entry point
├── WiFiManager.h         # Wi-Fi connection manager header
├── WiFiManager.cpp       # Wi-Fi connection manager implementation
├── WebService.h          # Web server header
├── WebService.cpp        # Web server implementation
├── WiFiConfigPage.cpp    # Wi-Fi configuration page (HTML/JavaScript)
├── TimeSync.h            # Time synchronization header
├── TimeSync.cpp          # Time synchronization implementation
├── JJYSender.h           # JJY signal transmitter header
├── JJYSender.cpp         # JJY signal transmitter implementation
├── IOPin.h               # Pin definitions
└── README.md             # Project documentation
```

### Core Components
1. **WiFiManager**: Handles Wi-Fi connection, configuration storage, and status management  
2. **WebService**: Provides the web configuration UI and API endpoints  
3. **TimeSync**: Synchronizes system time using the NTP protocol  
4. **JJYSender**: Generates and transmits the JJY-formatted time signal  
5. **Web UI**: User-friendly interface built with HTML/JavaScript for configuration

### Web Configuration Interface
- Scans and displays available Wi-Fi networks  
- Allows password input  
- Shows real-time connection status  
- Offers reset functionality

### Time Zone
The current time zone is hardcoded to UTC+8 (Beijing Time). To change it, please modify the 
```cpp
constexpr long UTC_OFFSET = 8 * 3600;
```  
variable in `TimeSync.cpp` to the desired offset in seconds from UTC (e.g., `9 * 3600` for UTC+9).
  
## Compilation and Flashing

### Compile and Flash from Source
1. Open Arduino IDE and go to **File > Open**, then navigate to the project folder  
2. Select **Tools > Board > ESP32 Arduino > ESP32C3 Dev Module**  
3. Select **Tools > Port** and choose the connected ESP32-C3 serial port  
4. Click **Sketch > Upload** to compile and flash the code

### Flash Precompiled Binary
1. Download the latest precompiled binary (`WiFi2JJY.bin`) from the [Releases page](https://github.com/Tomosawa/WIFI2JJY/releases)  
2. Use Espressif's official [Flash Download Tool](https://www.espressif.com/en/support/download/other-tools)  
3. Connect the ESP32-C3 board to your computer  
4. Launch the Flash Download Tool, select the correct COM port and board model (ESP32-C3)  
5. Click “Add File” and select the downloaded `.bin` file  
6. Set the flash address to `0x0000`; keep other settings as default  
7. Click “Start” to begin flashing

## Power Management

The project implements an efficient power-saving strategy:
1. After boot, the device quickly connects to Wi-Fi and synchronizes time  
2. Immediately enters deep sleep after completing JJY transmission  
3. Supports wake-up via the PON GPIO pin

## Troubleshooting

### Wi-Fi Connection Issues
- Verify that the Wi-Fi password is correct  
- Ensure sufficient signal strength  
- Try rebooting the device  
- Check serial output for error messages

### Time Synchronization Failures
- Confirm that Wi-Fi is connected  
- Review NTP server configuration  
- Ensure network access to NTP servers is not blocked

### JJY Signal Problems
- Verify that the DA pin is correctly connected  
- Confirm the JJY transmission task launched properly  
- Inspect serial debug logs for anomalies

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

## Contributions

Pull requests and issue reports are welcome!

## References
- [JJY Time Signal Specification (NICT)](https://www.nict.go.jp/JST/ntp/jjy.html)  
- [ESP32-C3 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32-c3_technical_reference_manual_en.pdf)  
- [Arduino ESP32 Core GitHub Repository](https://github.com/espressif/arduino-esp32)  
- [FreeRTOS Official Documentation](https://www.freertos.org/documentation.html)