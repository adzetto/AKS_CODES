# Raspberry Pi 5 LoRa Setup Guide

## Hardware Connections

### LoRa Module to Raspberry Pi 5 GPIO Pins:
- **VCC** → 3.3V (Pin 1)
- **GND** → Ground (Pin 6)
- **SCK** → GPIO11 (Pin 23) - SPI Clock
- **MISO** → GPIO9 (Pin 21) - SPI MISO
- **MOSI** → GPIO10 (Pin 19) - SPI MOSI
- **CS** → GPIO7 (Pin 26) - Chip Select
- **RST** → GPIO25 (Pin 22) - Reset
- **DIO0** → GPIO24 (Pin 18) - Digital I/O

## Software Installation

1. **Run installation script:**
   ```bash
   chmod +x install_requirements.sh
   ./install_requirements.sh
   ```

2. **Reboot Raspberry Pi:**
   ```bash
   sudo reboot
   ```

3. **Run the LoRa sender:**
   ```bash
   python3 main.py
   ```

## Features

- **868MHz LoRa frequency** (Europe)
- **Real-time telemetry data** simulation
- **JSON packet format** for structured data
- **5-second transmission interval**
- **Vehicle ID**: AKS-2025-001

## Troubleshooting

- Ensure SPI is enabled: `sudo raspi-config`
- Check wiring connections
- Verify LoRa module power (3.3V)
- Monitor serial output for errors