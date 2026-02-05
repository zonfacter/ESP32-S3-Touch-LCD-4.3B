/**
 * @file ui_manager.h
 * @brief VOLLSTÄNDIGER UI Manager mit allen Screens
 * @author BMS Monitor Team
 * @date 2025
 * 
 * KEINE SIMULATIONSDATEN - Nur echte Hardware-Daten!
 */

#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <Arduino.h>
#include <lvgl.h>
#include <esp_display_panel.hpp>
#include <functional>
#include "lvgl_v8_port.h"
#include "../core/bms_data_types.h"

// ============================================================================
// Screen Enum
// ============================================================================

enum Screen {
    SCREEN_MAIN,
    SCREEN_BMS_DATA,
    SCREEN_CAN,
    SCREEN_RS485,
    SCREEN_MQTT,
    SCREEN_WLAN,
    SCREEN_DISPLAY
};

// ============================================================================
// UIManager Class
// ============================================================================

class UIManager {
private:
    // Screens
    lv_obj_t* m_mainScreen;
    lv_obj_t* m_bmsDataScreen;
    lv_obj_t* m_canScreen;
    lv_obj_t* m_rs485Screen;
    lv_obj_t* m_mqttScreen;
    lv_obj_t* m_wlanScreen;
    lv_obj_t* m_displayScreen;
    
    Screen m_currentScreen;
    
    // BMS Data Labels
    lv_obj_t* m_bmsTypeLabel;
    lv_obj_t* m_bmsStatusLabel;
    lv_obj_t* m_bmsVoltageLabel;
    lv_obj_t* m_bmsCurrentLabel;
    lv_obj_t* m_bmsSocLabel;
    lv_obj_t* m_bmsTempLabel;
    lv_obj_t* m_bmsCyclesLabel;
    lv_obj_t* m_bmsAgeLabel;
    
    // Display Settings Widgets
    lv_obj_t* m_brightnessSlider;
    lv_obj_t* m_brightnessLabel;
    lv_obj_t* m_themeSwitch;
    lv_obj_t* m_timeoutDropdown;
    
    // Panel Referenz für Backlight-Steuerung
    esp_panel::board::Board* m_panel;
    
    // CAN Config Widgets
    lv_obj_t* m_canBaudrateDropdown;
    lv_obj_t* m_canAutoDetectSwitch;
    lv_obj_t* m_canProtocolDropdown;
    lv_obj_t* m_canStatusLabel;
    
    // RS485 Config Widgets
    lv_obj_t* m_rs485BaudrateDropdown;
    lv_obj_t* m_rs485SlaveIdSpinbox;
    lv_obj_t* m_rs485AutoDetectSwitch;
    lv_obj_t* m_rs485ProtocolDropdown;
    lv_obj_t* m_rs485StatusLabel;
    
    // MQTT Config Widgets
    lv_obj_t* m_mqttServerInput;
    lv_obj_t* m_mqttPortSpinbox;
    lv_obj_t* m_mqttUserInput;
    lv_obj_t* m_mqttPassInput;
    lv_obj_t* m_mqttTopicInput;
    lv_obj_t* m_mqttConnectBtn;
    lv_obj_t* m_mqttStatusLabel;
    
    // WLAN Config Widgets
    lv_obj_t* m_wlanSSIDInput;
    lv_obj_t* m_wlanPassInput;
    lv_obj_t* m_wlanConnectBtn;
    lv_obj_t* m_wlanStatusLabel;
    lv_obj_t* m_wlanIPLabel;
    
    // Settings
    int m_brightnessLevel;
    bool m_themeDark;
    int m_screenTimeout;
    
    // Screen Timeout System
    uint32_t m_lastTouchTime;
    bool m_displayActive;
    int m_savedBrightness;
    
    // Callbacks für Hardware-Änderungen
    std::function<void(uint32_t)> m_onCanBaudrateChange;
    std::function<void(bms_type_t)> m_onCanProtocolChange;
    std::function<void(bool)> m_onCanAutoDetectChange;
    
    // Screen Creation Methods
    void createMainScreen();
    void createBmsDataScreen();
    void createCanScreen();
    void createRs485Screen();
    void createMqttScreen();
    void createWlanScreen();
    void createDisplayScreen();
    
    // Helper Methods
    lv_obj_t* createHeader(lv_obj_t* parent, const char* title, bool withBackBtn = false);
    lv_obj_t* createContainer(lv_obj_t* parent, int y, int height);
    lv_obj_t* createLabel(lv_obj_t* parent, const char* text, int x, int y, int fontSize = 18);
    lv_obj_t* createButton(lv_obj_t* parent, const char* text, int x, int y, int w, int h, 
                          lv_event_cb_t cb, uint32_t color = 0x0066CC);
    
    // Static Instance
    static UIManager* s_instance;
    
    // Event Callbacks - Static weil LVGL C-API
    static void btnMainEventCb(lv_event_t* e);
    static void btnBmsDataEventCb(lv_event_t* e);
    static void btnCanEventCb(lv_event_t* e);
    static void btnRs485EventCb(lv_event_t* e);
    static void btnMqttEventCb(lv_event_t* e);
    static void btnWlanEventCb(lv_event_t* e);
    static void btnDisplayEventCb(lv_event_t* e);
    static void btnBackEventCb(lv_event_t* e);
    
    // Display Settings Callbacks
    static void brightnessSliderEventCb(lv_event_t* e);
    static void themeSwitchEventCb(lv_event_t* e);
    static void timeoutDropdownEventCb(lv_event_t* e);
    
    // CAN Config Callbacks
    static void canBaudrateEventCb(lv_event_t* e);
    static void canAutoDetectEventCb(lv_event_t* e);
    static void canProtocolEventCb(lv_event_t* e);
    
    // RS485 Config Callbacks
    static void rs485BaudrateEventCb(lv_event_t* e);
    static void rs485AutoDetectEventCb(lv_event_t* e);
    static void rs485ProtocolEventCb(lv_event_t* e);
    
