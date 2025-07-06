/*
 * AKS Data Generator - STM32F411RE Nucleo Board
 * Generates SIMULATED telemetry data for Vehicle Control System (AKS)
 * 
 * This system generates dummy/simulated vehicle data and sends via I2C to AKS_SCREEN
 * NO REAL SENSORS CONNECTED - ALL VALUES ARE SIMULATED IN SOFTWARE
 * 
 * Pin Connections (STM32F411RE Nucleo):
 * I2C1_SCL: PB6 (Arduino D10) - Connect to ESP32 SCL (GPIO 19)
 * I2C1_SDA: PB7 (Arduino D4)  - Connect to ESP32 SDA (GPIO 20)
 * GND: Connect to ESP32 GND
 * 3.3V: Connect to ESP32 3.3V
 * 
 * Vehicle Control System Functions Simulated:
 * 1. Motor Torque Control - Simulated speed changes
 * 2. Regenerative Braking Optimization - Energy recovery simulation
 * 3. Vehicle Energy Management System - Battery level simulation
 * 4. Vehicle Communication System - I2C data transfer
 * 5. Fault Diagnosis - Simulated fault conditions
 * 6. Vehicle Status Monitoring - Generated status data
 * 7. Data Transfer to Monitoring Center - Serial output logging
 */

#include <Arduino.h>
#include <Wire.h>

// No real sensor pins needed - all values are simulated
// Only I2C pins are used for communication

// I2C configuration
#define I2C_SLAVE_ADDRESS 0x42    // AKS data generator I2C address
#define I2C_SDA_PIN PB7          // I2C1_SDA 
#define I2C_SCL_PIN PB6          // I2C1_SCL

// AKS telemetry data structure
struct AKS_TelemetryData {
    uint32_t timestamp_ms;        // Time since vehicle start (ms)
    float vehicle_speed_kmh;      // Vehicle speed (km/h)
    float battery_temp_C;         // Battery temperature (°C)
    float battery_voltage_V;      // Total battery voltage (V)
    float remaining_energy_Wh;    // Remaining energy (Wh)
    float motor_temp_C;           // Motor temperature (°C)
    float brake_pressure_bar;     // Brake pressure (bar)
    uint8_t system_status;        // System status flags
    uint8_t fault_codes;          // Fault diagnostic codes
} __attribute__((packed));

// Function declarations
void updateTelemetryData();
void simulateVehicleDynamics();
void onI2CRequest();
void onI2CReceive(int numBytes);
void printTelemetryData();

// Global variables
AKS_TelemetryData telemetryData;
uint32_t lastDataUpdate = 0;
uint32_t vehicleStartTime = 0;
bool dataRequested = false;

// Vehicle simulation parameters
float simulatedSpeed = 0.0;
float batteryCapacity = 50000.0; // 50kWh in Wh
float energyConsumption = 0.0;
uint8_t simulationMode = 1; // 1=normal, 2=fault, 3=charging

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("AKS Data Generator - STM32F411RE");
    Serial.println("Vehicle Control System Telemetry");
    Serial.println("================================");
    
    // Initialize I2C as slave
    Wire.begin(I2C_SLAVE_ADDRESS);
    Wire.onRequest(onI2CRequest);
    Wire.onReceive(onI2CReceive);
    
    // Record vehicle start time
    vehicleStartTime = millis();
    
    Serial.println("System initialized successfully");
    Serial.println("I2C Slave Address: 0x42");
    Serial.println("Waiting for data requests...");
}

void loop() {
    // Update telemetry data every 1 second
    if (millis() - lastDataUpdate >= 1000) {
        updateTelemetryData();
        lastDataUpdate = millis();
        
        // Print telemetry data to serial for debugging
        printTelemetryData();
    }
    
    // Simulate vehicle dynamics
    simulateVehicleDynamics();
    
    delay(100);
}

void updateTelemetryData() {
    telemetryData.timestamp_ms = millis() - vehicleStartTime;
    
    // Generate simulated battery temperature (20-50°C)
    telemetryData.battery_temp_C = 25.0 + random(-50, 250) / 10.0;
    
    // Simulate vehicle speed (0-120 km/h)
    telemetryData.vehicle_speed_kmh = simulatedSpeed;
    
    // Simulate battery voltage (300-420V for electric vehicle)
    telemetryData.battery_voltage_V = 350.0 + (random(0, 4096) / 4095.0) * 70.0;
    
    // Calculate remaining energy based on consumption
    telemetryData.remaining_energy_Wh = max(0.0f, batteryCapacity - energyConsumption);
    
    // Simulate motor temperature (40-120°C)
    telemetryData.motor_temp_C = 40.0 + (random(0, 4096) / 4095.0) * 80.0;
    
    // Simulate brake pressure (0-200 bar)
    telemetryData.brake_pressure_bar = (random(0, 4096) / 4095.0) * 200.0;
    
    // System status flags (bit-encoded)
    // Bit 0: Motor ready
    // Bit 1: Battery OK
    // Bit 2: Brake system OK
    // Bit 3: Charging active
    // Bit 4: Regenerative braking active
    telemetryData.system_status = 0x1F; // All systems OK by default
    
    if (simulationMode == 3) { // Charging mode
        telemetryData.system_status |= 0x08; // Set charging bit
    }
    
    // Fault codes simulation
    telemetryData.fault_codes = 0x00; // No faults by default
    
    if (simulationMode == 2) { // Fault simulation
        telemetryData.fault_codes = 0x04; // Motor overtemperature fault
        telemetryData.system_status &= ~0x01; // Clear motor ready bit
    }
    
    if (telemetryData.battery_temp_C > 60.0) {
        telemetryData.fault_codes |= 0x02; // Battery overtemperature
        telemetryData.system_status &= ~0x02; // Clear battery OK bit
    }
}

