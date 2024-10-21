
/*
#include <Arduino.h>
#include <SoftwareSerial.h>
SoftwareSerial SIM900(32, 33); // RX, TX pins for SIM900 Shield
const int baudRate = 9600;
void setup() {
  Serial.begin(115200);
  SIM900.begin(baudRate);
  // Wait for the SIM900 module to initialize
  delay(1000);
  Serial.println("Initializing SIM900...");

  //sendATCommand("AT+CPIN?");
}
void loop() {
  // Your code goes here
  delay(1000);
  sendATCommand("AT+CMGF=1");
  delay(1000);
}
*/
//sendATCommand("AT+CMGF=1");
/*
void sendATCommand(String command) {
  SIM900.println(command);
  delay(500);
  while (SIM900.available()) {
    Serial.write(SIM900.read());
  }
}
*/
/*

*/

#include <Arduino.h>

HardwareSerial mySerial(0);
HardwareSerial mySerial1(1);

void setup()
{
  mySerial.begin(115200, SERIAL_8N1); // Default config
  mySerial1.begin(9600, SERIAL_8E1, 32, 33); // Set Serial Communication config and reassign pins to 32, 33

}

void loop()
{
sendATCommand("AT");
}

void sendATCommand(String cmd)
{
  mySerial1.println(cmd);
  delay(1000); // Wait for the command to be processed
  updateSerial();
}


void updateSerial()
{
    while (mySerial.available())
  {
    // Do your code
    mySerial1.write(mySerial.read()); // Forward data to SIM900 port
  }
  while (mySerial1.available())
  {
    // Do your code
    mySerial.write(mySerial1.read()); // Forward data to Serial port
  }
}