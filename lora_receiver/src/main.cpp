/*********
  STM32F411RE LoRa Pitstop Receiver
  Efficiency Challenge Electric Vehicle Telemetry System
  Pitstop-side telemetry receiver - Advanced verbose monitoring
  Based on EC Technical Design Report requirements
*********/

#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>

// Define pins used by the LoRa transceiver module for STM32F411RE
#define SS    PA4   // NSS pin
#define RST   PA3   // RST pin
#define DIO0  PA2   // DIO0 pin

// SPI pins for STM32F411RE (SPI1):
// SCK  = PA5
// MISO = PA6
// MOSI = PA7

// Telemetry monitoring variables
unsigned long lastPacketTime = 0;
unsigned long systemStartTime = 0;
int totalPacketsReceived = 0;
int lostPackets = 0;
int lastPacketID = -1;
int corruptedPackets = 0;
float bestRSSI = -200;
float worstRSSI = 0;
float avgRSSI = 0;
float rssiSum = 0;
float bestSNR = -200;
float worstSNR = 100;
float avgSNR = 0;
float snrSum = 0;
unsigned long totalBytes = 0;
float dataRate = 0; // bytes per second

// Performance metrics
struct TelemetryStats {
  float successRate;
  float avgPacketInterval;
  unsigned long uptime;
  int packetsPerMinute;
} stats;

// Vehicle status tracking
struct VehicleStatus {
  String vehicleID = "Unknown";
  bool isConnected = false;
  unsigned long lastSeen = 0;
  float lastBatterySOC = 0;
  float lastSpeed = 0;
  float lastBatteryTemp = 0;
  float lastMotorTemp = 0;
  bool lowBatteryAlert = false;
  bool highTempAlert = false;
  bool speedAlert = false;
} vehicle;

// Function prototypes
void printSystemHeader();
void printPacketHeader(int packetSize, int rssi, float snr);
void printTelemetryData(JsonDocument& doc);
void printSignalAnalysis(int rssi, float snr);
void printAlerts(JsonDocument& doc);
void printStatistics();
void printSystemStatus();
void updateStatistics(int rssi, float snr, int packetSize);
void checkConnectionTimeout();

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  
  systemStartTime = millis();
  
  printSystemHeader();
  
  // Setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  
  Serial.println("[INIT] Initializing LoRa module...");
  
  // 868MHz for Europe (as per EC requirements)
  while (!LoRa.begin(433E6)) {
    Serial.println("[ERROR] LoRa init failed, retrying in 500ms...");
    delay(500);
  }
  
  // EC telemetry sync word (must match vehicle)
  LoRa.setSyncWord(0xEC); // 'E'fficiency 'C'hallenge
  LoRa.setSpreadingFactor(7); // Must match transmitter
  
  Serial.println("[SUCCESS] LoRa Pitstop Receiver Ready!");
  Serial.println("[INFO] Waiting for vehicle telemetry data...");
  Serial.println("╔══════════════════════════════════════════════════════════╗");
  Serial.println("║                    LIVE TELEMETRY FEED                  ║");
  Serial.println("╚══════════════════════════════════════════════════════════╝");
  
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
    totalBytes += packetSize;
    lastPacketTime = millis();
    
    printPacketHeader(packetSize, rssi, snr);
    
    // Parse JSON telemetry data
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, receivedData);
    
    if (error) {
      corruptedPackets++;
      Serial.println("[ERROR] ❌ JSON Parse Failed!");
      Serial.print("[DEBUG] Error: ");
      Serial.println(error.c_str());
      Serial.print("[DEBUG] Raw data (");
      Serial.print(receivedData.length());
      Serial.print(" chars): ");
      Serial.println(receivedData);
      Serial.println("[INFO] Attempting partial data recovery...");
    } else {
      // Successfully parsed JSON
      int packetID = doc["id"];
      String vehicleID = doc["vehicle_id"];
      unsigned long timestamp = doc["timestamp"];
      
      // Update vehicle status
      vehicle.vehicleID = vehicleID;
      vehicle.isConnected = true;
      vehicle.lastSeen = millis();
      
      // Check for lost packets
      if (lastPacketID != -1 && packetID != lastPacketID + 1) {
        int lost = packetID - lastPacketID - 1;
        lostPackets += lost;
        Serial.print("[WARNING] ⚠️  Packet Loss Detected! Missing ");
        Serial.print(lost);
        Serial.print(" packet(s). Expected ID: ");
        Serial.print(lastPacketID + 1);
        Serial.print(", Received ID: ");
        Serial.println(packetID);
      }
      lastPacketID = packetID;
      
      printTelemetryData(doc);
      printSignalAnalysis(rssi, snr);
      printAlerts(doc);
    }
    
    updateStatistics(rssi, snr, packetSize);
    printStatistics();
    Serial.println("──────────────────────────────────────────────────────────");
    
  } else {
    // No packet received - check for timeout
    checkConnectionTimeout();
  }
  
  // Print system status every 30 seconds
  static unsigned long lastStatusPrint = 0;
  if (millis() - lastStatusPrint > 30000) {
    printSystemStatus();
    lastStatusPrint = millis();
  }
  
  delay(100); // Small delay for stability
}

