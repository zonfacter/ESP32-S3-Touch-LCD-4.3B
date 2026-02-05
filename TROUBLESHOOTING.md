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

## 🔴 Hintergrundbeleuchtung reagiert nicht / Dark Theme Crash

### Problem
```
[UI] Brightness set to 80%
E (1586) lcd_panel.rgb: esp_lcd_rgb_panel_get_frame_buffer(450): invalid frame buffer number
[E][Panel][esp_panel_lcd.cpp:0991](getFrameBufferByIndex): Get RGB buffer failed [ESP_ERR_INVALID_ARG]
Guru Meditation Error: Core 1 panic'ed (LoadProhibited). Exception was unhandled.
```

**Symptome:**
- Helligkeits-Slider lässt sich bewegen, aber die Helligkeit ändert sich nicht
- Umschalten auf Dark Theme führt zum Crash und Reboot

### Ursache

**Problem 1 - Hintergrundbeleuchtung:**
Das Board verwendet einen CH422G IO-Expander für die Backlight-Steuerung auf Pin 2. Dieser unterstützt **nur ON/OFF**, keine PWM-Helligkeitsregelung.

**Problem 2 - Dark Theme Crash:**
Die Frame-Buffer-Anzahl muss vor dem Zugriff auf die Buffers konfiguriert werden, wenn Avoid-Tearing-Mode aktiviert ist.

### ✅ Lösung

#### 1. Backlight-Konfiguration korrigieren

In `esp_panel_board_custom_conf.h`:

```cpp
// KORREKT: Nur ON/OFF möglich
#define ESP_PANEL_BOARD_BACKLIGHT_TYPE  (ESP_PANEL_BACKLIGHT_TYPE_SWITCH_EXPANDER)

// FALSCH: PWM nicht unterstützt
// #define ESP_PANEL_BOARD_BACKLIGHT_TYPE  (ESP_PANEL_BACKLIGHT_TYPE_PWM_LEDC)
```

#### 2. UI-Code anpassen

In `ui_manager.h`, Funktion `setBrightness()`:

```cpp
void UIManager::setBrightness(int level) {
    m_brightnessLevel = level;
    
    if (m_panel) {
        auto backlight = m_panel->getBacklight();
        if (backlight) {
            // Hardware unterstützt nur ON/OFF
            if (level > 0) {
                backlight->on();  // Backlight AN
                Serial.printf("[UI] Backlight turned ON (level: %d%%)\n", level);
            } else {
                backlight->off(); // Backlight AUS
                Serial.println("[UI] Backlight turned OFF");
            }
        }
    }
}
```

#### 3. Frame Buffer Konfiguration hinzufügen

Beim Initialisieren des Panels (in `setup()` oder `initDisplay()`):

```cpp
board->init();

#if LVGL_PORT_AVOID_TEARING_MODE
auto lcd = board->getLCD();
// Frame Buffer Anzahl konfigurieren BEVOR getFrameBufferByIndex() aufgerufen wird
lcd->configFrameBufferNumber(LVGL_PORT_DISP_BUFFER_NUM);
Serial.printf("[Display] Configured %d frame buffers\n", LVGL_PORT_DISP_BUFFER_NUM);

#if ESP_PANEL_DRIVERS_BUS_ENABLE_RGB && CONFIG_IDF_TARGET_ESP32S3
auto lcd_bus = lcd->getBus();
if (lcd_bus->getBasicAttributes().type == ESP_PANEL_BUS_TYPE_RGB) {
    static_cast<BusRGB *>(lcd_bus)->configRGB_BounceBufferSize(lcd->getFrameWidth() * 10);
    Serial.println("[Display] Configured RGB bounce buffer");
}
#endif
#endif

board->begin();
```

#### 4. Benutzer informieren

Füge einen Hinweis in der UI hinzu:

```cpp
lv_obj_t* noteLabel = lv_label_create(cont);
lv_label_set_text(noteLabel, "(Nur AN/AUS - Hardware-Limitation)");
lv_obj_set_style_text_color(noteLabel, lv_palette_main(LV_PALETTE_ORANGE), 0);
```

### Hardware-Limitation verstehen

Das Waveshare ESP32-S3-Touch-LCD-4.3B Board verwendet:
- **CH422G IO-Expander** für Backlight-Steuerung
- **Pin 2 des Expanders** für Backlight
- CH422G unterstützt **keine PWM-Ausgabe**
- Nur **digitale ON/OFF** Schaltung möglich

Wenn PWM-Helligkeitsregelung gewünscht wird, muss die Hardware modifiziert werden, um das Backlight an einen PWM-fähigen GPIO anzuschließen.

---

## 📚 Weitere Ressourcen

- **Hardware Details**: [HARDWARE.md](HARDWARE.md)
- **Waveshare Wiki**: https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4.3B
- **GitHub Issues**: [Projekt Issues](https://github.com/zonfacter/ESP32-S3-Touch-LCD-4.3B/issues)

---

**Dokumentversion**: 1.0  
**Zuletzt aktualisiert**: 2026-02-05