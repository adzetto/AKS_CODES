#!/bin/bash

# GDB Debugging Script for STM32 via ST-Link
# Project: LoRa Receiver

set -e

PROJECT_DIR="/home/lenovo/Desktop/AKS_CODES/lora_receiver"
FIRMWARE_ELF=".pio/build/bluepill/firmware.elf"

echo "=== STM32 GDB Debug Session ==="
echo "Project: $PROJECT_DIR"
echo "Target: STM32F103C8T6 (Blue Pill)"
echo "Debug Tool: ST-Link"
echo

# Check if required tools are installed
check_tools() {
    local missing_tools=()
    
    if ! command -v openocd &> /dev/null; then
        missing_tools+=("openocd")
    fi
    
    if ! command -v arm-none-eabi-gdb &> /dev/null && ! command -v gdb-multiarch &> /dev/null; then
        missing_tools+=("arm-none-eabi-gdb or gdb-multiarch")
    fi
    
    if [ ${#missing_tools[@]} -gt 0 ]; then
        echo "Error: Missing required tools: ${missing_tools[*]}"
        echo
        echo "Install with:"
        echo "  sudo apt update"
        echo "  sudo apt install openocd gcc-arm-none-eabi gdb-multiarch"
        echo
        echo "Alternative: Use PlatformIO debug instead:"
        echo "  pio debug --interface=gdb -x .pioinit"
        exit 1
    fi
}

# Check if project directory exists
if [ ! -d "$PROJECT_DIR" ]; then
    echo "Error: Project directory not found: $PROJECT_DIR"
    exit 1
fi

cd "$PROJECT_DIR"

# Check tools first
check_tools

# Check if firmware is built
if [ ! -f "$FIRMWARE_ELF" ]; then
    echo "Firmware not found. Building project..."
    pio run
    
    if [ ! -f "$FIRMWARE_ELF" ]; then
        echo "Error: Build failed or firmware not generated"
        exit 1
    fi
fi

echo "Starting OpenOCD server..."
# Start OpenOCD in background
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg &
OPENOCD_PID=$!

# Wait for OpenOCD to start
sleep 2

# Cleanup function
cleanup() {
    echo "Stopping OpenOCD..."
    kill $OPENOCD_PID 2>/dev/null || true
    exit 0
}

# Set trap for cleanup
trap cleanup INT TERM

echo "Starting GDB session..."
echo "Common GDB commands:"
echo "  load          - Load firmware to target"
echo "  monitor reset halt - Reset and halt target"
echo "  continue      - Continue execution"
echo "  break setup   - Set breakpoint at setup()"
echo "  break loop    - Set breakpoint at loop()"
echo "  step          - Step one instruction"
echo "  next          - Step over function calls"
echo "  info registers - Show CPU registers"
echo "  bt            - Show backtrace"
echo "  quit          - Exit GDB"
echo

# Start GDB with target connection
if command -v arm-none-eabi-gdb &> /dev/null; then
    GDB_CMD="arm-none-eabi-gdb"
else
    GDB_CMD="gdb-multiarch"
fi

$GDB_CMD "$FIRMWARE_ELF" \
    -ex "target extended-remote localhost:3333" \
    -ex "monitor reset halt" \
    -ex "load" \
    -ex "monitor reset halt" \
    -ex "break setup" \
    -ex "continue"

# Cleanup when GDB exits
cleanup