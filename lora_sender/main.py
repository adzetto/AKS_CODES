#!/usr/bin/env python3
"""
Raspberry Pi 5 LoRa Vehicle Sender
Efficiency Challenge Electric Vehicle Telemetry System
Vehicle-side telemetry sender - Sends real vehicle data to pitstop
Based on EC Technical Design Report requirements
"""

import time
import json
import random
import busio
import digitalio
import board
import adafruit_rfm9x

# LoRa module configuration for Raspberry Pi 5
# SPI setup
spi = busio.SPI(board.SCK, MOSI=board.MOSI, MISO=board.MISO)

# LoRa module pins
cs = digitalio.DigitalInOut(board.CE1)     # Chip Select (GPIO7)
reset = digitalio.DigitalInOut(board.D25)   # Reset (GPIO25)

# Initialize LoRa module
# 868MHz for Europe (as per EC requirements)
rfm9x = adafruit_rfm9x.RFM9x(spi, cs, reset, 868.0)

# LoRa configuration
rfm9x.tx_power = 20  # Max power for better range
rfm9x.spreading_factor = 7  # Balance between range and data rate
rfm9x.signal_bandwidth = 125000  # 125 kHz bandwidth
rfm9x.coding_rate = 5  # 4/5 coding rate

# Vehicle telemetry data variables
packet_id = 0
battery_voltage = 48.5      # V - Batarya paketi gerilimi  
battery_current = 15.3      # A - Batarya akımı
battery_soc = 85.2          # % - State of Charge
battery_temp = 32.1         # °C - Batarya sıcaklığı
motor_temp = 45.7           # °C - Motor sıcaklığı
motor_current = 12.8        # A - Motor akımı
vehicle_speed = 42.3        # km/h - Araç hızı
motor_rpm = 1850.0          # RPM - Motor devir
energy_consumption = 156.7  # Wh/km - Enerji tüketimi
motor_efficiency = 94       # % - Motor verimliliği

def update_sensor_readings():
    """Simulated sensor reading functions"""
    global battery_voltage, battery_current, battery_soc, battery_temp
    global motor_temp, motor_current, vehicle_speed, motor_rpm
    global energy_consumption, motor_efficiency
    
    # Simulate real-time data changes (in real system, read from actual sensors)
    battery_voltage += random.uniform(-0.5, 0.5)
    battery_current += random.uniform(-2.0, 2.0)
    battery_soc -= 0.1  # Gradually decrease
    battery_temp += random.uniform(-0.2, 0.3)
    motor_temp += random.uniform(-0.3, 0.4)
    motor_current += random.uniform(-1.5, 1.5)
    vehicle_speed += random.uniform(-5.0, 5.0)
    motor_rpm += random.uniform(-100, 100)
    energy_consumption += random.uniform(-1.0, 1.0)
    motor_efficiency += random.randint(-2, 2)
    
    # Keep values in realistic ranges
    battery_voltage = max(40.0, min(54.6, battery_voltage))
    battery_soc = max(10.0, min(100.0, battery_soc))
    vehicle_speed = max(0, min(60, vehicle_speed))
    motor_efficiency = max(85, min(98, motor_efficiency))

def create_telemetry_packet():
    """Create JSON telemetry packet"""
    global packet_id
    
    telemetry_data = {
        "id": packet_id,
        "timestamp": int(time.time() * 1000),  # milliseconds
        "vehicle_id": "AKS-2025-001",
        "battery": {
            "voltage": round(battery_voltage, 1),
            "current": round(battery_current, 1),
            "soc": round(battery_soc, 1),
            "temp": round(battery_temp, 1)
        },
        "motor": {
            "temp": round(motor_temp, 1),
            "current": round(motor_current, 1),
            "rpm": int(motor_rpm),
            "efficiency": motor_efficiency
        },
        "vehicle": {
            "speed": round(vehicle_speed, 1),
            "energy_consumption": round(energy_consumption, 1)
        }
    }
    
    packet_id += 1
    return json.dumps(telemetry_data)

def main():
    """Main program loop"""
    print("EC Vehicle Telemetry System - VEHICLE SIDE")
    print("Raspberry Pi 5 LoRa Transmitter")
    print("Sending vehicle data to pitstop...")
    print("Vehicle ID: AKS-2025-001")
    print("LoRa Vehicle Transmitter Ready!")
    print("=" * 50)
    
    try:
        while True:
            # Update sensor readings
            update_sensor_readings()
            
            # Create telemetry packet
            telemetry_json = create_telemetry_packet()
            
            # Send LoRa packet to pitstop
            print(f"Sending telemetry packet #{packet_id - 1} ({len(telemetry_json)} bytes)")
            
            # Convert string to bytes for transmission
            data_bytes = telemetry_json.encode('utf-8')
            
            # Send packet
            rfm9x.send(data_bytes)
            
            # Print summary
            print(f"Battery: {battery_voltage:.1f}V, {battery_soc:.1f}% | "
                  f"Speed: {vehicle_speed:.1f} km/h | "
                  f"Motor: {motor_temp:.1f}°C")
            print("-" * 50)
            
            # Wait 5 seconds before next transmission
            time.sleep(5)
            
    except KeyboardInterrupt:
        print("\nTransmission stopped by user")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    main()