    // MQTT Callbacks
    static void mqttConnectEventCb(lv_event_t* e);
    
    // WLAN Callbacks
    static void wlanConnectEventCb(lv_event_t* e);

public:
    UIManager();
    ~UIManager();
    
    static UIManager* getInstance() {
        if (!s_instance) {
            s_instance = new UIManager();
        }
        return s_instance;
    }
    
    bool init();
    void setPanel(esp_panel::board::Board* panel) { m_panel = panel; }
    void createAllScreens();
    void switchToScreen(Screen screen);
    Screen getCurrentScreen() const { return m_currentScreen; }
    
    // BMS Data Update
    void updateBmsData(const bms_data_t& data);
    void showNoConnection();
    
    // Display Settings
    void setBrightness(int level);
    void toggleTheme();
    void applyTheme();
    void setScreenTimeout(int minutes);
    
    // Screen Timeout System
    void resetInactivityTimer();
    void checkInactivityTimeout();
    bool isDisplayActive() const { return m_displayActive; }
    
    // Hardware Change Callbacks registrieren
    void setCanBaudrateCallback(std::function<void(uint32_t)> callback) { m_onCanBaudrateChange = callback; }
    void setCanProtocolCallback(std::function<void(bms_type_t)> callback) { m_onCanProtocolChange = callback; }
    void setCanAutoDetectCallback(std::function<void(bool)> callback) { m_onCanAutoDetectChange = callback; }
    
    // CAN/RS485 Status Update
    void updateCanStatus(const char* status);
    void updateRs485Status(const char* status);
    void updateMqttStatus(const char* status);
    void updateWlanStatus(const char* status, const char* ip = nullptr);
};

// ============================================================================
// IMPLEMENTATION
// ============================================================================

UIManager* UIManager::s_instance = nullptr;

UIManager::UIManager()
    : m_mainScreen(nullptr)
    , m_bmsDataScreen(nullptr)
    , m_canScreen(nullptr)
    , m_rs485Screen(nullptr)
    , m_mqttScreen(nullptr)
    , m_wlanScreen(nullptr)
    , m_displayScreen(nullptr)
    , m_currentScreen(SCREEN_MAIN)
    , m_bmsTypeLabel(nullptr)
    , m_bmsStatusLabel(nullptr)
    , m_bmsVoltageLabel(nullptr)
    , m_bmsCurrentLabel(nullptr)
    , m_bmsSocLabel(nullptr)
    , m_bmsTempLabel(nullptr)
    , m_bmsCyclesLabel(nullptr)
    , m_bmsAgeLabel(nullptr)
    , m_brightnessSlider(nullptr)
    , m_brightnessLabel(nullptr)
    , m_themeSwitch(nullptr)
    , m_timeoutDropdown(nullptr)
    , m_canBaudrateDropdown(nullptr)
    , m_canAutoDetectSwitch(nullptr)
    , m_canProtocolDropdown(nullptr)
    , m_canStatusLabel(nullptr)
    , m_rs485BaudrateDropdown(nullptr)
    , m_rs485SlaveIdSpinbox(nullptr)
    , m_rs485AutoDetectSwitch(nullptr)
    , m_rs485ProtocolDropdown(nullptr)
    , m_rs485StatusLabel(nullptr)
    , m_mqttServerInput(nullptr)
    , m_mqttPortSpinbox(nullptr)
    , m_mqttUserInput(nullptr)
    , m_mqttPassInput(nullptr)
    , m_mqttTopicInput(nullptr)
    , m_mqttConnectBtn(nullptr)
    , m_mqttStatusLabel(nullptr)
    , m_wlanSSIDInput(nullptr)
    , m_wlanPassInput(nullptr)
    , m_wlanConnectBtn(nullptr)
    , m_wlanStatusLabel(nullptr)
    , m_panel(nullptr)
    , m_wlanIPLabel(nullptr)
    , m_brightnessLevel(80)
    , m_themeDark(false)  // Standard: Light Theme (false = light, true = dark)
    , m_screenTimeout(10)
    , m_lastTouchTime(0)
    , m_displayActive(true)
    , m_savedBrightness(80)
    , m_onCanBaudrateChange(nullptr)
    , m_onCanProtocolChange(nullptr)
    , m_onCanAutoDetectChange(nullptr)
{
    Serial.println("[UI] Manager created");
}

UIManager::~UIManager() {}

bool UIManager::init() {
    Serial.println("[UI] Initializing...");
    
    m_lastTouchTime = millis();
    m_displayActive = true;
    
    createAllScreens();
    switchToScreen(SCREEN_MAIN);
    
    Serial.println("[UI] Initialized successfully");
    return true;
}

void UIManager::createAllScreens() {
    Serial.println("[UI] Creating all screens...");
    lvgl_port_lock(-1);
    
    createMainScreen();
    createBmsDataScreen();
    createCanScreen();
    createRs485Screen();
    createMqttScreen();
    createWlanScreen();
    createDisplayScreen();
    
    lvgl_port_unlock();
    Serial.println("[UI] All screens created");
}

// ============================================================================
// Helper Methods
// ============================================================================

lv_obj_t* UIManager::createHeader(lv_obj_t* parent, const char* title, bool withBackBtn) {
    lv_obj_t* header = lv_obj_create(parent);
    lv_obj_set_size(header, 800, 60);
    lv_obj_set_pos(header, 0, 0);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x1E1E1E), 0);
    lv_obj_set_style_border_width(header, 0, 0);
    
    if (withBackBtn) {
        lv_obj_t* btn_back = lv_btn_create(header);
        lv_obj_set_size(btn_back, 80, 40);
        lv_obj_set_pos(btn_back, 10, 10);
        lv_obj_add_event_cb(btn_back, btnBackEventCb, LV_EVENT_CLICKED, this);
        lv_obj_t* label = lv_label_create(btn_back);
        lv_label_set_text(label, LV_SYMBOL_LEFT " Back");
        lv_obj_center(label);
    }
    
    lv_obj_t* titleLabel = lv_label_create(header);
    lv_label_set_text(titleLabel, title);
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_24, 0);
    lv_obj_align(titleLabel, LV_ALIGN_CENTER, 0, 0);
    
    return header;
}

