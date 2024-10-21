#include <WiFi.h>
WiFiServer server(80);

const char* ssid = "Esp_Naol";
const char* password = "naoliscoming";


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.softAP(ssid,password);

  IPAddress apIP(192,168,4,1);
  IPAddress subnet(255,255,255,0);

  WiFi.softAPConfig(apIP,apIP,subnet);

  server.begin();
  
  Serial.print("Wifi Direct Group Owner IP Address: ");
  Serial.println(WiFi.softAPIP());
}


void loop() {
  // put your main code here, to run repeatedly:
  WiFiClient client = server.available();
  if(client){
    Serial.println("Client Connected.");
    client.println("Hello from Group Owner!");
    while(client.connected()){
      if(client.available()){
        String message = client.readStringUntil('\n');
        Serial.print("Recieved message: ");
        Serial.println(message);
      }
    }
    client.stop();
    Serial.println("Client disconnected.");
  }

}
