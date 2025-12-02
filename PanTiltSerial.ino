#include <Servo.h>

#define TILT_MAX 120.00
#define PAN_MAX 170.00
#define BAUD_RATE 9600

// Create two servo objects
Servo servo1; // Joint 1 (Pan)
Servo servo2; // Joint 2 (Tilt)

// Define the pins
const int servo1Pin = 3;
const int servo2Pin = 2;

void writeServo(Servo servo, int pin, int angle) {
  servo.attach(pin);
  servo.write(angle);
  servo.detach();
}

void setup() {
  // Attach the servos to their respective pins
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);

  // Start the serial communication
  Serial.begin(BAUD_RATE);
  Serial.println("Pan-Tilt Serial Control Ready.");
  Serial.println("Enter 'pan tilt' (e.g., '0.5 -0.25'):");
  Serial.println("Input range: -1.0 to 1.0 for both.");
  Serial.println("Center is '0.0 0.0' (maps to 85 60)");

  // Set initial position (Center)
  // PAN_MAX / 2 is center for pan (0-PAN_MAX)
  // TILT_MAX / 2 is center for tilt (0-TILT_MAX)
  servo1.write(PAN_MAX / 2);
  servo2.write(TILT_MAX / 2);
}

void loop() {
  // Check if there is data available to read from serial
  if (Serial.available() > 0) {
    // Read the first float (pan for servo 1)
    float pan_input = Serial.parseFloat();
    
    // Read the second float (tilt for servo 2)
    float tilt_input = Serial.parseFloat();

    // Check if the newline character is next, which signifies the end of the input
    if (Serial.read() == '\n') {
      
      // Constrain the input values to be within the -1.0 to 1.0 range
      pan_input = constrain(pan_input, -1.0, 1.0);
      tilt_input = constrain(tilt_input, -1.0, 1.0);

      // Map the -1.0 to 1.0 range to the desired angle range
      // Pan: -1.0 -> 0, 0.0 -> 85, 1.0 -> 170
      int pan_angle = (int)(((pan_input + 1.0) / 2.0) * PAN_MAX);
      
      // Tilt: -1.0 -> 0, 0.0 -> 60, 1.0 -> 120
      int tilt_angle = (int)(TILT_MAX * (1.0 - ((tilt_input + 1.0) / 2.0)));

      // Write the values to the servos
      servo1.write(pan_angle);
      servo2.write(tilt_angle);

      // Print back what it received and the resulting angle
      Serial.print("Input: ");
      Serial.print(pan_input, 4); // Print float with 4 decimal places
      Serial.print(", ");
      Serial.print(tilt_input, 4);
      Serial.print(" | Mapped to -> Pan (Pin 3): ");
      Serial.print(pan_angle);
      Serial.print(", Tilt (Pin 2): ");
      Serial.println(tilt_angle);
    }
  }
}