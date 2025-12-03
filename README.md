# Pan-Tilt Serial Control

## Overview
This Arduino sketch enables **serial control** of a two-axis **pan-tilt mechanism** using standard hobby servo motors. It allows users to send simple, normalized float values over the Serial Monitor (or any serial-sending application) to control the horizontal **Pan** and vertical **Tilt** angles of the mechanism. The input values range from **-1.0 to 1.0**, which are then mapped to the physical angle limits of the servos.

The center position is defined by sending **`0.0 0.0`**.

---

## Authors

* **Dr. Judhi Prasetyo**
* **Dr. Sameer Kishore**
* **Aman Mishra**

---

## Hardware Requirements

| Component | Description |
| :--- | :--- |
| **Microcontroller** | Arduino Uno or compatible board |
| **Servo 1 (Pan)** | Standard Hobby Servo |
| **Servo 2 (Tilt)** | Standard Hobby Servo |
| **Wiring** | Jumper Wires |

### Pin Assignments
The code is configured to use the following pins:

| Axis | Servo Object | Arduino Pin |
| :--- | :--- | :--- |
| **Pan** | `servo1` | **Digital Pin 3** |
| **Tilt** | `servo2` | **Digital Pin 2** |

### Constants and Limits
The maximum range for the mechanism is defined by constants in the code:
* `PAN_MAX`: **170.00** degrees (Pan axis range)
* `TILT_MAX`: **120.00** degrees (Tilt axis range)

---

## Setup and Usage

### 1. Upload the Sketch
Upload the `PanTiltSerial.ino` sketch to your Arduino board.

### 2. Configure Serial Monitor
Open the Arduino Serial Monitor and set the **baud rate** to **9600**.

### 3. Send Commands
The system expects two space-separated floating-point numbers followed by a newline character. Both values must be within the range **-1.0 to 1.0**.

The input format is: `[pan_value] [tilt_value]\n` (e.g., `0.5 -0.25`).

#### Input-to-Angle Mapping

| Input Value | Pan Angle (Pin 3) | Tilt Angle (Pin 2) | Notes |
| :--- | :--- | :--- | :--- |
| **1.0** | **170°** | **0°** | Maximum extent (Pan), Minimum extent (Tilt) |
| **0.0** | **85°** | **60°** | **Center Position** (Pan and Tilt) |
| **-1.0** | **0°** | **120°** | Minimum extent (Pan), Maximum extent (Tilt) |

* **Pan**: A value of `1.0` maps to **170°**, `0.0` to **85°**, and `-1.0` to **0°**.
* **Tilt**: A value of `1.0` maps to **0°**, `0.0` to **60°**, and `-1.0` to **120°**. Note the inverse mapping.

### Example Serial Output

After sending an input like `0.5 -0.25`, the Arduino will print the resulting angles:
```
Input: 0.5000, -0.2500 | Mapped to -> Pan (Pin 3): 127, Tilt (Pin 2): 75
```
*(Calculations: Pan (0.5) -> 127.5 (approx) 127°; Tilt (-0.25) -> 75°)*
