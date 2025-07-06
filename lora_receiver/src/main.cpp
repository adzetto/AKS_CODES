/*********
  ESP32 LoRa Pitstop Receiver
  Efficiency Challenge Electric Vehicle Telemetry System  
  Pitstop-side telemetry receiver - Receives vehicle data for monitoring
  Based on EC Technical Design Report requirements
*********/

#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <SPIFFS.h>

// Define pins used by the LoRa transceiver module for ESP32
#define SS    5     // NSS pin
#define RST   14    // RST pin
#define DIO0  2     // DIO0 pin

// SPI pins are default for ESP32:
// SCK  = GPIO18
// MISO = GPIO19  
// MOSI = GPIO23

// Telemetry monitoring variables
unsigned long lastPacketTime = 0;
int totalPacketsReceived = 0;
int lostPackets = 0;
int lastPacketID = -1;
float bestRSSI = -200;
float worstRSSI = 0;

// Function to log telemetry data
void logTelemetryData(String jsonData, int rssi) {
  Serial.println("=== TELEMETRY LOG ===");
  Serial.print("Time: ");
  Serial.print(millis() / 1000);
  Serial.print("s | RSSI: ");
  Serial.print(rssi);
  Serial.println(" dBm");
  Serial.println("Raw JSON: " + jsonData);
  Serial.println("====================");
}

// Function to analyze signal quality
void analyzeSignalQuality(int rssi) {
  if (rssi > bestRSSI) {
    bestRSSI = rssi;
  }
  if (rssi < worstRSSI) {
    worstRSSI = rssi;
  }
  
  String quality;
  if (rssi > -50) quality = "EXCELLENT";
  else if (rssi > -70) quality = "GOOD";
  else if (rssi > -85) quality = "FAIR"; 
  else quality = "POOR";
  
  Serial.print("Signal Quality: ");
  Serial.print(quality);
  Serial.print(" (Best: ");
  Serial.print(bestRSSI);
  Serial.print(" dBm, Worst: ");
  Serial.print(worstRSSI);
  Serial.println(" dBm)");
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  Serial.println("=======================================");
  Serial.println("EC Telemetry System - PITSTOP SIDE");
  Serial.println("ESP32 LoRa Receiver for Vehicle Data");
  Serial.println("=======================================");

  // Initialize SPIFFS for data logging
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed - Data logging disabled");
  } else {
    Serial.println("SPIFFS Ready - Data logging enabled");
  }

  // Setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  
  // 868MHz for Europe (as per EC requirements)
  while (!LoRa.begin(868E6)) {
    Serial.println("LoRa init failed, retrying...");
    delay(500);
  }
  
  // EC telemetry sync word (must match vehicle)
  LoRa.setSyncWord(0xEC); // 'E'fficiency 'C'hallenge
  LoRa.setSpreadingFactor(7); // Must match transmitter
  
  Serial.println("LoRa Pitstop Receiver Ready!");
  Serial.println("Waiting for vehicle telemetry data...");
  Serial.println("=====================================");
  
  lastPacketTime = millis();
}

void loop() {
  // Check for incoming LoRa packets
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Read the complete packet
    String receivedData = "";
    while (LoRa.available()) {
      receivedData += (char)LoRa.read();
    }
    
    int rssi = LoRa.packetRssi();
    float snr = LoRa.packetSnr();
    totalPacketsReceived++;
    lastPacketTime = millis();
    
    Serial.print("\n>>> PACKET RECEIVED #");
    Serial.print(totalPacketsReceived);
    Serial.print(" | Size: ");
    Serial.print(packetSize);
    Serial.print(" bytes | RSSI: ");
    Serial.print(rssi);
    Serial.print(" dBm | SNR: ");
    Serial.print(snr);
    Serial.println(" dB");
    
    // Parse JSON telemetry data
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, receivedData);
    
    if (error) {
      Serial.println("❌ JSON Parse Error: " + String(error.c_str()));
      Serial.println("Raw data: " + receivedData);
    } else {
      // Successfully parsed JSON - Extract vehicle data
      int packetID = doc["id"];
      String vehicleID = doc["vehicle_id"];
      unsigned long timestamp = doc["timestamp"];
      
      // Check for lost packets
      if (lastPacketID != -1 && packetID != lastPacketID + 1) {
        int lost = packetID - lastPacketID - 1;
        lostPackets += lost;
        Serial.print("⚠️  Lost ");
        Serial.print(lost);
        Serial.println(" packet(s)");
      }
      lastPacketID = packetID;
      
      // Extract battery data
      float battVoltage = doc["battery"]["voltage"];
      float battCurrent = doc["battery"]["current"];
      float battSOC = doc["battery"]["soc"];
      float battTemp = doc["battery"]["temp"];
      
      // Extract motor data
      float motorTemp = doc["motor"]["temp"];
      float motorCurrent = doc["motor"]["current"];
      int motorRPM = doc["motor"]["rpm"];
      int motorEff = doc["motor"]["efficiency"];
      
      // Extract vehicle data
      float speed = doc["vehicle"]["speed"];
      float energyCons = doc["vehicle"]["energy_consumption"];
      
      // Display telemetry in organized format
      Serial.println("┌─────────────────────────────────────────────┐");
      Serial.print("│ Vehicle: ");
      Serial.print(vehicleID);
      Serial.print(" | Packet ID: ");
      Serial.print(packetID);
      Serial.print(" | Time: ");
      Serial.print(timestamp/1000);
      Serial.println("s │");
      Serial.println("├─────────────────────────────────────────────┤");
      Serial.print("│ BATTERY: ");
      Serial.print(battVoltage);
      Serial.print("V, ");
      Serial.print(battCurrent);
      Serial.print("A, ");
      Serial.print(battSOC);
      Serial.print("%, ");
      Serial.print(battTemp);
      Serial.println("°C    │");
      Serial.print("│ MOTOR:   ");
      Serial.print(motorTemp);
      Serial.print("°C, ");
      Serial.print(motorCurrent);
      Serial.print("A, ");
      Serial.print(motorRPM);
      Serial.print("RPM, ");
      Serial.print(motorEff);
      Serial.println("%   │");
      Serial.print("│ VEHICLE: ");
      Serial.print(speed);
      Serial.print("km/h, ");
      Serial.print(energyCons);
      Serial.println("Wh/km              │");
      Serial.println("└─────────────────────────────────────────────┘");
      
      // Analyze signal quality
      analyzeSignalQuality(rssi);
      
      // Check for critical alerts
      if (battSOC < 20.0) {
        Serial.println("🔋 LOW BATTERY WARNING!");
      }
      if (battTemp > 40.0 || motorTemp > 60.0) {
        Serial.println("🌡️  HIGH TEMPERATURE WARNING!");
      }
      if (speed > 55.0) {
        Serial.println("⚡ HIGH SPEED WARNING!");
      }
      
      // Log data to SPIFFS
      logTelemetryData(receivedData, rssi);
    }
    
    Serial.print("📊 Stats: Total: ");
    Serial.print(totalPacketsReceived);
    Serial.print(" | Lost: ");
    Serial.print(lostPackets);
    Serial.print(" | Success Rate: ");
    Serial.print(((float)(totalPacketsReceived - lostPackets) / totalPacketsReceived) * 100, 1);
    Serial.println("%");
    Serial.println();
    
  } else {
    // Check for communication timeout
    if (millis() - lastPacketTime > 15000 && totalPacketsReceived > 0) {
      Serial.println("⚠️  Vehicle communication timeout!");
      lastPacketTime = millis(); // Reset to avoid spam
    }
  }
  
  delay(100); // Small delay for stability
}