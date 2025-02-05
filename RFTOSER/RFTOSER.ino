#include <SPI.h>
#include <MFRC522.h>

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h> 
#include <HTTPClient.h>
#include <NetworkClientSecure.h>

#define SS_PIN  21  // ESP32 pin GPIO
#define RST_PIN 22 // ESP32 pin GPIO

MFRC522 rfid0(SS_PIN, RST_PIN);

byte tag [5];
byte testTag[5] = {0x5D, 0xE5, 0xA2, 0x82}; 

// Domain Name with full URL Path for HTTP POST Request
const char* serverName = "https://api.thingspeak.com/update";
// Service API Key
String apiKey = "VSTNKEYGR6QICXAO"; //REPLACE WITH YOUR APIKEY ;

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
// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned

void setClock() {
  configTime(0, 0, "pool.ntp.org");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
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
        
        String httpRequestData = "{\"api_key\":\"" + apiKey + "\",\"field1\":\"" + String(data) + "\"}";
        
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
