# ESP32-S3-Touch-LCD-4.3B LVGL Beispiele

Dieses Repository zeigt, wie man LVGL (v8.3.x) mit dem Waveshare ESP32-S3-Touch-LCD-4.3B Board und RGB LCD Displays verwendet.

## Hardware

- **Board**: Waveshare ESP32-S3-Touch-LCD-4.3B
- **Display**: 4.3" RGB LCD, 800×480 Pixel
- **Touch**: GT911 Capacitive Touch (I2C)
- **MCU**: ESP32-S3-WROOM-1-N16R8

## Schnellstart

### Erforderliche Bibliotheken

Installiere die folgenden Bibliotheken über den Arduino Library Manager:

1. **ESP32_Display_Panel** (neueste Version)
2. **LVGL** (v8.3.x) - **Wichtig**: Verwende Version 8.3.x, NICHT 9.x

### Arduino IDE Konfiguration

**Board-Einstellungen:**
- Board: "ESP32S3 Dev Module" oder "Waveshare ESP32-S3-Touch-LCD-4.3B"
- **PSRAM**: `Enabled` (KRITISCH!)
- **Partition Scheme**: "Huge APP (3MB No OTA/1MB SPIFFS)"
- **Upload Speed**: 921600
- **USB CDC On Boot**: Disabled

Siehe [setup.md](setup.md) für detaillierte Arduino IDE Einstellungen.

## LVGL Konfiguration für Arduino

### Methode 1: `lv_conf.h` im Arduino Libraries Ordner platzieren (Empfohlen)

Dies ist die zuverlässigste Methode für die Arduino IDE:

1. Kopiere die mitgelieferte `lv_conf.h` in deinen Arduino Libraries Ordner:
   - **Windows**: `C:\Users\<DeinBenutzername>\Documents\Arduino\libraries\lv_conf.h`
   - **macOS**: `~/Documents/Arduino/libraries/lv_conf.h`
   - **Linux**: `~/Arduino/libraries/lv_conf.h`

2. Die Datei sollte **außerhalb** des `lvgl` Ordners liegen, auf derselben Ebene:
   ```
   Arduino/
   └── libraries/
       ├── lv_conf.h          ← Hier platzieren
       ├── lvgl/
       │   └── ...
       └── ESP32_Display_Panel/
           └── ...
   ```

3. Stelle sicher, dass `lv_conf.h` mit `#if 1` beginnt (nicht `#if 0`)

### Methode 2: `lv_conf.h` im Sketch-Ordner platzieren

Alternativ kannst du `lv_conf.h` direkt in deinen Sketch-Ordner legen:

```
DeinSketch/
├── DeinSketch.ino
├── lv_conf.h              ← Hier platzieren
├── lvgl_v8_port.h
└── lvgl_v8_port.cpp
```

### Wichtige LVGL-Einstellungen

Stelle sicher, dass diese Schriftarten in deiner `lv_conf.h` aktiviert sind:

```c
#define LV_FONT_MONTSERRAT_14  1
#define LV_FONT_MONTSERRAT_16  1
#define LV_FONT_MONTSERRAT_18  1
#define LV_FONT_MONTSERRAT_20  1
#define LV_FONT_MONTSERRAT_24  1
#define LV_FONT_MONTSERRAT_30  1
```

Für Demos aktiviere:
```c
#define LV_USE_DEMO_WIDGETS    1
```

## Verwendung der aktuellen API

Dieses Repository verwendet die **aktuelle, nicht-veraltete API**. Verwende immer:

```cpp
#include <esp_display_panel.hpp>  // NICHT <ESP_Panel_Library.h>
#include <lvgl.h>

using namespace esp_panel::drivers;
using namespace esp_panel::board;

void setup() {
    // Verwende Board, nicht ESP_Panel
    Board *board = new Board();
    board->init();
    board->begin();
    
    // Verwende getLCD(), nicht getLcd()
    lvgl_port_init(board->getLCD(), board->getTouch());
}
```

### Migration von veralteter API

Falls du Warnungen über veraltete APIs siehst, aktualisiere deinen Code:

