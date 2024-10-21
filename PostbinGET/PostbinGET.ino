// This is test on postbin GET
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h> 
#include <HTTPClient.h> 
#include <Arduino_JSON.h> 
#include <NetworkClientSecure.h>

String jsonBuffer; 

// This is a GTS Root R4 Certification Authority, the root Certificate Authority that
// signed the server certificate for the demo server https://www.postb.in/b/1725612931957-6564117060042 in this
// example. This certificate is valid until Sun, 22 Jun 2036 00:00:00 GMT
const char *rootCACertificate = R"literal(
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

// Not sure if NetworkClientSecure checks the validity date of the certificate.
// Setting clock just to be sure...
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

 Serial.begin(115200);
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

} 

void loop() {  
  String serverPath = "https://www.postb.in/api/bin/1725612931957-6564117060042";   
  jsonBuffer = httpGETRequest(serverPath.c_str()); 
  JSONVar weatherData = JSON.parse(jsonBuffer); 
  if (JSON.typeof(weatherData) == "undefined") { 
    Serial.println("Parsing input failed!"); 
    return; 
  } 

  Serial.print("JSON object = "); 
  Serial.println(weatherData); 
  
  Serial.println();
  Serial.println("Waiting 2min before the next round...");
  delay(10000);

} 

String httpGETRequest(const char* serverName) { 
  NetworkClientSecure *client = new NetworkClientSecure;
  String payload = "{}"; 
  if (client) {
    client->setCACert(rootCACertificate);
    
    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before NetworkClientSecure *client is
      HTTPClient https;

      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, serverName)) {  // HTTPS for postbin use this link: https://www.postb.in/b/1724831336735-1994523359462. for jigsaw use this link: https://jigsaw.w3.org/HTTP/connection.html
        Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        int httpCode = https.GET();
        
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            payload = https.getString();
          }
        } else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }

        https.end();
        return payload;
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
        return payload;
      }

      // End extra scoping block
    }

    delete client;
  } else {
    Serial.println("Unable to create client");
    return payload;
  }
} 


