
#include <SPI.h>
#include <MFRC522.h>

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h> 
#include <HTTPClient.h>
#include <NetworkClientSecure.h>

#include <WiFiUdp.h>
#include <time.h>

#include<vector>
#include<algorithm>
#include<functional>
#include <Arduino_JSON.h> 

#define SS_PIN  21  // ESP32 pin GPIO
#define RST_PIN 22 // ESP32 pin GPIO

MFRC522 rfid0(SS_PIN, RST_PIN);

byte tag [5];
byte testTag[5] = {0x5D, 0xE5, 0xA2, 0x82}; 

const char* serverName = "";

String apiKey = "VSTNKEYGR6QICXAO";

const char *rootCACertificate = "-----BEGIN CERTIFICATE-----\n";

// NTP Server
const char* ntpServer = "pool.ntp.org";
const int timeZone = 3; // GMT+3

WiFiUDP udp;
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; // Buffer to hold incoming & outgoing packets

struct tm timeinfo;

void setClock() {

  configTime(timeZone * 3600, 0, ntpServer);

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  int attempts = 0;
  const int maxAttempts = 20; // Maximum attempts to sync time
  while (nowSecs < 1000000000 && attempts < maxAttempts) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
    attempts++;
  }

  Serial.println();
  
  if (nowSecs < 1000000000) {
    Serial.println(F("Failed to obtain time"));
    return; // Exit if time is not obtained
  }
  localtime_r(&nowSecs, &timeinfo); 
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}

WiFiMulti WiFiMulti;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  SPI.begin(); // init SPI bus
  rfid0.PCD_Init(); // init MFRC522

  Serial.println();
  Serial.println();
  Serial.println(); 
  
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("SQUARED_TECH_SOLUTIONS-2", "STS@2024");
  Serial.print("Waiting for WiFi to connect...");
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected");

  setClock();


  Serial.println("Tap an RFID/NFC tag on the RFID-RC522 reader");


}

void loop() {

  // put your main code here, to run repeatedly:

  if (rfid0.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid0.PICC_ReadCardSerial()) { // NUID has been readed
      MFRC522::PICC_Type piccType = rfid0.PICC_GetType(rfid0.uid.sak);
      Serial.print("RFID/NFC Tag Type: ");
      Serial.println(rfid0.PICC_GetTypeName(piccType));

      // print UID in Serial Monitor in the hex format
      Serial.print("UID:");
      for (int i = 0; i < rfid0.uid.size; i++) {
        Serial.print(rfid0.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid0.uid.uidByte[i], HEX);
        tag[i] = rfid0.uid.uidByte[i];

      }
      if(memcmp(tag, testTag, 4) == 0 ){
        Serial.println("\n Your data is being sent to the server");
        //httpPostRequest(serverName);

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

        Serial.print(F("Current time: "));
        Serial.println(asctime(&timeinfo));

      }
      Serial.println();

      rfid0.PICC_HaltA(); // halt PICC
      rfid0.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }

}