lv_obj_t* UIManager::createContainer(lv_obj_t* parent, int y, int height) {
    lv_obj_t* cont = lv_obj_create(parent);
    lv_obj_set_size(cont, 760, height);
    lv_obj_set_pos(cont, 20, y);
    lv_obj_set_style_pad_all(cont, 20, 0);
    return cont;
}

lv_obj_t* UIManager::createLabel(lv_obj_t* parent, const char* text, int x, int y, int fontSize) {
    lv_obj_t* label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_pos(label, x, y);
    
    if (fontSize == 24) {
        lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
    } else if (fontSize == 20) {
        lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0);
    } else {
        lv_obj_set_style_text_font(label, &lv_font_montserrat_18, 0);
    }
    
    return label;
}

lv_obj_t* UIManager::createButton(lv_obj_t* parent, const char* text, int x, int y, int w, int h, 
                                 lv_event_cb_t cb, uint32_t color) {
    lv_obj_t* btn = lv_btn_create(parent);
    lv_obj_set_size(btn, w, h);
    lv_obj_set_pos(btn, x, y);
    lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, this);
    lv_obj_set_style_bg_color(btn, lv_color_hex(color), 0);
    
    lv_obj_t* label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0);
    lv_obj_center(label);
    
    return btn;
}

// ============================================================================
// Main Screen
// ============================================================================

void UIManager::createMainScreen() {
    m_mainScreen = lv_obj_create(NULL);
    
    createHeader(m_mainScreen, "BMS Monitor System", false);
    
    lv_obj_t* cont = createContainer(m_mainScreen, 70, 400);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(cont, 15, 0);
    
    // BMS Data Button
    createButton(cont, LV_SYMBOL_BATTERY_3 "\nBMS DATA", 0, 0, 220, 120, 
                                btnBmsDataEventCb, 0xFF8800);
    
    // CAN Button
    createButton(cont, LV_SYMBOL_CALL "\nCAN", 0, 0, 220, 120, btnCanEventCb, 0x0066CC);
    
    // RS485 Button
    createButton(cont, LV_SYMBOL_USB "\nRS485", 0, 0, 220, 120, btnRs485EventCb, 0x00AA00);
    
    // MQTT Button
    createButton(cont, LV_SYMBOL_WIFI "\nMQTT", 0, 0, 220, 120, btnMqttEventCb, 0x9C27B0);
    
    // WLAN Button
    createButton(cont, LV_SYMBOL_WIFI "\nWLAN", 0, 0, 220, 120, btnWlanEventCb, 0x2196F3);
    
    // Display Button
    createButton(cont, LV_SYMBOL_SETTINGS "\nDISPLAY", 0, 0, 220, 120, btnDisplayEventCb, 0x607D8B);
}

// ============================================================================
// BMS Data Screen
// ============================================================================

void UIManager::createBmsDataScreen() {
    m_bmsDataScreen = lv_obj_create(NULL);
    createHeader(m_bmsDataScreen, "BMS Live Data", true);
    
    lv_obj_t* cont = createContainer(m_bmsDataScreen, 70, 400);
    
    m_bmsTypeLabel = createLabel(cont, "Type: Waiting...", 20, 20, 20);
    m_bmsStatusLabel = createLabel(cont, "Status: No Connection", 20, 55, 18);
    m_bmsAgeLabel = createLabel(cont, "Data Age: --", 20, 85, 16);
    
    // Separator
    lv_obj_t* line = lv_obj_create(cont);
    lv_obj_set_size(line, 700, 2);
    lv_obj_set_pos(line, 20, 120);
    lv_obj_set_style_bg_color(line, lv_color_hex(0x404040), 0);
    lv_obj_set_style_border_width(line, 0, 0);
    
    m_bmsVoltageLabel = createLabel(cont, "Voltage: -- V", 20, 140, 24);
    m_bmsCurrentLabel = createLabel(cont, "Current: -- A", 20, 180, 24);
    m_bmsSocLabel = createLabel(cont, "SOC: -- %", 20, 220, 24);
    m_bmsTempLabel = createLabel(cont, "Temperature: -- °C", 20, 260, 24);
    m_bmsCyclesLabel = createLabel(cont, "Cycles: --", 20, 300, 24);
}

// ============================================================================
// CAN Configuration Screen
// ============================================================================

void UIManager::createCanScreen() {
    m_canScreen = lv_obj_create(NULL);
    createHeader(m_canScreen, "CAN Configuration", true);
    
    lv_obj_t* cont = createContainer(m_canScreen, 70, 400);
    
    int y = 20;
    
    // Baudrate
    createLabel(cont, "Baudrate:", 20, y, 18);
    m_canBaudrateDropdown = lv_dropdown_create(cont);
    lv_dropdown_set_options(m_canBaudrateDropdown, "125 kBit/s\n250 kBit/s\n500 kBit/s\n1 MBit/s");
    lv_dropdown_set_selected(m_canBaudrateDropdown, 2); // 500k default
    lv_obj_set_size(m_canBaudrateDropdown, 200, 40);
    lv_obj_set_pos(m_canBaudrateDropdown, 250, y);
    lv_obj_add_event_cb(m_canBaudrateDropdown, canBaudrateEventCb, LV_EVENT_VALUE_CHANGED, this);
    
    y += 60;
    
    // Auto-Detect
    createLabel(cont, "Auto-Detect BMS:", 20, y, 18);
    m_canAutoDetectSwitch = lv_switch_create(cont);
    lv_obj_set_pos(m_canAutoDetectSwitch, 250, y - 5);
    lv_obj_add_state(m_canAutoDetectSwitch, LV_STATE_CHECKED); // Default ON
    lv_obj_add_event_cb(m_canAutoDetectSwitch, canAutoDetectEventCb, LV_EVENT_VALUE_CHANGED, this);
    
    y += 60;
    
    // Protocol Selection
    createLabel(cont, "Protocol:", 20, y, 18);
    m_canProtocolDropdown = lv_dropdown_create(cont);
    lv_dropdown_set_options(m_canProtocolDropdown, "Auto-Detect\nPylontech\nJK BMS\nDALY");
    lv_dropdown_set_selected(m_canProtocolDropdown, 0);
    lv_obj_set_size(m_canProtocolDropdown, 200, 40);
    lv_obj_set_pos(m_canProtocolDropdown, 250, y);
    lv_obj_add_event_cb(m_canProtocolDropdown, canProtocolEventCb, LV_EVENT_VALUE_CHANGED, this);
    
    y += 80;
    
    // Status
    m_canStatusLabel = createLabel(cont, "Status: CAN Bus active, waiting for messages...", 20, y, 16);
}

