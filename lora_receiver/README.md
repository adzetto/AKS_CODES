# LoRa Pitstop Receiver - STM32F103C8T6

## Efficiency Challenge Telemetry System - Pitstop Module

Bu proje STM32F103C8T6 (Blue Pill) ile LoRa modülü kullanarak araçtan gelen telemetri verilerini almak ve analiz etmek için tasarlanmıştır.

## Hardware Bağlantıları

### STM32F103C8T6 - RFM95 LoRa Modülü

| STM32F103C8T6 Pin | RFM95 Pin | Açıklama |
|-------------------|-----------|-----------|
| PA5               | SCK       | SPI Clock |
| PA6               | MISO      | SPI Master In Slave Out |
| PA7               | MOSI      | SPI Master Out Slave In |
| PA4               | NSS       | Chip Select |
| PA3               | RST       | Reset |
| PA2               | DIO0      | Digital I/O 0 |
| 3.3V              | 3.3V      | Power |
| GND               | GND       | Ground |
| -                 | ANA       | Antenna (86.3mm wire for 868MHz) |

## Kurulum

1. PlatformIO kurulu olmalı
2. ST-Link programlayıcı gerekli
3. Proje klasörünü açın: `lora_receiver`
4. Gerekli kütüphaneler otomatik yüklenecek
5. STM32F103C8T6 kartını ST-Link ile bağlayın
6. Upload edin

## Özellikler

### Telemetri İzleme
- Gerçek zamanlı araç verisi alımı
- JSON formatında telemetri verisi işleme
- RSSI ve SNR sinyal kalitesi analizi
- Kayıp paket tespit sistemi
- Serial port üzerinden veri loglama

### Kritik Uyarılar
- Düşük batarya uyarısı (<%20)
- Yüksek sıcaklık uyarısı (>40°C batarya, >60°C motor)
- Hız limitini aşma uyarısı (>55 km/h)
- İletişim zaman aşımı tespiti

## Test Çıktısı

Karşı tarafta STM32 vehicle sender çalışır durumda olmalı. Program başladığında:
```
=======================================
EC Telemetry System - PITSTOP SIDE
STM32F103C8T6 LoRa Receiver for Vehicle Data
=======================================
LoRa Pitstop Receiver Ready!
Waiting for vehicle telemetry data...

>>> PACKET RECEIVED #1 | Size: 285 bytes | RSSI: -45 dBm | SNR: 9.2 dB
┌─────────────────────────────────────────────┐
│ Vehicle: AKS-2025-001 | Packet ID: 0 | Time: 12s       │
├─────────────────────────────────────────────┤
│ BATTERY: 48.5V, 15.3A, 85.2%, 32.1°C              │
│ MOTOR:   45.7°C, 12.8A, 1850RPM, 94%              │
│ VEHICLE: 42.3km/h, 156.7Wh/km                      │
└─────────────────────────────────────────────┘
Signal Quality: GOOD (Best: -45 dBm, Worst: -45 dBm)
📈 Stats: Total: 1 | Lost: 0 | Success Rate: 100.0%
```

## RSSI Değerleri

RSSI (Received Signal Strength Indicator) değerleri:
- -30 dBm: Çok güçlü sinyal
- -50 dBm: Güçlü sinyal
- -70 dBm: Orta sinyal
- -90 dBm: Zayıf sinyal
- -100 dBm ve altı: Çok zayıf sinyal