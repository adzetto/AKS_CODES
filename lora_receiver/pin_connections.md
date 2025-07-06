# STM32F103C8T6 - LoRa Module Pin Connections

## Hardware Setup Guide

### STM32F103C8T6 (Blue Pill) Pin Mapping

```
    STM32F103C8T6              RFM95/SX127x LoRa Module
    ┌─────────────┐            ┌─────────────────────┐
    │     PA5     │◄──────────►│        SCK          │
    │     PA6     │◄──────────►│       MISO          │
    │     PA7     │◄──────────►│       MOSI          │
    │     PA4     │◄──────────►│    NSS (SS)         │
    │     PA3     │◄──────────►│       RST           │
    │     PA2     │◄──────────►│       DIO0          │
    │    3.3V     │◄──────────►│       3.3V          │
    │     GND     │◄──────────►│       GND           │
    └─────────────┘            │       ANA           │◄── 86.3mm wire antenna
                               └─────────────────────┘
```

### Pin Configuration Summary

| STM32F103C8T6 Pin | Function    | RFM95 Pin | Description                    |
|-------------------|-------------|-----------|--------------------------------|
| PA5               | SPI1_SCK    | SCK       | Serial Clock                   |
| PA6               | SPI1_MISO   | MISO      | Master In Slave Out            |
| PA7               | SPI1_MOSI   | MOSI      | Master Out Slave In            |
| PA4               | GPIO Output | NSS       | Chip Select (Slave Select)     |
| PA3               | GPIO Output | RST       | Reset Pin                      |
| PA2               | GPIO Input  | DIO0      | Digital I/O 0 (Interrupt)     |
| 3.3V              | Power       | 3.3V      | Power Supply                   |
| GND               | Ground      | GND       | Ground Reference               |

### Important Notes

1. **Power Supply**: Use 3.3V for both STM32F103C8T6 and LoRa module
2. **Antenna**: Use 86.3mm straight wire for 868MHz (λ/4 antenna)
3. **Programming**: Use ST-Link V2 programmer for uploading code
4. **Serial Monitor**: Connect USB-to-Serial adapter to PA9 (TX) and PA10 (RX) for debugging

### Programming Setup

```
ST-Link V2                    STM32F103C8T6
┌─────────────┐              ┌─────────────┐
│    3.3V     │◄────────────►│    3.3V     │
│     GND     │◄────────────►│     GND     │
│   SWDIO     │◄────────────►│    PA13     │
│   SWCLK     │◄────────────►│    PA14     │
└─────────────┘              └─────────────┘
```

### Serial Debug Connection (Optional)

```
USB-to-Serial Adapter         STM32F103C8T6
┌─────────────────────┐      ┌─────────────┐
│        RX           │◄─────│     PA9     │ (TX)
│        TX           │─────►│    PA10     │ (RX)
│       GND           │◄─────│     GND     │
└─────────────────────┘      └─────────────┘
```

### Frequency and Range

- **Operating Frequency**: 868MHz (Europe)
- **Expected Range**: 1-5km (depending on environment)
- **Power Output**: Adjustable (default: 17dBm)
- **Bandwidth**: 125kHz
- **Spreading Factor**: 7 (configurable)
