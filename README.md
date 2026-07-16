# Indoor Security Rover

An Arduino-based indoor security rover designed to patrol interior spaces, detect nearby obstacles, and provide a movable pan-tilt camera platform. This project is being assembled from individual components rather than a complete rover kit.

## Project Objective

The goal is to build a compact mobile security platform that combines:

- Differential-drive movement
- Forward and rear obstacle detection
- A two-axis pan-tilt camera
- Manual and autonomous operating modes
- A modular two-level chassis that can be expanded later

## Current Prototype

> **Project photo placeholder — current hardware prototype with wiring and rear support wheel installation still in progress.**

A completed build photograph will replace this placeholder after wiring and initial movement testing.

## Hardware

- Arduino Uno-compatible controller
- Arduino expansion and motor-control board
- Two TT DC gear motors
- Two drive wheels
- Rear support wheel or caster (pending)
- Two ultrasonic distance sensors
  - One fixed forward-facing sensor
  - One fixed rear-facing sensor
- Pan-and-tilt camera bracket
- Two SG90 micro servos
- ELEGOO camera module
- Battery holder with power switch
- Hand-cut two-level acrylic chassis
- Standoffs, motor mounts, brackets, and wiring

## Mechanical Design

The rover uses a two-level chassis:

- **Lower level:** drive motors, battery, power wiring, and obstacle sensors
- **Upper level:** Arduino controller, expansion board, and pan-tilt camera assembly

The battery is positioned low and toward the rear to improve stability. The camera is elevated on the upper platform for a clearer field of view. The ultrasonic sensors remain fixed to the chassis so obstacle readings are independent of camera direction.

## Current Build Status

- [x] Cut and shaped the acrylic chassis
- [x] Installed both drive motors and wheels
- [x] Installed upper-deck standoffs
- [x] Mounted the battery holder on the lower deck
- [x] Mounted the Arduino and expansion board
- [x] Assembled the pan-tilt mechanism
- [x] Mounted the camera module
- [ ] Install rear support wheel or caster
- [ ] Mount the front and rear ultrasonic sensors
- [ ] Complete power and signal wiring
- [ ] Test left and right motors independently
- [ ] Test pan and tilt limits
- [ ] Test obstacle-distance measurements
- [ ] Integrate autonomous movement logic
- [ ] Complete indoor driving tests

## Planned Rover Behavior

The first autonomous version will use the following basic responses:

- Move forward while the path is clear
- Stop when the front sensor detects an obstacle
- Turn away or reverse when forward movement is blocked
- Stop reversing when the rear sensor detects an obstacle
- Keep the camera pan and tilt controls independent from obstacle detection

## Initial Test Sequence

1. Verify battery voltage and polarity.
2. Power and test the Arduino controller.
3. Test the pan servo.
4. Test the tilt servo.
5. Test each ultrasonic sensor independently.
6. Test the left drive motor.
7. Test the right drive motor.
8. Verify camera operation.
9. Perform a low-speed combined movement test.
10. Add autonomous obstacle-avoidance logic.

## Safety and Wiring Notes

- The drive motors will be powered through the motor driver, not directly from Arduino GPIO pins.
- The servos will use an appropriate regulated supply.
- Controller, motor-driver, sensor, and servo grounds will share a common ground.
- Wiring will be secured away from the wheels and moving pan-tilt assembly.
- Motor terminals will be insulated after individual testing.

## Project Status

**In progress — mechanical assembly is substantially complete, with rear support, wiring, firmware, and testing remaining.**