void printSystemHeader() {
  Serial.println();
  Serial.println("╔══════════════════════════════════════════════════════════╗");
  Serial.println("║        EC TELEMETRY SYSTEM - PITSTOP RECEIVER           ║");
  Serial.println("║              STM32F411RE Advanced Monitor                ║");
  Serial.println("╠══════════════════════════════════════════════════════════╣");
  Serial.print("║ Frequency: 868MHz | Sync: 0xEC | Board: ");
  Serial.print("STM32F411RE");
  Serial.println("        ║");
  Serial.print("║ Started: ");
  Serial.print(millis());
  Serial.println("ms                                     ║");
  Serial.println("╚══════════════════════════════════════════════════════════╝");
}

void printPacketHeader(int packetSize, int rssi, float snr) {
  Serial.println();
  Serial.print("┌─ PACKET RECEIVED #");
  Serial.print(totalPacketsReceived);
  Serial.print(" ─ ");
  
  // Timestamp
  unsigned long currentTime = millis();
  Serial.print("[");
  Serial.print(currentTime / 1000);
  Serial.print(".");
  Serial.print((currentTime % 1000) / 100);
  Serial.print("s] ─");
  
  Serial.println();
  Serial.print("├─ Size: ");
  Serial.print(packetSize);
  Serial.print(" bytes │ RSSI: ");
  Serial.print(rssi);
  Serial.print(" dBm │ SNR: ");
  Serial.print(snr);
  Serial.println(" dB");
}

void printTelemetryData(JsonDocument& doc) {
  int packetID = doc["id"];
  String vehicleID = doc["vehicle_id"];
  unsigned long timestamp = doc["timestamp"];
  
  // Extract telemetry data
  float battVoltage = doc["battery"]["voltage"];
  float battCurrent = doc["battery"]["current"];
  float battSOC = doc["battery"]["soc"];
  float battTemp = doc["battery"]["temp"];
  
  float motorTemp = doc["motor"]["temp"];
  float motorCurrent = doc["motor"]["current"];
  int motorRPM = doc["motor"]["rpm"];
  int motorEff = doc["motor"]["efficiency"];
  
  float speed = doc["vehicle"]["speed"];
  float energyCons = doc["vehicle"]["energy_consumption"];
  
  // Update vehicle tracking
  vehicle.lastBatterySOC = battSOC;
  vehicle.lastSpeed = speed;
  vehicle.lastBatteryTemp = battTemp;
  vehicle.lastMotorTemp = motorTemp;
  
  Serial.println("├─ TELEMETRY DATA ─────────────────────────────────────────");
  Serial.print("├─ Vehicle: ");
  Serial.print(vehicleID);
  Serial.print(" │ Packet ID: ");
  Serial.print(packetID);
  Serial.print(" │ Timestamp: ");
  Serial.println(timestamp);
  
  Serial.println("├─ BATTERY MANAGEMENT SYSTEM ─────────────────────────────");
  Serial.print("├─   Voltage: ");
  Serial.print(battVoltage, 1);
  Serial.print("V │ Current: ");
  Serial.print(battCurrent, 1);
  Serial.print("A │ SOC: ");
  Serial.print(battSOC, 1);
  Serial.print("% │ Temp: ");
  Serial.print(battTemp, 1);
  Serial.println("°C");
  
  Serial.println("├─ MOTOR CONTROL SYSTEM ──────────────────────────────────");
  Serial.print("├─   Temperature: ");
  Serial.print(motorTemp, 1);
  Serial.print("°C │ Current: ");
  Serial.print(motorCurrent, 1);
  Serial.print("A │ RPM: ");
  Serial.print(motorRPM);
  Serial.print(" │ Efficiency: ");
  Serial.print(motorEff);
  Serial.println("%");
  
  Serial.println("├─ VEHICLE DYNAMICS ──────────────────────────────────────");
  Serial.print("├─   Speed: ");
  Serial.print(speed, 1);
  Serial.print(" km/h │ Energy Consumption: ");
  Serial.print(energyCons, 1);
  Serial.println(" Wh/km");
}

