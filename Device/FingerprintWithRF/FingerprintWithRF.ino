#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>  // RFID Library
#include <Adafruit_Fingerprint.h>  // Fingerprint sensor library
#include <HardwareSerial.h>  // For Serial communication on ESP32


// macro for selecting the core of stensaw processor of esp32  which is two core processors
#if CONFIG_FREERTOS_UNICORE
// The number one core processor
#define ARDUINO_RUNNING_CORE 0
#else
// The number two core processor
#define ARDUINO_RINNING_CORE 1
#endif


// Define pin configurations
#define RST_PIN  22      // Reset pin for RFID
#define SS_PIN   21      // Slave select pin for RFID
#define FINGERPRINT_RX 16 // RX pin for fingerprint
#define FINGERPRINT_TX 17 // TX pin for fingerprint
#define ENGINE_START_PIN 2  // Pin to start the engine
#define NORMAL_ENGINE_CUT 4    // Pin to cut off the engine
#define EMERGENCY_BUTTON_PIN 15  // Emergency button pin
#define HARD_ENGINE_CUT  27
#define notReady_LED 26 // Not ready to start LED (blue) pin
#define Ready_LED 25  // Ready to start (Green) pin
#define Emergency_Pressed 5 // to know if the driver is succesfully pressed the emergency button
#define CALL_DURATION 30000 // Call duration 30 seconds in milliseconds


// Authenticated phone number
const char authPhone[] = "+251977245580";

// Emergency Car Details
const String carID = "FP1291(Fikremariam)";  // Car ID or number
const String emergencyCarNumber = "+251977245580";  // Emergency car number

const byte off = 1;
const byte on = 0;

//volatile bool buttonPressed = false; // Flag to indicate button press

// RFID and Fingerprint objects
MFRC522 rfid(SS_PIN, RST_PIN);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);


// User-specific RFID tags
byte FikremariamRfid[] = {0x1D, 0x8F, 0x9F, 0x83};
byte SamuelRfid[] = {0x04, 0x16, 0xDA, 0xEA, 0x58, 0x13, 0x90};
byte NaolRfid[] = {0x03, 0x0F, 0x67, 0x3B};

// Banned RFID tags
byte bannedRfidTags[][4] = {
    {0x1D, 0x8F, 0x9F, 0x82},  // Example banned RFID tag 1
    {0x55, 0x66, 0x77, 0x88}   // Example banned RFID tag 2
};

// User-specific Fingerprint IDs
byte NaolId[] = {1, 2};
byte SamuelId[] = {3, 4};
byte AronId[] = {5, 6};
byte FikremariamId[] = {22, 21};

// Banned Fingerprint IDs
byte bannedFingerprintIds[] = {12, 8, 9};  // Example banned fingerprint IDs

// Global Variables
String users[] = {"Naol", "Samuel", "Aron", "Fikremariam"};
int userID = -1;  // Variable to store authenticated user ID
bool engineStart = false;  // Engine status
bool hardCutEngine = false;
bool engineState;           // Variable to store the engine state

byte sendingSMS = 0; // Testing if SMS is sending, by default is not sending.

SoftwareSerial SIM900(33, 32);  // RX (33), TX (32)
//void IRAM_ATTR handleButtonPress();


void taskFunction2(void * pvParameters){

  (void) pvParameters;
  
  while(true){
    //code excuited by the task
    //This could be reading a sensor, controlling an output, ..etc
    
    // Check for emergency button press
    checkEmergencyButton();
    // the time is calculated in terms of ticks per milliseconds
  
  }
}