| Veraltet | Verwende stattdessen |
|------------|-------------|
| `#include <ESP_Panel_Library.h>` | `#include <esp_display_panel.hpp>` |
| `ESP_Panel` | `esp_panel::board::Board` |
| `panel->getLcd()` | `panel->getLCD()` |

## Beispiele

### 1. Basis LVGL Porting Beispiel

Siehe [Waveshare_43B_09_lvgl_Porting.ino](Waveshare_43B_09_lvgl_Porting.ino) - Ein einfaches Beispiel, das das LVGL-Setup mit Demo-Widgets zeigt.

### 2. Erweitertes BMS Monitor Beispiel

Siehe [Waveshare 4.3B/ESP32_CAN_43B/](Waveshare%204.3B/ESP32_CAN_43B/) - Ein vollständiges BMS-Überwachungssystem mit:
- Multi-Protokoll CAN BMS-Unterstützung (Pylontech, JK BMS, DALY)
- Auto-Erkennung
- Vollständige LVGL Touch-Benutzeroberfläche
- Mehrere Konfigurationsbildschirme

## Konfigurationsdateien

### `esp_panel_board_custom_conf.h`

Board-spezifische Konfiguration einschließlich:
- Display-Auflösung (800×480 für 4.3B)
- Touch-Controller-Einstellungen
- RGB-Timing-Parameter
- IO-Expander-Konfiguration

Wichtige Einstellungen:
```c
#define ESP_PANEL_USE_1024_600_LCD  (0)  // 0 für 800x480
#define ESP_OPEN_TOUCH              (1)  // Touch aktivieren
```

### `lvgl_v8_port.h`

LVGL-Porting-Konfiguration einschließlich:
- Buffer-Einstellungen
- Task-Stack-Größe und Priorität
- Anti-Tearing-Modus (empfohlen: Modus 3)

Wichtige Einstellungen:
```c
#define LVGL_PORT_TASK_STACK_SIZE      (10 * 1024)  // 10KB
#define LVGL_PORT_AVOID_TEARING_MODE   (3)          // Direct-Modus
```

## Fehlerbehebung

### Fehler: `lv_conf.h: No such file or directory`

**Lösung**: Platziere `lv_conf.h` in deinem Arduino Libraries Ordner (siehe Methode 1 oben) oder in deinem Sketch-Ordner (Methode 2).

### Fehler: `lv_font_montserrat_XX not declared`

**Lösung**: Aktiviere die benötigten Schriftarten in `lv_conf.h`:
```c
#define LV_FONT_MONTSERRAT_XX  1  // Ersetze XX mit der Schriftgröße
```

### Warnung: Veraltete API-Verwendung

**Lösung**: Aktualisiere deine Includes und API-Aufrufe wie im Abschnitt "Verwendung der aktuellen API" oben gezeigt.

### Watchdog Reset / Absturz beim Starten

**Lösungen**:
1. Stelle sicher, dass PSRAM in der Arduino IDE aktiviert ist
2. Setze `LVGL_PORT_TASK_STACK_SIZE` auf mindestens `(10 * 1024)` in `lvgl_v8_port.h`
3. Überprüfe, ob `ESP_PANEL_USE_1024_600_LCD` für das 4.3" Display auf `(0)` gesetzt ist
4. Verwende das "Huge APP" Partition Scheme

### Display zeigt nichts an

**Lösungen**:
1. Überprüfe die Backlight-Einstellungen in `esp_panel_board_custom_conf.h`
2. Stelle sicher, dass `ESP_PANEL_BOARD_USE_BACKLIGHT` auf `(1)` gesetzt ist
3. Versuche, das USB-Kabel neu anzuschließen

## Zusätzliche Ressourcen

- [Detaillierte Setup-Anleitung](setup.md) - Vollständige Konfigurationsanleitung für Arduino IDE
- [Waveshare Wiki](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4.3B)
- [ESP32_Display_Panel GitHub](https://github.com/esp-arduino-libs/ESP32_Display_Panel)
- [LVGL Dokumentation](https://docs.lvgl.io/8.3/)

## Lizenz

Dieses Projekt verwendet:
- Espressif ESP32_Display_Panel Library
- LVGL (Light and Versatile Graphics Library)
- Waveshare Hardware-Dokumentation

Siehe [LICENSE](LICENSE) für Details.
