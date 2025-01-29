#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include "BluetoothSerial.h"
#include "MAX30105.h"
#include "heartRate.h"
#define m1 13
#define m2 12
#define m3 14
#define m4 27
#define enb1 26
#define enb2 25
int motorspeed = 200;

BluetoothSerial SerialBT;
LiquidCrystal_I2C lcd(0x27, 16, 2); 
MAX30105 particleSensor;

#define ONE_WIRE_BUS 2               

DeviceAddress thermometerAddress;    
OneWire oneWire(ONE_WIRE_BUS);        
DallasTemperature tempSensor(&oneWire);  

const byte RATE_SIZE = 4; // Increase for more averaging
byte rates[RATE_SIZE]; // Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; // Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;
float temperatureC;
float temperatureF;
long irValue;
int value;
unsigned long lastSendTime = 0;  // To store the last time data was sent
const unsigned long sendInterval = 1000;  // Send data every 2 seconds

// Function prototype
void printAddress(DeviceAddress deviceAddress);

void setup() {
  Serial.begin(115200);                   
  Serial.println("DS18B20 Temperature IC Test");
  Serial.println("Locating devices...");
  SerialBT.begin("ROBOT DOCTOR");  // Start Bluetooth with device name "ROBOT DOCTOR"
  tempSensor.begin();                         // Initialize the temperature sensor
  pinMode(4,OUTPUT);
  if (!tempSensor.getAddress(thermometerAddress, 0))
    Serial.println("Unable to find Device.");
  else {
    Serial.print("Device 0 Address: ");
    printAddress(thermometerAddress);
    Serial.println();
  }
   pinMode(m1,OUTPUT);
  pinMode(m2,OUTPUT);
  pinMode(m3,OUTPUT);
  pinMode(m4,OUTPUT);
  pinMode(enb1,OUTPUT);
  pinMode(enb2,OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0); 
  lcd.print("  ROBOT DOCTOR ");
  lcd.setCursor(0, 1); 
  lcd.print("..WELCOMES YOU..");
  delay(2000);
  lcd.clear();
  
  tempSensor.setResolution(thermometerAddress, 12);      // (9-12)

  // Initialize MAX30105 sensor for heart rate detection
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 was not found. Please check wiring/power.");
    while (1);
  }
  particleSensor.setup(); // Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); // Low Red LED amplitude
  particleSensor.setPulseAmplitudeGreen(0); // Turn off Green LED
  
  // Start heart rate detection task on core 1
  xTaskCreatePinnedToCore(heartRateTask, "HeartRateTask", 10000, NULL, 1, NULL, 1);  // Core 1
}
String movement;
void loop() {
  tempSensor.requestTemperatures();                     
  temperatureC = tempSensor.getTempC(thermometerAddress);  
  temperatureF = DallasTemperature::toFahrenheit(temperatureC);  

  // Check if 2 seconds have passed
  if (millis() - lastSendTime >= sendInterval) {
    lastSendTime = millis();  // Update the last send time

    displayTemp(temperatureC, temperatureF);  // Update LCD and send data via Bluetooth
  }
  delay(50);  // Short delay to prevent excessive loop speed
}

void displayTemp(float temperatureC, float temperatureF) {             
  lcd.setCursor(0, 0);
  lcd.print("TEMP : ");
  lcd.print(temperatureF);
  lcd.print(" F");
  lcd.setCursor(0, 1);
  lcd.print("BPM : ");
  if(beatAvg > 40){
    lcd.print(beatAvg);
    lcd.print(" ");
  }
  else if (irValue < 50000){
    lcd.print(0);
    lcd.print("  ");
  }
  else{
    lcd.print("...");
  }
  // Send data to Bluetooth
  if (SerialBT.hasClient()) {
    digitalWrite(4,HIGH);
    // Send all four data (temperature, BPM, avg BPM) over Bluetooth
    String message = "Temp : " + String(temperatureF) + " F  BPM: " + String(beatAvg);
    SerialBT.print(message);
  }
  else{
    digitalWrite(4,LOW);
  }
  delay(100);
}

void heartRateTask(void *pvParameters) {
  while (true) {
    irValue = particleSensor.getIR();

    if (checkForBeat(irValue) == true) {
      long delta = millis() - lastBeat;
      lastBeat = millis();
      beatsPerMinute = 60 / (delta / 1000.0);

      if (beatsPerMinute < 255 && beatsPerMinute > 20) {
        rates[rateSpot++] = (byte)beatsPerMinute;
        rateSpot %= RATE_SIZE;
        beatAvg = 0;
        for (byte x = 0; x < RATE_SIZE; x++)
          beatAvg += rates[x];
        beatAvg /= RATE_SIZE;
      }
    }
    Serial.print(temperatureC);     
    Serial.print("°C  ");
    Serial.print(temperatureF);  
    Serial.print("°F");
    Serial.print("  IR=");
    Serial.print(irValue);
    Serial.print(", BPM=");
    Serial.print(beatsPerMinute);
    Serial.print(", Avg BPM=");
    Serial.print(beatAvg);
    Serial.print(" value : ");
    Serial.print(value);
    Serial.print(" movement : ");
    Serial.print(movement);
    if (irValue < 50000) {
      beatsPerMinute = 0;
      beatAvg = 0;
      Serial.print(" No finger?");
    }

    Serial.println();
    delay(10); // Add small delay to prevent task overload
    analogWrite(enb1,motorspeed);
  analogWrite(enb2,motorspeed);
  if(SerialBT.available() > 0){     
    value = SerialBT.read();     
  }
  if (value == 1){
      movement = "forward";
      moveforward();
    }
    if (value == 3){
      movement = "right";
      turnright();
    }
    if (value == 5){
      movement = "stop";
      motorstops();
    }
    if (value == 2){
      movement = "reverse";
      movereverse();
    }
    
    if(value==4){
      turnleft();
      movement = "left";
    }
  }
}

void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void motorstops()
  {
    digitalWrite(m1,0);
    digitalWrite(m2,0);
    digitalWrite(m3,0);
    digitalWrite(m4,0);

  }
  void moveforward()
  {
    digitalWrite(m1,0);
    digitalWrite(m2,1);
    digitalWrite(m3,0);
    digitalWrite(m4,1);

  }
  void movereverse()
  {
    digitalWrite(m1,1);
    digitalWrite(m2,0);
    digitalWrite(m3,1);
    digitalWrite(m4,0);
  }
  void turnleft()
  {
    digitalWrite(m1,0);
    digitalWrite(m2,1);
    digitalWrite(m3,1);
    digitalWrite(m4,0);
  }
  void turnright()
  {

    digitalWrite(m1,1);
    digitalWrite(m2,0);
    digitalWrite(m3,0);
    digitalWrite(m4,1);
  }