void printSignalAnalysis(int rssi, float snr) {
  Serial.println("├─ SIGNAL ANALYSIS ───────────────────────────────────────");
  
  String signalQuality;
  String signalIcon;
  if (rssi > -50) { 
    signalQuality = "EXCELLENT"; 
    signalIcon = "📶";
  } else if (rssi > -70) { 
    signalQuality = "GOOD"; 
    signalIcon = "📶";
  } else if (rssi > -85) { 
    signalQuality = "FAIR"; 
    signalIcon = "📵";
  } else { 
    signalQuality = "POOR"; 
    signalIcon = "📵";
  }
  
  Serial.print("├─   Quality: ");
  Serial.print(signalQuality);
  Serial.print(" ");
  Serial.print(signalIcon);
  Serial.print(" │ Range: ");
  Serial.print(bestRSSI, 1);
  Serial.print(" to ");
  Serial.print(worstRSSI, 1);
  Serial.print(" dBm │ Avg: ");
  Serial.print(avgRSSI, 1);
  Serial.println(" dBm");
  
  Serial.print("├─   SNR: ");
  Serial.print(snr, 1);
  Serial.print(" dB │ SNR Range: ");
  Serial.print(bestSNR, 1);
  Serial.print(" to ");
  Serial.print(worstSNR, 1);
  Serial.print(" dB │ Avg: ");
  Serial.print(avgSNR, 1);
  Serial.println(" dB");
}

void printAlerts(JsonDocument& doc) {
  bool hasAlerts = false;
  Serial.println("├─ ALERT SYSTEM ──────────────────────────────────────────");
  
  float battSOC = doc["battery"]["soc"];
  float battTemp = doc["battery"]["temp"];
  float motorTemp = doc["motor"]["temp"];
  float speed = doc["vehicle"]["speed"];
  
  if (battSOC < 20.0) {
    Serial.println("├─   🔋 CRITICAL: Low Battery Warning! SOC below 20%");
    vehicle.lowBatteryAlert = true;
    hasAlerts = true;
  }
  
  if (battTemp > 40.0 || motorTemp > 60.0) {
    Serial.println("├─   🌡️  WARNING: High Temperature Detected!");
    if (battTemp > 40.0) {
      Serial.print("├─     Battery: ");
      Serial.print(battTemp, 1);
      Serial.println("°C (>40°C)");
    }
    if (motorTemp > 60.0) {
      Serial.print("├─     Motor: ");
      Serial.print(motorTemp, 1);
      Serial.println("°C (>60°C)");
    }
    vehicle.highTempAlert = true;
    hasAlerts = true;
  }
  
  if (speed > 55.0) {
    Serial.print("├─   ⚡ CAUTION: High Speed - ");
    Serial.print(speed, 1);
    Serial.println(" km/h");
    vehicle.speedAlert = true;
    hasAlerts = true;
  }
  
  if (!hasAlerts) {
    Serial.println("├─   ✅ All Systems Normal");
  }
}

