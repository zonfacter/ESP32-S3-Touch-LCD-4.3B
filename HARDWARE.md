# Hardware-Dokumentation - ESP32-S3-Touch-LCD-4.3B

Vollständige technische Spezifikationen und Hardware-Details für das Waveshare ESP32-S3-Touch-LCD-4.3B Development Board.

---

## 📋 Übersicht

Das **Waveshare ESP32-S3-Touch-LCD-4.3B** ist ein kompaktes, vielseitiges Development Board mit integriertem 4.3" Touchscreen-Display, basierend auf dem leistungsstarken ESP32-S3 SoC von Espressif.

### Hauptmerkmale
- ✅ ESP32-S3 Dual-Core (bis 240 MHz) mit 8MB PSRAM
- ✅ 4.3" Kapazitives Touchscreen (800×480)
- ✅ Wi-Fi 2.4 GHz & Bluetooth 5.0 LE
- ✅ CAN Bus & RS485 Schnittstellen
- ✅ Isolierte Digital I/Os (5-36V)
- ✅ RTC & MicroSD
- ✅ Breiter Spannungsbereich (7-36V DC)

---

## 🧠 Prozessor & Speicher

### ESP32-S3-WROOM-1-N16R8 Modul

| Spezifikation | Details |
|---------------|---------|
| **SoC** | Espressif ESP32-S3R8 |
| **CPU** | Dual-Core Xtensa LX7, bis 240 MHz |
| **ROM** | 384 KB |
| **SRAM** | 512 KB |
| **Flash** | 16 MB (extern) |
| **PSRAM** | 8 MB Octal SPI PSRAM |
| **AI Beschleunigung** | Vektor-Instruktionen für ML |
| **FPU** | Single-precision FPU |

### ⚠️ WICHTIG: PSRAM
Das Board verfügt über **8MB PSRAM**, welches **zwingend aktiviert** werden muss:
- Arduino IDE: `Tools → PSRAM → "OPI PSRAM"`
- Ohne PSRAM-Aktivierung schlägt die LCD-Initialisierung mit `ESP_ERR_NO_MEM` fehl!

---

## 🖥️ Display & Touch

### LCD Display

| Spezifikation | Wert |
|---------------|------|
| **Größe** | 4.3 Zoll (Diagonale) |
| **Auflösung** | 800 × 480 Pixel |
| **Technologie** | TFT RGB LCD |
| **Controller** | ST7262 |
| **Farbtiefe** | 16-bit (65K Farben) |
| **Hintergrundbeleuchtung** | LED, steuerbar |
| **Betrachtungswinkel** | 160° (H) / 140° (V) |
| **Interface** | 16-bit RGB Parallel |

### Touch Controller

| Spezifikation | Wert |
|---------------|------|
| **Controller** | Goodix GT911 |
| **Technologie** | Kapazitiv, Multi-Touch |
| **Touch-Punkte** | 5-Point Touch |
| **Interface** | I2C |
| **I2C Adresse** | 0x5D (7-bit) / 0x14 (alternativer Modus) |
| **Interrupt** | Touch-IRQ unterstützt |

---

## 📡 Konnektivität

### Wi-Fi

| Spezifikation | Details |
|---------------|---------|
| **Standard** | IEEE 802.11 b/g/n |
| **Frequenz** | 2.4 GHz (2400-2483.5 MHz) |
| **Modi** | Station, SoftAP, Station+SoftAP |
| **Bandbreite** | 20 MHz, 40 MHz |
| **Sicherheit** | WPA/WPA2/WPA3, WEP, TKIP, AES |
| **Max. Tx Power** | 20.5 dBm |
| **Reichweite** | ~100m (Freifeld) |

### Bluetooth

| Spezifikation | Details |
|---------------|---------|
| **Version** | Bluetooth 5.0, BLE |
| **Modi** | BLE, Bluetooth Mesh |
| **Datenrate** | Bis zu 2 Mbps |
| **Reichweite** | Long-Range Mode unterstützt |
| **Tx Power** | 20.5 dBm |
| **Profile** | GATT, GAP, SMP |

---

## 🔌 Schnittstellen

### CAN Bus

