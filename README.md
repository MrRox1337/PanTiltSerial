# Pan-Tilt Serial Control

## Overview

This Arduino sketch enables **serial control** of a two-axis **pan-tilt mechanism** using standard hobby servo motors. It allows users to send simple, normalized float values over the Serial Monitor (or any serial-sending application) to control the horizontal **Pan** and vertical **Tilt** angles of the mechanism. The input values range from **-1.0 to 1.0**, which are then mapped to the physical angle limits of the servos.

The center position is defined by sending **`0.0 0.0`**.

---

## Authors

- **Aman Mishra**
- **Dr. Judhi Prasetyo**
- **Dr. Sameer Kishore**

---

## Hardware Requirements

| Component           | Description                     |
| :------------------ | :------------------------------ |
| **Microcontroller** | Arduino Uno or compatible board |
| **Servo 1 (Pan)**   | Standard Hobby Servo            |
| **Servo 2 (Tilt)**  | Standard Hobby Servo            |
| **Wiring**          | Jumper Wires                    |

### Pin Assignments

Default pins (change in `config.h`):

| Axis     | Arduino Pin       | Config key |
| :------- | :---------------- | :--------- |
| **Pan**  | **Digital Pin 3** | `PAN_PIN`  |
| **Tilt** | **Digital Pin 2** | `TILT_PIN` |

---

## Configuration (`config.h`)

All tunables live in [`config.h`](config.h). Edit it and re-upload; there's no need to touch the sketch.

| Key                                   | Default       | Purpose                                                 |
| :------------------------------------ | :------------ | :------------------------------------------------------ |
| `BAUD_RATE`                           | `9600`        | Serial speed                                            |
| `PAN_MIN_DEG` / `PAN_MAX_DEG`         | `0` / `170`   | Pan angle range (degrees)                               |
| `TILT_MIN_DEG` / `TILT_MAX_DEG`       | `0` / `120`   | Tilt angle range (degrees)                              |
| `PAN_INVERT` / `TILT_INVERT`          | `0` / `1`     | Reverse an axis' direction                              |
| `PAN_HOME` / `TILT_HOME`              | `0.0` / `0.0` | Boot position in normalized units                       |
| `SERVO_MIN_US` / `SERVO_MAX_US`       | `544`/`2400`  | Pulse width for 0°/180° (Servo library defaults)        |
| `ACK_STRING` / `ACK_NEWLINE`          | `"OK"` / `0`  | Reply after each command, with or without a line ending |
| `ENABLE_ERROR_REPLY` / `ERROR_STRING` | `0` / `"ERR"` | Reply to unparseable or overlong lines                  |
| `LINE_BUFFER_SIZE`                    | `32`          | Max command length                                      |
| `PRINT_BANNER`                        | `1`           | Print usage text on boot                                |

Invalid limits are caught at compile time.

---

## Setup and Usage

### 1. Upload the Sketch

Upload the `PanTiltSerial.ino` sketch to your Arduino board.

### 2. Configure Serial Monitor

Open the Arduino Serial Monitor and set the **baud rate** to **9600**.

### 3. Send Commands

The system expects two floating-point numbers followed by a line ending (`\n`, `\r` or `\r\n`). Values outside **-1.0 to 1.0** are clamped. Separators are flexible, so `0.5 -0.25`, `0.5,-0.25` and `P0.5 T-0.25` all work.

The input format is: `[pan_value] [tilt_value]\n` (e.g., `0.5 -0.25`).

#### Input-to-Angle Mapping

| Input Value | Pan Angle (Pin 3) | Tilt Angle (Pin 2) | Notes                                       |
| :---------- | :---------------- | :----------------- | :------------------------------------------ |
| **1.0**     | **170°**          | **0°**             | Maximum extent (Pan), Minimum extent (Tilt) |
| **0.0**     | **85°**           | **60°**            | **Center Position** (Pan and Tilt)          |
| **-1.0**    | **0°**            | **120°**           | Minimum extent (Pan), Maximum extent (Tilt) |

- **Pan**: A value of `1.0` maps to **170°**, `0.0` to **85°**, and `-1.0` to **0°**.
- **Tilt**: A value of `1.0` maps to **0°**, `0.0` to **60°**, and `-1.0` to **120°**. Note the inverse mapping.

### Example Serial Output

After each valid command (e.g. `0.5 -0.25`) the Arduino replies with `ACK_STRING`. By default that's `OK` with no line ending:

```
OK
```
