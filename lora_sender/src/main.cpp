/*********
  ESP32 LoRa Vehicle Sender
  Efficiency Challenge Electric Vehicle Telemetry System
  Vehicle-side telemetry sender - Sends real vehicle data to pitstop
  Based on EC Technical Design Report requirements
*********/

#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>

// Define pins used by the LoRa transceiver module for ESP32
#define SS    5    // NSS pin (GPIO5)
#define RST   14   // RST pin (GPIO14)
#define DIO0  2    // DIO0 pin (GPIO2)

// SPI pins for ESP32 (VSPI):
// SCK  = GPIO18
// MISO = GPIO19
// MOSI = GPIO23

// Vehicle telemetry data variables
int packetID = 0;
float batteryVoltage = 48.5;      // V - Batarya paketi gerilimi  
float batteryCurrent = 15.3;      // A - Batarya akımı
float batterySOC = 85.2;          // % - State of Charge
float batteryTemp = 32.1;         // °C - Batarya sıcaklığı
float motorTemp = 45.7;           // °C - Motor sıcaklığı
float motorCurrent = 12.8;        // A - Motor akımı
float vehicleSpeed = 42.3;        // km/h - Araç hızı
float motorRPM = 1850.0;          // RPM - Motor devir
float energyConsumption = 156.7;  // Wh/km - Enerji tüketimi
int motorEfficiency = 94;         // % - Motor verimliliği

// Simulated sensor reading functions
void updateSensorReadings() {
  // Simulate real-time data changes (in real system, read from actual sensors)
  batteryVoltage += random(-5, 5) / 10.0;
  batteryCurrent += random(-20, 20) / 10.0; 
  batterySOC -= 0.1; // Gradually decrease
  batteryTemp += random(-2, 3) / 10.0;
  motorTemp += random(-3, 4) / 10.0;
  motorCurrent += random(-15, 15) / 10.0;
  vehicleSpeed += random(-50, 50) / 10.0;
  motorRPM += random(-100, 100);
  energyConsumption += random(-10, 10) / 10.0;
  motorEfficiency += random(-2, 2);
  
  // Keep values in realistic ranges
  if (batteryVoltage < 40.0) batteryVoltage = 40.0;
  if (batteryVoltage > 54.6) batteryVoltage = 54.6;
  if (batterySOC < 10.0) batterySOC = 10.0;
  if (batterySOC > 100.0) batterySOC = 100.0;
  if (vehicleSpeed < 0) vehicleSpeed = 0;
  if (vehicleSpeed > 60) vehicleSpeed = 60;
  if (motorEfficiency < 85) motorEfficiency = 85;
  if (motorEfficiency > 98) motorEfficiency = 98;
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  Serial.println("EC Vehicle Telemetry System - VEHICLE SIDE");
  Serial.println("ESP32 LoRa Transmitter");
  Serial.println("Sending vehicle data to pitstop...");

  // Initialize random seed
  randomSeed(esp_random());

  // Setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  
  // 868MHz for Europe (as per EC requirements)
  while (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed, retrying...");
    delay(500);
  }
  
  // EC telemetry sync word 
  LoRa.setSyncWord(0xEC); // 'E'fficiency 'C'hallenge
  LoRa.setTxPower(20); // Max power for better range
  LoRa.setSpreadingFactor(7); // Balance between range and data rate
  
  Serial.println("LoRa Vehicle Transmitter Ready!");
  Serial.println("Vehicle ID: AKS-2025-001");
  Serial.println("================================");
}

void loop() {
  // Update sensor readings
  updateSensorReadings();
  
  // Create JSON telemetry packet
  StaticJsonDocument<512> telemetryData;
  telemetryData["id"] = packetID++;
  telemetryData["timestamp"] = millis();
  telemetryData["vehicle_id"] = "AKS-2025-001";
  
  // Battery Management System data
  JsonObject battery = telemetryData.createNestedObject("battery");
  battery["voltage"] = round(batteryVoltage * 10) / 10.0;
  battery["current"] = round(batteryCurrent * 10) / 10.0; 
  battery["soc"] = round(batterySOC * 10) / 10.0;
  battery["temp"] = round(batteryTemp * 10) / 10.0;
  
  // Motor Control System data  
  JsonObject motor = telemetryData.createNestedObject("motor");
  motor["temp"] = round(motorTemp * 10) / 10.0;
  motor["current"] = round(motorCurrent * 10) / 10.0;
  motor["rpm"] = (int)motorRPM;
  motor["efficiency"] = motorEfficiency;
  
  // Vehicle Control System data
  JsonObject vehicle = telemetryData.createNestedObject("vehicle");
  vehicle["speed"] = round(vehicleSpeed * 10) / 10.0;
  vehicle["energy_consumption"] = round(energyConsumption * 10) / 10.0;
  
  // Serialize JSON to string
  String telemetryJson;
  serializeJson(telemetryData, telemetryJson);
  
  // Send LoRa packet to pitstop
  Serial.print("Sending telemetry packet #");
  Serial.print(packetID - 1);
  Serial.print(" (");
  Serial.print(telemetryJson.length());
  Serial.println(" bytes)");
  
  LoRa.beginPacket();
  LoRa.print(telemetryJson);
  LoRa.endPacket();
  
  // Print summary to serial
  Serial.print("Battery: ");
  Serial.print(batteryVoltage);
  Serial.print("V, ");
  Serial.print(batterySOC);
  Serial.print("% | Speed: ");
  Serial.print(vehicleSpeed);
  Serial.print(" km/h | Motor: ");
  Serial.print(motorTemp);
  Serial.println("°C");
  Serial.println("------------------------");
  
  delay(5000); // Send every 5 seconds for real-time monitoring
}