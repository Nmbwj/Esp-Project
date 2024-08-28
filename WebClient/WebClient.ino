#include <WiFi.h> 
#include <HTTPClient.h> 
#include <Arduino_JSON.h> 

const char* wifiSSID = "SQUARED_TECH_SOLUTIONS-2"; 
const char* wifiPassword = "STS@2024"; 

String openWeatherMapApiKey = "e15379817caada44c2d65cc8509d71c5"; 
String latitude = "8.98060340"; //Exact Location
String longtude = "38.75776050"; //Exact Location
String jsonBuffer; 

void setup() { 

 Serial.begin(115200); 
 WiFi.begin(wifiSSID, wifiPassword); 
 Serial.println("Connecting to WiFi..."); 

 while (WiFi.status() != WL_CONNECTED) { 
  delay(500); 
  Serial.print("."); 

 } 

 Serial.println("\nConnected to WiFi network. IP Address: " + WiFi.localIP().toString()); 


} 

void loop() {  

  if (WiFi.status() == WL_CONNECTED) { 
   String serverPath = "http://api.openweathermap.org/data/2.5/weather?lat=" + latitude + "&lon=" + longtude + "&APPID=" + openWeatherMapApiKey + "&units=metric";   
   jsonBuffer = httpGETRequest(serverPath.c_str()); 
   Serial.println(jsonBuffer); 
   JSONVar weatherData = JSON.parse(jsonBuffer); 

   if (JSON.typeof(weatherData) == "undefined") { 
    Serial.println("Parsing input failed!"); 
    return; 
   } 

   Serial.print("JSON object = "); 
   Serial.println(weatherData); 
   Serial.print("Temperature: "); 
   Serial.println(weatherData["main"]["temp"]); 
   Serial.print("Pressure: "); 
   Serial.println(weatherData["main"]["pressure"]); 
   Serial.print("Humidity: "); 
   Serial.println(weatherData["main"]["humidity"]); 
   Serial.print("Wind Speed: "); 
   Serial.println(weatherData["wind"]["speed"]); 

  } else { 

   Serial.println("WiFi Disconnected"); 
  } 

  delay(120000); 

 } 

String httpGETRequest(const char* serverName) { 

 WiFiClient client; 
 HTTPClient http; 

 http.begin(client, serverName); 

 int httpResponseCode = http.GET(); 

 String payload = "{}";  

 if (httpResponseCode > 0) { 
  Serial.print("HTTP Response code: "); 
  Serial.println(httpResponseCode); 

  payload = http.getString(); 

 } else { 

  Serial.print("HTTP Request failed. Error code: "); 

  Serial.println(httpResponseCode); 

 } 

 http.end(); 

 return payload;
} 


