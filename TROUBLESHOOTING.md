# Troubleshooting Guide - ESP32-S3-Touch-LCD-4.3B

Dieser Leitfaden hilft bei der Lösung häufiger Probleme mit dem Waveshare ESP32-S3-Touch-LCD-4.3B Board.

---

## 🔴 Häufige Fehler und Lösungen

### 1. ESP_ERR_NO_MEM - LCD Panel Initialisierung schlägt fehl

**Fehlermeldung:**
```
E (1162) lcd_panel.rgb: lcd_rgb_panel_alloc_frame_buffers(165): no mem for frame buffer
E (1163) lcd_panel.rgb: esp_lcd_new_rgb_panel(353): alloc frame buffers failed
[E][Panel][esp_panel_lcd_st7262.cpp:0079](init): Create refresh panel failed [ESP_ERR_NO_MEM]
```

**Ursache:** PSRAM ist nicht aktiviert oder nicht verfügbar.

**Lösung:**
1. **Arduino IDE Board-Einstellungen öffnen**
2. **PSRAM auf "Enabled" setzen** (KRITISCH!)
3. Partition Scheme: "Huge APP (3MB No OTA/1MB SPIFFS)" wählen
4. Projekt neu kompilieren und hochladen

**Wichtig:** Das ESP32-S3-Touch-LCD-4.3B Board benötigt die 8MB PSRAM für die Frame Buffer des 800×480 Displays!

---

### 2. Compiler-Warnungen: -Wreorder

**Warnung:**
```
warning: 'UIManager::m_wlanIPLabel' will be initialized after [-Wreorder]
warning:   'esp_panel::board::Board* UIManager::m_panel' [-Wreorder]
```

**Ursache:** Mitgliedsvariablen werden in der falschen Reihenfolge im Konstruktor initialisiert.

**Lösung:**
Stelle sicher, dass die Initialisierungsliste im Konstruktor der Reihenfolge der Deklaration in der Klasse entspricht:

```cpp
class UIManager {
private:
    esp_panel::board::Board* m_panel;      // Erst deklariert
    lv_obj_t* m_wlanIPLabel;               // Dann deklariert
};

// Konstruktor - Initialisierung in gleicher Reihenfolge
UIManager::UIManager() 
    : m_panel(nullptr),        // Erst initialisieren
      m_wlanIPLabel(nullptr)   // Dann initialisieren
{
    // ...
}
```

---

### 3. Compiler-Warnungen: -Wunused-but-set-variable

**Warnung:**
```
warning: variable 'btn' set but not used [-Wunused-but-set-variable]
```

**Lösung:**
- **Option 1**: Variable verwenden oder speichern
- **Option 2**: Variable entfernen, wenn nicht benötigt
- **Option 3**: Mit `(void)btn;` als "absichtlich ungenutzt" markieren

---

### 4. Compiler-Warnungen: -Wformat= für int32_t

**Warnung:**
```
warning: format '%d' expects argument of type 'int', but argument has type 'int32_t' {aka 'long int'}
```

**Lösung:**
Verwende den korrekten Format-Specifier für `int32_t`:

```cpp
// ❌ Falsch:
Serial.printf("Port: %d\n", port);  // port ist int32_t

// ✅ Richtig:
Serial.printf("Port: %" PRId32 "\n", port);

// Oder explizit casten:
Serial.printf("Port: %ld\n", (long)port);
```

**Header erforderlich:**
```cpp
#include <inttypes.h>  // Für PRId32
```

---

## ⚙️ Arduino IDE Konfiguration (Vollständig)

### Board-Einstellungen
```
Board: "ESP32S3 Dev Module"
Upload Speed: 921600
USB Mode: "Hardware CDC and JTAG"
USB CDC On Boot: "Disabled"
USB Firmware MSC On Boot: "Disabled"
USB DFU On Boot: "Disabled"
Upload Mode: "UART0 / Hardware CDC"
CPU Frequency: "240MHz (WiFi)"
Flash Mode: "QIO 80MHz"
Flash Size: "16MB (128Mb)"
Partition Scheme: "Huge APP (3MB No OTA/1MB SPIFFS)"
Core Debug Level: "None"
PSRAM: "OPI PSRAM"  ← KRITISCH! MUSS aktiviert sein!
Arduino Runs On: "Core 1"
Events Run On: "Core 1"
```

