/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-rfid-nfc
 */

#include <SPI.h>
#include <MFRC522.h>

#define LED 2
#define SS_PIN  21  // ESP32 pin GPIO5 
#define SS_PIN2  12  // ESP32 pin GPIO12
#define RST_PIN 22 // ESP32 pin GPIO27 
#define RST_PIN2 25 // ESP32 pin GPIO25 

MFRC522 rfid0(SS_PIN, RST_PIN);
//MFRC522 rfid1(SS_PIN2, RST_PIN2);
byte tag [5];
byte testTag[5] = {0x1D, 0x8F, 0x9F, 0x82}; 
void setup() {
  Serial.begin(115200);
  SPI.begin(); // init SPI bus
  rfid0.PCD_Init(); // init MFRC522
  //rfid1.PCD_Init(); // init MFRC522

  Serial.println("Tap an RFID/NFC tag on the RFID-RC522 reader");
  pinMode(LED,OUTPUT);
}

void loop() {
  if (rfid0.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid0.PICC_ReadCardSerial()) { // NUID has been readed
      MFRC522::PICC_Type piccType = rfid0.PICC_GetType(rfid0.uid.sak);
      Serial.print("RFID/NFC Tag Type: ");
      Serial.println(rfid0.PICC_GetTypeName(piccType));

      // print UID in Serial Monitor in the hex format
      Serial.print("UID:");
      for (int i = 0; i < rfid0.uid.size; i++) {
        Serial.print(rfid0.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid0.uid.uidByte[i], HEX);
        tag[i] = rfid0.uid.uidByte[i];

      }
      if(memcmp(tag, testTag, 4) == 0 ){
        digitalWrite(LED,HIGH);
      }
      Serial.println();

      rfid0.PICC_HaltA(); // halt PICC
      rfid0.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }
/*
  //______________________________________________________Second RFID____________________________

    if (rfid1.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid1.PICC_ReadCardSerial()) { // NUID has been readed
      MFRC522::PICC_Type piccType = rfid1.PICC_GetType(rfid1.uid.sak);
      Serial.print("RFID/NFC Tag Type: ");
      Serial.println(rfid1.PICC_GetTypeName(piccType));

      // print UID in Serial Monitor in the hex format
      Serial.print("UID:");
      for (int i = 0; i < rfid1.uid.size; i++) {
        Serial.print(rfid1.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid1.uid.uidByte[i], HEX);
        tag[i] = rfid1.uid.uidByte[i];
      }

      if(memcmp(tag, testTag, 4) == 0 ){
        digitalWrite(LED,LOW);
      }
      Serial.println();

      rfid1.PICC_HaltA(); // halt PICC
      rfid1.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }
*/

}
