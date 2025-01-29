Portable ECG Monitor
Description
A portable system for real-time ECG monitoring using an ESP32 microcontroller, AD8232 ECG sensor, and LCD display. The project integrates with the Ubidots IoT platform for remote data visualization via line charts.
Features
•	Real-time ECG monitoring with local display on LCD.
•	Wireless data transfer to Ubidots via ESP32 Wi-Fi.
•	Remote visualization of ECG data through the Ubidots dashboard.
•	Compact and portable design.
Hardware Requirements
•	ESP32 Microcontroller
•	AD8232 ECG Sensor
•	16x2 LCD with I2C Module
•	Power Supply (Battery/USB)
Software Requirements
•	Arduino IDE (with ESP32 board package)
•	Ubidots Account
Setup
1.	Connect the AD8232 sensor and LCD to the ESP32.
2.	Upload the provided Arduino code to ESP32.
3.	Configure Ubidots with your device token and create a variable for ECG data.
4.	Update Wi-Fi credentials and Ubidots token in the code.
5.	Power the system and monitor ECG data locally and remotely.
Future Scope
•	Add Bluetooth for mobile app integration.
•	Implement alerts for abnormal ECG readings.


"# -ECG-monitor" 
