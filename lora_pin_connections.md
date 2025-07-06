# LoRa Pin Connections

## STM32F103C8T6 (Sender) Pin Configuration

### LoRa Module Connections
- **SS (NSS)**: PA4 - Slave Select pin for SPI communication
- **RST**: PA3 - Reset pin for LoRa module
- **DIO0**: PA2 - Digital I/O pin for LoRa interrupts

### SPI1 Bus Connections
- **SCK**: PA5 - Serial Clock (SPI1_SCK)
- **MISO**: PA6 - Master In Slave Out (SPI1_MISO)
- **MOSI**: PA7 - Master Out Slave In (SPI1_MOSI)

### Power Connections
- **VCC**: 3.3V - Power supply for LoRa module
- **GND**: GND - Ground connection

## STM32F411RE (Receiver) Pin Configuration

### LoRa Module Connections
- **SS (NSS)**: PA4 - Slave Select pin for SPI communication
- **RST**: PA3 - Reset pin for LoRa module
- **DIO0**: PA2 - Digital I/O pin for LoRa interrupts

### SPI1 Bus Connections
- **SCK**: PA5 - Serial Clock (SPI1_SCK)
- **MISO**: PA6 - Master In Slave Out (SPI1_MISO)
- **MOSI**: PA7 - Master Out Slave In (SPI1_MOSI)

### Power Connections
- **VCC**: 3.3V - Power supply for LoRa module
- **GND**: GND - Ground connection

## Communication Parameters
- **Frequency**: 868MHz (Europe ISM band)
- **Sync Word**: 0xEC (Efficiency Challenge)
- **Spreading Factor**: 7 (Balance between range and data rate)
- **Transmission Power**: 20dBm (Maximum)
- **Bandwidth**: 125kHz (Default)
- **Coding Rate**: 4/5 (Default)

## Wiring Diagram

```
STM32F103C8T6/F411RE    LoRa Module (SX1278)
    PA4    ----------->    NSS
    PA3    ----------->    RST
    PA2    ----------->    DIO0
    PA5    ----------->    SCK
    PA6    ----------->    MISO
    PA7    ----------->    MOSI
    3.3V   ----------->    VCC
    GND    ----------->    GND
```

## Important Notes
- Both sender and receiver use identical pin configurations for consistency
- Use 3.3V power supply for LoRa module (not 5V)
- Ensure proper antenna connection for optimal range
- DIO0 pin is used for transmission/reception complete interrupts
- SPI1 bus is used for communication with LoRa module