| Spezifikation | Wert |
|---------------|------|
| **Controller** | TJA1051T/3 |
| **Standard** | CAN 2.0A/B |
| **Geschwindigkeit** | Bis 1 Mbps |
| **Terminator** | 120Ω (onboard, optional) |
| **Schutz** | ESD-geschützt |

**Anschluss**: 3-Pin Terminal Block (CAN_H, CAN_L, GND)

### RS485

| Spezifikation | Wert |
|---------------|------|
| **Transceiver** | SP3485EN |
| **Standard** | TIA/EIA-485-A |
| **Geschwindigkeit** | Bis 10 Mbps |
| **Maximale Buslänge** | 1200m (bei 100 kbps) |
| **Terminator** | 120Ω (extern empfohlen) |

**Anschluss**: 2-Pin Terminal Block (RS485_A, RS485_B)

### I2C

| Spezifikation | Wert |
|---------------|------|
| **Verfügbare Busse** | 2 (I2C0, I2C1) |
| **Geschwindigkeit** | Standard (100 kHz), Fast (400 kHz), Fast+ (1 MHz) |
| **Verwendung** | Touch (GT911), RTC, Expander |

---

## 🔢 Digital I/O

### Isolierte Digital Inputs (DI)

| Spezifikation | Wert |
|---------------|------|
| **Anzahl** | 2 Kanäle |
| **Spannungsbereich** | 5-36V DC |
| **Isolation** | Bidirektionale Optokoppler |
| **Eingangsimpedanz** | ~10kΩ |
| **Erkennung** | High: >3.5V, Low: <1.5V |

**Anschluss**: 4-Pin Terminal Block (DI1+, DI1-, DI2+, DI2-)

### Isolierte Digital Outputs (DO)

| Spezifikation | Wert |
|---------------|------|
| **Anzahl** | 2 Kanäle |
| **Spannungsbereich** | 5-36V DC |
| **Max. Strom** | 450 mA pro Kanal |
| **Isolation** | Optokoppler-isoliert |
| **Schutz** | Überstrom-, Kurzschluss-Schutz |

**Anschluss**: 4-Pin Terminal Block (DO1+, DO1-, DO2+, DO2-)

---

## ⏰ Real-Time Clock (RTC)

| Spezifikation | Details |
|---------------|---------|
| **Chip** | PCF8563 |
| **Interface** | I2C |
| **Backup** | Batterie (CR1220, optional) |
| **Genauigkeit** | ±5 ppm bei 25°C |
| **Alarm** | Programmierbar |

---

## 💾 Speicher & Storage

### MicroSD Card Slot

| Spezifikation | Details |
|---------------|---------|
| **Interface** | SDIO / SPI |
| **Unterstützte Karten** | microSD, microSDHC (bis 32GB) |
| **Dateisysteme** | FAT16, FAT32 |
| **Hot-Plug** | Unterstützt |

---

## ⚡ Stromversorgung

### Eingänge

| Typ | Spezifikation |
|-----|---------------|
| **DC Terminal** | 7-36V DC, 2-Pin Screw Terminal |
| **USB Type-C** | 5V DC, USB 2.0 |
| **Akku** | 3.7V Lithium (JST-Anschluss) |

### Power Management

- **LDO Regler**: Onboard 3.3V und 5V
- **Akkuladung**: Integriertes Lade-IC
- **Power LEDs**: Status-Anzeige (Power, Battery)
- **Stromaufnahme**:
  - Idle: ~100 mA (ohne Display)
  - Aktiv: ~400-600 mA (mit Display)
  - Deep Sleep: <10 mA

### ⚠️ Wichtige Hinweise
- **Überspannungsschutz**: Bis 40V (kurzzeitig)
- **Verpolungsschutz**: Onboard
- **Maximale Ausgangsleistung DO**: 900 mA gesamt

---

## 🌡️ Umgebungsbedingungen

| Parameter | Wert |
|-----------|------|
| **Betriebstemperatur** | 0°C bis +65°C |
| **Lagertemperatur** | -20°C bis +85°C |
| **Luftfeuchtigkeit** | 10% - 90% (nicht kondensierend) |

---

## 📐 Mechanische Spezifikationen

