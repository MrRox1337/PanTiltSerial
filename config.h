// ============================================================================
//  PanTiltSerial configuration
//  Edit the values below to adapt the sketch to your hardware.
// ============================================================================
#ifndef PAN_TILT_CONFIG_H
#define PAN_TILT_CONFIG_H

// ---------------------------------------------------------------------------
//  Serial
// ---------------------------------------------------------------------------
#define BAUD_RATE         115200

// Longest accepted command line (characters, excluding the line ending).
// Longer lines are discarded.
#define LINE_BUFFER_SIZE  32

// Reply sent after a valid command has been applied.
#define ACK_STRING        "OK"
// 1 = terminate the reply with "\r\n" (println), 0 = no line ending (print).
#define ACK_NEWLINE       0

// Reply sent when a line could not be parsed. Set ENABLE_ERROR_REPLY to 0
// to silently ignore bad input.
#define ENABLE_ERROR_REPLY 0
#define ERROR_STRING      "ERR"

// 1 = print the usage banner on boot.
#define PRINT_BANNER      1

// ---------------------------------------------------------------------------
//  Pins
// ---------------------------------------------------------------------------
// Any digital pin works: the Servo library generates pulses in software
// (Timer1), not with hardware PWM. Avoid 0/1 (Serial). Note that on the Uno
// the Servo library disables analogWrite() on pins 9 and 10.
#define PAN_PIN   5
#define TILT_PIN  6

// ---------------------------------------------------------------------------
//  Angle limits (degrees, 0-180)
//  Input -1.0 maps to MIN, +1.0 maps to MAX (or the reverse when INVERT is 1).
// ---------------------------------------------------------------------------
#define PAN_MIN_DEG   0.0f
#define PAN_MAX_DEG   170.0f
#define PAN_INVERT    0

#define TILT_MIN_DEG  0.0f
#define TILT_MAX_DEG  110.0f
#define TILT_INVERT   1        // +1.0 -> TILT_MIN_DEG, -1.0 -> TILT_MAX_DEG

// ---------------------------------------------------------------------------
//  Home position on boot, in normalized input units (-1.0 to 1.0)
// ---------------------------------------------------------------------------
#define PAN_HOME   0.0f
#define TILT_HOME  0.0f

// ---------------------------------------------------------------------------
//  Servo pulse calibration (microseconds)
//  These match the Servo library defaults, so 0-180 deg behaves exactly like
//  Servo.write(). Angles are sent with writeMicroseconds() for finer
//  resolution than whole degrees. Adjust if your servos need it.
// ---------------------------------------------------------------------------
#define SERVO_MIN_US  544
#define SERVO_MAX_US  2400

// ---------------------------------------------------------------------------
//  Power saving / anti-jitter
//  When enabled, each servo is attached only while moving: it is attached on
//  a new command, kept attached for SERVO_HOLD_MS so it can reach the target,
//  then detached (pin held LOW). A detached servo draws idle current only and
//  cannot jitter, but it also has NO holding torque, so a loaded axis (e.g.
//  tilt under gravity) may sag. Increase SERVO_HOLD_MS if large moves stop
//  short. Continuous command streams keep the servo attached.
// ---------------------------------------------------------------------------
#define DETACH_WHEN_IDLE  1
#define SERVO_HOLD_MS     600

#endif // PAN_TILT_CONFIG_H
