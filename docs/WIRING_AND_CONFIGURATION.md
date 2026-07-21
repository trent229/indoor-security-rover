# Wiring and Configuration Record

## Safety rule

Remove the battery, USB cable, and every other power source before measuring resistance or continuity.

## Connector verification procedure

1. Identify both ends of the conductor from the wiring map.
2. Verify continuity from the source pin to the expected destination pin.
3. Check that the conductor is not shorted to adjacent pins, 5 V, ground, or the chassis.
4. Confirm supply polarity before reconnecting power.
5. Flex the connector and perform a light tug test while measuring.
6. Label the connector and update the wiring record after it passes.

## Uno connections

| Function | Connection | Notes |
| --- | --- | --- |
| Front ultrasonic TRIG | D13 | Sensor trigger output |
| Front ultrasonic ECHO | D12 | Sensor echo input |
| Ultrasonic scanning servo | D10 / SERVO1 | 30° right, 90° center, 150° left |
| Camera rotation servo | A1 | Installed; control logic remains pending |
| Motor control | ELEGOO SmartCar Shield / TB6612 | Four motors controlled as left and right groups |

Do not share A1 with a rear ultrasonic sensor or line-tracking input while it is assigned to the camera servo.

## Uno software baseline

| Item | Value |
| --- | --- |
| Board | Arduino Uno |
| Sketch | `SmartRobotCarV4_0_V1_20230201` |
| Hardware variant | TB6612 and MPU6050 |
| FastLED | 3.3.3 |
| Servo | 1.2.2 |
| Wire | 1.0 |
| Obstacle threshold | 35 cm |
| Forward / reverse / turn speed | 80 / 75 / 85 |
| Obstacle-mode `Kp` | 0 |
| Obstacle-mode `UpperLimit` | 110 |

## ESP32-S3 camera baseline

| Item | Value |
| --- | --- |
| Module | ESP32-S3-WROOM-1 on ESP32S3-Camera-V1.0 |
| Arduino board | ESP32S3 Dev Module |
| Espressif core | 2.0.14 |
| Flash | 8 MB |
| PSRAM | Disabled |
| USB CDC On Boot | Enabled |
| Wi-Fi network | Device-hosted local access point; keep the exact SSID in the private build record |
| Camera page | Use the local address printed by the serial monitor at startup |

PSRAM was disabled because the board reported a PSRAM ID error and crashed when configured with unsupported PSRAM settings.

## Remaining verification

- Keep the camera feed open for at least 10 minutes while stationary.
- Repeat the endurance test while motors and servos operate.
- Determine whether a disconnect is caused by an ESP32 reset, 5 V voltage drop, or the phone leaving a network without internet access.
- Add bounded control for the A1 camera servo and verify that the cable does not bind.
