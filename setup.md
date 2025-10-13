# Waveshare ESP32-S3-Touch-LCD-4.3B - LVGL Setup Guide

Vollständige Konfigurationsanleitung für LVGL-Projekte mit dem Waveshare ESP32-S3-Touch-LCD-4.3B Display.

## 📋 Hardware-Spezifikationen

| Komponente | Spezifikation |
|------------|---------------|
| **Board** | Waveshare ESP32-S3-Touch-LCD-4.3B |
| **Display** | 4.3" RGB LCD, 800×480 Pixel, 65K Farben |
| **Touch** | GT911 Capacitive Touch (I2C) |
| **MCU** | ESP32-S3-WROOM-1-N16R8 |
| **Flash** | 16MB |
| **PSRAM** | 8MB (OPI PSRAM) |
| **LCD Controller** | ST7262 |
| **IO Expander** | CH422G |

---

## 🔧 Arduino IDE Einstellungen

### Board-Auswahl
```
Board: "Waveshare ESP32-S3-Touch-LCD-4.3B"
oder
Board: "ESP32S3 Dev Module"
Port: "COM6" (je nach System)
```

### Kritische Einstellungen

| Einstellung | Wert | Beschreibung |
|-------------|------|--------------|
| **USB CDC On Boot** | `Disabled` | Standard USB-Kommunikation |
| **CPU Frequency** | `240MHz (WiFi)` | Maximale Performance |
| **Core Debug Level** | `Error` | Für Produktion |
| **USB DFU On Boot** | `Disabled` | Standard Boot-Verhalten |
| **Erase All Flash Before Sketch Upload** | `Disabled` | Nur bei Problemen auf `Enabled` |
| **Events Run On** | `Core 1` | LVGL läuft auf Core 1 |
| **Flash Mode** | `QIO 120MHz` | Schneller Flash-Zugriff |
| **Arduino Runs On** | `Core 1` | Standard Arduino Core |
| **USB Firmware MSC On Boot** | `Disabled` | Kein MSC-Modus |
| **Partition Scheme** | `Huge APP (3MB No OTA/1MB SPIFFS)` | Mehr App-Speicher |
| **PSRAM** | `Enabled` | **WICHTIG!** OPI PSRAM aktivieren |
| **Upload Mode** | `UART0 / Hardware CDC` | Standard Upload |
| **Upload Speed** | `921600` | Schneller Upload |
| **USB Mode** | `Hardware CDC and JTAG` | Debug-Unterstützung |

---

## 📚 Erforderliche Bibliotheken

### Installation über Arduino Library Manager

1. **ESP32_Display_Panel** (neueste Version)
   ```
   Sketch → Include Library → Manage Libraries
   Suche: "ESP32_Display_Panel"
   ```

2. **LVGL** (Version 8.3.x)
   ```
   Suche: "lvgl"
   Version: 8.3.11 (oder neuer 8.3.x)
   WICHTIG: NICHT Version 9.x!
   ```

3. **Weitere Abhängigkeiten** (werden automatisch installiert)
   - ESP32_IO_Expander
   - ESP32_Expander_ADDA

---

## ⚙️ Konfigurationsdateien

### 1. `esp_panel_board_custom_conf.h`

**Kritische Einstellungen:**

```cpp
// Display-Auflösung: 800x480 für 4.3B!
#define ESP_PANEL_USE_1024_600_LCD           (0)     // 0: 800x480, 1: 1024x600

// Touch aktivieren
#define ESP_OPEN_TOUCH                       (1)     // 1: Touch an, 0: Touch aus

// Board-Name
#define ESP_PANEL_BOARD_NAME                 "Waveshare:ESP32-S3-Touch-LCD-5"

// Display-Auflösung (automatisch bei 1024_600=0)
#define ESP_PANEL_BOARD_WIDTH                (800)
#define ESP_PANEL_BOARD_HEIGHT               (480)

// RGB Timing für 800x480
#define ESP_PANEL_BOARD_LCD_RGB_CLK_HZ       (16 * 1000 * 1000)
#define ESP_PANEL_BOARD_LCD_RGB_HPW          (4)
#define ESP_PANEL_BOARD_LCD_RGB_HBP          (8)
#define ESP_PANEL_BOARD_LCD_RGB_HFP          (8)
#define ESP_PANEL_BOARD_LCD_RGB_VPW          (4)
#define ESP_PANEL_BOARD_LCD_RGB_VBP          (16)
#define ESP_PANEL_BOARD_LCD_RGB_VFP          (16)

// Bounce Buffer (wichtig für ESP32-S3 mit RGB LCD)
#define ESP_PANEL_BOARD_LCD_RGB_BOUNCE_BUF_SIZE (ESP_PANEL_BOARD_WIDTH * 10)
```

**Touch-Konfiguration:**

