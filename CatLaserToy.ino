#include <ArduinoBLE.h>
#include <Servo.h>

Servo XServo;
Servo YServo;

const int LaserPin = 12;
const int XServoPin = 3;
const int YServoPin = 2;

BLEService customService("180F");  // Custom BLE Service
BLECharacteristic customCharacteristic("2A19", BLERead | BLEWrite, 20);

void setup() {
  Serial.begin(9600);
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1)
      ;
  }

  pinMode(LaserPin, OUTPUT);
  XServo.attach(XServoPin);
  YServo.attach(YServoPin);

  BLE.setLocalName("ArduinoBLE");
  BLE.setAdvertisedService(customService);
  customService.addCharacteristic(customCharacteristic);
  BLE.addService(customService);

  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
}

bool LaserSwitch = false;
int XRandom;
int YRandom;
int temp = 1;
int temp2 = 0;
int timer = millis();

void loop() {
  BLEDevice central = BLE.central();
  if (central) {
    //loops continously while bluetooth is still connected
    while (central.connected()) {
      if (customCharacteristic.written()) {
        const uint8_t* data = customCharacteristic.value();
        int dataLength = customCharacteristic.valueLength();

        //Convert to String -- dont really need this since we're taking one number but if you
        //wanted to send more than one digit or a string over this is how you'd make it usable.
        //it's just here for future use case
        String strData = "";
        for (int i = 0; i < dataLength; i++) {
          strData += (char)data[i];
        }

        //Serial.println(data[0]);
        //Recieving input is in Decimal ASCII so 49 for 1 and 48 for 0
        //Used data directly instead of strData. Was having weird issues comparing the string.
        if(data[0] == 49){
          LaserSwitch = true;
          //Serial.println("Inside"); -- debug line
          digitalWrite(LaserPin, HIGH);
          timer = millis();
        } else if(data[0] == 48){
          LaserSwitch = false;
          digitalWrite(LaserPin, LOW);
        }

        Serial.println(strData);
      }

      //if laser switch has been activated and it hasn't been 5 minutes yet then move laser
      //if it's been more than 5 mins then disconnect from bluetooth.
      if(LaserSwitch && (millis() - timer < 300000)){
        MoveLaser();
      } else if(millis() - timer > 300000){
        central.disconnect();
      }
    }
    Serial.println("Disconnected from central");
  }

}

void MoveLaser(){
  //Servos will move randomly between the specified bounds below
    XRandom = random(6, 12);
    YRandom = random(10, 15);
    XRandom = XRandom * 10;
    YRandom = YRandom * 10;

    XServo.write(XRandom);
    YServo.write(YRandom);

    delay(2500);
}