#include <WiFi.h>
#include <HTTPClient.h>
#include <UrlEncode.h>

//WiFi Credentitals
#define WIFI_SSID "SQUARED_TECH_SOLUTIONS-2"
#define WIFI_PASSWORD "STS@2024"

//connectWiFi() Function
void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void setup() {
  Serial.begin(115200);
  connectWiFi();
  
}

void loop() {
  
  sendWhatsAppMessage("Hello Naol on Whatsup");
  sendTelegramMessage("Hello Naol on Telegram");
  delay(20000);
}

void sendWhatsAppMessage(const String& message) {
  String apiUrl = "https://api.callmebot.com/whatsapp.php";
  String phoneNumber = "+251977245580"; // Replace with the desired phone number
  String apiKey = "5598810"; // Replace with your API key
  String url = apiUrl + "?phone=" + phoneNumber + "&text=" + urlEncode(message) + "&apikey=" + apiKey;
  HTTPClient http;
  http.begin(url);
  int httpResponseCode = http.GET();
  if (httpResponseCode == 200) {
    Serial.println("WhatsApp message sent successfully!");
  } else {
    Serial.print("Error sending WhatsApp message. HTTP code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

void sendTelegramMessage(const String& message) {
  String apiUrl = "http://api.callmebot.com/text.php";
  String user = "Naol_Mergia"; // Replace with your user ID
  String url = apiUrl + "?user=" + user + "&text=" + urlEncode(message);
  HTTPClient http;
  http.begin(url);
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    Serial.println("Telegram message sent successfully!");
  } else {
    Serial.print("Error sending Telegram message. HTTP code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}