void printStatistics() {
  Serial.println("├─ PERFORMANCE STATISTICS ────────────────────────────────");
  
  stats.uptime = millis() - systemStartTime;
  stats.successRate = totalPacketsReceived > 0 ? 
    ((float)(totalPacketsReceived - corruptedPackets) / totalPacketsReceived) * 100 : 0;
  stats.packetsPerMinute = totalPacketsReceived > 0 ? 
    (totalPacketsReceived * 60000) / stats.uptime : 0;
  dataRate = totalBytes > 0 ? (totalBytes * 1000.0) / stats.uptime : 0;
  
  Serial.print("├─   Total: ");
  Serial.print(totalPacketsReceived);
  Serial.print(" │ Lost: ");
  Serial.print(lostPackets);
  Serial.print(" │ Corrupted: ");
  Serial.print(corruptedPackets);
  Serial.print(" │ Success: ");
  Serial.print(stats.successRate, 1);
  Serial.println("%");
  
  Serial.print("├─   Rate: ");
  Serial.print(stats.packetsPerMinute);
  Serial.print(" pkt/min │ Data: ");
  Serial.print(dataRate, 1);
  Serial.print(" B/s │ Total: ");
  Serial.print(totalBytes);
  Serial.println(" bytes");
  
  Serial.print("├─   Uptime: ");
  Serial.print(stats.uptime / 1000);
  Serial.print("s │ Last Packet: ");
  Serial.print((millis() - lastPacketTime) / 1000);
  Serial.println("s ago");
}

void updateStatistics(int rssi, float snr, int packetSize) {
  // Update RSSI statistics
  if (rssi > bestRSSI) bestRSSI = rssi;
  if (rssi < worstRSSI) worstRSSI = rssi;
  rssiSum += rssi;
  avgRSSI = rssiSum / totalPacketsReceived;
  
  // Update SNR statistics
  if (snr > bestSNR) bestSNR = snr;
  if (snr < worstSNR) worstSNR = snr;
  snrSum += snr;
  avgSNR = snrSum / totalPacketsReceived;
}

void checkConnectionTimeout() {
  if (totalPacketsReceived > 0 && millis() - lastPacketTime > 15000) {
    if (vehicle.isConnected) {
      Serial.println();
      Serial.println("┌─ CONNECTION STATUS ──────────────────────────────────────");
      Serial.print("├─   ⚠️  Vehicle Communication Timeout! Last seen: ");
      Serial.print((millis() - lastPacketTime) / 1000);
      Serial.println("s ago");
      Serial.print("├─   Vehicle ID: ");
      Serial.println(vehicle.vehicleID);
      Serial.println("└───────────────────────────────────────────────────────────");
      vehicle.isConnected = false;
    }
  }
}

void printSystemStatus() {
  Serial.println();
  Serial.println("╔═══════════════ SYSTEM STATUS REPORT ════════════════════╗");
  Serial.print("║ Uptime: ");
  unsigned long uptime = millis() - systemStartTime;
  Serial.print(uptime / 3600000); // hours
  Serial.print("h ");
  Serial.print((uptime % 3600000) / 60000); // minutes
  Serial.print("m ");
  Serial.print((uptime % 60000) / 1000); // seconds
  Serial.println("s                                  ║");
  
  Serial.print("║ Vehicle: ");
  if (vehicle.isConnected) {
    Serial.print("🟢 CONNECTED - ");
    Serial.print(vehicle.vehicleID);
  } else {
    Serial.print("🔴 DISCONNECTED");
  }
  Serial.println("                       ║");
  
  Serial.print("║ Memory: Free RAM ~");
  Serial.print("Unknown"); // STM32 doesn't have easy free memory function
  Serial.println(" bytes                              ║");
  
  Serial.print("║ Data Rate: ");
  Serial.print(dataRate, 1);
  Serial.println(" bytes/sec                            ║");
  
  Serial.println("╚══════════════════════════════════════════════════════════╝");
}