void setup() {
    EEPROM.begin(1);
  // Setup engine control pins
    pinMode(HARD_ENGINE_CUT, OUTPUT);
    pinMode(ENGINE_START_PIN, OUTPUT);
    pinMode(NORMAL_ENGINE_CUT, OUTPUT);
    pinMode(notReady_LED, OUTPUT);
    pinMode(Ready_LED, OUTPUT);
    pinMode(Emergency_Pressed, OUTPUT);
    digitalWrite(Ready_LED, LOW);
    digitalWrite(notReady_LED, HIGH);
    digitalWrite(Emergency_Pressed, LOW);
    digitalWrite(ENGINE_START_PIN, off); // Ensure engine start pin is off
    digitalWrite(NORMAL_ENGINE_CUT, off);// Ensure engine cut pin is off

    engineState = EEPROM.read(0);  // Read value from address 0
    digitalWrite(HARD_ENGINE_CUT, engineState);  // Set the pin to the saved state

    // Initialize serial communication
    Serial.begin(115200);
    while (!Serial);        // Wait for serial port to be available
    SIM900.begin(9600);     // SIM900 communication on 9600 baud rate
    Serial.println("ESP32 SIM900 Initialized with SoftwareSerial");
    initModule();
    // Initialize SPI bus for RFID
    SPI.begin();
    rfid.PCD_Init();
    
    // Initialize the fingerprint sensor
    Serial2.begin(57600, SERIAL_8N1, FINGERPRINT_RX, FINGERPRINT_TX);
    if (finger.verifyPassword()) {
        Serial.println("Fingerprint sensor detected.");
    } else {
        Serial.println("Fingerprint sensor not detected.");
        while (1); // Halt execution if fingerprint sensor is not found
    }
    if (hardCutEngine == false){
    digitalWrite(notReady_LED, LOW);
    Serial.println("NotReady LOW.");
    digitalWrite(Ready_LED, HIGH);
    Serial.println("Ready HIGH.");
    digitalWrite(Emergency_Pressed, LOW);
    Serial.println("Emergency Pressed LOW.");
    }

    while (hardCutEngine == true){
      delay(500);

      digitalWrite(Emergency_Pressed, HIGH);
      Serial.println("Emergency Pressed HIGH.");

    }

    // Setup emergency button pin
    pinMode(EMERGENCY_BUTTON_PIN, INPUT_PULLUP); // Use internal pull-up resistor

    //attachInterrupt(digitalPinToInterrupt(EMERGENCY_BUTTON_PIN), handleButtonPress, FALLING); // Attach interrupt
    
    // create the task
    // taskName, descriptive name, stack depth, Pv parameter, priority, pointer to a task_T, arduino_running_core

    //xTaskCreatePinnedToCore(taskFunction1, "Task1", 1024, NULL, 2, NULL,ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(taskFunction2, "Task2", 1024, NULL, 1, NULL,ARDUINO_RUNNING_CORE);


    // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.


    Serial.println("Setup complete.");
}

void loop() {
    if (SIM900.available()) {
    String message = SIM900.readString();
    Serial.println("Message received: " + message);

    // Check if message is from the authenticated number
    if (message.indexOf(authPhone) != -1) {

      if (message.indexOf("CUTENGINE") != -1){
        engineState = LOW;        // Set the state to HIGH
      digitalWrite(HARD_ENGINE_CUT, engineState);
      EEPROM.write(0, engineState);  // Save the state to EEPROM at address 0
      EEPROM.commit();               // Commit the changes to ensure they are saved
        hardCutEngine = true;
        Serial.println("ENGINE IS ON CUT STATUS YOU CAN'T START THE CAR WITHOUT PERMISSION");
      }
      else if (message.indexOf("STARTENGINE") != -1) {
        engineState = HIGH;         // Set the state to LOW
      digitalWrite(HARD_ENGINE_CUT, engineState);
      EEPROM.write(0, engineState);  // Save the state to EEPROM at address 0
      EEPROM.commit();               // Commit the changes to ensure they are saved
        hardCutEngine = false;
        Serial.println("Welcome back");
      }
      else if ((message.indexOf("START") != -1) && (hardCutEngine = false)) {
        Serial.println("START Engine command received");
        startEngineBySms();
      }

      else if ((message.indexOf("STOP") != -1) && (!hardCutEngine)) {
        Serial.println("STOP command received");
        cutEngineBySms();
      }
    }
  }

    // Check for emergency button press
    //checkEmergencyButton();

    // Check for user authentication by RFID or Fingerprint
    bool authenticated = authenticateUser();

    if ((authenticated)&&(!hardCutEngine)) {
      sendingSMS = 1;
        if (engineStart) {
            normalCutEngine();  // Turn off the engine
            sendSMSstop();
        } else {
            startEngine(); // Turn on the engine
            sendSMSstart();
        }
      sendingSMS = 0;
    }

    delay(1000); // Delay to avoid flooding the loop

    
}

// Interrupt Service Routine
/*void IRAM_ATTR handleButtonPress() {
    //buttonPressed = true; // Set the flag when the button is pressed
}*/

void checkEmergencyButton() {
    //if (buttonPressed) {  // Button pressed
    if (digitalRead(EMERGENCY_BUTTON_PIN) == LOW) {  // Button pressed
        //buttonPressed = false; // Reset the flag
        // Handle button press event
        Serial.println("Button pressed!");
        Serial.print("Emergency! Car ID: ");
        Serial.print(carID);
        Serial.print(" | Emergency Contact: ");
        Serial.print(emergencyCarNumber);
        Serial.print(" | Date and Time: ");
        Serial.print("dd");
        Serial.print("/");
        Serial.print("mm");
        Serial.print("/");
        Serial.print("yyyy");
        Serial.println(" ");
        digitalWrite(Ready_LED, LOW);
        delay(50);
        digitalWrite(Emergency_Pressed, HIGH);
        delay(100);
        digitalWrite(Emergency_Pressed, LOW);
        digitalWrite(notReady_LED, HIGH);
        delay(100);
        digitalWrite(notReady_LED, LOW);
        digitalWrite(Emergency_Pressed, HIGH);
        delay(100);
        digitalWrite(notReady_LED, HIGH);
        delay(100);
        digitalWrite(notReady_LED, LOW);
        digitalWrite(Emergency_Pressed, LOW);
        delay(50);
        digitalWrite(Emergency_Pressed, HIGH);
        delay(140);
        digitalWrite(Emergency_Pressed, LOW);
        delay(600);                               // Debounce delay
        digitalWrite(Ready_LED, HIGH);
        makeCall(authPhone);
        delay(100);
    }
}

