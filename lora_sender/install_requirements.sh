#!/bin/bash
"""
Installation script for Raspberry Pi 5 LoRa dependencies
"""

echo "Installing Python dependencies for Raspberry Pi 5 LoRa sender..."

# Update system packages
sudo apt update

# Install Python3 and pip if not already installed
sudo apt install -y python3 python3-pip

# Install required Python libraries
pip3 install adafruit-circuitpython-rfm9x
pip3 install adafruit-circuitpython-busdevice
pip3 install adafruit-blinka

# Enable SPI interface
echo "Enabling SPI interface..."
sudo raspi-config nonint do_spi 0

echo "Installation completed!"
echo "Please reboot your Raspberry Pi 5 to enable SPI interface."
echo "After reboot, run: python3 main.py"