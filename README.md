# ESP32-S3-Touch-LCD-4.3B LVGL Examples

This repository demonstrates how to use LVGL (v8.3.x) with the Waveshare ESP32-S3-Touch-LCD-4.3B board and RGB LCD displays.

## Hardware

- **Board**: Waveshare ESP32-S3-Touch-LCD-4.3B
- **Display**: 4.3" RGB LCD, 800×480 pixels
- **Touch**: GT911 Capacitive Touch (I2C)
- **MCU**: ESP32-S3-WROOM-1-N16R8

## Quick Start

### Required Libraries

Install the following libraries via Arduino Library Manager:

1. **ESP32_Display_Panel** (latest version)
2. **LVGL** (v8.3.x) - **Important**: Use version 8.3.x, NOT 9.x

### Arduino IDE Configuration

**Board Settings:**
- Board: "ESP32S3 Dev Module" or "Waveshare ESP32-S3-Touch-LCD-4.3B"
- **PSRAM**: `Enabled` (CRITICAL!)
- **Partition Scheme**: "Huge APP (3MB No OTA/1MB SPIFFS)"
- **Upload Speed**: 921600
- **USB CDC On Boot**: Disabled

See [setup.md](setup.md) for detailed Arduino IDE settings.

## LVGL Configuration for Arduino

### Method 1: Place `lv_conf.h` in Arduino Libraries Folder (Recommended)

This is the most reliable method for Arduino IDE:

1. Copy the provided `lv_conf.h` to your Arduino libraries folder:
   - **Windows**: `C:\Users\<YourUsername>\Documents\Arduino\libraries\lv_conf.h`
   - **macOS**: `~/Documents/Arduino/libraries/lv_conf.h`
   - **Linux**: `~/Arduino/libraries/lv_conf.h`

2. The file should be **outside** the `lvgl` folder, at the same level:
   ```
   Arduino/
   └── libraries/
       ├── lv_conf.h          ← Place here
       ├── lvgl/
       │   └── ...
       └── ESP32_Display_Panel/
           └── ...
   ```

3. Make sure `lv_conf.h` has `#if 1` at the top (not `#if 0`)

### Method 2: Place `lv_conf.h` in Your Sketch Folder

Alternatively, you can place `lv_conf.h` directly in your sketch folder:

```
YourSketch/
├── YourSketch.ino
├── lv_conf.h              ← Place here
├── lvgl_v8_port.h
└── lvgl_v8_port.cpp
```

### Important LVGL Settings

Ensure these fonts are enabled in your `lv_conf.h`:

```c
#define LV_FONT_MONTSERRAT_14  1
#define LV_FONT_MONTSERRAT_16  1
#define LV_FONT_MONTSERRAT_18  1
#define LV_FONT_MONTSERRAT_20  1
#define LV_FONT_MONTSERRAT_24  1
#define LV_FONT_MONTSERRAT_30  1
```

For demos, enable:
```c
#define LV_USE_DEMO_WIDGETS    1
```

## Using the Updated API

This repository now uses the **updated, non-deprecated API**. Always use:

```cpp
#include <esp_display_panel.hpp>  // NOT <ESP_Panel_Library.h>
#include <lvgl.h>

using namespace esp_panel::drivers;
using namespace esp_panel::board;

void setup() {
    // Use Board, not ESP_Panel
    Board *board = new Board();
    board->init();
    board->begin();
    
    // Use getLCD(), not getLcd()
    lvgl_port_init(board->getLCD(), board->getTouch());
}
```

### Migrating from Deprecated API

If you see warnings about deprecated APIs, update your code:

| Deprecated | Use Instead |
|------------|-------------|
| `#include <ESP_Panel_Library.h>` | `#include <esp_display_panel.hpp>` |
| `ESP_Panel` | `esp_panel::board::Board` |
| `panel->getLcd()` | `panel->getLCD()` |

## Examples

### 1. Basic LVGL Porting Example

See [Waveshare_43B_09_lvgl_Porting.ino](Waveshare_43B_09_lvgl_Porting.ino) - A simple example showing LVGL setup with demo widgets.

### 2. Advanced BMS Monitor Example

See [Waveshare 4.3B/ESP32_CAN_43B/](Waveshare%204.3B/ESP32_CAN_43B/) - A complete BMS monitoring system with:
- Multi-protocol CAN BMS support (Pylontech, JK BMS, DALY)
- Auto-detection
- Full LVGL touch UI
- Multiple configuration screens

## Configuration Files

### `esp_panel_board_custom_conf.h`

Board-specific configuration including:
- Display resolution (800×480 for 4.3B)
- Touch controller settings
- RGB timing parameters
- IO expander configuration

Key settings:
```c
#define ESP_PANEL_USE_1024_600_LCD  (0)  // 0 for 800x480
#define ESP_OPEN_TOUCH              (1)  // Enable touch
```

### `lvgl_v8_port.h`

LVGL porting configuration including:
- Buffer settings
- Task stack size and priority
- Anti-tearing mode (recommended: Mode 3)

Key settings:
```c
#define LVGL_PORT_TASK_STACK_SIZE      (10 * 1024)  // 10KB
#define LVGL_PORT_AVOID_TEARING_MODE   (3)          // Direct mode
```

## Troubleshooting

### Error: `lv_conf.h: No such file or directory`

**Solution**: Place `lv_conf.h` in your Arduino libraries folder (see Method 1 above) or in your sketch folder (Method 2).

### Error: `lv_font_montserrat_XX not declared`

**Solution**: Enable the required fonts in `lv_conf.h`:
```c
#define LV_FONT_MONTSERRAT_XX  1  // Replace XX with the font size
```

### Warning: Deprecated API usage

**Solution**: Update your includes and API calls as shown in the "Using the Updated API" section above.

### Watchdog Reset / Crash on Startup

**Solutions**:
1. Ensure PSRAM is enabled in Arduino IDE
2. Set `LVGL_PORT_TASK_STACK_SIZE` to at least `(10 * 1024)` in `lvgl_v8_port.h`
3. Verify `ESP_PANEL_USE_1024_600_LCD` is set to `(0)` for 4.3" display
4. Use "Huge APP" partition scheme

### Display Shows Nothing

**Solutions**:
1. Check backlight settings in `esp_panel_board_custom_conf.h`
2. Ensure `ESP_PANEL_BOARD_USE_BACKLIGHT` is set to `(1)`
3. Try reconnecting USB cable

## Additional Resources

- [Detailed Setup Guide](setup.md) - Complete Arduino IDE configuration guide
- [Waveshare Wiki](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4.3B)
- [ESP32_Display_Panel GitHub](https://github.com/esp-arduino-libs/ESP32_Display_Panel)
- [LVGL Documentation](https://docs.lvgl.io/8.3/)

## License

This project uses:
- Espressif ESP32_Display_Panel Library
- LVGL (Light and Versatile Graphics Library)
- Waveshare hardware documentation

See [LICENSE](LICENSE) for details.