bool authenticateUser() {
    // Check for fingerprint authentication
    int fingerprintID = getFingerprintID();
    if (fingerprintID != -1) {
        if (isBannedFingerprint(fingerprintID)) {
            logBannedAttempt("Fingerprint");
            return false;
        }
        userID = matchFingerprintID(fingerprintID);
        if (userID != -1) {
            logEvent("Fingerprint");
            return true;
        } else {
            logUnknownAttempt("Fingerprint");  // Log unknown fingerprint attempt
            return false;
        }
    }

    // Check for RFID authentication
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        if (isBannedRfid(rfid.uid.uidByte, rfid.uid.size)) {
            logBannedAttempt("RFID");
            rfid.PICC_HaltA();  // Halt RFID card
            return false;
        }
        userID = matchRFID(rfid.uid.uidByte, rfid.uid.size);
        rfid.PICC_HaltA();  // Halt RFID card
        if (userID != -1) {
            logEvent("RFID");
            return true;
        } else {
            logUnknownAttempt("RFID");  // Log unknown RFID attempt
            return false;
        }
    }

    return false;  // No authentication detected
}

int getFingerprintID() {
    int id = finger.getImage();
    if (id == FINGERPRINT_OK) {
        id = finger.image2Tz();
        if (id == FINGERPRINT_OK) {
            id = finger.fingerFastSearch();
            if (id == FINGERPRINT_OK) {
                return finger.fingerID;  // Return the fingerprint ID
            }
        }
    }
    return -1; // No valid fingerprint detected
}

bool isBannedFingerprint(int id) {
    for (byte i = 0; i < sizeof(bannedFingerprintIds); i++) {
        if (bannedFingerprintIds[i] == id) {
            return true; // ID is banned
        }
    }
    return false; // ID is not banned
}

bool isBannedRfid(byte *uid, byte size) {
    for (byte i = 0; i < sizeof(bannedRfidTags) / sizeof(bannedRfidTags[0]); i++) {
        if (memcmp(uid, bannedRfidTags[i], size) == 0) {
            return true; // RFID tag is banned
        }
    }
    return false; // RFID tag is not banned
}

int matchRFID(byte *uid, byte size) {
    if (size == sizeof(FikremariamRfid) && memcmp(uid, FikremariamRfid, size) == 0) {
        return 3; // Fikremariam
    } else if (size == sizeof(SamuelRfid) && memcmp(uid, SamuelRfid, size) == 0) {
        return 1; // Samuel
    } else if (size == sizeof(NaolRfid) && memcmp(uid, NaolRfid, size) == 0) {
        return 0; // Naol
    }
    Serial.println("ID: "+String(uid[0])+" "+String(uid[1])+" " +String(uid[2])+" " +String(uid[3])+" Done" );
    return -1; // No match found
}

int matchFingerprintID(int id) {
    for (byte i = 0; i < sizeof(NaolId); i++) {
        if (NaolId[i] == id) {
            return 0; // Naol
        }
    }
    for (byte i = 0; i < sizeof(SamuelId); i++) {
        if (SamuelId[i] == id) {
            return 1; // Samuel
        }
    }
    for (byte i = 0; i < sizeof(AronId); i++) {
        if (AronId[i] == id) {
            return 2; // Aron
        }
    }
    for (byte i = 0; i < sizeof(FikremariamId); i++) {
        if (FikremariamId[i] == id) {
            return 3; // Fikremariam
        }
    }
    return -1; // No match found
}

void startEngine() {
    // Trigger the engine start pin
    digitalWrite(ENGINE_START_PIN, on);
    delay(800);  // Keep the start pin high for 800ms
    digitalWrite(ENGINE_START_PIN, off);
    
    engineStart = true;  // Set engine status to on
    Serial.print(users[userID]);
    Serial.print(" has started the engine on ");
    Serial.print("dd");
    Serial.print("/");
    Serial.print("mm");
    Serial.print("/");
    Serial.print("yyyy");
    Serial.println(" ");
}

void startEngineBySms() {
    // Trigger the engine start pin
    if (!engineStart){
    digitalWrite(ENGINE_START_PIN, on);
    delay(800);  // Keep the start pin high for 800ms
    digitalWrite(ENGINE_START_PIN, off);
    
    engineStart = true;  // Set engine status to on
    Serial.println("Engine started By SMS Command");
    sendSMSstart();
    }
    else if(engineStart){
      Serial.println("Engine Already started No need to start the engine");
    }
}

