# Pan-Tilt Robot: Communication Guide

Your OpenCV program controls the pan-tilt robot over a USB serial connection. This guide covers everything your program needs to do to talk to it. Treat the robot as a black box: send it commands in the format below and it points where you tell it.

---

## 1. Connection settings

| Setting      | Value                                                                       |
| :----------- | :-------------------------------------------------------------------------- |
| Interface    | USB serial (the robot appears as a serial / COM port)                       |
| Baud rate    | **115200**                                                                  |
| Data format  | 8 data bits, no parity, 1 stop bit (8N1)                                    |
| Flow control | None                                                                        |
| Port name    | Windows: `COM3`, `COM4`, … · Linux: `/dev/ttyACM0` · macOS: `/dev/cu.usbmodem…` |

### Opening the port

**The robot restarts every time you open the serial port.** After opening it:

1. Wait **2 seconds** before sending anything.
2. **Discard anything already received.** The robot prints some startup text that isn't part of the protocol.

Open the port **once** at the start of your program and keep it open. Don't open and close it for every command, or the robot will restart each time.

---

## 2. Command format

Each command is **one line of plain ASCII text** with two numbers:

```
<pan> <tilt>\n
```

| Part     | Rule                                                          |
| :------- | :------------------------------------------------------------ |
| `<pan>`  | Decimal number from **-1.0 to 1.0**                           |
| `<tilt>` | Decimal number from **-1.0 to 1.0**                           |
| Separator | A single space between the two numbers                       |
| Ending   | A newline character, `\n` (`\r\n` is also accepted)           |
| Length   | At most 32 characters, not counting the newline               |

Use plain decimal notation such as `0.25`, `-0.5` or `1`. Don't use scientific notation (`1e-3`), and don't add units, labels or brackets. Two or three decimal places is plenty.

### Examples

| You send          | Meaning                                              |
| :---------------- | :--------------------------------------------------- |
| `0 0\n`           | Point straight ahead (the centre / home position)     |
| `1 0\n`           | Pan fully to one side, tilt centred                  |
| `-1 0\n`          | Pan fully to the other side, tilt centred            |
| `0 1\n`           | Tilt fully one way, pan centred                      |
| `0.25 -0.5\n`     | Pan a quarter of the way, tilt halfway the other way  |

---

## 3. What the numbers mean

The two values are an **absolute aim point**, not a relative step or a speed:

- `0 0` is the centre of the robot's range. It's also where the robot points when it starts up.
- `1` and `-1` are the two ends of each axis's travel.
- Values in between are spread evenly across the range. `0.5` is halfway between the centre and one end.
- Sending the same command twice leaves the robot where it is. Sending `0.1 0` ten times does **not** move it ten steps.
- Values outside -1.0 to 1.0 are treated as -1.0 or 1.0. The robot won't go past the ends of its travel.

**This guide deliberately doesn't say which physical direction positive values move each axis.** Finding that out, and mapping your camera image coordinates onto these values correctly, is part of your task. Use the test commands in section 7.

---

## 4. The robot's reply

After every **valid** command, the robot replies with exactly two characters:

```
OK
```

There is **no newline** after `OK`. Read exactly 2 bytes. Don't use a "read line" function, or it will wait until it times out.

| Situation                                      | Reply          |
| :--------------------------------------------- | :------------- |
| Valid command                                  | `OK`           |
| Line can't be read as two numbers              | *Nothing*      |
| Line longer than 32 characters                 | *Nothing*      |
| Empty line                                     | *Nothing*      |

`OK` means the robot **received and accepted** the command. It does **not** mean the robot has finished moving. A large move takes a fraction of a second to complete.

---

## 5. Timing and sending rate

- **Recommended pattern:** send a command, wait for `OK`, then send the next one. Use a read timeout of about 100 ms. If no `OK` arrives, carry on with your next command rather than stopping your program.
- **Rate:** 20–50 commands per second is ideal for tracking. Sending faster than about 50 per second gains nothing, because the robot can't move faster than that.
- **Holding position:** the robot only holds its position firmly for about **1 second** after a command. After that it relaxes, and a loaded axis may drift slightly. To keep it locked on target, keep sending commands, even if the value hasn't changed.
- **Losing the target:** decide what your program should do when it loses the target. It could hold the last command, return to `0 0`, or search. Whatever you choose, send it explicitly.

---

## 6. Example (Python)

Install the serial library with `pip install pyserial`.

```python
import time
import serial

PORT = "COM3"          # change to your robot's port
BAUD = 115200


def open_robot(port=PORT):
    ser = serial.Serial(port, BAUD, timeout=0.1)
    time.sleep(2.0)            # robot restarts when the port opens
    ser.reset_input_buffer()   # throw away startup text
    return ser


def send(ser, pan, tilt):
    """Send an absolute aim point. Returns True if the robot replied OK."""
    pan = max(-1.0, min(1.0, pan))
    tilt = max(-1.0, min(1.0, tilt))
    ser.write(f"{pan:.3f} {tilt:.3f}\n".encode("ascii"))
    return ser.read(2) == b"OK"


if __name__ == "__main__":
    robot = open_robot()
    try:
        for pan, tilt in [(0, 0), (0.5, 0), (-0.5, 0), (0, 0.5), (0, -0.5), (0, 0)]:
            print(pan, tilt, "->", "OK" if send(robot, pan, tilt) else "no reply")
            time.sleep(1.0)
    finally:
        robot.close()
```

In your tracking loop, work out `pan` and `tilt` from your OpenCV result and call `send(robot, pan, tilt)` once per frame.

---

## 7. Testing without your vision code

Before connecting your OpenCV code, check the link by hand:

1. Open the Arduino IDE Serial Monitor (or any serial terminal).
2. Select the robot's port and set **115200 baud**, with the line ending set to **Newline** or **Both NL & CR**.
3. Send `0 0`, then `0.5 0`, `-0.5 0`, `0 0.5` and `0 -0.5`, and watch which way the robot moves for each.

Write down the direction each sign moves each axis. You'll need this to map your camera image onto robot commands.

The Serial Monitor shows `OK` replies run together (`OKOKOK`) because they have no newline. That's expected.

---

## 8. Troubleshooting

| Symptom                                    | Likely cause / fix                                                                  |
| :----------------------------------------- | :---------------------------------------------------------------------------------- |
| No `OK` and no movement                    | Wrong baud rate, missing `\n`, or commands sent within 2 s of opening the port       |
| Garbled text when the port opens           | Startup output. Clear the input buffer after the 2 s wait                          |
| Program freezes waiting for a reply        | Using a "read line" call. Read exactly 2 bytes with a timeout                      |
| Robot restarts / returns to centre at random | Your program is reopening the port. Open it once and keep it open                |
| "Port busy" / "access denied"              | Another program (often the Serial Monitor) has the port open. Close it first       |
| Robot moves the wrong way                  | Sign of pan or tilt reversed in your mapping. Re-check with section 7               |
| Robot moves but stops short of the target  | Your mapping from image coordinates to -1…1 is scaled wrong                         |
| Axis droops when you stop sending          | Expected. Keep sending commands to hold position (section 5)                        |

---

## Quick reference

```
Port:     115200 baud, 8N1, no flow control
Startup:  open once -> wait 2 s -> clear input buffer
Send:     "<pan> <tilt>\n"   values -1.0 .. 1.0, absolute aim point
Centre:   "0 0\n"
Reply:    "OK" (2 bytes, no newline) for each valid command; nothing otherwise
Rate:     20-50 commands/s; keep sending to hold position
```
