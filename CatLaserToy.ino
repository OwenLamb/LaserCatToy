#include <ArduinoBLE.h>
#include <Servo.h>

Servo XServo;
Servo YServo;

const int LaserPin = 12;
const int XServoPin = 3;
const int YServoPin = 2;

bool LaserSwitch = false;
int XValue = 90;
int YValue = 130;
int tempX;
int tempY;
int timer = millis();

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

  XServo.write(XValue);
  YServo.write(YValue);

  BLE.setLocalName("ArduinoBLE");
  BLE.setAdvertisedService(customService);
  customService.addCharacteristic(customCharacteristic);
  BLE.addService(customService);

  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
}

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
        if (data[0] == 49) {
          LaserSwitch = true;
          //Serial.println("Inside"); -- debug line
          digitalWrite(LaserPin, HIGH);
          timer = millis();
        } else if (data[0] == 48) {
          LaserSwitch = false;
          digitalWrite(LaserPin, LOW);
        }

        Serial.println(strData);
      }

      //if laser switch has been activated and it hasn't been 5 minutes yet then move laser
      //if it's been more than 5 mins then disconnect from bluetooth.
      if (LaserSwitch && (millis() - timer < 300000)) {
        MoveLaser();
      } else if (millis() - timer > 300000) {
        central.disconnect();
      }
    }
    Serial.println("Disconnected from central");
  }
}

void MoveLaser() {
  //Servos will move randomly between the specified bounds below
  //This way the amount it moves is smaller so it's easier to follow
  do {
    tempX = random(0, 10) - 5;
    tempY = random(0, 6) - 3;
} while (!((40 <= (tempX + XValue) && (tempX + XValue) <= 100) && (110 <= (tempY + YValue) && (tempY + YValue) <= 130)));

XValue = tempX + XValue;
YValue = tempY + YValue;


  
  XValue = tempX + XValue;
  YValue = tempY + YValue;

  XServo.write(XValue);
  YServo.write(YValue);
  Serial.print("X Value: ");
  Serial.println(XValue);
  Serial.print("Y Value: ");
  Serial.println(YValue);
  delay(500);
}