// ============================================================================
// RS485 Configuration Screen
// ============================================================================

void UIManager::createRs485Screen() {
    m_rs485Screen = lv_obj_create(NULL);
    createHeader(m_rs485Screen, "RS485 Configuration", true);
    
    lv_obj_t* cont = createContainer(m_rs485Screen, 70, 400);
    
    int y = 20;
    
    // Baudrate
    createLabel(cont, "Baudrate:", 20, y, 18);
    m_rs485BaudrateDropdown = lv_dropdown_create(cont);
    lv_dropdown_set_options(m_rs485BaudrateDropdown, "9600 Baud\n19200 Baud\n38400 Baud\n115200 Baud");
    lv_dropdown_set_selected(m_rs485BaudrateDropdown, 0); // 9600 default
    lv_obj_set_size(m_rs485BaudrateDropdown, 200, 40);
    lv_obj_set_pos(m_rs485BaudrateDropdown, 250, y);
    lv_obj_add_event_cb(m_rs485BaudrateDropdown, rs485BaudrateEventCb, LV_EVENT_VALUE_CHANGED, this);
    
    y += 60;
    
    // Slave ID
    createLabel(cont, "Slave ID:", 20, y, 18);
    m_rs485SlaveIdSpinbox = lv_spinbox_create(cont);
    lv_spinbox_set_range(m_rs485SlaveIdSpinbox, 1, 247);
    lv_spinbox_set_value(m_rs485SlaveIdSpinbox, 1);
    lv_obj_set_size(m_rs485SlaveIdSpinbox, 100, 40);
    lv_obj_set_pos(m_rs485SlaveIdSpinbox, 250, y);
    
    y += 60;
    
    // Auto-Detect
    createLabel(cont, "Auto-Detect BMS:", 20, y, 18);
    m_rs485AutoDetectSwitch = lv_switch_create(cont);
    lv_obj_set_pos(m_rs485AutoDetectSwitch, 250, y - 5);
    lv_obj_add_state(m_rs485AutoDetectSwitch, LV_STATE_CHECKED);
    lv_obj_add_event_cb(m_rs485AutoDetectSwitch, rs485AutoDetectEventCb, LV_EVENT_VALUE_CHANGED, this);
    
    y += 60;
    
    // Protocol
    createLabel(cont, "Protocol:", 20, y, 18);
    m_rs485ProtocolDropdown = lv_dropdown_create(cont);
    lv_dropdown_set_options(m_rs485ProtocolDropdown, "Auto-Detect\nModbus RTU");
    lv_dropdown_set_selected(m_rs485ProtocolDropdown, 0);
    lv_obj_set_size(m_rs485ProtocolDropdown, 200, 40);
    lv_obj_set_pos(m_rs485ProtocolDropdown, 250, y);
    lv_obj_add_event_cb(m_rs485ProtocolDropdown, rs485ProtocolEventCb, LV_EVENT_VALUE_CHANGED, this);
    
    y += 80;
    
    // Status
    m_rs485StatusLabel = createLabel(cont, "Status: RS485 not configured", 20, y, 16);
}

// ============================================================================
// MQTT Configuration Screen
// ============================================================================

void UIManager::createMqttScreen() {
    m_mqttScreen = lv_obj_create(NULL);
    createHeader(m_mqttScreen, "MQTT Configuration", true);
    
    lv_obj_t* cont = createContainer(m_mqttScreen, 70, 400);
    
    int y = 20;
    
    // Server
    createLabel(cont, "Server:", 20, y, 18);
    m_mqttServerInput = lv_textarea_create(cont);
    lv_textarea_set_one_line(m_mqttServerInput, true);
    lv_textarea_set_placeholder_text(m_mqttServerInput, "mqtt.example.com");
    lv_obj_set_size(m_mqttServerInput, 400, 40);
    lv_obj_set_pos(m_mqttServerInput, 150, y);
    
    y += 60;
    
    // Port
    createLabel(cont, "Port:", 20, y, 18);
    m_mqttPortSpinbox = lv_spinbox_create(cont);
    lv_spinbox_set_range(m_mqttPortSpinbox, 1, 65535);
    lv_spinbox_set_value(m_mqttPortSpinbox, 1883);
    lv_obj_set_size(m_mqttPortSpinbox, 120, 40);
    lv_obj_set_pos(m_mqttPortSpinbox, 150, y);
    
    y += 60;
    
    // Username
    createLabel(cont, "Username:", 20, y, 18);
    m_mqttUserInput = lv_textarea_create(cont);
    lv_textarea_set_one_line(m_mqttUserInput, true);
    lv_textarea_set_placeholder_text(m_mqttUserInput, "optional");
    lv_obj_set_size(m_mqttUserInput, 300, 40);
    lv_obj_set_pos(m_mqttUserInput, 150, y);
    
    y += 60;
    
    // Password
    createLabel(cont, "Password:", 20, y, 18);
    m_mqttPassInput = lv_textarea_create(cont);
    lv_textarea_set_one_line(m_mqttPassInput, true);
    lv_textarea_set_password_mode(m_mqttPassInput, true);
    lv_textarea_set_placeholder_text(m_mqttPassInput, "optional");
    lv_obj_set_size(m_mqttPassInput, 300, 40);
    lv_obj_set_pos(m_mqttPassInput, 150, y);
    
    y += 60;
    
    // Topic
    createLabel(cont, "Topic:", 20, y, 18);
    m_mqttTopicInput = lv_textarea_create(cont);
    lv_textarea_set_one_line(m_mqttTopicInput, true);
    lv_textarea_set_text(m_mqttTopicInput, "bms/data");
    lv_obj_set_size(m_mqttTopicInput, 300, 40);
    lv_obj_set_pos(m_mqttTopicInput, 150, y);
    
    y += 70;
    
    // Connect Button
    m_mqttConnectBtn = createButton(cont, "Connect to MQTT", 20, y, 200, 50, mqttConnectEventCb, 0x9C27B0);
    
    // Status
    m_mqttStatusLabel = createLabel(cont, "Status: Not connected", 250, y + 15, 16);
}

