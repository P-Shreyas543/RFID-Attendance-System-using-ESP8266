#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define SS_PIN 15  // SDA (SS) connected to D8
MFRC522DriverPinSimple ss_pin(SS_PIN);
MFRC522DriverSPI driver{ss_pin};
MFRC522 mfrc522{driver};

// Wi-Fi Credentials
const char *ssid = "ABC";
const char *password = "12345678";

// NTP Client Setup
WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org", 19800, 60000);

struct Attendance {
  String uid;
  String name;
  String checkInTime;
  String checkOutTime;
};

Attendance attendanceList[10];
int attendanceCount = 0;

AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial, text-align: center; background-color: #f4f4f4; padding: 20px; }
    h2 { font-size: 2rem; color: #333; }
    table { width: 90%; margin: auto; border-collapse: collapse; background: white; }
    th, td { padding: 10px; border: 1px solid #ddd; font-size: 1.2rem; text-align: center; }
    th { background: #4CAF50; color: white; }
    td { font-weight: bold; }
  </style>
</head>
<body>
  <h2>RFID Attendance System</h2>
  <p>Scan your RFID card to mark attendance.</p>
  <table>
    <tr><th>Employee Name</th><th>UID</th><th>Check-In Time</th><th>Check-Out Time</th></tr>
    <tbody id="attendance-table">%ATTENDANCE_LIST%</tbody>
  </table>
  <script>
    setInterval(() => {
      fetch('/data').then(response => response.text()).then(data => {
        document.getElementById("attendance-table").innerHTML = data;
      });
    }, 5000);
  </script>
</body>
</html>)rawliteral";

String getEmployeeName(String uid) {
  if (uid == "E321B1D9") return "Shreyas P";
  if (uid == "D1F0973F") return "Ruchith T";
  return "Unknown";
}

String getUIDString(MFRC522::Uid uid) {
  String uidString = "";
  for (byte i = 0; i < uid.size; i++) {
    uidString += String(uid.uidByte[i], HEX);
  }
  uidString.toUpperCase();
  return uidString;
}

void addOrUpdateAttendance(String uid, String time) {
  for (int i = 0; i < attendanceCount; i++) {
    if (attendanceList[i].uid == uid) {
      if (attendanceList[i].checkOutTime == "") {
        attendanceList[i].checkOutTime = time;
        Serial.println("Check-Out Updated for " + uid);
      }
      return;
    }
  }
  if (attendanceCount < 10) {
    attendanceList[attendanceCount] = {uid, getEmployeeName(uid), time, ""};
    attendanceCount++;
    Serial.println("Check-In Recorded for " + uid);
  }
}

String generateAttendanceTable() {
  String tableData = "";
  for (int i = 0; i < attendanceCount; i++) {
    tableData += "<tr><td>" + attendanceList[i].name +
                 "</td><td>" + attendanceList[i].uid +
                 "</td><td>" + attendanceList[i].checkInTime +
                 "</td><td>" + (attendanceList[i].checkOutTime == "" ? "Not Yet" : attendanceList[i].checkOutTime) +
                 "</td></tr>";
  }
  return tableData;
}

void handleRoot(AsyncWebServerRequest *request) {
  String html = index_html;
  html.replace("%ATTENDANCE_LIST%", generateAttendanceTable());
  request->send(200, "text/html", html);
}

void handleData(AsyncWebServerRequest *request) {
  request->send(200, "text/plain", generateAttendanceTable());
}

void setup() {
  Serial.begin(9600);
  mfrc522.PCD_Init();
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());

  timeClient.begin();

  server.on("/", HTTP_GET, handleRoot);
  server.on("/data", HTTP_GET, handleData);
  server.begin();
}

void loop() {
  timeClient.update();
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  String uid = getUIDString(mfrc522.uid);
  Serial.println("Scanned UID: " + uid);

  String currentTime = timeClient.getFormattedTime();
  addOrUpdateAttendance(uid, currentTime);

  delay(2000);
}