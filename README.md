## Technische Spezifikationen (Waveshare ESP32-S3-Touch-LCD-4.3B)

### Prozessor & Speicher
- **SoC**: Espressif ESP32-S3R8
  - Dual-Core Xtensa LX7, bis zu 240 MHz
  - Vector Instructions für AI-Beschleunigung
  - 384KB ROM, 512KB RAM
- **Flash**: 16MB
- **PSRAM**: 8MB (MUSS in Arduino IDE aktiviert sein!)

### Display & Touch
- **Display**: 4.3" kapazitives Touchscreen
- **Auflösung**: 800 × 480 Pixel
- **Touch-Controller**: GT911 (I2C)

### Konnektivität
- **Wi-Fi**: 2.4 GHz (802.11 b/g/n)
- **Bluetooth**: 5.0 LE (Long-Range, Mesh-Support, bis zu 2 Mbps)

### Schnittstellen
- **CAN Bus**: TJA1051T/3
- **RS485**: SP3485EN
- **I2C**: Ja
- **Isolierte Digital Inputs**: 2x DI (5–36V, bidirektionale Optokoppler)
- **Isolierte Digital Outputs**: 2x DO (5–36V, 450mA/Kanal)
- **RTC**: Onboard Real Time Clock Chip
- **MicroSD**: Card Slot vorhanden

### Stromversorgung
- **DC-Eingang**: 7–36V via Terminal Block
- **USB Type-C**: Ja
- **Akku-Support**: 3.7V Lithium-Batterie (wiederaufladbar)
- **LED-Indikatoren**: Power und Battery

### Physikalische Eigenschaften
- **Betriebstemperatur**: 0°C bis 65°C
- **Abmessungen**: 112.4 mm × 75.1 mm

### Wichtige Links & Ressourcen
- **Offizielle Dokumentation**: [Waveshare Wiki](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4.3B)
  - Pinouts, Schaltpläne, elektrische Spezifikationen
  - Arduino & ESP-IDF Setup-Anleitungen
  - Demo-Code & Beispiele (LVGL, SD-Test, RS485, RTC)
  - Datasheets aller Hauptkomponenten
  - Troubleshooting & Support
- **GitHub Repository**: [Waveshare ESP32 Components](https://github.com/waveshareteam/Waveshare-ESP32-components)
  - Board Support Packages
  - Treiber & Demos für LVGL und Peripherie

### Typische Anwendungen
- IoT Human-Machine Interfaces (HMI)
- Smart Home Automation Dashboards
- Standalone Touch GUI Geräte
- Industrielle Steuerungen mit CAN/RS485

---

**Hinweis**: Bei Speicherproblemen (ESP_ERR_NO_MEM) ist PSRAM-Aktivierung in den Arduino IDE Board-Einstellungen zwingend erforderlich!