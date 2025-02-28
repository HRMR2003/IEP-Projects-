#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "BluetoothSerial.h" // Library for Classic Bluetooth

#define DHT11_PIN 13  // GPIO4 for DHT11
#define MQ2_PIN 4 // GPIO36 for MQ2 AO pin
#define TRIG_PIN 14   // GPIO5 for Ultrasonic Sensor
#define ECHO_PIN 12  // GPIO18 for Ultrasonic Sensor

#define SOUND_SPEED 0.034
#define TANK_DEPTH 10.0  // Tank depth in cm

// DHT11 Sensor
DHT dht11(DHT11_PIN, DHT11);

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x27, 16 columns, 2 rows

// Bluetooth setup
BluetoothSerial SerialBT;

// Variables for sensors
float humidity, tempC;
int gasValue;
int gasPercentage;  // Gas percentage in integer
long duration;
float distanceCm;
int fuelLevelPercent;  // Fuel level percentage in integer

void setup() {
  Serial.begin(115200);     // For Serial Monitor
  SerialBT.begin("VEHICLE_DIAGNOSIS"); // Bluetooth device name

  // Initialize DHT11
  dht11.begin();

  // Initialize MQ2
  analogSetAttenuation(ADC_11db); // For ~3.3V range
  Serial.println("Warming up MQ2 sensor...");
  delay(2000);  // Warm-up time for MQ2

  // Initialize Ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Read DHT11 sensor
  humidity = dht11.readHumidity();
  tempC = dht11.readTemperature();

  // Check if DHT11 reading is valid
  if (isnan(humidity) || isnan(tempC)) {
    Serial.println("Failed to read from DHT11 sensor!");
    lcd.setCursor(0, 0);
    lcd.print("DHT11 Error");
    delay(2000);
    return;
  }

  // Read MQ2 gas sensor
  gasValue = analogRead(MQ2_PIN);
  gasPercentage = map(gasValue, 512, 0, 0, 100); // Map gasValue to percentage (0–100%)

  // Read Ultrasonic Sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distanceCm = duration * SOUND_SPEED / 2;

  // Calculate fuel level percentage
  fuelLevelPercent = ((TANK_DEPTH - distanceCm) / TANK_DEPTH) * 100;
  fuelLevelPercent = constrain(fuelLevelPercent, 0, 100); // Clamp value between 0-100%

  // Combine all values into a single string
 

  if (SerialBT.hasClient()) {
    String combinedValues = String(fuelLevelPercent) + " %|" + 
                          String(gasPercentage) + " %|" + 
                          String(tempC,1) + " C";
    SerialBT.print(combinedValues);
    Serial.println(combinedValues);
    delay(1000);
  }

  // Print combined string to Serial Monitor

  // Print values to LCD
  lcd.setCursor(0, 0);
  lcd.print("H:");
  lcd.print(humidity, 1);  // Humidity with 1 decimal place
  lcd.print("% T:");
  lcd.print(tempC, 1);     // Temperature with 1 decimal place

  lcd.setCursor(0, 1);
  lcd.print("Gas:");
  lcd.print(gasPercentage); // Gas percentage as integer
  lcd.print("% F:");
  lcd.print(fuelLevelPercent,1); // Fuel level percentage as integer
  lcd.print("%");
  delay(100);
}