void normalCutEngine() {
    // Trigger the engine cut pin
    digitalWrite(NORMAL_ENGINE_CUT, on);
    delay(2500);  // Keep the cut pin high for 2.5 second
    digitalWrite(NORMAL_ENGINE_CUT, off);
    
    engineStart = false;  // Set engine status to off
    Serial.print(users[userID]);
    Serial.print(" has cut the engine on ");
    Serial.print("dd");
    Serial.print("/");
    Serial.print("mm");
    Serial.print("/");
    Serial.print("yyyy");
    Serial.println(" ");
 
}

void cutEngineBySms() {
    // Trigger the engine cut pin
    if(engineStart){
    digitalWrite(NORMAL_ENGINE_CUT, off);
    delay(3000);  // Keep the cut pin high for 3 second
    digitalWrite(NORMAL_ENGINE_CUT, on);
    
    engineStart = false;  // Set engine status to off
    Serial.println("Engine cut-off By Text command");
    sendSMSstop();
    }
    else if(!engineStart){
      Serial.println("Engine already cut-off no need to cut-off the Engine!");
    }
}

void logEvent(String method) {
    Serial.print(users[userID]);
    Serial.print(" authenticated using ");
    Serial.print(method);
    Serial.print(" on ");
    Serial.print("dd");
    Serial.print("/");
    Serial.print("mm");
    Serial.print("/");
    Serial.print("yyyy");
    Serial.println(" ");
}

void logUnknownAttempt(String method) {
    Serial.print("Unknown ");
    Serial.print(method);
    Serial.print(" tried to access the car on ");
    Serial.print("dd");
    Serial.print("/");
    Serial.print("mm");
    Serial.print("/");
    Serial.print("yyyy");
    Serial.println(" ");
}

void logBannedAttempt(String method) {
    Serial.print("Banned driver tried to access the car using ");
    Serial.print(method);
    Serial.print(" on ");
    Serial.print("dd");
    Serial.print("/");
    Serial.print("mm");
    Serial.print("/");
    Serial.print("yyyy");
    Serial.println(" ");
}
void initModule() {
  SIM900.println("AT");
  delay(1000);
  SIM900.println("AT+CMGF=1"); // Set SMS mode to text
  delay(1000);
  SIM900.println("AT+CNMI=1,2,0,0,0"); // Configure SIM900 to send incoming SMS to serial
  delay(1000);
}

void makeCall(const char* number) {
  Serial.println("Calling: " + String(number));
  while(sendingSMS);// while we are sending SMS Suspend the call and call letter!
  delay(500);
  SIM900.print("ATD");
  SIM900.print(number);
  SIM900.println(";"); // Initiate call

  // Keep the call active for 1 minute
  delay(CALL_DURATION);

  // Hang up the call
  SIM900.println("ATH");
  Serial.println("Call ended");
}

void sendSMSstart() {
  // Test communication with the SIM900
  delay(500);
  SIM900.println("AT");
  delay(500);
  if (SIM900.available()) {
    Serial.println(SIM900.readString());  // Read and display response from SIM900
  }

  // Set SMS mode to text
  sendATCommand("AT+CMGF=1", 1000);  // Set SIM900 to SMS mode

  // Send SMS to the specified number
  sendATCommand("AT+CMGS=\"+251977245580\"", 1000);  // Replace with your phone number
  delay(500);
  
  SIM900.println("ENGINE IS STARTED!");  // Message content
  delay(500);

  SIM900.write(26);  // ASCII code for CTRL+Z to send the message
  delay(5000);

  Serial.println("Message Sent!");

}

void sendSMSstop() {

  // Test communication with the SIM900
  delay(500);
  SIM900.println("AT");
  delay(500);
  if (SIM900.available()) {
    Serial.println(SIM900.readString());  // Read and display response from SIM900
  }

  // Set SMS mode to text
  sendATCommand("AT+CMGF=1", 500);  // Set SIM900 to SMS mode

  // Send SMS to the specified number
  sendATCommand("AT+CMGS=\"+251977245580\"", 100);  // Replace with your phone number
  delay(500);
  
  SIM900.println("ENGINE IS STOPED!");  // Message content
  delay(500);

  SIM900.write(26);  // ASCII code for CTRL+Z to send the message
  delay(5000);

  Serial.println("Message Sent!");
   
}

void sendATCommand(String cmd, int delayTime) {
  SIM900.println(cmd);  // Send AT command to SIM900
  delay(delayTime);     // Wait for a response
  if (SIM900.available()) {
    Serial.println(SIM900.readString());  // Print out the response
  }
}