# Indoor Security Rover

An ELEGOO Smart Robot Car V4.0 adapted as an indoor security rover with autonomous obstacle avoidance and an ESP32-S3 camera feed.

## Current working baseline

Project 1 is operational as a standalone rover:

- Arduino Uno controls the four drive motors as left and right motor groups.
- A front ultrasonic sensor detects obstacles.
- A dedicated servo scans the ultrasonic sensor to compare the left and right routes.
- Autonomous mode uses reduced speed and disables aggressive MPU6050 yaw correction to prevent weaving.
- An ESP32-S3 camera creates a Wi-Fi access point and provides a live phone-viewable feed.

The working Uno firmware is in [`firmware/uno/SmartRobotCarV4_0_V1_20230201`](firmware/uno/SmartRobotCarV4_0_V1_20230201).

## Final Uno behavior

The autonomous obstacle routine currently uses:

- Forward speed: `80`
- Reverse speed: `75`
- Turn speed: `85`
- Blocked-distance threshold: `35 cm`
- Ultrasonic scan angles: right `30°`, center `90°`, left `150°`
- Obstacle-mode straight-line correction: `Kp = 0`
- Obstacle-mode motor upper limit: `110`

When the path is blocked, the rover stops, backs away, scans right and left, returns the sensor to center, and turns toward the route with more space. If both routes are blocked, it backs farther away and performs a longer turn.

## Hardware and wiring

| Device | Connection |
| --- | --- |
| Front ultrasonic sensor | TRIG D13, ECHO D12, 5 V, GND |
| Ultrasonic scan servo | SERVO1 / D10, 5 V, GND |
| Camera rotation servo | A1 signal, 5 V, GND |
| Drive motors | TB6612 motor driver; left and right motor groups |
| ESP32-S3 camera | Camera communication port and regulated 5 V supply |

See [`docs/WIRING_AND_CONFIGURATION.md`](docs/WIRING_AND_CONFIGURATION.md) for the complete programming and configuration record.

## Arduino Uno setup

- Board: Arduino Uno
- Final programming port: COM9 on the development computer
- Required libraries:
  - FastLED 3.3.3
  - Servo 1.2.2
  - Wire 1.0

Open `SmartRobotCarV4_0_V1_20230201.ino` from its folder so Arduino IDE loads all companion `.cpp` and `.h` files.

### Upload procedure

1. Turn the rover battery off.
2. Disconnect the camera communication cable from the Uno shield.
3. Move the shield switch to **Upload**.
4. Close Serial Monitor and any other application using the COM port.
5. Select **Arduino Uno** and the correct COM port.
6. Upload the sketch.
7. Reconnect the camera cable and move the switch back to **Cam**.
8. Power the rover and test it with the wheels raised before floor testing.

## ESP32-S3 camera baseline

- Board: ESP32S3 Dev Module
- Espressif Arduino core: 2.0.14
- Flash: 8 MB
- Partition: default 8 MB layout
- USB CDC On Boot: enabled
- PSRAM: disabled
- Access point: device-hosted local Wi-Fi network
- Camera page: local device page shown by the serial monitor at startup

The working ESP32 camera source was not included in the current source archive, so it has not been added to this update. Its verified build settings are preserved here for reproduction.

## Verification status

- [x] Uno firmware compiles and uploads
- [x] Four drive motors operate as left and right groups
- [x] Reduced-speed autonomous movement works
- [x] Front obstacle detection works
- [x] Ultrasonic servo scans left and right
- [x] Rover selects a clearer route
- [x] ESP32-S3 camera feed opens on a phone
- [ ] Complete a 10-minute Wi-Fi endurance test
- [ ] Add software control for the A1 camera-rotation servo
- [ ] Archive the final ESP32-S3 camera source in this repository

## Engineering lesson learned

Ohm out every connector immediately after completing it. With power disconnected, verify the expected pin-to-pin continuity, check for shorts to adjacent pins and power rails, confirm polarity, and perform a light tug/flex test. This prevents wiring faults from becoming lengthy software troubleshooting problems later.

## Project phases

- **Project 1:** standalone autonomous rover with obstacle avoidance and camera feed — operational.
- **Project 2:** integration with the larger security system, including remote commands, event logging, alerts, and fail-safe behavior.