```cpp
#define ESP_PANEL_BOARD_USE_TOUCH            (1)
#define ESP_PANEL_BOARD_TOUCH_CONTROLLER     GT911
#define ESP_PANEL_BOARD_TOUCH_BUS_TYPE       (ESP_PANEL_BUS_TYPE_I2C)
#define ESP_PANEL_BOARD_TOUCH_I2C_IO_SCL     (9)
#define ESP_PANEL_BOARD_TOUCH_I2C_IO_SDA     (8)
#define ESP_PANEL_BOARD_TOUCH_INT_IO         (4)
```

**IO Expander:**

```cpp
#define ESP_PANEL_BOARD_USE_EXPANDER         (1)
#define ESP_PANEL_BOARD_EXPANDER_CHIP        CH422G
#define ESP_PANEL_BOARD_EXPANDER_I2C_IO_SCL  (9)
#define ESP_PANEL_BOARD_EXPANDER_I2C_IO_SDA  (8)
#define ESP_PANEL_BOARD_EXPANDER_I2C_ADDRESS (0x20)
```

### 2. `lvgl_v8_port.h`

**Wichtige Anpassungen:**

```cpp
// LVGL Task Stack-Größe erhöhen!
#define LVGL_PORT_TASK_STACK_SIZE            (10 * 1024)  // 10KB (war 6KB)

// Task-Priorität
#define LVGL_PORT_TASK_PRIORITY              (2)

// Task Core
#define LVGL_PORT_TASK_CORE                  (1)

// Anti-Tearing Modus (empfohlen: Mode 3)
#define LVGL_PORT_AVOID_TEARING_MODE         (3)
// Mode 1: Double-buffer + full-refresh
// Mode 2: Triple-buffer + full-refresh  
// Mode 3: Double-buffer + direct-mode (empfohlen!)

// Rotation (0/90/180/270)
#define LVGL_PORT_ROTATION_DEGREE            (0)
```

---

## 💻 Basis-Sketch Template

### Minimales Beispiel

```cpp
#include <Arduino.h>
#include <esp_display_panel.hpp>
#include <lvgl.h>
#include "lvgl_v8_port.h"

using namespace esp_panel::drivers;
using namespace esp_panel::board;

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("=== ESP32-S3-Touch-LCD-4.3B LVGL Init ===");

    // Board initialisieren
    Serial.println("Initializing board...");
    Board *board = new Board();
    board->init();

    #if LVGL_PORT_AVOID_TEARING_MODE
    auto lcd = board->getLCD();
    lcd->configFrameBufferNumber(LVGL_PORT_DISP_BUFFER_NUM);
    
    #if ESP_PANEL_DRIVERS_BUS_ENABLE_RGB && CONFIG_IDF_TARGET_ESP32S3
    auto lcd_bus = lcd->getBus();
    if (lcd_bus->getBasicAttributes().type == ESP_PANEL_BUS_TYPE_RGB) {
        static_cast<BusRGB *>(lcd_bus)->configRGB_BounceBufferSize(
            lcd->getFrameWidth() * 10
        );
    }
    #endif
    #endif

    if(!board->begin()) {
        Serial.println("ERROR: Board initialization failed!");
        while(1) delay(1000);
    }
    Serial.println("Board initialized!");

    // LVGL initialisieren
    Serial.println("Initializing LVGL...");
    lvgl_port_init(board->getLCD(), board->getTouch());

    // UI erstellen
    Serial.println("Creating UI...");
    lvgl_port_lock(-1);

    // Einfaches Label
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Waveshare ESP32-S3\n4.3B Display OK!");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
    lv_obj_center(label);

    lvgl_port_unlock();
    
    Serial.println("Setup complete!");
}

void loop()
{
    delay(1000);
}
```

### Mit LVGL Demo Widgets

```cpp
#include <demos/lv_demos.h>

void setup()
{
    // ... Board-Initialisierung wie oben ...

    // UI erstellen
    lvgl_port_lock(-1);
    
    // LVGL Demo starten
    lv_demo_widgets();
    
    lvgl_port_unlock();
}
```

---

## 🐛 Troubleshooting

### Problem: Crash beim Starten / Watchdog Reset

**Symptome:**
```
Guru Meditation Error: Core 1 panic'ed (LoadProhibited)
rst:0xc (RTC_SW_CPU_RST)
```

**Lösungen:**
1. ✅ `ESP_PANEL_USE_1024_600_LCD` auf `(0)` setzen
2. ✅ `LVGL_PORT_TASK_STACK_SIZE` auf mindestens `(10 * 1024)` erhöhen
3. ✅ PSRAM in Arduino IDE aktivieren
4. ✅ Partition Scheme auf "Huge APP" setzen

### Problem: Display bleibt schwarz