// ============================================================================
// WLAN Configuration Screen
// ============================================================================

void UIManager::createWlanScreen() {
    m_wlanScreen = lv_obj_create(NULL);
    createHeader(m_wlanScreen, "WLAN Configuration", true);
    
    lv_obj_t* cont = createContainer(m_wlanScreen, 70, 400);
    
    int y = 40;
    
    // SSID
    createLabel(cont, "SSID:", 20, y, 20);
    m_wlanSSIDInput = lv_textarea_create(cont);
    lv_textarea_set_one_line(m_wlanSSIDInput, true);
    lv_textarea_set_placeholder_text(m_wlanSSIDInput, "WiFi Network Name");
    lv_obj_set_size(m_wlanSSIDInput, 500, 45);
    lv_obj_set_pos(m_wlanSSIDInput, 150, y);
    
    y += 80;
    
    // Password
    createLabel(cont, "Password:", 20, y, 20);
    m_wlanPassInput = lv_textarea_create(cont);
    lv_textarea_set_one_line(m_wlanPassInput, true);
    lv_textarea_set_password_mode(m_wlanPassInput, true);
    lv_textarea_set_placeholder_text(m_wlanPassInput, "WiFi Password");
    lv_obj_set_size(m_wlanPassInput, 500, 45);
    lv_obj_set_pos(m_wlanPassInput, 150, y);
    
    y += 100;
    
    // Connect Button
    m_wlanConnectBtn = createButton(cont, "Connect to WiFi", 150, y, 250, 60, wlanConnectEventCb, 0x2196F3);
    
    y += 80;
    
    // Status
    m_wlanStatusLabel = createLabel(cont, "Status: Not connected", 20, y, 18);
    m_wlanIPLabel = createLabel(cont, "IP Address: --", 20, y + 35, 18);
}

// ============================================================================
// Display Settings Screen
// ============================================================================

