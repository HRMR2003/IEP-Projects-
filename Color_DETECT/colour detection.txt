#include <ESP32Servo.h>  // Correct library for ESP32

// TCS3200 Color Sensor Pin Definition
#define S0 2
#define S1 4
#define S2 19
#define S3 18
#define OUT 5
#define red 12
#define blue 13
#define green 26

// Servo pin definition
#define servoPin 15

// Variables to store color data
int data, r, g, b;

// Create a Servo object using ESP32Servo
Servo myServo;

void setup() {
  // Pin Modes
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);
  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(green, OUTPUT);

  // Initialize Serial Monitor
  Serial.begin(9600);
  Serial.println("Starting Color Detector");

  // Set Frequency Scaling to 100%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, HIGH);

  // Initialize servo
  myServo.attach(servoPin); // Attach the servo to the pin

  // Turn off all LEDs initially
  digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);
}

void loop() {
  // Turn off all LEDs at the start of each loop
  digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);

  // Measure Red
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  Serial.print("Red value: ");
  GetData();
  r = data;
  Serial.print(r);

  // Measure Blue
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  Serial.print(" | Blue value: ");
  GetData();
  b = data;
  Serial.print(b);

  // Measure Green
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  Serial.print(" | Green value: ");
  GetData();
  g = data * 0.65; // Adjusting green for better accuracy
  Serial.println(g);

  // Determine and display the color and control servo
  if (r < g && r < b && r > 5) {
    Serial.println("Detected Color: RED");
    digitalWrite(red, HIGH);
    digitalWrite(green, LOW);
    digitalWrite(blue, LOW);
    myServo.write(60); // Move servo to 60 degrees for RED
  } else if (g < r && g < b && g > 5&&g<50) {
    Serial.println("Detected Color: GREEN");
     digitalWrite(red,LOW);
    digitalWrite(green,HIGH);
    digitalWrite(blue, LOW);
    myServo.write(90); // Move servo to 90 degrees for GREEN
  } else if (b < r && b < g && b > 5) {
    Serial.println("Detected Color: BLUE");
     digitalWrite(red, LOW);
    digitalWrite(green, LOW);
    digitalWrite(blue, HIGH);
    myServo.write(180); // Move servo to 180 degrees for BLUE
  } else if(g>50&&r>150&&b>150)
  {
    Serial.println("Detected Color: UNKNOWN");
    // All LEDs remain off for unknown colors
     digitalWrite(red, LOW);
    digitalWrite(green, LOW);
    digitalWrite(blue, LOW);
    myServo.write(0); // Move servo to 0 degrees for UNKNOWN
  }

  delay(1000); // Delay for stability
}

void GetData() {
  // Measure duration of LOW pulse
  data = pulseIn(OUT, LOW);
  delay(20); // Short delay for stability
}
