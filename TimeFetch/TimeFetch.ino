#include <WiFi.h>
#include <WiFiUdp.h>
#include <time.h>

// Replace with your network credentials
const char* ssid = "SQUARED_TECH_SOLUTIONS-2";
const char* password = "STS@2024";

// NTP Server
const char* ntpServer = "pool.ntp.org";
const int timeZone = 3; // GMT+3

WiFiUDP udp;
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; // Buffer to hold incoming & outgoing packets

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");
  
  // Initialize NTP
  configTime(timeZone * 3600, 0, ntpServer);
}

void loop() {
  // Get the current time
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 1000)) {
    Serial.println("Failed to obtain time");
    return;
  }

  // Print the time in GMT+3
  Serial.print("Current time: ");
  Serial.print(timeinfo.tm_hour);
  Serial.print(":");
  Serial.print(timeinfo.tm_min);
  Serial.print(":");
  Serial.print(timeinfo.tm_sec);
  Serial.print(" ");
  Serial.print(timeinfo.tm_mday);
  Serial.print("/");
  Serial.print(timeinfo.tm_mon + 1);
  Serial.print("/");
  Serial.println(timeinfo.tm_year + 1900);

  delay(10000); // Update every 10 seconds
}
