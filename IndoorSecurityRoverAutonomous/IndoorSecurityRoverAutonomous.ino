#include <Servo.h>

// Indoor Security Rover - front-sensor autonomous mode
// This version does not reverse because the rear sensor is unavailable.

// ELEGOO Smart Robot Car V4.0 motor-shield pins
const uint8_t RIGHT_MOTOR_PWM = 5;
const uint8_t LEFT_MOTOR_PWM  = 6;
const uint8_t RIGHT_MOTOR_DIR = 7;
const uint8_t LEFT_MOTOR_DIR  = 8;
const uint8_t MOTOR_STANDBY   = 3;

// Pan-tilt servos
const uint8_t PAN_SERVO_PIN  = 10;
const uint8_t TILT_SERVO_PIN = 11;

// Fixed front ultrasonic sensor
const uint8_t FRONT_TRIG_PIN = 13;
const uint8_t FRONT_ECHO_PIN = 12;

// Conservative indoor settings
const uint8_t DRIVE_SPEED = 100;
const uint8_t TURN_SPEED = 100;
const uint16_t STOP_DISTANCE_CM = 30;
const unsigned long TURN_TIME_MS = 450;
const unsigned long SENSOR_INTERVAL_MS = 100;

Servo panServo;
Servo tiltServo;

enum RoverState {
  STOPPED,
  CHECKING_PATH,
  MOVING_FORWARD,
  TURNING
};

RoverState roverState = STOPPED;
bool autonomousEnabled = false;
bool nextTurnIsLeft = true;
unsigned long turnStartedAt = 0;
unsigned long lastSensorCheck = 0;
uint16_t frontDistanceCm = 0;

uint16_t readFrontDistanceCm()
{
  digitalWrite(FRONT_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(FRONT_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(FRONT_TRIG_PIN, LOW);

  const unsigned long duration = pulseIn(FRONT_ECHO_PIN, HIGH, 20000UL);

  // No echo is treated as unsafe so the rover remains stopped.
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

void beginTurn()
{
  stopMotors();

  if (nextTurnIsLeft) {
    driveMotors(-TURN_SPEED, TURN_SPEED);
    Serial.println(F("Obstacle detected: turning left."));
  } else {
    driveMotors(TURN_SPEED, -TURN_SPEED);
    Serial.println(F("Obstacle detected: turning right."));
  }

  nextTurnIsLeft = !nextTurnIsLeft;
  turnStartedAt = millis();
  roverState = TURNING;
}

void printHelp()
{
  Serial.println(F("Indoor Security Rover autonomous test"));
  Serial.println(F("A = enable/disable autonomous mode"));
  Serial.println(F("X = emergency stop"));
  Serial.println(F("C = center camera"));
  Serial.println(F("J/K = pan left/right"));
  Serial.println(F("I/M = tilt up/down"));
  Serial.println(F("Autonomous mode starts DISABLED."));
}

void handleCommand(char command)
{
  if (command >= 'a' && command <= 'z') {
    command = command - ('a' - 'A');
  }

  switch (command) {
    case 'A':
      autonomousEnabled = !autonomousEnabled;
      stopMotors();

      if (autonomousEnabled) {
        roverState = CHECKING_PATH;
        Serial.println(F("Autonomous mode ENABLED."));
      } else {
        roverState = STOPPED;
        Serial.println(F("Autonomous mode DISABLED."));
      }
      break;

    case 'X':
      autonomousEnabled = false;
      roverState = STOPPED;
      stopMotors();
      Serial.println(F("EMERGENCY STOP."));
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

    default:
      break;
  }
}

void runAutonomousMode()
{
  if (!autonomousEnabled) {
    stopMotors();
    return;
  }

  const unsigned long now = millis();

  if (roverState == TURNING) {
    if (now - turnStartedAt >= TURN_TIME_MS) {
      stopMotors();
      roverState = CHECKING_PATH;
    }
    return;
  }

  if (now - lastSensorCheck < SENSOR_INTERVAL_MS) {
    return;
  }

  lastSensorCheck = now;
  frontDistanceCm = readFrontDistanceCm();

  Serial.print(F("Front: "));
  if (frontDistanceCm == 0) {
    Serial.println(F("no echo - stopped"));
    stopMotors();
    roverState = CHECKING_PATH;
    return;
  }

  Serial.print(frontDistanceCm);
  Serial.println(F(" cm"));

  if (frontDistanceCm <= STOP_DISTANCE_CM) {
    beginTurn();
  } else {
    driveMotors(DRIVE_SPEED, DRIVE_SPEED);
    roverState = MOVING_FORWARD;
  }
}

void setup()
{
  pinMode(RIGHT_MOTOR_PWM, OUTPUT);
  pinMode(LEFT_MOTOR_PWM, OUTPUT);
  pinMode(RIGHT_MOTOR_DIR, OUTPUT);
  pinMode(LEFT_MOTOR_DIR, OUTPUT);
  pinMode(MOTOR_STANDBY, OUTPUT);
  stopMotors();

  pinMode(FRONT_TRIG_PIN, OUTPUT);
  pinMode(FRONT_ECHO_PIN, INPUT);
  digitalWrite(FRONT_TRIG_PIN, LOW);

  panServo.attach(PAN_SERVO_PIN, 500, 2400);
  tiltServo.attach(TILT_SERVO_PIN, 500, 2400);
  panServo.write(90);
  tiltServo.write(90);

  Serial.begin(9600);
  delay(500);
  printHelp();
}

void loop()
{
  while (Serial.available() > 0) {
    handleCommand(Serial.read());
  }

  runAutonomousMode();
}
