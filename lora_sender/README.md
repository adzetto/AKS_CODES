# LoRa Vehicle Sender - STM32 Nucleo F411RE

## Efficiency Challenge Telemetry System - Vehicle Module

Bu proje STM32 Nucleo F411RE ile LoRa modülü kullanarak araç telemetri verilerini pitstop'a göndermek için tasarlanmıştır.

## Hardware Bağlantıları

### STM32 Nucleo F411RE - RFM95 LoRa Modülü

| STM32 Pin | RFM95 Pin | Açıklama |
|-----------|-----------|-----------|
| PA5 (D13) | SCK       | SPI Clock |
| PA6 (D12) | MISO      | SPI Master In Slave Out |
| PA7 (D11) | MOSI      | SPI Master Out Slave In |
| PA4 (D10) | NSS       | Chip Select |
| PA8 (D7)  | RST       | Reset |
| PA9 (D8)  | DIO0      | Digital I/O 0 |
| 3.3V      | 3.3V      | Power |
| GND       | GND       | Ground |
| -         | ANA       | Antenna (86.3mm wire for 868MHz) |

## Kurulum

1. PlatformIO kurulu olmalı
2. Proje klasörünü açın: `lora_sender`
3. Gerekli kütüphaneler otomatik yüklenecek
4. STM32 Nucleo F411RE kartını bilgisayara bağlayın
5. Upload edin

## Kullanım

- Program gerçek zamanlı araç telemetri verilerini JSON formatında gönderir
- Her 5 saniyede bir telemetri paketi iletilir
- EC raporuna uygun veriler: Batarya, Motor, Araç hızı
- Frekans: 868MHz (Avrupa)
- Sync Word: 0xEC (Efficiency Challenge)

## Telemetri Verileri

### Batarya Yönetim Sistemi (BYS)
- Batarya Gerilimi (V)
- Batarya Akımı (A) 
- State of Charge - SOC (%)
- Batarya Sıcaklığı (°C)

### Motor Kontrol Sistemi
- Motor Sıcaklığı (°C)
- Motor Akımı (A)
- Motor RPM
- Motor Verimliliği (%)

### Araç Kontrol Sistemi
- Araç Hızı (km/h)
- Enerji Tüketimi (Wh/km)

## Test

Karşı tarafta ESP32 pitstop receiver çalışır durumda olmalı. Program başladığında:
```
EC Vehicle Telemetry System - VEHICLE SIDE
STM32 F411RE LoRa Transmitter
LoRa Vehicle Transmitter Ready!
Vehicle ID: AKS-2025-001
Sending telemetry packet #0 (XXX bytes)
Battery: 48.5V, 85.2% | Speed: 42.3 km/h | Motor: 45.7°C
```