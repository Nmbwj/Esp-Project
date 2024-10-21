#include <WiFi.h>
const char* ssid = "SQUARED_TECH_SOLUTIONS-2";
const char* password = "STS@2024";
int on = 1;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\n");
  Serial.print("Connected to WiFi network with IP address: ");
  Serial.println(WiFi.localIP());
  

}

void loop() {
  // put your main code here, to run repeatedly:

}
