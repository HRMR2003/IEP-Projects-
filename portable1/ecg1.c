#include <WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <NTPClient.h>

// WiFi and MQTT Credentials
#define SENSORPIN 36               // Sensor connected to GPIO 36 (ADC1_CH0)
#define WIFISSID "IEP"             // Replace with your WiFi SSID
#define PASSWORD "12345678"        // Replace with your WiFi Password
#define TOKEN "BBUS-muEiyyexlPM3NcPGfLbFtvYm7Q4d5c" // Ubidots TOKEN
#define MQTT_CLIENT_NAME "ECG_34356789" // Unique MQTT Client Name

/****************************************
 * Define Constants
 ****************************************/
#define VARIABLE_LABEL "ecg_data"              // Variable label for Ubidots
#define DEVICE_LABEL "ecg_mon" // Device label for Ubidots

// MQTT Broker
const char mqttBroker[] = "industrial.api.ubidots.com";
char topic[150];
char payload[512]; // Optimized payload size
char str_sensor[10];
char str_millis[20];

// Time and MQTT Clients
WiFiClient ubidots;
PubSubClient client(ubidots);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

double epochMilliseconds = 0;
double initialMillis = 0;

// Callback function for MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);
}

// Reconnect to MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {
      Serial.println("Connected to MQTT Broker");
    } else {
      Serial.print("Connection failed, state=");
      Serial.println(client.state());
      delay(2000); // Wait before retrying
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFISSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize Time and MQTT Clients
  timeClient.begin();
  client.setServer(mqttBroker, 1883);
  client.setCallback(callback);

  // Synchronize time with NTP
  timeClient.update();
  epochMilliseconds = timeClient.getEpochTime() * 1000;
  initialMillis = millis();
}

void loop() {
  // Ensure MQTT connection
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Prepare JSON Payload
  snprintf(topic, sizeof(topic), "/v1.6/devices/%s", DEVICE_LABEL);
  snprintf(payload, sizeof(payload), "{\"%s\": [", VARIABLE_LABEL);

  for (int i = 0; i < 3; i++) {
    float sensorValue = analogRead(SENSORPIN); // Read sensor value
    dtostrf(sensorValue, 4, 2, str_sensor);    // Convert to string

    double currentMillis = millis();
    double timestamp = epochMilliseconds + (currentMillis - initialMillis);
    snprintf(str_millis, sizeof(str_millis), "%.0f", timestamp);

    char dataPoint[100];
    snprintf(dataPoint, sizeof(dataPoint), "{\"value\":%s,\"timestamp\":%s}%s",
             str_sensor, str_millis, (i < 2) ? "," : "]");

    strncat(payload, dataPoint, sizeof(payload) - strlen(payload) - 1);
    delay(150); // Optional delay
  }

  // Publish to Ubidots
  strncat(payload, "}", sizeof(payload) - strlen(payload) - 1); // Close JSON payload
  Serial.println("Publishing data to Ubidots Cloud...");
  client.publish(topic, payload);
  Serial.println(payload);

  delay(10); // Adjust this based on your desired data publishing interval
}