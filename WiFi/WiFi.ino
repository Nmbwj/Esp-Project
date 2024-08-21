#include <WiFi.h>
const char* ssid = "Esp_Sami";
const char* password = "samiiscoming";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  IPAddress apIP(192,168,4,1);
  IPAddress subnet(255,255,255,0);

  WiFi.softAPConfig(apIP,apIP,subnet);
  Serial.print("Access Point IP Address: ");
  Serial.println(WiFi.softAPIP());

}

void loop() {
  // put your main code here, to run repeatedly:

}