| Parameter | Wert |
|-----------|------|
| **Abmessungen (Board)** | 112.4 mm × 75.1 mm |
| **Display Größe** | 98.7 mm × 59.8 mm (aktive Fläche) |
| **Dicke** | ~8 mm (ohne Stecker) |
| **Gewicht** | ~85g |
| **Befestigungslöcher** | 4x M3 Gewindelöcher |

---

## 🔧 GPIO Pinout

### Wichtige GPIO-Zuordnungen

| Funktion | GPIO | Hinweise |
|----------|------|----------|
| **LCD Backlight** | GPIO38 | PWM-fähig |
| **Touch INT** | GPIO3 | Interrupt vom GT911 |
| **Touch RST** | GPIO48 | Touch Reset |
| **I2C SDA** | GPIO8 | Touch, RTC, Expander |
| **I2C SCL** | GPIO9 | Touch, RTC, Expander |
| **SD Card CS** | GPIO10 | SPI Chip Select |
| **SD Card MOSI** | GPIO11 | SPI MOSI |
| **SD Card MISO** | GPIO13 | SPI MISO |
| **SD Card CLK** | GPIO12 | SPI Clock |
| **CAN TX** | GPIO17 | CAN Bus Transmit |
| **CAN RX** | GPIO18 | CAN Bus Receive |
| **RS485 TX** | GPIO1 | RS485 Transmit |
| **RS485 RX** | GPIO2 | RS485 Receive |
| **DI1** | GPIO4 | Digital Input 1 |
| **DI2** | GPIO5 | Digital Input 2 |
| **DO1** | GPIO6 | Digital Output 1 |
| **DO2** | GPIO7 | Digital Output 2 |

### RGB LCD Pins
| Signal | GPIO(s) |
|--------|---------|
| **Red Data** | GPIO39, 40, 41, 42, 45 (5-bit) |
| **Green Data** | GPIO0, 46, 14, 21, 47, 48 (6-bit) |
| **Blue Data** | GPIO15, 16, 4, 5, 6 (5-bit) |
| **HSYNC** | GPIO3 |
| **VSYNC** | GPIO46 |
| **PCLK** | GPIO9 |
| **DE** | GPIO17 |

---

## 📚 Schaltpläne & Datasheets

### Offizielle Ressourcen
- **Schaltplan**: [PDF auf Waveshare Wiki](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4.3B#Resources)
- **Board-Layout**: Verfügbar auf Anfrage
- **3D-Modell**: STEP-Datei verfügbar

### Datasheets
- [ESP32-S3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)
- [GT911 Touch Controller](https://github.com/goodix/gt9xx)
- [TJA1051T CAN Transceiver](https://www.nxp.com/docs/en/data-sheet/TJA1051.pdf)
- [SP3485 RS485 Transceiver](https://www.maxlinear.com/products/SP3485)

---

## 🛠️ Entwicklungsumgebung

### Unterstützte IDEs
- ✅ Arduino IDE (v1.8.x oder v2.x)
- ✅ PlatformIO (VS Code Extension)
- ✅ ESP-IDF (Espressif Native SDK)

### Beispiel-Projekte
- [Waveshare GitHub](https://github.com/waveshareteam/Waveshare-ESP32-components)
- Arduino LVGL Demos
- CAN Bus Beispiele
- RS485 Kommunikation

---

## 💡 Anwendungsbeispiele

### Typische Use Cases
- 🏠 **Smart Home Control Panel**
- 🏭 **Industrielle HMI**
- 🚗 **Automotive Display (CAN)**
- 📊 **IoT Dashboard**
- 🔧 **Test & Messgeräte**
- 🌐 **MQTT Control Panel**

---

## 📖 Weitere Dokumentation

- **Troubleshooting**: [TROUBLESHOOTING.md](TROUBLESHOOTING.md)
- **Waveshare Wiki**: [Offizielle Dokumentation](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4.3B)
- **ESP32-S3 Technical Reference**: [Espressif Docs](https://www.espressif.com/en/support/documents/technical-documents)

---

**Dokumentversion**: 1.0  
**Zuletzt aktualisiert**: 2026-02-05  
**Board-Revision**: Rev B (überprüfe Aufkleber auf Rückseite)