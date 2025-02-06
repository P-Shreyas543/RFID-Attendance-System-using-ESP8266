# RFID Attendance System using ESP8266

## Overview
This project implements an RFID-based attendance system using the ESP8266 Wi-Fi module. The system reads RFID tags using an MFRC522 reader, logs check-in and check-out times, and displays attendance records on a web page.

## Features
- Uses RFID to mark attendance.
- Displays attendance records on a web-based interface.
- Retrieves real-time clock data using an NTP client.
- Wi-Fi connectivity for remote access.
- Automatically updates attendance records.

## Components Used
- **ESP8266 (NodeMCU)**
- **MFRC522 RFID Module**
- **RFID Tags**
- **Wi-Fi Router for connectivity**

## Libraries Required
Ensure you have the following libraries installed in your Arduino IDE:
- `ESP8266WiFi.h` (for Wi-Fi connectivity)
- `ESPAsyncWebServer.h` (for handling web requests)
- `MFRC522v2.h` (for RFID communication)
- `MFRC522DriverSPI.h` (SPI driver for RFID module)
- `MFRC522DriverPinSimple.h`
- `NTPClient.h` (for real-time clock synchronization)
- `WiFiUdp.h` (for NTP client communication)

## Circuit Connections
| ESP8266 Pin | RFID Pin |
|------------|---------|
| D8 (GPIO15) | SDA (SS) |
| D7 (GPIO13) | SCK |
| D6 (GPIO12) | MOSI |
| D5 (GPIO14) | MISO |
| GND | GND |
| 3.3V | VCC |
| RST | RST |

## How It Works
1. The ESP8266 connects to a Wi-Fi network.
2. The NTP client fetches the real-time clock.
3. The RFID module scans for cards and logs the check-in or check-out time.
4. Attendance records are updated in an array.
5. A web interface displays the attendance data and refreshes every 5 seconds.

## Setting Up the Project
1. Install the required libraries in the Arduino IDE.
2. Connect the hardware as per the circuit connections.
3. Upload the code to the ESP8266.
4. Open the Serial Monitor to check the Wi-Fi IP address.
5. Access the web interface by entering the IP address in a browser.

## Web Interface
The system hosts a webpage where users can:
- View attendance records in a table format.
- Automatically update data without refreshing the page.

## Example RFID Tags
Modify the `getEmployeeName(String uid)` function to map RFID UIDs to employee names.
```cpp
String getEmployeeName(String uid) {
  if (uid == "E321B1D9") return "Shreyas P";
  if (uid == "D1F0973F") return "Ruchith T";
  return "Unknown";
}
```

## Future Improvements
- Store attendance logs in an external database.
- Implement authentication for secure access.
- Add support for multiple users with unique IDs.
