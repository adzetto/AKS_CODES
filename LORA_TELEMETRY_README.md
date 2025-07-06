# LoRa Telemetry System - Efficiency Challenge

## Proje Açıklaması

Bu proje, Efficiency Challenge elektrikli araç yarışması için tasarlanmış telemetri sistemidir. Pitstop ve araç arasında LoRa teknolojisi kullanarak kablosuz iletişim sağlar.

## Sistem Mimarisi

```
┌─────────────────┐      LoRa 868MHz      ┌─────────────────┐
│     VEHICLE     │ ────────────────────► │    PITSTOP      │
│ STM32 F411RE    │   Telemetri Verisi   │     ESP32       │
│   + RFM95       │                      │   + RFM95       │
└─────────────────┘                      └─────────────────┘
     SENDER                                  RECEIVER
   (Araç Tarafı)                         (Pitstop Tarafı)
```

## Klasör Yapısı

```
AKS_CODES/
├── lora_sender/          # STM32 Nucleo F411RE Sender
│   ├── platformio.ini    # PlatformIO konfigürasyonu
│   ├── src/
│   │   └── main.cpp      # Ana kod
│   ├── include/
│   ├── lib/
│   ├── test/
│   └── README.md         # STM32 sender rehberi
│
└── lora_receiver/        # ESP32 Receiver  
    ├── platformio.ini    # PlatformIO konfigürasyonu
    ├── src/
    │   └── main.cpp      # Ana kod
    ├── include/
    ├── lib/
    ├── test/
    └── README.md         # ESP32 receiver rehberi
```

## Teknik Özellikler

### LoRa Parametreleri
- **Frekans**: 868MHz (Avrupa)
- **Sync Word**: 0xEC (Efficiency Challenge)
- **Modulation**: LoRa
- **Bandwidth**: 125kHz 
- **Spreading Factor**: 7 (hız/menzil dengesi)
- **Coding Rate**: 4/5
- **TX Power**: 20dBm (maksimum menzil için)

### Hardware Gereksinimleri
- STM32 Nucleo F411RE
- ESP32 Development Board
- 2x RFM95 LoRa Transceiver
- Antenler (86.3mm tel, 868MHz için)
- Jumper kablolar

## Kurulum ve Test

### 1. STM32 Sender Kurulumu
```bash
cd lora_sender
pio run -t upload
```

### 2. ESP32 Receiver Kurulumu  
```bash
cd lora_receiver
pio run -t upload
```

### 3. Test Süreci
1. Her iki karta da kodu yükleyin
2. Önce receiver'ı çalıştırın
3. Sonra sender'ı çalıştırın
4. Seri monitörlerden iletişimi gözlemleyin

## Beklenen Çıktı

**Vehicle Sender (STM32):**
```
EC Vehicle Telemetry System - VEHICLE SIDE
STM32 F411RE LoRa Transmitter
LoRa Vehicle Transmitter Ready!
Vehicle ID: AKS-2025-001
Sending telemetry packet #0 (285 bytes)
Battery: 48.5V, 85.2% | Speed: 42.3 km/h | Motor: 45.7°C
```

**Pitstop Receiver (ESP32):**
```
EC Telemetry System - PITSTOP SIDE
ESP32 LoRa Receiver for Vehicle Data
>>> PACKET RECEIVED #1 | Size: 285 bytes | RSSI: -45 dBm
┌─────────────────────────────────────────────┐
│ Vehicle: AKS-2025-001 | Packet ID: 0        │
├─────────────────────────────────────────────┤
│ BATTERY: 48.5V, 15.3A, 85.2%, 32.1°C       │
│ MOTOR:   45.7°C, 12.8A, 1850RPM, 94%       │
│ VEHICLE: 42.3km/h, 156.7Wh/km              │
└─────────────────────────────────────────────┘
Signal Quality: GOOD | Stats: Total: 1 | Success Rate: 100%
```

## Geliştirme Notları

- Kodlar Random Nerd Tutorials ESP32 LoRa örneğinden adapte edilmiştir
- EC Technical Design Report gereksinimlerine uygun telemetri verileri
- STM32 araç tarafında simüle edilmiş sensör verileri
- JSON formatında yapılandırılmış veri iletişimi
- Her 5 saniyede bir gerçek zamanlı telemetri paketi
- RSSI, SNR ve paket kaybı analizi
- Kritik durum uyarı sistemi

## Telemetri Veri Formatı

```json
{
  "id": 0,
  "timestamp": 12000,
  "vehicle_id": "AKS-2025-001",
  "battery": {
    "voltage": 48.5,
    "current": 15.3,
    "soc": 85.2,
    "temp": 32.1
  },
  "motor": {
    "temp": 45.7,
    "current": 12.8,
    "rpm": 1850,
    "efficiency": 94
  },
  "vehicle": {
    "speed": 42.3,
    "energy_consumption": 156.7
  }
}
```

## Sorun Giderme

### Sender çalışmıyor
- STM32 bağlantılarını kontrol edin
- LoRa modülü 3.3V ile beslendiğinden emin olun
- Anteni takın

### Receiver mesaj almıyor
- Her iki tarafta da aynı frekans (868MHz) kullanıldığından emin olun
- Sync word (0xEC) aynı olmalı
- Antenler takılı olmalı
- JSON parse hatası: ArduinoJson kütüphanesi kurulu olmalı
- Mesafe fazla uzak olabilir (test için 100m içinde deneyin)
- RSSI değeri -100dBm'den kötüyse sinyal çok zayıf

## Lisans

Bu proje Efficiency Challenge yarışması kapsamında eğitim amaçlı hazırlanmıştır.