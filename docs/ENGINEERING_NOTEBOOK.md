# Engineering Notebook Entry — July 20, 2026

## Objective

Complete a standalone indoor security rover that moves autonomously, avoids obstacles, and provides a live camera feed to a phone.

## Work completed

- Configured the ESP32-S3 camera as a Wi-Fi access point.
- Installed the ELEGOO Smart Robot Car V4.0 main program on the Arduino Uno.
- Corrected the ultrasonic wiring and obstacle interpretation.
- Reduced motor speed for smoother indoor movement.
- Disabled aggressive yaw correction in obstacle mode to reduce weaving.
- Installed a dedicated servo so the ultrasonic sensor scans the same directions used for route decisions.
- Verified autonomous movement, obstacle response, route scanning, and phone camera viewing.

## Problems and corrections

| Problem | Cause | Correction |
| --- | --- | --- |
| ESP32 board package would not install | Invalid Arduino CLI proxy entry | Removed the proxy and installed Espressif core 2.0.14 |
| ESP32 crashed during camera startup | Unsupported PSRAM configuration | Disabled PSRAM |
| Rover turned in an open room | Zero ultrasonic readings were treated as nearby obstacles | Required a positive reading before declaring an obstacle |
| Rover swayed left and right | MPU yaw correction was too aggressive at low speed | Set obstacle-mode `Kp` to 0 and `UpperLimit` to 110 |
| Camera movement did not match obstacle readings | The camera servo moved while the ultrasonic sensor was fixed | Added a dedicated scanning servo for the ultrasonic sensor |
| Uno upload lost synchronization | COM port or camera communication link interfered | Closed Serial Monitor, disconnected the camera link, and used Upload mode |

## Lesson learned

Every connector should be checked with an ohmmeter immediately after it is completed. The check must include pin-to-pin continuity, isolation from adjacent pins, power-to-ground isolation, polarity, and a flex/tug test. Performing this test during assembly prevents hours of later troubleshooting and makes it easier to distinguish wiring faults from code faults.

## Result

Project 1 reached its operating goal. The rover can patrol in autonomous obstacle-avoidance mode while providing a phone-viewable camera feed. Wi-Fi endurance and software control of the A1 camera servo remain open tasks for the next revision.

## Next phase

Project 2 will integrate the rover into the larger security system with remote commands, event logging, alerts, connection monitoring, and fail-safe stop behavior.

