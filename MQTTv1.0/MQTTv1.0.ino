#include <WiFi.h> 
#include <PubSubClient.h> 

#define LED 26 

uint32_t delayMS; 

const char* ssid = "SQUARED_TECH_SOLUTIONS-2"; 
const char* password = "STS@2024"; 

const char* mqttServer = "broker.hivemq.com"; 
const char* clientID = "rQkyNOccHd"; 
const char* topic = "Tempdata"; 

unsigned long previousMillis = 0; 

const long interval = 120000; 

String msgStr = ""; 

float temp, hum; 

WiFiClient espClient; 

PubSubClient client(espClient); 

void setup_wifi() { 

  delay(10); 
  WiFi.begin(ssid, password); 

  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
  } 

} 

 

void reconnect() { 

  while (!client.connected()) { 

    if (client.connect(clientID)) {  
      Serial.println("MQTT connected"); 
      client.subscribe("lights"); 
      client.subscribe("servo"); 
    } 

    else { 
      delay(5000); 
    } 

  } 

} 

 

void callback(char* topic, byte* payload, unsigned int length) { 

  String data = ""; 

  for (int i = 0; i < length; i++) { 
    data += (char)payload[i]; 
  } 

  if (String(topic) == "lights") { 
    if (data == "ON") { 
      digitalWrite(LED, HIGH); 
    } 

    else { 
      digitalWrite(LED, LOW); 
    } 

  } 

  else if (String(topic) == "servo") { 
    int degree = data.toInt(); 
    Serial.println("Servo moved: "+ String(degree));
  } 

} 

 

void setup() { 
  Serial.begin(115200); 
  pinMode(LED, OUTPUT); 
  digitalWrite(LED, LOW); 
  setup_wifi(); 
  client.setServer(mqttServer, 1883); 
  client.setCallback(callback); 

} 

 

void loop() { 

  if (!client.connected()) { 
    reconnect(); 
  } 

  client.loop(); 

  unsigned long currentMillis = millis(); 

  if (currentMillis - previousMillis >= interval) { 
    previousMillis = currentMillis; 

    temp = 50.0; 


    hum = 23.1; 

    msgStr = String(temp) + "," + String(hum) + ","; 

    byte arrSize = msgStr.length() + 1; 

    char msg[arrSize]; 

    msgStr.toCharArray(msg, arrSize); 

    client.publish(topic, msg); 

    msgStr = ""; 

    delay(1); 

  } 

} 