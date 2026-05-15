#include <Servo.h>

#define TRIG_PIN    10
#define ECHO_PIN    2
#define SERVO_PIN   5

#define DETECT_DIST 20
#define TOO_CLOSE   4

#define MOTOR_SPEED 120
#define TURN_SPEED  140

#define INVERT_RIGHT  -1
#define INVERT_LEFT   1

int enableRightMotor = 3;
int rightMotorPin1   = 8;
int rightMotorPin2   = 7;

int enableLeftMotor  = 9;
int leftMotorPin1    = 6;
int leftMotorPin2    = 4;

Servo scanServo;

long distLeft   = 0;
long distCenter = 0;
long distRight  = 0;

void setup() {
  Serial.begin(9600);

  pinMode(enableRightMotor, OUTPUT);
  pinMode(rightMotorPin1,   OUTPUT);
  pinMode(rightMotorPin2,   OUTPUT);

  pinMode(enableLeftMotor,  OUTPUT);
  pinMode(leftMotorPin1,    OUTPUT);
  pinMode(leftMotorPin2,    OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  scanServo.attach(SERVO_PIN);
  scanServo.write(115);
  delay(500);

  rotateMotor(0, 0);
  delay(2000);
}

long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  long distance = duration * 0.034 / 2;
  return distance;
}

long scanAt(int angle) {
  scanServo.write(angle);
  delay(300);
  return getDistance();
}

void loop() {
  distLeft   = scanAt(160);
  distCenter = scanAt(115);
  distRight  = scanAt(70);

  bool leftDetected   = (distLeft   > 0 && distLeft   <= DETECT_DIST);
  bool centerDetected = (distCenter > 0 && distCenter <= DETECT_DIST);
  bool rightDetected  = (distRight  > 0 && distRight  <= DETECT_DIST);

  if (!leftDetected && !centerDetected && !rightDetected) {
    rotateMotor(0, 0);
    return;
  }

  if (centerDetected) {
    if (distCenter < TOO_CLOSE) {
      rotateMotor(-MOTOR_SPEED, -MOTOR_SPEED);
      delay(200);
      rotateMotor(0, 0);
      delay(100);
    } else {
      rotateMotor(MOTOR_SPEED, MOTOR_SPEED);
      delay(200);
      rotateMotor(0, 0);
      delay(100);
    }
    return;
  }

  // ✅ FIXED HERE: swapped turning directions
  if (leftDetected && rightDetected) {
    if (distLeft <= distRight) {
      // object on LEFT → turn LEFT
      rotateMotor(-MOTOR_SPEED, MOTOR_SPEED);
    } else {
      // object on RIGHT → turn RIGHT
      rotateMotor(MOTOR_SPEED, -MOTOR_SPEED);
    }
    delay(250);
    rotateMotor(0, 0);
    delay(100);
    return;
  }

  if (leftDetected) {
    // object on LEFT → turn LEFT
    rotateMotor(-MOTOR_SPEED, MOTOR_SPEED);
    delay(250);
    rotateMotor(0, 0);
    delay(100);
    return;
  }

  if (rightDetected) {
    // object on RIGHT → turn RIGHT
    rotateMotor(MOTOR_SPEED, -MOTOR_SPEED);
    delay(250);
    rotateMotor(0, 0);
    delay(100);
    return;
  }
}

void rotateMotor(int rightMotorSpeed, int leftMotorSpeed) {
  rightMotorSpeed *= INVERT_RIGHT;
  leftMotorSpeed  *= INVERT_LEFT;

  if (rightMotorSpeed < 0) {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, HIGH);
  } else if (rightMotorSpeed > 0) {
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);
  } else {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, LOW);
  }

  if (leftMotorSpeed < 0) {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, HIGH);
  } else if (leftMotorSpeed > 0) {
    digitalWrite(leftMotorPin1, HIGH);
    digitalWrite(leftMotorPin2, LOW);
  } else {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, LOW);
  }

  analogWrite(enableRightMotor, abs(rightMotorSpeed));
  analogWrite(enableLeftMotor,  abs(leftMotorSpeed));
}