**Lösungen:**
1. Überprüfe Backlight-Einstellung in `esp_panel_board_custom_conf.h`
2. Stelle sicher, dass `ESP_PANEL_BOARD_USE_BACKLIGHT` auf `(1)` steht
3. USB-Kabel neu einstecken

### Problem: Touch funktioniert nicht

**Lösungen:**
1. Setze `ESP_OPEN_TOUCH` auf `(1)`
2. Überprüfe I2C-Pins (SCL=9, SDA=8)
3. Teste mit Touch-Test-Sketch

### Problem: Bildschirm driftet (Screen Drift)

**Lösung:**
```cpp
// Bounce Buffer erhöhen
#define ESP_PANEL_BOARD_LCD_RGB_BOUNCE_BUF_SIZE (800 * 12)
```

### Problem: Compilation Error "LVGL version mismatch"

**Lösung:**
- Deinstalliere LVGL 9.x
- Installiere LVGL 8.3.x (z.B. 8.3.11)
- Arduino IDE neu starten

---

## 📁 Projekt-Struktur

```
Waveshare_43B_LVGL_Project/
│
├── Waveshare_43B_LVGL_Project.ino    # Haupt-Sketch
├── esp_panel_board_custom_conf.h      # Display-Konfiguration
├── lvgl_v8_port.h                     # LVGL Port Header
├── lvgl_v8_port.cpp                   # LVGL Port Implementation
└── README.md                          # Diese Datei
```

---

## 🚀 Schnellstart-Checkliste

- [ ] Arduino IDE mit ESP32-Board-Support installiert
- [ ] Waveshare ESP32-S3-Touch-LCD-4.3B Board ausgewählt
- [ ] **PSRAM aktiviert** (kritisch!)
- [ ] Partition Scheme: "Huge APP (3MB No OTA/1MB SPIFFS)"
- [ ] Bibliotheken installiert:
  - [ ] ESP32_Display_Panel
  - [ ] LVGL 8.3.x
- [ ] `ESP_PANEL_USE_1024_600_LCD` = `(0)` gesetzt
- [ ] `ESP_OPEN_TOUCH` = `(1)` gesetzt
- [ ] `LVGL_PORT_TASK_STACK_SIZE` = `(10 * 1024)` gesetzt
- [ ] Upload Speed: 921600
- [ ] Code kompiliert ohne Fehler
- [ ] Display zeigt UI an

---

## 📊 Performance-Tipps

### FPS optimieren

1. **Anti-Tearing Mode 3 verwenden** (Direct Mode)
   ```cpp
   #define LVGL_PORT_AVOID_TEARING_MODE (3)
   ```

2. **Rotation deaktivieren** (0°)
   ```cpp
   #define LVGL_PORT_ROTATION_DEGREE (0)
   ```

3. **PSRAM für große Buffer nutzen**
   ```cpp
   #define LVGL_PORT_BUFFER_MALLOC_CAPS (MALLOC_CAP_SPIRAM)
   ```

### Speicher sparen

1. **Demo-Widgets deaktivieren** wenn nicht benötigt
2. **Nur benötigte Fonts aktivieren** in `lv_conf.h`
3. **Animationen reduzieren**

---

## 📖 Weiterführende Links

- [Waveshare Wiki](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4.3B)
- [ESP32_Display_Panel GitHub](https://github.com/esp-arduino-libs/ESP32_Display_Panel)
- [LVGL Dokumentation](https://docs.lvgl.io/8.3/)
- [ESP32-S3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)

---

## ✅ Getestete Konfiguration

**Hardware:**
- Waveshare ESP32-S3-Touch-LCD-4.3B (800x480)

**Software:**
- Arduino IDE 2.3.x
- ESP32 Board Package 3.0.x
- ESP32_Display_Panel 1.0.x
- LVGL 8.3.11

**Ergebnis:**
- ✅ Display funktioniert (800x480)
- ✅ Touch funktioniert
- ✅ LVGL Demo Widgets läuft stabil
- ✅ Kein Watchdog Reset
- ✅ ~26 FPS bei Benchmark

---

## 📝 Lizenz & Credits

Diese Konfiguration basiert auf:
- Espressif ESP32_Display_Panel Library
- LVGL (Light and Versatile Graphics Library)
- Waveshare Hardware-Dokumentation

**Erstellt:** 2025
**Letzte Aktualisierung:** Oktober 2025

---

## 💡 Support

Bei Problemen:
1. Überprüfe diese Checkliste vollständig
2. Aktiviere Debug-Level auf "Debug" für mehr Logs
3. Erstelle ein Issue mit vollständigen Serial-Logs
4. GitHub Issues: [ESP32_Display_Panel](https://github.com/esp-arduino-libs/ESP32_Display_Panel/issues)

**Viel Erfolg mit deinem LVGL-Projekt! 🎉**