---

## 📚 LVGL Konfiguration

### lv_conf.h Platzierung (Empfohlen)

**Windows:**
```
C:\Users\<DeinBenutzername>\Documents\Arduino\libraries\lv_conf.h
```

**macOS/Linux:**
```
~/Documents/Arduino/libraries/lv_conf.h
```

### Wichtige LVGL-Einstellungen

Stelle sicher, dass folgende Werte in `lv_conf.h` gesetzt sind:

```c
#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN

// Aktiviere benötigte Schriftarten
#define LV_FONT_MONTSERRAT_14  1
#define LV_FONT_MONTSERRAT_16  1
#define LV_FONT_MONTSERRAT_18  1
#define LV_FONT_MONTSERRAT_20  1

// Memory-Management für PSRAM
#define LV_MEM_CUSTOM 0
```

---

## 🔍 Hardware-Debugging

### Serieller Monitor zeigt nichts

**Mögliche Ursachen:**
1. Falsche Baudrate (sollte 115200 sein)
2. "USB CDC On Boot" ist auf "Enabled" gesetzt (sollte "Disabled" sein)
3. Falscher COM-Port ausgewählt

**Lösung:**
```cpp
void setup() {
    Serial.begin(115200);
    delay(1000);  // Zeit für Serial-Verbindung
    Serial.println("ESP32-S3 gestartet!");
}
```

---

### Touch funktioniert nicht

**Checklist:**
1. GT911 Touch-Controller I2C-Adresse korrekt konfiguriert (0x5D oder 0x14)?
2. Touch-Kalibrierung durchgeführt?
3. I2C-Bus initialisiert?

**Test-Code:**
```cpp
#include <Wire.h>

void scanI2C() {
    Wire.begin();
    Serial.println("Scanning I2C...");
    for (byte addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            Serial.printf("Device found at 0x%02X\n", addr);
        }
    }
}
```

---

### Display bleibt schwarz

**Checklist:**
1. PSRAM aktiviert? (siehe oben)
2. Backlight-Pin korrekt konfiguriert?
3. Display-Panel korrekt initialisiert?
4. Stromversorgung ausreichend (7-36V DC)?

**Debug-Code:**
```cpp
Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());
```

---

## 🌐 Netzwerk-Probleme

### WiFi verbindet nicht

**Lösung:**
```cpp
WiFi.mode(WIFI_STA);
WiFi.begin(ssid, password);

int timeout = 20;
while (WiFi.status() != WL_CONNECTED && timeout > 0) {
    delay(500);
    Serial.print(".");
    timeout--;
}

if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.println(WiFi.localIP());
} else {
    Serial.println("\nWiFi connection failed!");
}
```

---

## 📖 Zusätzliche Ressourcen

- **Offizielle Dokumentation**: [Waveshare Wiki](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4.3B)
- **GitHub Issues**: [ESP32-S3-Touch-LCD-4.3B Issues](https://github.com/zonfacter/ESP32-S3-Touch-LCD-4.3B/issues)
- **ESP32-S3 Datasheet**: [Espressif Systems](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)
- **LVGL Documentation**: [docs.lvgl.io](https://docs.lvgl.io/)

---

## 🆘 Weitere Hilfe benötigt?

Wenn dein Problem hier nicht aufgeführt ist:
1. Erstelle ein [GitHub Issue](https://github.com/zonfacter/ESP32-S3-Touch-LCD-4.3B/issues/new) mit:
   - Vollständigen Fehlerausgaben
   - Board-Einstellungen
   - Code-Ausschnitt (wenn relevant)
   - Arduino IDE Version
2. Überprüfe die [offizielle Waveshare-Dokumentation](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4.3B)

---

**Letzte Aktualisierung**: 2026-02-05