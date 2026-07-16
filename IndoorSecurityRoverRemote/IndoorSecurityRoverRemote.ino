#include <Servo.h>

// Decode only the NEC protocol used by the ELEGOO remote.
#define DECODE_NEC
#include <IRremote.hpp>

// Indoor Security Rover - autonomous and infrared remote control
// Rearward movement remains disabled until a working rear sensor is installed.

// ELEGOO Smart Robot Car V4.0 motor-shield pins
const uint8_t RIGHT_MOTOR_PWM = 5;
const uint8_t LEFT_MOTOR_PWM  = 6;
const uint8_t RIGHT_MOTOR_DIR = 7;
const uint8_t LEFT_MOTOR_DIR  = 8;
const uint8_t MOTOR_STANDBY   = 3;

// Infrared receiver and pan-tilt servos
const uint8_t IR_RECEIVE_PIN = 9;
const uint8_t PAN_SERVO_PIN  = 10;
const uint8_t TILT_SERVO_PIN = 11;

// Fixed front ultrasonic sensor
const uint8_t FRONT_TRIG_PIN = 13;
const uint8_t FRONT_ECHO_PIN = 12;

// Conservative indoor settings
const uint8_t DRIVE_SPEED = 100;
const uint8_t TURN_SPEED = 100;
const uint16_t STOP_DISTANCE_CM = 30;
const unsigned long MANUAL_DRIVE_TIME_MS = 300;
const unsigned long MANUAL_TURN_TIME_MS = 250;
const unsigned long AUTO_TURN_TIME_MS = 450;
const unsigned long SENSOR_INTERVAL_MS = 100;

// ELEGOO NEC remote command bytes
const uint8_t IR_UP    = 0x46;
const uint8_t IR_DOWN  = 0x15;
const uint8_t IR_LEFT  = 0x44;
const uint8_t IR_RIGHT = 0x43;
const uint8_t IR_OK    = 0x40;
const uint8_t IR_1     = 0x16;
const uint8_t IR_2     = 0x19;
const uint8_t IR_4     = 0x0C;
const uint8_t IR_5     = 0x18;
const uint8_t IR_6     = 0x5E;
const uint8_t IR_8     = 0x1C;

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

uint16_t readFrontDistanceCm()
{
  digitalWrite(FRONT_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(FRONT_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(FRONT_TRIG_PIN, LOW);

  const unsigned long duration = pulseIn(FRONT_ECHO_PIN, HIGH, 20000UL);

  // No echo is treated as unsafe.
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

void disableAutonomous()
{
  autonomousEnabled = false;
  roverState = STOPPED;
  stopMotors();
}

void manualForwardPulse()
{
  disableAutonomous();
  const uint16_t distance = readFrontDistanceCm();

  if (distance == 0 || distance <= STOP_DISTANCE_CM) {
    Serial.println(F("Remote forward blocked by front sensor."));
    return;
  }

  driveMotors(DRIVE_SPEED, DRIVE_SPEED);
  delay(MANUAL_DRIVE_TIME_MS);
  stopMotors();
}

void manualTurnPulse(bool turnLeft)
{
  disableAutonomous();

  if (turnLeft) {
    driveMotors(-TURN_SPEED, TURN_SPEED);
  } else {
    driveMotors(TURN_SPEED, -TURN_SPEED);
  }

  delay(MANUAL_TURN_TIME_MS);
  stopMotors();
}

void beginAutonomousTurn()
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

void toggleAutonomous()
{
  autonomousEnabled = !autonomousEnabled;
  stopMotors();

  if (autonomousEnabled) {
    roverState = CHECKING_PATH;
    Serial.println(F("Autonomous mode ENABLED."));
  } else {
    roverState = STOPPED;
    Serial.println(F("Autonomous mode DISABLED."));
  }
}

void handleRemoteCommand(uint8_t command)
{
  switch (command) {
    case IR_UP:
      manualForwardPulse();
      break;

    case IR_DOWN:
      disableAutonomous();
      Serial.println(F("Reverse disabled until rear sensor is replaced."));
      break;

    case IR_LEFT:
      manualTurnPulse(true);
      break;

    case IR_RIGHT:
      manualTurnPulse(false);
      break;

    case IR_OK:
      disableAutonomous();
      Serial.println(F("EMERGENCY STOP."));
      break;

    case IR_1:
      toggleAutonomous();
      break;

    case IR_2:
      tiltServo.write(75);
      break;

    case IR_8:
      tiltServo.write(105);
      break;

    case IR_4:
      panServo.write(75);
      break;

    case IR_6:
      panServo.write(105);
      break;

    case IR_5:
      panServo.write(90);
      tiltServo.write(90);
      break;

    default:
      Serial.print(F("Unmapped remote command: 0x"));
      Serial.println(command, HEX);
      break;
  }
}

void handleSerialCommand(char command)
{
  if (command >= 'a' && command <= 'z') {
    command = command - ('a' - 'A');
  }

  if (command == 'A') {
    toggleAutonomous();
  } else if (command == 'X') {
    disableAutonomous();
    Serial.println(F("EMERGENCY STOP."));
  }
}

void runAutonomousMode()
{
  if (!autonomousEnabled) {
    return;
  }

  const unsigned long now = millis();

  if (roverState == TURNING) {
    if (now - turnStartedAt >= AUTO_TURN_TIME_MS) {
      stopMotors();
      roverState = CHECKING_PATH;
    }
    return;
  }

  if (now - lastSensorCheck < SENSOR_INTERVAL_MS) {
    return;
  }

  lastSensorCheck = now;
  const uint16_t distance = readFrontDistanceCm();

  if (distance == 0) {
    Serial.println(F("Front sensor lost echo: stopped."));
    stopMotors();
    roverState = CHECKING_PATH;
    return;
  }

  if (distance <= STOP_DISTANCE_CM) {
    beginAutonomousTurn();
  } else {
    driveMotors(DRIVE_SPEED, DRIVE_SPEED);
    roverState = MOVING_FORWARD;
  }
}

void printHelp()
{
  Serial.println(F("Indoor Security Rover remote control"));
  Serial.println(F("Arrows: manual movement (reverse disabled)"));
  Serial.println(F("OK: emergency stop"));
  Serial.println(F("1: toggle autonomous mode"));
  Serial.println(F("4/6: pan, 2/8: tilt, 5: center camera"));
  Serial.println(F("Autonomous mode starts DISABLED."));
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
  IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);

  delay(500);
  printHelp();
}

void loop()
{
  if (IrReceiver.decode()) {
    // Ignore held-button repeat frames to keep movement pulses controlled.
    if (!(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) {
      handleRemoteCommand(IrReceiver.decodedIRData.command);
    }
    IrReceiver.resume();
  }

  while (Serial.available() > 0) {
    handleSerialCommand(Serial.read());
  }

  runAutonomousMode();
}
