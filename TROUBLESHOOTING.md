# Troubleshooting Guide - ESP32-S3-Touch-LCD-4.3B

Lösungen für häufige Probleme und Fehler beim Entwickeln mit dem Waveshare ESP32-S3-Touch-LCD-4.3B Board.

---

## 🔴 LCD Panel Initialisierungsfehler: ESP_ERR_NO_MEM

### Problem
```
E (1162) lcd_panel.rgb: lcd_rgb_panel_alloc_frame_buffers(165): no mem for frame buffer
E (1163) lcd_panel.rgb: esp_lcd_new_rgb_panel(353): alloc frame buffers failed
[E][Panel][esp_panel_lcd_st7262.cpp:0079](init): Create refresh panel failed [ESP_ERR_NO_MEM]
[E][Panel][esp_panel_lcd.cpp:0404](begin): Init failed
[E][Panel][esp_panel_board.cpp:0315](begin): LCD device begin failed
```

**Siehe auch**: [Issue #5](https://github.com/zonfacter/ESP32-S3-Touch-LCD-4.3B/issues/5)

### Ursache
Der ESP32-S3 hat nicht genügend Arbeitsspeicher für die LCD-Frame-Buffer. Das 800×480 RGB Display benötigt große Speicherbereiche, die nur im externen PSRAM verfügbar sind.

### ✅ Lösung

#### 1. PSRAM in Arduino IDE aktivieren

**Kritische Board-Einstellungen:**

```
Tools → Board: "ESP32S3 Dev Module"
Tools → PSRAM: "OPI PSRAM"  ← MUSS aktiviert sein!
Tools → Partition Scheme: "Huge APP (3MB No OTA/1MB SPIFFS)"
Tools → Flash Mode: "QIO 80MHz"
Tools → Flash Size: "16MB (128Mb)"
Tools → Upload Speed: "921600"
Tools → USB CDC On Boot: "Disabled"
```

#### 2. PlatformIO Konfiguration

```ini
[env:esp32-s3-devkitc-1]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino

board_build.arduino.memory_type = opi_opi  ; PSRAM aktivieren!
board_build.partitions = huge_app.csv
board_build.f_flash = 80000000L
board_build.flash_mode = qio

build_flags = 
    -DBOARD_HAS_PSRAM
    -DARDUINO_USB_CDC_ON_BOOT=0
```

#### 3. Überprüfung im Code

```cpp
void setup() {
    Serial.begin(115200);
    
    // PSRAM-Check
    if (psramFound()) {
        Serial.printf("PSRAM found: %d bytes\n", ESP.getPsramSize());
        Serial.printf("PSRAM free: %d bytes\n", ESP.getFreePsram());
    } else {
        Serial.println("ERROR: PSRAM not found! Enable OPI PSRAM in Tools menu!");
        while(1) delay(1000);
    }
}
```

---

## ⚠️ Compiler-Warnungen

**Siehe auch**: [Issue #4](https://github.com/zonfacter/ESP32-S3-Touch-LCD-4.3B/issues/4)

### 1. Member-Initialisierungsreihenfolge [-Wreorder]

#### Problem
```
warning: 'UIManager::m_wlanIPLabel' will be initialized after [-Wreorder]
warning:   'esp_panel::board::Board* UIManager::m_panel' [-Wreorder]
```

#### ✅ Lösung
```cpp
// RICHTIG:
class UIManager {
private:
    esp_panel::board::Board* m_panel;
    lv_obj_t* m_wlanIPLabel;
    
public:
    UIManager() 
        : m_panel(nullptr),        // Reihenfolge wie in Klasse
          m_wlanIPLabel(nullptr)
    {}
};
```

---

## 📚 Weitere Ressourcen

- **Hardware Details**: [HARDWARE.md](HARDWARE.md)
- **Waveshare Wiki**: https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4.3B
- **GitHub Issues**: [Projekt Issues](https://github.com/zonfacter/ESP32-S3-Touch-LCD-4.3B/issues)

---

**Dokumentversion**: 1.0  
**Zuletzt aktualisiert**: 2026-02-05