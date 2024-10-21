#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h> 
#include <HTTPClient.h> 
#include <Arduino_JSON.h> 
#include <NetworkClientSecure.h>

String openWeatherMapApiKey = "e15379817caada44c2d65cc8509d71c5"; 
String latitude = "8.98060340"; //Exact Location
String longtude = "38.75776050"; //Exact Location
String jsonBuffer; 

// This is a USERTrust RSA Certification Authority, the root Certificate Authority that
// signed the server certificate for the demo server https://api.openweathermap.org/data/2.5/weather?lat=8.98060340&lon=38.75776050&APPID=e15379817caada44c2d65cc8509d71c5&units=metric in this
// example. This certificate is valid until Mon, 18 Jan 2038 23:59:59 GMT
const char *rootCACertificate = "-----BEGIN CERTIFICATE-----\n"
                                "MIIF3jCCA8agAwIBAgIQAf1tMPyjylGoG7xkDjUDLTANBgkqhkiG9w0BAQwFADCB\n"
                                "iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl\n"
                                "cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV\n"
                                "BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAw\n"
                                "MjAxMDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBiDELMAkGA1UEBhMCVVMxEzARBgNV\n"
                                "BAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNleSBDaXR5MR4wHAYDVQQKExVU\n"
                                "aGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMTJVVTRVJUcnVzdCBSU0EgQ2Vy\n"
                                "dGlmaWNhdGlvbiBBdXRob3JpdHkwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIK\n"
                                "AoICAQCAEmUXNg7D2wiz0KxXDXbtzSfTTK1Qg2HiqiBNCS1kCdzOiZ/MPans9s/B\n"
                                "3PHTsdZ7NygRK0faOca8Ohm0X6a9fZ2jY0K2dvKpOyuR+OJv0OwWIJAJPuLodMkY\n"
                                "tJHUYmTbf6MG8YgYapAiPLz+E/CHFHv25B+O1ORRxhFnRghRy4YUVD+8M/5+bJz/\n"
                                "Fp0YvVGONaanZshyZ9shZrHUm3gDwFA66Mzw3LyeTP6vBZY1H1dat//O+T23LLb2\n"
                                "VN3I5xI6Ta5MirdcmrS3ID3KfyI0rn47aGYBROcBTkZTmzNg95S+UzeQc0PzMsNT\n"
                                "79uq/nROacdrjGCT3sTHDN/hMq7MkztReJVni+49Vv4M0GkPGw/zJSZrM233bkf6\n"
                                "c0Plfg6lZrEpfDKEY1WJxA3Bk1QwGROs0303p+tdOmw1XNtB1xLaqUkL39iAigmT\n"
                                "Yo61Zs8liM2EuLE/pDkP2QKe6xJMlXzzawWpXhaDzLhn4ugTncxbgtNMs+1b/97l\n"
                                "c6wjOy0AvzVVdAlJ2ElYGn+SNuZRkg7zJn0cTRe8yexDJtC/QV9AqURE9JnnV4ee\n"
                                "UB9XVKg+/XRjL7FQZQnmWEIuQxpMtPAlR1n6BB6T1CZGSlCBst6+eLf8ZxXhyVeE\n"
                                "Hg9j1uliutZfVS7qXMYoCAQlObgOK6nyTJccBz8NUvXt7y+CDwIDAQABo0IwQDAd\n"
                                "BgNVHQ4EFgQUU3m/WqorSs9UgOHYm8Cd8rIDZsswDgYDVR0PAQH/BAQDAgEGMA8G\n"
                                "A1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEMBQADggIBAFzUfA3P9wF9QZllDHPF\n"
                                "Up/L+M+ZBn8b2kMVn54CVVeWFPFSPCeHlCjtHzoBN6J2/FNQwISbxmtOuowhT6KO\n"
                                "VWKR82kV2LyI48SqC/3vqOlLVSoGIG1VeCkZ7l8wXEskEVX/JJpuXior7gtNn3/3\n"
                                "ATiUFJVDBwn7YKnuHKsSjKCaXqeYalltiz8I+8jRRa8YFWSQEg9zKC7F4iRO/Fjs\n"
                                "8PRF/iKz6y+O0tlFYQXBl2+odnKPi4w2r78NBc5xjeambx9spnFixdjQg3IM8WcR\n"
                                "iQycE0xyNN+81XHfqnHd4blsjDwSXWXavVcStkNr/+XeTWYRUc+ZruwXtuhxkYze\n"
                                "Sf7dNXGiFSeUHM9h4ya7b6NnJSFd5t0dCy5oGzuCr+yDZ4XUmFF0sbmZgIn/f3gZ\n"
                                "XHlKYC6SQK5MNyosycdiyA5d9zZbyuAlJQG03RoHnHcAP9Dc1ew91Pq7P8yF1m9/\n"
                                "qS3fuQL39ZeatTXaw2ewh0qpKJ4jjv9cJ2vhsE/zB+4ALtRZh8tSQZXq9EfX7mRB\n"
                                "VXyNWQKV3WKdwrnuWih0hKWbt5DHDAff9Yk2dDLWKMGwsAvgnEzDHNb842m1R0aB\n"
                                "L6KCq9NjRHDEjf8tM7qtj3u1cIiuPhnPQCjY/MiQu12ZIvVS5ljFH4gxQ+6IHdfG\n"
                                "jjxDah2nGN59PRbxYvnKkKj9\n"
                                "-----END CERTIFICATE-----\n";
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
  String serverPath = "https://api.openweathermap.org/data/2.5/weather?lat=" + latitude + "&lon=" + longtude + "&APPID=" + openWeatherMapApiKey + "&units=metric";   
  jsonBuffer = httpGETRequest(serverPath.c_str()); 
  JSONVar weatherData = JSON.parse(jsonBuffer); 
  if (JSON.typeof(weatherData) == "undefined") { 
    Serial.println("Parsing input failed!"); 
    return; 
  } 

  Serial.print("JSON object = "); 
  Serial.println(weatherData); 
  
  Serial.print("\n\nCountry: ");
  Serial.println(weatherData["sys"]["country"]);
  Serial.print("City: ");
  Serial.println(weatherData["name"]);
  Serial.print("Weather Condition: ");
  Serial.println(weatherData["weather"][0]["description"]);

  Serial.print("\n\nTemperature: "); 
  Serial.println(weatherData["main"]["temp"]); 
  Serial.print("Minimum Tempratuter: ");
  Serial.println(weatherData["main"]["temp_min"]);
  Serial.print("Pressure: "); 
  Serial.println(weatherData["main"]["pressure"]); 
  Serial.print("Humidity: "); 
  Serial.println(weatherData["main"]["humidity"]); 
  Serial.print("Wind Speed: "); 
  Serial.println(weatherData["wind"]["speed"]); 

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


