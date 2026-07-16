#include <Servo.h>

// Indoor Security Rover - controlled hardware test
// ELEGOO Smart Robot Car V4.0 shield pin assignments.

// Motor driver pins
const uint8_t RIGHT_MOTOR_PWM = 5;
const uint8_t LEFT_MOTOR_PWM  = 6;
const uint8_t RIGHT_MOTOR_DIR = 7;
const uint8_t LEFT_MOTOR_DIR  = 8;
const uint8_t MOTOR_STANDBY   = 3;

// Pan-tilt servo pins
const uint8_t PAN_SERVO_PIN  = 10;
const uint8_t TILT_SERVO_PIN = 11;

// Front ultrasonic sensor (shield ultrasonic connector)
const uint8_t FRONT_TRIG_PIN = 13;
const uint8_t FRONT_ECHO_PIN = 12;

// Rear ultrasonic sensor (unused line-tracking inputs)
const uint8_t REAR_TRIG_PIN = A0;
const uint8_t REAR_ECHO_PIN = A1;

// Conservative starting values
const uint8_t TEST_SPEED = 110;
const unsigned long MOTOR_TEST_TIME_MS = 300;
const uint16_t FRONT_STOP_DISTANCE_CM = 25;
const uint16_t REAR_STOP_DISTANCE_CM = 20;

Servo panServo;
Servo tiltServo;

unsigned long lastDistanceReport = 0;

uint16_t readDistanceCm(uint8_t triggerPin, uint8_t echoPin)
{
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  const unsigned long duration = pulseIn(echoPin, HIGH, 20000UL);

  // Zero means no echo was received. Treat that as invalid, not clear.
  if (duration == 0) {
    return 0;
  }

  return static_cast<uint16_t>(duration / 58UL);
}

void stopMotors()
{
  analogWrite(RIGHT_MOTOR_PWM, 0);
  analogWrite(LEFT_MOTOR_PWM, 0);
  digitalWrite(MOTOR_STANDBY, LOW);
}

void driveMotors(int16_t leftSpeed, int16_t rightSpeed)
{
  const uint8_t leftPwm = constrain(abs(leftSpeed), 0, 255);
  const uint8_t rightPwm = constrain(abs(rightSpeed), 0, 255);

  if (leftPwm == 0 && rightPwm == 0) {
    stopMotors();
    return;
  }

  digitalWrite(MOTOR_STANDBY, HIGH);
  digitalWrite(LEFT_MOTOR_DIR, leftSpeed >= 0 ? HIGH : LOW);
  digitalWrite(RIGHT_MOTOR_DIR, rightSpeed >= 0 ? HIGH : LOW);
  analogWrite(LEFT_MOTOR_PWM, leftPwm);
  analogWrite(RIGHT_MOTOR_PWM, rightPwm);
}

void runMotorPulse(int16_t leftSpeed, int16_t rightSpeed)
{
  driveMotors(leftSpeed, rightSpeed);
  delay(MOTOR_TEST_TIME_MS);
  stopMotors();
}

void reportDistances()
{
  const uint16_t frontDistance = readDistanceCm(FRONT_TRIG_PIN, FRONT_ECHO_PIN);

  // Prevent the rear sensor from hearing the front sensor's pulse.
  delay(50);

  const uint16_t rearDistance = readDistanceCm(REAR_TRIG_PIN, REAR_ECHO_PIN);

  Serial.print(F("Front: "));
  if (frontDistance == 0) {
    Serial.print(F("no echo"));
  } else {
    Serial.print(frontDistance);
    Serial.print(F(" cm"));
  }

  Serial.print(F(" | Rear: "));
  if (rearDistance == 0) {
    Serial.println(F("no echo"));
  } else {
    Serial.print(rearDistance);
    Serial.println(F(" cm"));
  }
}

void printCommands()
{
  Serial.println(F("Indoor Security Rover hardware test"));
  Serial.println(F("Raise the drive wheels before motor testing."));
  Serial.println(F("F = forward pulse"));
  Serial.println(F("B = backward pulse"));
  Serial.println(F("L = left-turn pulse"));
  Serial.println(F("R = right-turn pulse"));
  Serial.println(F("X = stop"));
  Serial.println(F("C = center camera"));
  Serial.println(F("J/K = pan left/right"));
  Serial.println(F("I/M = tilt up/down"));
  Serial.println(F("D = report both distances"));
}

void handleCommand(char command)
{
  switch (command) {
    case 'F': {
      const uint16_t frontDistance = readDistanceCm(FRONT_TRIG_PIN, FRONT_ECHO_PIN);
      if (frontDistance >= FRONT_STOP_DISTANCE_CM) {
        runMotorPulse(TEST_SPEED, TEST_SPEED);
      } else {
        stopMotors();
        Serial.println(F("Forward blocked: front obstacle or invalid reading."));
      }
      break;
    }

    case 'B': {
      const uint16_t rearDistance = readDistanceCm(REAR_TRIG_PIN, REAR_ECHO_PIN);
      if (rearDistance >= REAR_STOP_DISTANCE_CM) {
        runMotorPulse(-TEST_SPEED, -TEST_SPEED);
      } else {
        stopMotors();
        Serial.println(F("Reverse blocked: rear obstacle or invalid reading."));
      }
      break;
    }

    case 'L':
      runMotorPulse(-TEST_SPEED, TEST_SPEED);
      break;

    case 'R':
      runMotorPulse(TEST_SPEED, -TEST_SPEED);
      break;

    case 'X':
      stopMotors();
      break;

    case 'C':
      panServo.write(90);
      tiltServo.write(90);
      break;

    case 'J':
      panServo.write(75);
      break;

    case 'K':
      panServo.write(105);
      break;

    case 'I':
      tiltServo.write(75);
      break;

    case 'M':
      tiltServo.write(105);
      break;

    case 'D':
      reportDistances();
      break;

    default:
      break;
  }
}

void setup()
{
  // Keep the motor driver disabled throughout startup.
  pinMode(RIGHT_MOTOR_PWM, OUTPUT);
  pinMode(LEFT_MOTOR_PWM, OUTPUT);
  pinMode(RIGHT_MOTOR_DIR, OUTPUT);
  pinMode(LEFT_MOTOR_DIR, OUTPUT);
  pinMode(MOTOR_STANDBY, OUTPUT);
  stopMotors();

  pinMode(FRONT_TRIG_PIN, OUTPUT);
  pinMode(FRONT_ECHO_PIN, INPUT);
  pinMode(REAR_TRIG_PIN, OUTPUT);
  pinMode(REAR_ECHO_PIN, INPUT);

  digitalWrite(FRONT_TRIG_PIN, LOW);
  digitalWrite(REAR_TRIG_PIN, LOW);

  panServo.attach(PAN_SERVO_PIN, 500, 2400);
  tiltServo.attach(TILT_SERVO_PIN, 500, 2400);
  panServo.write(90);
  tiltServo.write(90);

  Serial.begin(9600);
  delay(500);
  printCommands();
}

void loop()
{
  if (Serial.available() > 0) {
    char command = Serial.read();

    if (command >= 'a' && command <= 'z') {
      command = command - ('a' - 'A');
    }

    handleCommand(command);
  }

  if (millis() - lastDistanceReport >= 1000UL) {
    lastDistanceReport = millis();
    reportDistances();
  }
}
