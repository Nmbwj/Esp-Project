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

const char* serverName = "https://www.squaredtech-fms.com/api/gps-data/";


// This is a GTS Root R4 RSA Certification Authority, the root Certificate Authority that
// signed the server certificate for the demo server https://www.squaredtech-fms.com/api/gps-data/ in this
// example. This certificate is valid until Sun, 22 Jun 2036 00:00:00 GMT


const char *rootCACertificate =R"literal(
-----BEGIN CERTIFICATE-----
MIICCTCCAY6gAwIBAgINAgPlwGjvYxqccpBQUjAKBggqhkjOPQQDAzBHMQswCQYD
VQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEUMBIG
A1UEAxMLR1RTIFJvb3QgUjQwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAwMDAw
WjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2Vz
IExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjQwdjAQBgcqhkjOPQIBBgUrgQQAIgNi
AATzdHOnaItgrkO4NcWBMHtLSZ37wWHO5t5GvWvVYRg1rkDdc/eJkTBa6zzuhXyi
QHY7qca4R9gq55KRanPpsXI5nymfopjTX15YhmUPoYRlBtHci8nHc8iMai/lxKvR
HYqjQjBAMA4GA1UdDwEB/wQEAwIBhjAPBgNVHRMBAf8EBTADAQH/MB0GA1UdDgQW
BBSATNbrdP9JNqPV2Py1PsVq8JQdjDAKBggqhkjOPQQDAwNpADBmAjEA6ED/g94D
9J+uHXqnLrmvT/aDHQ4thQEd0dlq7A/Cr8deVl5c1RxYIigL9zC2L7F8AjEA8GE8
p/SgguMh1YQdc4acLa/KNJvxn7kjNuK8YAOdgLOaVsjh4rsUecrNIdSUtUlD
-----END CERTIFICATE-----
)literal";

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
        httpPostRequest(serverName);

        if (!getLocalTime(&timeinfo, 1000)) {
          Serial.println("Failed to obtain time");
          return;
        }

        // Get the time as a string
        char* timeString = asctime(&timeinfo);

        // Remove the newline character
        timeString[strlen(timeString) - 1] = '\0'; // Replace the newline with a null terminator

         
        // Print the time without a newline
        Serial.print("Current time: ");
        Serial.print(timeString);
      }
      Serial.println();

      rfid0.PICC_HaltA(); // halt PICC
      rfid0.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }

}



void httpPostRequest(const char* serverName){
  NetworkClientSecure *client = new NetworkClientSecure; 
  // Get the time as a string
  static char* timeString;
  // Add the token header
  static String token = "5d1c4088654fec69eac044a4a4c50a74be571318"; // Replace with your actual token

  if (client) {
    client->setCACert(rootCACertificate);
    
    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before NetworkClientSecure *client is
      HTTPClient https;

      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, serverName)) {  // HTTPS for postbin use this link: https://www.postb.in/b/1724831336735-1994523359462. for jigsaw use this link: https://jigsaw.w3.org/HTTP/connection.html
        Serial.print("[HTTPS] POST...\n");
        // start connection and send HTTP header

        https.addHeader("x-token", token); // Add the token header

        // If you need an HTTP request with a content type: application/json, use the following:
        https.addHeader("Content-Type", "application/json");
        // JSON data to send with HTTP POST
        if (!getLocalTime(&timeinfo, 1000)) {
          timeString= asctime(&timeinfo);
          // Remove the newline character
          timeString[strlen(timeString) - 1] = '\0'; // Replace the newline with a null terminator

          String httpRequestData = "{\"gps_info_id\":1,\"information\":\""+String(timeString)+"\"}";


        }
        
        
        timeString= asctime(&timeinfo);
        // Remove the newline character
        timeString[strlen(timeString) - 1] = '\0'; // Replace the newline with a null terminator

        String httpRequestData = "{\"gps_info_id\":1,\"information\":\""+String(timeString)+"\"}";

  
        Serial.println(httpRequestData);
 
        // Send HTTP POST request
        int httpResponseCode = https.POST(httpRequestData);
        if(httpResponseCode>0){
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);

        }else
          Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpResponseCode).c_str());
        // Free resources
        https.end();
        return ;
      }else {
        Serial.printf("[HTTPS] Unable to connect\n");
        return ;
      }
    // End extra scoping block
    }
      delete client;



  }else {
    Serial.println("Unable to create client");
    return ;
  }
}




