# AKS (Araç Kontrol Sistemi) - Vehicle Control System

This project implements a complete **Vehicle Control System (AKS)** according to Turkish automotive competition specifications, featuring real-time telemetry data generation, display, and monitoring capabilities.

## Project Structure

```
AKS_CODES/
├── AKS_SCREEN/                 # ESP32-S3 7" Display & Control Unit
└── AKS_DATA_GENERATOR_DUMP/    # STM32F411RE Data Generator & Sensors
```

## System Overview

The AKS system consists of two main components:

1. **AKS_SCREEN** (ESP32-S3): 7-inch HMI display for real-time vehicle monitoring
2. **AKS_DATA_GENERATOR_DUMP** (STM32F411RE): Sensor data collection and telemetry generation

## Hardware Requirements

### AKS_SCREEN (ESP32-S3 CrowPanel 7")
- **Board**: CrowPanel ESP32-S3 7.0" HMI Display (800x480)
- **Features**: 
  - LVGL-based touch interface
  - Real-time data visualization
  - LED control buttons
  - I2C communication master

### AKS_DATA_GENERATOR_DUMP (STM32F411RE Nucleo)
- **Board**: STM32F411RE Nucleo Development Board
- **Function**: Generates simulated vehicle telemetry data
- **Features**:
  - No real sensors - all data is simulated in software
  - I2C slave communication
  - Vehicle dynamics simulation

## Pin Connections

### I2C Communication Between Boards

| STM32F411RE (Slave) | ESP32-S3 (Master) | Function |
|---------------------|-------------------|----------|
| PB6 (I2C1_SCL)     | GPIO 19           | I2C Clock |
| PB7 (I2C1_SDA)     | GPIO 20           | I2C Data |
| GND                 | GND               | Ground |
| 3.3V                | 3.3V              | Power |

### STM32F411RE I2C Communication Only

| Pin | Arduino Pin | Function | Connection |
|-----|-------------|----------|------------|
| PB6 | D10 | I2C1_SCL | I2C Clock Line to ESP32 |
| PB7 | D4 | I2C1_SDA | I2C Data Line to ESP32 |

**Note**: No physical sensors are connected. All telemetry data is generated through software simulation.

### ESP32-S3 CrowPanel Connections

| Pin | Function | Connection |
|-----|----------|------------|
| GPIO 19 | I2C_SCL | STM32 I2C Clock |
| GPIO 20 | I2C_SDA | STM32 I2C Data |
| GPIO 38 | LED Control | Onboard LED |
| Touch Panel | Internal | GT911 Touch Controller |

## AKS Functions Implemented

According to competition requirements, the following **7 main AKS functions** are implemented:

### 1. Motor Torque Control (`motor_tork_kontrolu`)
- Real-time speed monitoring and control
- Acceleration/deceleration optimization
- Current limiting and overspeed protection

### 2. Regenerative Braking Optimization (`geri_kazanimli_frenleme`)
- Energy recovery during braking
- Battery charge state consideration
- Mechanical brake integration

### 3. Vehicle Energy Management System (`arac_enerji_yonetimi`)
- Energy consumption monitoring
- Power source optimization
- Remaining energy calculation

### 4. Vehicle Communication System (`arac_ici_haberlesme`)
- I2C protocol implementation
- Data standardization and conversion
- Multi-system integration

### 5. Fault Diagnosis (`ariza_teshisi`)
- Real-time system monitoring
- Fault code generation
- Warning display on screen

### 6. Vehicle Status Monitoring (`arac_durumu_izleme`)
- Speed, temperature, voltage monitoring
- Real-time data display
- User interface updates

### 7. Data Transfer to Monitoring Center (`veri_aktarimi`)
- CSV format telemetry logging
- Serial communication output
- Timestamp synchronization

## Telemetry Data Format

The system generates telemetry data in the following CSV format:

```
zaman_ms;hiz_kmh;T_bat_C;V_bat_V;kalan_enerji_Wh
10000;45.2;28.5;365.2;45500
11000;48.1;28.7;364.8;45450
12000;46.5;29.0;365.0;45400
```

### Data Fields:
- `zaman_ms`: Timestamp in milliseconds since vehicle start
- `hiz_kmh`: Vehicle speed in km/h
- `T_bat_C`: Battery temperature in °C
- `V_bat_V`: Battery voltage in V
- `kalan_enerji_Wh`: Remaining energy in Wh

## Installation & Setup

### 1. Install PlatformIO

```bash
curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py -o get-platformio.py
python3 get-platformio.py
export PATH="/home/$USER/.platformio/penv/bin:$PATH"
```

### 2. Build AKS_SCREEN (ESP32-S3)

```bash
cd AKS_CODES/AKS_SCREEN
platformio run
platformio run --target upload
```

### 3. Build AKS_DATA_GENERATOR_DUMP (STM32F411RE)

```bash
cd AKS_CODES/AKS_DATA_GENERATOR_DUMP
platformio run
platformio run --target upload
```

## System Operation

### Normal Operation Flow:

1. **STM32F411RE** continuously generates simulated telemetry data (no real sensors)
2. **ESP32-S3** requests data via I2C every 5 seconds
3. **Touch Interface** allows manual LED control and system interaction
4. **Serial Output** provides CSV telemetry for monitoring center
5. **Real-time Display** shows current vehicle status

### Simulation Modes:

The STM32 data generator includes 3 simulation modes:

1. **Normal Mode**: Standard vehicle operation
2. **Fault Mode**: Simulates system faults and reduced performance
3. **Charging Mode**: Simulates vehicle charging with energy recovery

Modes automatically cycle every 30 seconds for demonstration purposes.

## Competition Compliance

This system meets all **Turkish Automotive Competition** requirements:

✅ **Indigenous Development**: Custom PCB design and software  
✅ **Multiple Functions**: Implements 7+ required AKS functions  
✅ **Communication Protocol**: I2C for inter-system communication  
✅ **Telemetry System**: Real-time data logging and transmission  
✅ **User Interface**: 7" touch display for monitoring  
✅ **Fault Diagnosis**: Real-time fault detection and display  
✅ **Energy Management**: Battery and energy monitoring  

## Technical Specifications

### Performance Metrics:
- **Data Update Rate**: 1 Hz (1 sample per second)
- **Communication Speed**: I2C at 100kHz
- **Display Resolution**: 800x480 pixels
- **Memory Usage**: <2MB Flash, <128KB RAM
- **Power Consumption**: <5W total system

### Communication Protocol:
- **I2C Address**: 0x42 (STM32 slave)
- **Data Packet Size**: 37 bytes
- **Error Handling**: Timeout and retry mechanisms
- **Data Integrity**: Packed structure with checksum

## Development Team

Developed for Turkish automotive competitions focusing on:
- Electric vehicle control systems
- Real-time embedded programming
- HMI design and implementation
- Sensor integration and data acquisition

## License

This project is developed for educational and competition purposes. Please refer to individual component licenses for specific usage rights.

---

**Note**: This system is designed for demonstration and competition purposes. For production vehicle use, additional safety certifications and testing would be required.