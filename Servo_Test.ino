#include <Servo.h>

Servo XServo;
Servo YServo;

const int LaserPin = 12;
const int XServoPin = 2;
const int YServoPin = 3;

void setup() {
  // Initialize LaserPin as output
  pinMode(LaserPin, OUTPUT);

  // Attach servos to their respective pins
  XServo.attach(XServoPin);
  YServo.attach(YServoPin);

  // Begin serial communication
  Serial.begin(9600);
}

void loop() {
  delay(2000);

  // Move both servos to midpoint (90 degrees)
  XServo.write(90);
  YServo.write(90);
  digitalWrite(LaserPin, HIGH);
}
