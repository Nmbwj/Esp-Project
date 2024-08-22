#include <WiFi.h>
#include <WebServer.h>
const char* ssid = "SQUARED_TECH_SOLUTIONS-2";
const char* password = "STS@2024";

WebServer server(80);

const int ledPin = 13;
const int buttonPin = 12;
bool ledState = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(2,OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  digitalWrite(ledPin, ledState);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println("Connecting to WiFi...");

  }
  Serial.println("WiFi connected");
  Serial.println("IP address:  ");
  Serial.println(WiFi.localIP());
  server.on("/", HTTP_GET, handleRoot);
  server.on("/toggle",HTTP_GET, handleToggle);
  server.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  
  int buttonState = digitalRead(buttonPin);

  if(buttonState == LOW){
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
  }

}

void handleRoot(){
  String html = "<html><body>";
  html += "<h1>ESP32 LED Control</h1>";
  html += "<p>LED Status: " + String(ledState) + "</p>";
  html += "<form method='get' action= '/toggle'>";
  html += "<button type= 'submit'> Toggle LED</button>";
  html += "</form>";
  html += "</body></html>";
  server.send(200,"text/html",html);

}

void handleToggle() { 

 ledState = !ledState; 

 digitalWrite(ledPin, ledState);
 delay(1000);
 digitalWrite(2, HIGH);
 delay(1000);

 server.send(200, "text/plain", "LED toggled"); 

}
