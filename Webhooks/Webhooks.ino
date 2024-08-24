#include <WiFi.h> 

#include <HTTPClient.h> 

const char* ssid = "SQUARED_TECH_SOLUTIONS-2"; 

const char* password = "STS@2024"; 

const char* webhookURL = "https://webhook.site/bc535e69-b222-4a4a-9143-bb2c0c6a2471"; 

 

void setup() { 

  Serial.begin(115200); 

  WiFi.begin(ssid, password); 

  while (WiFi.status() != WL_CONNECTED) { 

    delay(1000); 

  } 

} 

 

void loop() { 
  
  sendWebhookRequest();  
  delay(10000);

} 

 

void sendWebhookRequest() { 

  HTTPClient http; 

  String url = String(webhookURL) + "?random=" + String(random(30)); 

  http.begin(url); 

  int httpResponseCode = http.GET(); 

  if (httpResponseCode > 0) { 

    Serial.print("Webhook request sent. Response code: "); 

    Serial.println(httpResponseCode); 

  } else { 

    Serial.print("Error sending webhook request. HTTP response code: "); 

    Serial.println(httpResponseCode); 

  } 

  http.end(); 

} 