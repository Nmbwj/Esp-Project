
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

const char* serverName = "https://api.thingspeak.com/update"; //https://api.thingspeak.com/update

String apiKey = "VSTNKEYGR6QICXAO";

//const char *rootCACertificate = "-----BEGIN CERTIFICATE-----\n";

// This is a USERTrust RSA Certification Authority, the root Certificate Authority that
// signed the server certificate for the demo server http://api.thingspeak.com/update in this
// example. This certificate is valid until Fri, 15 Jan 2038 12:00:00 GMT
const char *rootCACertificate = "-----BEGIN CERTIFICATE-----\n"
                                "MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh\n"
                                "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
                                "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n"
                                "MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT\n"
                                "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
                                "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG\n"
                                "9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI\n"
                                "2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx\n"
                                "1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ\n"
                                "q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz\n"
                                "tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ\n"
                                "vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP\n"
                                "BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV\n"
                                "5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY\n"
                                "1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4\n"
                                "NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG\n"
                                "Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91\n"
                                "8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe\n"
                                "pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl\n"
                                "MrY=\n"
                                "-----END CERTIFICATE-----\n";


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



void httpPostRequest(const char* serverName){
  NetworkClientSecure *client = new NetworkClientSecure; 
  static int data, down, count;
  if (client) {
    client->setCACert(rootCACertificate);
    
    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before NetworkClientSecure *client is
      HTTPClient https;

      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, serverName)) {  // HTTPS for postbin use this link: https://www.postb.in/b/1724831336735-1994523359462. for jigsaw use this link: https://jigsaw.w3.org/HTTP/connection.html
        Serial.print("[HTTPS] POST...\n");
        // start connection and send HTTP header
        // If you need an HTTP request with a content type: application/json, use the following:
        https.addHeader("Content-Type", "application/json");
        // JSON data to send with HTTP POST
        if (!getLocalTime(&timeinfo, 1000)) {
          
          String httpRequestData = "{\"api_key\":\"" + apiKey + "\",\"field1\":\"" + String(data) + "\", \"field2\": \""+String(-1)+
                                  "\",\"field3\": \""+String(-1)+"\",\"field4\": \""+String(-1)+"\",\"field5\": \""+String(-1)+
                                  "\",\"field6\": \""+String(-1)+"\",\"field7\": \""+String(-1)+"\"}";
        
        }

        String httpRequestData = "{\"api_key\":\"" + apiKey + "\",\"field1\":\"" + String(data) + "\", \"field2\": \""+String(timeinfo.tm_mday)+
                                  "\",\"field3\": \""+String(timeinfo.tm_hour)+"\",\"field4\": \""+String(timeinfo.tm_min)+"\",\"field5\": \""+String(timeinfo.tm_sec)+
                                  "\",\"field6\": \""+String(timeinfo.tm_mon + 1)+"\",\"field7\": \""+String(timeinfo.tm_year + 1900)+"\"}";
        
  
        Serial.println(httpRequestData);
        if(!down) data++;
        else data--;
        count++;
        if(count == 5){
          count =0;
          if(down)down =0;
          else down =1;
        }
        // Send HTTP POST request
        int httpResponseCode = https.POST(httpRequestData);
        if(httpResponseCode>0){
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        Serial.print("Data: ");
        Serial.println(data-1);
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