void UIManager::createDisplayScreen() {
    m_displayScreen = lv_obj_create(NULL);
    createHeader(m_displayScreen, "Display Settings", true);
    
    lv_obj_t* cont = createContainer(m_displayScreen, 70, 400);
    
    int y = 30;
    
    // Brightness
    createLabel(cont, "Helligkeit:", 20, y, 20);
    
    // HINWEIS: Hardware limitation - nur ON/OFF möglich
    lv_obj_t* noteLabel = lv_label_create(cont);
    lv_label_set_text(noteLabel, "(Nur AN/AUS - Hardware-Limitation)");
    lv_obj_set_pos(noteLabel, 200, y - 20);
    lv_obj_set_style_text_font(noteLabel, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(noteLabel, lv_palette_main(LV_PALETTE_ORANGE), 0);
    
    m_brightnessSlider = lv_slider_create(cont);
    lv_obj_set_size(m_brightnessSlider, 450, 25);
    lv_obj_set_pos(m_brightnessSlider, 200, y + 5);
    lv_slider_set_range(m_brightnessSlider, 10, 100);
    lv_slider_set_value(m_brightnessSlider, m_brightnessLevel, LV_ANIM_OFF);
    lv_obj_add_event_cb(m_brightnessSlider, brightnessSliderEventCb, LV_EVENT_VALUE_CHANGED, this);
    
    m_brightnessLabel = lv_label_create(cont);
    lv_label_set_text_fmt(m_brightnessLabel, "%d%%", m_brightnessLevel);
    lv_obj_set_pos(m_brightnessLabel, 670, y);
    lv_obj_set_style_text_font(m_brightnessLabel, &lv_font_montserrat_20, 0);
    
    y += 80;
    
    // Dark Theme
    createLabel(cont, "Dark Theme:", 20, y, 20);
    
    m_themeSwitch = lv_switch_create(cont);
    lv_obj_set_pos(m_themeSwitch, 200, y - 5);
    // Initial-Zustand basierend auf m_themeDark (initial = false = Light)
    if (m_themeDark) {
        lv_obj_add_state(m_themeSwitch, LV_STATE_CHECKED);
    }
    lv_obj_add_event_cb(m_themeSwitch, themeSwitchEventCb, LV_EVENT_VALUE_CHANGED, this);
    
    y += 80;
    
    // Screen Timeout
    createLabel(cont, "Bildschirm aus:", 20, y, 20);
    
    m_timeoutDropdown = lv_dropdown_create(cont);
    lv_dropdown_set_options(m_timeoutDropdown, "Nie\n1 Minute\n5 Minuten\n10 Minuten\n30 Minuten");
    lv_obj_set_size(m_timeoutDropdown, 200, 45);
    lv_obj_set_pos(m_timeoutDropdown, 200, y);
    lv_dropdown_set_selected(m_timeoutDropdown, 3); // 10 Min default
    lv_obj_add_event_cb(m_timeoutDropdown, timeoutDropdownEventCb, LV_EVENT_VALUE_CHANGED, this);
}

// ============================================================================
// Screen Management
// ============================================================================

void UIManager::switchToScreen(Screen screen) {
    lvgl_port_lock(-1);
    
    m_currentScreen = screen;
    
    // Touch-Event durch Screen-Wechsel
    resetInactivityTimer();
    
    switch(screen) {
        case SCREEN_MAIN:     lv_scr_load(m_mainScreen); break;
        case SCREEN_BMS_DATA: lv_scr_load(m_bmsDataScreen); break;
        case SCREEN_CAN:      lv_scr_load(m_canScreen); break;
        case SCREEN_RS485:    lv_scr_load(m_rs485Screen); break;
        case SCREEN_MQTT:     lv_scr_load(m_mqttScreen); break;
        case SCREEN_WLAN:     lv_scr_load(m_wlanScreen); break;
        case SCREEN_DISPLAY:  lv_scr_load(m_displayScreen); break;
    }
    
    lvgl_port_unlock();
}

// ============================================================================
// BMS Data Update
// ============================================================================

void UIManager::updateBmsData(const bms_data_t& data) {
    if (!m_bmsVoltageLabel) return;
    
    lvgl_port_lock(-1);
    
    // Type und Status
    lv_label_set_text_fmt(m_bmsTypeLabel, "Type: %s", getBmsTypeName(data.type));
    lv_label_set_text_fmt(m_bmsStatusLabel, "Status: %s", data.status_text);
    lv_label_set_text_fmt(m_bmsAgeLabel, "Data Age: %lu ms", millis() - data.last_update);
    
    // Werte
    lv_label_set_text_fmt(m_bmsVoltageLabel, "Voltage: %.2f V", data.voltage);
    
    // Current mit Richtung
    const char* direction = "";
    if (data.charging) direction = " (Charging)";
    else if (data.discharging) direction = " (Discharging)";
    lv_label_set_text_fmt(m_bmsCurrentLabel, "Current: %.1f A%s", data.current, direction);
    
    lv_label_set_text_fmt(m_bmsSocLabel, "SOC: %.1f %%", data.soc);
    lv_label_set_text_fmt(m_bmsTempLabel, "Temperature: %.1f °C", data.temperature);
    lv_label_set_text_fmt(m_bmsCyclesLabel, "Cycles: %u", data.cycles);
    
    lvgl_port_unlock();
}

void UIManager::showNoConnection() {
    if (!m_bmsStatusLabel) return;
    
    lvgl_port_lock(-1);
    lv_label_set_text(m_bmsTypeLabel, "Type: Unknown");
    lv_label_set_text(m_bmsStatusLabel, "Status: No BMS Connected");
    lv_label_set_text(m_bmsAgeLabel, "Data Age: --");
    lv_label_set_text(m_bmsVoltageLabel, "Voltage: -- V");
    lv_label_set_text(m_bmsCurrentLabel, "Current: -- A");
    lv_label_set_text(m_bmsSocLabel, "SOC: -- %");
    lv_label_set_text(m_bmsTempLabel, "Temperature: -- °C");
    lv_label_set_text(m_bmsCyclesLabel, "Cycles: --");
    lvgl_port_unlock();
}

// ============================================================================
// Status Updates
// ============================================================================

void UIManager::updateCanStatus(const char* status) {
    if (!m_canStatusLabel) return;
    lvgl_port_lock(-1);
    lv_label_set_text_fmt(m_canStatusLabel, "Status: %s", status);
    lvgl_port_unlock();
}

void UIManager::updateRs485Status(const char* status) {
    if (!m_rs485StatusLabel) return;
    lvgl_port_lock(-1);
    lv_label_set_text_fmt(m_rs485StatusLabel, "Status: %s", status);
    lvgl_port_unlock();
}

void UIManager::updateMqttStatus(const char* status) {
    if (!m_mqttStatusLabel) return;
    lvgl_port_lock(-1);
    lv_label_set_text_fmt(m_mqttStatusLabel, "Status: %s", status);
    lvgl_port_unlock();
}

void UIManager::updateWlanStatus(const char* status, const char* ip) {
    if (!m_wlanStatusLabel) return;
    lvgl_port_lock(-1);
    lv_label_set_text_fmt(m_wlanStatusLabel, "Status: %s", status);
    if (ip && m_wlanIPLabel) {
        lv_label_set_text_fmt(m_wlanIPLabel, "IP Address: %s", ip);
    }
    lvgl_port_unlock();
}

// ============================================================================
// Display Settings
// ============================================================================

void UIManager::setBrightness(int level) {
    m_brightnessLevel = level;
    
    if (m_panel) {
        // Backlight über Panel steuern
        auto backlight = m_panel->getBacklight();
        if (backlight) {
            // WICHTIG: Das Board verwendet einen SWITCH_EXPANDER Backlight-Typ,
            // der nur ON/OFF unterstützt, KEINE PWM-Helligkeitsregelung.
            // Daher: level > 0 = ON, level == 0 = OFF
            if (level > 0) {
                backlight->on();
                Serial.printf("[UI] Backlight turned ON (requested level: %d%%)\n", level);
            } else {
                backlight->off();
                Serial.println("[UI] Backlight turned OFF");
            }
        } else {
            Serial.println("[UI] WARNING: Backlight not available");
        }
    } else {
        Serial.println("[UI] WARNING: Panel not set");
    }
}

void UIManager::toggleTheme() {
    m_themeDark = !m_themeDark;
    Serial.printf("[UI] Theme toggled to %s\n", m_themeDark ? "dark" : "light");
    applyTheme();
}

void UIManager::applyTheme() {
    Serial.printf("[UI] Applying %s theme\n", m_themeDark ? "dark" : "light");
    
    lvgl_port_lock(-1);
    
    // LVGL Theme anwenden
    lv_disp_t* disp = lv_disp_get_default();
    if (!disp) {
        Serial.println("[UI] ERROR: No LVGL display available for theme change");
        lvgl_port_unlock();
        return;
    }
    
    lv_theme_t* theme = lv_theme_default_init(
        disp,
        lv_palette_main(LV_PALETTE_BLUE),      // Primary Color
        lv_palette_main(LV_PALETTE_RED),       // Secondary Color
        m_themeDark,                             // Dark mode flag
        LV_FONT_DEFAULT
    );
    
    if (!theme) {
        Serial.println("[UI] ERROR: Theme initialization failed");
        lvgl_port_unlock();
        return;
    }
    
    lv_disp_set_theme(disp, theme);
    
    // Alle Screens neu laden damit Theme wirksam wird
    Screen currentScreen = m_currentScreen;
    
    // Screens neu erstellen mit neuem Theme
    if (m_mainScreen) {
        lv_obj_del_async(m_mainScreen);
        m_mainScreen = nullptr;
    }
    if (m_bmsDataScreen) {
        lv_obj_del_async(m_bmsDataScreen);
        m_bmsDataScreen = nullptr;
    }
    if (m_canScreen) {
        lv_obj_del_async(m_canScreen);
        m_canScreen = nullptr;
    }
    if (m_rs485Screen) {
        lv_obj_del_async(m_rs485Screen);
        m_rs485Screen = nullptr;
    }
    if (m_mqttScreen) {
        lv_obj_del_async(m_mqttScreen);
        m_mqttScreen = nullptr;
    }
    if (m_wlanScreen) {
        lv_obj_del_async(m_wlanScreen);
        m_wlanScreen = nullptr;
    }
    if (m_displayScreen) {
        lv_obj_del_async(m_displayScreen);
        m_displayScreen = nullptr;
    }
    
    // Neu erstellen
    createAllScreens();
    
    // Zurück zum aktuellen Screen
    switchToScreen(currentScreen);
    
    lvgl_port_unlock();
    
    Serial.println("[UI] Theme applied successfully");
}

void UIManager::setScreenTimeout(int minutes) {
    m_screenTimeout = minutes;
    Serial.printf("[UI] Screen timeout set to %d minutes\n", minutes);
}

// ============================================================================
// Screen Timeout System
// ============================================================================

void UIManager::resetInactivityTimer() {
    m_lastTouchTime = millis();
    
    // Display war aus? Dann wieder einschalten
    if (!m_displayActive) {
        m_displayActive = true;
        setBrightness(m_savedBrightness);
        Serial.println("[UI] Display activated by touch");
    }
}

void UIManager::checkInactivityTimeout() {
    // Timeout deaktiviert (0 = Nie)?
    if (m_screenTimeout == 0) {
        return;
    }
    
    // Display bereits aus?
    if (!m_displayActive) {
        return;
    }
    
    // Zeit seit letztem Touch
    uint32_t inactiveTime = millis() - m_lastTouchTime;
    uint32_t timeoutMs = m_screenTimeout * 60 * 1000; // Minuten zu Millisekunden
    
    // Timeout erreicht?
    if (inactiveTime >= timeoutMs) {
        m_displayActive = false;
        m_savedBrightness = m_brightnessLevel;
        setBrightness(0); // Display ausschalten
        Serial.println("[UI] Display deactivated due to inactivity");
    }
}

// ============================================================================
// EVENT CALLBACKS
// ============================================================================

void UIManager::btnMainEventCb(lv_event_t* e) {
    UIManager* ui = (UIManager*)lv_event_get_user_data(e);
    ui->resetInactivityTimer();
    ui->switchToScreen(SCREEN_MAIN);
}

void UIManager::btnBmsDataEventCb(lv_event_t* e) {
    UIManager* ui = (UIManager*)lv_event_get_user_data(e);
    ui->resetInactivityTimer();
    ui->switchToScreen(SCREEN_BMS_DATA);
}

void UIManager::btnCanEventCb(lv_event_t* e) {
    UIManager* ui = (UIManager*)lv_event_get_user_data(e);
    ui->resetInactivityTimer();
    ui->switchToScreen(SCREEN_CAN);
}

void UIManager::btnRs485EventCb(lv_event_t* e) {
    UIManager* ui = (UIManager*)lv_event_get_user_data(e);
    ui->resetInactivityTimer();
    ui->switchToScreen(SCREEN_RS485);
}

void UIManager::btnMqttEventCb(lv_event_t* e) {
    UIManager* ui = (UIManager*)lv_event_get_user_data(e);
    ui->resetInactivityTimer();
    ui->switchToScreen(SCREEN_MQTT);
}

void UIManager::btnWlanEventCb(lv_event_t* e) {
    UIManager* ui = (UIManager*)lv_event_get_user_data(e);
    ui->resetInactivityTimer();
    ui->switchToScreen(SCREEN_WLAN);
}

void UIManager::btnDisplayEventCb(lv_event_t* e) {
    UIManager* ui = (UIManager*)lv_event_get_user_data(e);
    ui->resetInactivityTimer();
    ui->switchToScreen(SCREEN_DISPLAY);
}

void UIManager::btnBackEventCb(lv_event_t* e) {
    UIManager* ui = (UIManager*)lv_event_get_user_data(e);
    ui->resetInactivityTimer();
    ui->switchToScreen(SCREEN_MAIN);
}

// Display Settings Callbacks
void UIManager::brightnessSliderEventCb(lv_event_t* e) {
    UIManager* ui = (UIManager*)lv_event_get_user_data(e);
    lv_obj_t* slider = lv_event_get_target(e);
    int value = lv_slider_get_value(slider);
    
    ui->resetInactivityTimer(); // Touch-Event
    ui->setBrightness(value);
    
    if (ui->m_brightnessLabel) {
        lv_label_set_text_fmt(ui->m_brightnessLabel, "%d%%", value);
    }
}

void UIManager::themeSwitchEventCb(lv_event_t* e) {
    UIManager* ui = (UIManager*)lv_event_get_user_data(e);
    ui->resetInactivityTimer(); // Touch-Event
    ui->toggleTheme();
}

void UIManager::timeoutDropdownEventCb(lv_event_t* e) {
    UIManager* ui = (UIManager*)lv_event_get_user_data(e);
    lv_obj_t* dropdown = lv_event_get_target(e);
    uint16_t selected = lv_dropdown_get_selected(dropdown);
    
    ui->resetInactivityTimer(); // Touch-Event
    
    int timeouts[] = {0, 1, 5, 10, 30}; // Nie, 1, 5, 10, 30 Minuten
    ui->setScreenTimeout(timeouts[selected]);
}

// CAN Config Callbacks - Jetzt mit echter Hardware-Steuerung!
void UIManager::canBaudrateEventCb(lv_event_t* e) {
    UIManager* ui = (UIManager*)lv_event_get_user_data(e);
    lv_obj_t* dropdown = lv_event_get_target(e);
    uint16_t selected = lv_dropdown_get_selected(dropdown);
    
    ui->resetInactivityTimer(); // Touch-Event
    
    uint32_t baudrates[] = {125000, 250000, 500000, 1000000};
    uint32_t newBaudrate = baudrates[selected];
    
    Serial.printf("[UI] CAN Baudrate changed to %lu\n", newBaudrate);
    
    // Hardware-Callback aufrufen
    if (ui->m_onCanBaudrateChange) {
        ui->m_onCanBaudrateChange(newBaudrate);
        ui->updateCanStatus("Baudrate changed - CAN restarted");
    }
}

void UIManager::canAutoDetectEventCb(lv_event_t* e) {
    UIManager* ui = (UIManager*)lv_event_get_user_data(e);
    lv_obj_t* sw = lv_event_get_target(e);
    bool enabled = lv_obj_has_state(sw, LV_STATE_CHECKED);
    
    ui->resetInactivityTimer(); // Touch-Event
    
    Serial.printf("[UI] CAN Auto-Detect %s\n", enabled ? "enabled" : "disabled");
    
    // Hardware-Callback aufrufen
    if (ui->m_onCanAutoDetectChange) {
        ui->m_onCanAutoDetectChange(enabled);
        ui->updateCanStatus(enabled ? "Auto-Detection enabled" : "Auto-Detection disabled");
    }
}

void UIManager::canProtocolEventCb(lv_event_t* e) {
    UIManager* ui = (UIManager*)lv_event_get_user_data(e);
    lv_obj_t* dropdown = lv_event_get_target(e);
    uint16_t selected = lv_dropdown_get_selected(dropdown);
    
    ui->resetInactivityTimer(); // Touch-Event
    
    const char* protocols[] = {"Auto", "Pylontech", "JK BMS", "DALY"};
    bms_type_t types[] = {BMS_NONE, BMS_PYLONTECH, BMS_JK_BMS, BMS_DALY};
    
    Serial.printf("[UI] CAN Protocol changed to %s\n", protocols[selected]);
    
    // Hardware-Callback aufrufen
    if (ui->m_onCanProtocolChange && selected > 0) {
        ui->m_onCanProtocolChange(types[selected]);
        ui->updateCanStatus("Protocol manually selected");
    }
}

// RS485 Config Callbacks
void UIManager::rs485BaudrateEventCb(lv_event_t* e) {
    UIManager* ui = (UIManager*)lv_event_get_user_data(e);
    lv_obj_t* dropdown = lv_event_get_target(e);
    uint16_t selected = lv_dropdown_get_selected(dropdown);
    
    ui->resetInactivityTimer(); // Touch-Event
    
    uint32_t baudrates[] = {9600, 19200, 38400, 115200};
    Serial.printf("[UI] RS485 Baudrate changed to %lu\n", baudrates[selected]);
    ui->updateRs485Status("RS485 not yet implemented");
}

void UIManager::rs485AutoDetectEventCb(lv_event_t* e) {
    UIManager* ui = (UIManager*)lv_event_get_user_data(e);
    lv_obj_t* sw = lv_event_get_target(e);
    bool enabled = lv_obj_has_state(sw, LV_STATE_CHECKED);
    
    ui->resetInactivityTimer(); // Touch-Event
    
    Serial.printf("[UI] RS485 Auto-Detect %s\n", enabled ? "enabled" : "disabled");
    ui->updateRs485Status("RS485 not yet implemented");
}

void UIManager::rs485ProtocolEventCb(lv_event_t* e) {
    UIManager* ui = (UIManager*)lv_event_get_user_data(e);
    lv_obj_t* dropdown = lv_event_get_target(e);
    uint16_t selected = lv_dropdown_get_selected(dropdown);
    
    ui->resetInactivityTimer(); // Touch-Event
    
    const char* protocols[] = {"Auto", "Modbus RTU"};
    Serial.printf("[UI] RS485 Protocol changed to %s\n", protocols[selected]);
    ui->updateRs485Status("RS485 not yet implemented");
}

// MQTT Callbacks
void UIManager::mqttConnectEventCb(lv_event_t* e) {
    UIManager* ui = (UIManager*)lv_event_get_user_data(e);
    ui->resetInactivityTimer(); // Touch-Event
    
    Serial.println("[UI] MQTT Connect clicked");
    
    const char* server = lv_textarea_get_text(ui->m_mqttServerInput);
    int32_t port = lv_spinbox_get_value(ui->m_mqttPortSpinbox);
    const char* topic = lv_textarea_get_text(ui->m_mqttTopicInput);
    
    Serial.printf("[UI] MQTT Config: %s:%ld, topic: %s\n", server, port, topic);
    ui->updateMqttStatus("MQTT not yet implemented");
}

// WLAN Callbacks
void UIManager::wlanConnectEventCb(lv_event_t* e) {
    UIManager* ui = (UIManager*)lv_event_get_user_data(e);
    ui->resetInactivityTimer(); // Touch-Event
    
    Serial.println("[UI] WLAN Connect clicked");
    
    const char* ssid = lv_textarea_get_text(ui->m_wlanSSIDInput);
    
    Serial.printf("[UI] WLAN Config: SSID=%s\n", ssid);
    ui->updateWlanStatus("WLAN not yet implemented");
}

#endif // UI_MANAGER_H