void simulateVehicleDynamics() {
    static uint32_t lastUpdate = 0;
    static float acceleration = 0.0;
    
    if (millis() - lastUpdate < 100) return;
    lastUpdate = millis();
    
    // Simple vehicle dynamics simulation
    switch (simulationMode) {
        case 1: // Normal driving
            acceleration = (random(-20, 30) / 10.0); // -2 to +3 m/s²
            break;
        case 2: // Fault mode - reduced performance
            acceleration = (random(-10, 10) / 10.0); // -1 to +1 m/s²
            break;
        case 3: // Charging mode
            simulatedSpeed = 0.0;
            acceleration = 0.0;
            // Simulate energy recovery during charging
            if (telemetryData.remaining_energy_Wh < batteryCapacity) {
                energyConsumption -= 50.0; // Add 50Wh per second while charging
            }
            break;
    }
    
    // Update speed (convert acceleration from m/s² to km/h change)
    simulatedSpeed += acceleration * 3.6 * 0.1; // 0.1s update interval
    simulatedSpeed = constrain(simulatedSpeed, 0.0, 120.0);
    
    // Energy consumption calculation (simplified)
    if (simulatedSpeed > 0 && simulationMode != 3) {
        // Energy consumption in Wh (speed-dependent)
        float powerConsumption = 150.0 + (simulatedSpeed * 2.0); // Base 150W + speed factor
        energyConsumption += (powerConsumption * 0.1) / 3600.0; // Convert to Wh per 0.1s
    }
    
    // Cycle through simulation modes periodically
    static uint32_t modeChangeTime = 0;
    if (millis() - modeChangeTime > 30000) { // Change mode every 30 seconds
        simulationMode++;
        if (simulationMode > 3) simulationMode = 1;
        modeChangeTime = millis();
        
        Serial.print("Switching to simulation mode: ");
        Serial.println(simulationMode);
    }
}

void onI2CRequest() {
    // Send telemetry data when requested by master (ESP32)
    Wire.write((uint8_t*)&telemetryData, sizeof(telemetryData));
    dataRequested = true;
}

void onI2CReceive(int numBytes) {
    // Handle commands from master if needed
    while (Wire.available()) {
        uint8_t command = Wire.read();
        
        switch (command) {
            case 0x01: // Reset energy consumption
                energyConsumption = 0.0;
                Serial.println("Energy consumption reset");
                break;
            case 0x02: // Set normal mode
                simulationMode = 1;
                Serial.println("Set to normal mode");
                break;
            case 0x03: // Set fault mode
                simulationMode = 2;
                Serial.println("Set to fault mode");
                break;
            case 0x04: // Set charging mode
                simulationMode = 3;
                Serial.println("Set to charging mode");
                break;
        }
    }
}

void printTelemetryData() {
    Serial.println("\n=== AKS Telemetry Data ===");
    Serial.print("Timestamp: "); Serial.print(telemetryData.timestamp_ms); Serial.println(" ms");
    Serial.print("Vehicle Speed: "); Serial.print(telemetryData.vehicle_speed_kmh, 1); Serial.println(" km/h");
    Serial.print("Battery Temp: "); Serial.print(telemetryData.battery_temp_C, 1); Serial.println(" °C");
    Serial.print("Battery Voltage: "); Serial.print(telemetryData.battery_voltage_V, 1); Serial.println(" V");
    Serial.print("Remaining Energy: "); Serial.print(telemetryData.remaining_energy_Wh, 0); Serial.println(" Wh");
    Serial.print("Motor Temp: "); Serial.print(telemetryData.motor_temp_C, 1); Serial.println(" °C");
    Serial.print("Brake Pressure: "); Serial.print(telemetryData.brake_pressure_bar, 1); Serial.println(" bar");
    Serial.print("System Status: 0x"); Serial.println(telemetryData.system_status, HEX);
    Serial.print("Fault Codes: 0x"); Serial.println(telemetryData.fault_codes, HEX);
    Serial.print("Simulation Mode: "); Serial.println(simulationMode);
    
    if (dataRequested) {
        Serial.println("*** Data sent to AKS_SCREEN ***");
        dataRequested = false;
    }
}
