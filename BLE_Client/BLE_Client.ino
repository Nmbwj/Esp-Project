#include <BLEDevice.h> 

#include <BLEServer.h> 

#include <BLEUtils.h> 

#include <BLE2902.h> 

#include <BLEBeacon.h> 

#define DEVICE_NAME "ESP32" 

#define SERVICE_UUID "7A0247E7-8E88-409B-A959-AB5092DDB03E" 

#define BEACON_UUID "2D7A9F0C-E0E8-4CC9-A71B-A21DB2D034A1" 

#define BEACON_UUID_REV "A134D0B2-1DA2-1BA7-C94C-E8E00C9F7A2D" 

#define CHARACTERISTIC_UUID "82258BAA-DF72-47E8-99BC-B73D7ECD08A5" 

BLEServer *pServer; 

BLECharacteristic *pCharacteristic; 

bool deviceConnected = false; 

uint8_t value = 0; 

class MyServerCallbacks: public BLEServerCallbacks { 
}; 

class MyCallbacks: public BLECharacteristicCallbacks { 
}; 

void init_service() { 
} 

void init_beacon() { 
} 

void setup() { 
} 

void loop() { 
} 