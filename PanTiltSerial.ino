// Pan-Tilt Serial Control
// Receives "pan tilt" lines with normalized values (-1.0 to 1.0) and drives
// two servos. All tunables live in config.h.

#include <Servo.h>
#include <math.h>
#include <stdlib.h>
#include "config.h"

static_assert(PAN_MIN_DEG >= 0.0f && PAN_MAX_DEG <= 180.0f && PAN_MIN_DEG < PAN_MAX_DEG,
              "Pan limits must satisfy 0 <= PAN_MIN_DEG < PAN_MAX_DEG <= 180");
static_assert(TILT_MIN_DEG >= 0.0f && TILT_MAX_DEG <= 180.0f && TILT_MIN_DEG < TILT_MAX_DEG,
              "Tilt limits must satisfy 0 <= TILT_MIN_DEG < TILT_MAX_DEG <= 180");
static_assert(SERVO_MIN_US < SERVO_MAX_US, "SERVO_MIN_US must be less than SERVO_MAX_US");
static_assert(LINE_BUFFER_SIZE >= 8, "LINE_BUFFER_SIZE is too small");
static_assert(SERVO_HOLD_MS >= 40, "SERVO_HOLD_MS must cover at least two 20 ms servo frames");

struct Axis {
  Servo servo;
  uint8_t pin;
  float minDeg;
  float maxDeg;
  bool invert;
  int lastUs = -1;               // last pulse written, -1 = none yet
  unsigned long lastMoveMs = 0;  // millis() of the last new target

  Axis(uint8_t pin, float minDeg, float maxDeg, bool invert)
    : pin(pin), minDeg(minDeg), maxDeg(maxDeg), invert(invert) {}

  // Map a normalized input (-1.0 to 1.0) to degrees within this axis' limits.
  float toDegrees(float input) const {
    input = constrain(input, -1.0f, 1.0f);
    if (invert) input = -input;
    return minDeg + (input + 1.0f) * 0.5f * (maxDeg - minDeg);
  }

  void begin(float homeInput) {
    // Drive the pin LOW so the servo sees a clean line until the first pulse.
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    moveTo(homeInput);
  }

  void moveTo(float input) {
    const float deg = toDegrees(input);
    const int us = (int)lroundf(SERVO_MIN_US + deg * ((SERVO_MAX_US - SERVO_MIN_US) / 180.0f));
    // Skip redundant writes; a repeat while detached re-attaches (re-asserts
    // the position if the axis sagged).
    if (us == lastUs && servo.attached()) return;
    // Set the pulse width before attaching so the very first pulse is correct
    // (the library stores it even while detached).
    servo.writeMicroseconds(us);
    if (!servo.attached()) servo.attach(pin, SERVO_MIN_US, SERVO_MAX_US);
    lastUs = us;
    lastMoveMs = millis();
  }

  // Detach once the servo has had SERVO_HOLD_MS to reach its target.
  void update(unsigned long now) {
#if DETACH_WHEN_IDLE
    if (servo.attached() && now - lastMoveMs >= SERVO_HOLD_MS) {
      servo.detach();
      // The library stops toggling a detached pin, possibly mid-pulse; force
      // it LOW so the servo doesn't see a stuck-high signal.
      digitalWrite(pin, LOW);
    }
#else
    (void)now;
#endif
  }
};

static Axis pan (PAN_PIN,  PAN_MIN_DEG,  PAN_MAX_DEG,  PAN_INVERT  != 0);
static Axis tilt(TILT_PIN, TILT_MIN_DEG, TILT_MAX_DEG, TILT_INVERT != 0);

static char lineBuf[LINE_BUFFER_SIZE + 1];
static uint8_t lineLen = 0;
static bool lineOverflow = false;

static inline bool isNumberStart(char c) {
  return (c >= '0' && c <= '9') || c == '-' || c == '+' || c == '.';
}

// Find and parse the next number in *p, skipping any non-numeric characters
// (so "0.5 -0.2", "0.5,-0.2" and "P0.5 T-0.2" all work). Advances *p.
static bool nextNumber(const char **p, float *out) {
  const char *s = *p;
  while (*s) {
    if (isNumberStart(*s)) {
      char *end;
      const double v = strtod(s, &end);
      if (end != s) {
        *p = end;
        if (isnan(v) || isinf(v)) return false;
        *out = (float)v;
        return true;
      }
    }
    ++s;
  }
  *p = s;
  return false;
}

static void reply(const char *msg) {
#if ACK_NEWLINE
  Serial.println(msg);
#else
  Serial.print(msg);
#endif
}

static void handleLine(const char *line) {
  float panIn, tiltIn;
  const char *p = line;
  if (nextNumber(&p, &panIn) && nextNumber(&p, &tiltIn)) {
    pan.moveTo(panIn);
    tilt.moveTo(tiltIn);
    reply(ACK_STRING);
  }
#if ENABLE_ERROR_REPLY
  else {
    reply(ERROR_STRING);
  }
#endif
}

// Non-blocking line reader: accepts "\n", "\r" or "\r\n" line endings.
static void pollSerial() {
  while (Serial.available() > 0) {
    const char c = (char)Serial.read();

    if (c == '\n' || c == '\r') {
      if (lineOverflow) {
        lineOverflow = false;
#if ENABLE_ERROR_REPLY
        reply(ERROR_STRING);
#endif
      } else if (lineLen > 0) {
        lineBuf[lineLen] = '\0';
        handleLine(lineBuf);
      }
      lineLen = 0;
    } else if (lineLen < LINE_BUFFER_SIZE) {
      lineBuf[lineLen++] = c;
    } else {
      lineOverflow = true;  // drop the rest of this line
    }
  }
}

#if PRINT_BANNER
static void printBanner() {
  Serial.println(F("Pan-Tilt Serial Control Ready."));
  Serial.println(F("Enter 'pan tilt' (e.g., '0.5 -0.25'):"));
  Serial.println(F("Input range: -1.0 to 1.0 for both."));
  Serial.print(F("Home is '"));
  Serial.print(PAN_HOME, 1);
  Serial.print(' ');
  Serial.print(TILT_HOME, 1);
  Serial.print(F("' (maps to "));
  Serial.print(pan.toDegrees(PAN_HOME), 1);
  Serial.print(' ');
  Serial.print(tilt.toDegrees(TILT_HOME), 1);
  Serial.println(F(" deg)"));
}
#endif

void setup() {
  pan.begin(PAN_HOME);
  tilt.begin(TILT_HOME);

  Serial.begin(BAUD_RATE);
#if PRINT_BANNER
  printBanner();
#endif
}

void loop() {
  pollSerial();

  const unsigned long now = millis();
  pan.update(now);
  tilt.update(now);
}
