## ⚠️ KRITISCH: PSRAM MUSS AKTIVIERT SEIN!

**Bevor Sie beginnen**: Das LCD-Panel **wird nicht funktionieren** ohne PSRAM-Aktivierung in der Arduino IDE!

### Arduino IDE Einstellung (ZWINGEND ERFORDERLICH)
```
Tools → Board: "ESP32S3 Dev Module"
Tools → PSRAM: "OPI PSRAM"  ← ⚠️ MUSS AKTIVIERT SEIN!
```

**Ohne diese Einstellung erhalten Sie folgenden Fehler:**
```
E (1162) lcd_panel.rgb: lcd_rgb_panel_alloc_frame_buffers(165): no mem for frame buffer
E (1163) lcd_panel.rgb: esp_lcd_new_rgb_panel(353): alloc frame buffers failed
[E][Panel] Create refresh panel failed [ESP_ERR_NO_MEM]
```

➡️ **Lösung**: Siehe [Setup-Anleitung](setup.md) und [Troubleshooting Guide](TROUBLESHOOTING.md#-lcd-panel-initialisierungsfehler-esp_err_no_mem)

---

## Technische Spezifikationen (Waveshare ESP32-S3-Touch-LCD-4.3B)

### Prozessor & Speicher
- **SoC**: Espressif ESP32-S3R8
  - Dual-Core Xtensa LX7, bis zu 240 MHz
  - Vector Instructions für AI-Beschleunigung
  - 384KB ROM, 512KB RAM
- **Flash**: 16MB
- **PSRAM**: 8MB (⚠️ MUSS in Arduino IDE aktiviert sein!)

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

## 🚀 Schnellstart

1. ✅ **Arduino IDE öffnen**
2. ✅ **Board wählen**: `Tools → Board → ESP32S3 Dev Module`
3. ⚠️ **PSRAM AKTIVIEREN**: `Tools → PSRAM → OPI PSRAM` ← **KRITISCH!**
4. ✅ **Partition Scheme**: `Tools → Partition Scheme → Huge APP (3MB No OTA/1MB SPIFFS)`
5. ✅ **Bibliotheken installieren**: ESP32_Display_Panel, LVGL 8.3.x
6. ✅ **Code hochladen und testen**

📖 **Vollständige Anleitung**: [setup.md](setup.md)  
🔧 **Probleme?**: [TROUBLESHOOTING.md](TROUBLESHOOTING.md)