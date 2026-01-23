// 
//  ESP32_CAN_43B.ino
//  BMS Monitor System - FINALE KORRIGIERTE VERSION
//  BMS Monitor Team
//  2025
//   
//   Hardware: Waveshare ESP32-S3-Touch-LCD-4.3B
//   Features:
//   - Multi-Protocol CAN BMS Support ( Pylontech, JK BMS, DALY)
//   - Auto-Detection
//   - Vollständige LVGL Touch UI
//   - KEINE Simulationsdaten!
// 

#include <Arduino.h>
#include <ESP_Panel_Library.h>
#include <lvgl.h>

// Core Includes
#include "src/core/bms_data_types.h"
#include "src/hardware/can_driver.h"
#include "src/managers/protocol_manager.h"

// Protocol Includes
#include "src/protocols/protocol_base_can.h"
#include "src/protocols/pylontech_can.h"
#include "src/protocols/jk_bms_can.h"
#include "src/protocols/daly_can.h"

// LVGL Port
#include "lvgl_v8_port.h"

// UI Manager
#include "src/ui/ui_manager.h"

// ============================================================================
// Globale Objekte
// ============================================================================

// Hardware
CanDriver canDriver;
ESP_Panel* panel = nullptr;

// Protokolle
PylontechCan pylontechProtocol;
JkBmsCan jkBmsProtocol;
DalyCan dalyProtocol;
ProtocolManager protocolManager;

// UI
UIManager* uiManager = nullptr;

// BMS Daten
bms_data_t currentBmsData;
bool dataValid = false;

// ============================================================================
// Konfiguration
// ============================================================================

struct AppConfig {
    static constexpr uint8_t CAN_TX_PIN = 12;
    static constexpr uint8_t CAN_RX_PIN = 13;
    static constexpr uint32_t CAN_BAUDRATE = 500000;
    static constexpr uint32_t DATA_DISPLAY_INTERVAL = 1000;
    static constexpr uint32_t STATUS_UPDATE_INTERVAL = 500;  // Status alle 500ms
    static constexpr uint32_t STATS_INTERVAL = 10000;
    static constexpr uint32_t DATA_TIMEOUT = 5000;
};

// Debug Flag (kann auskommentiert werden)
// #define DEBUG_CAN_MESSAGES

// ============================================================================
// CAN Callbacks
// ============================================================================

void onCanMessageReceived(uint32_t canId, const uint8_t* data, uint8_t length) {
    #ifdef DEBUG_CAN_MESSAGES
    Serial.printf("[CAN RX] ID: 0x%03X, LEN: %d, DATA: ", canId, length);
    for (int i = 0; i < length; i++) {
        Serial.printf("%02X ", data[i]);
    }
    Serial.println();
    #endif
    
    // An Protocol Manager weiterleiten
    bool processed = protocolManager.routeMessage(canId, data, length);
    
    if (processed) {
        // Daten vom aktiven Protokoll holen
        dataValid = protocolManager.getData(currentBmsData);
        
        // UI aktualisieren wenn auf BMS Data Screen
        if (uiManager && dataValid && uiManager->getCurrentScreen() == SCREEN_BMS_DATA) {
            uiManager->updateBmsData(currentBmsData);
        }
    }
}

void onCanError(twai_state_t state) {
    Serial.printf("[CAN ERROR] State: %d\n", state);
    
    if (state == TWAI_STATE_BUS_OFF) {
        Serial.println("[CAN] Bus-Off detected, attempting recovery...");
        canDriver.stop();
        delay(100);
        canDriver.start();
        
        if (uiManager) {
            uiManager->updateCanStatus("Bus-Off - Recovery attempted");
        }
    }
}

// ============================================================================
// Hardware-Konfigurations-Callbacks (von UI aufgerufen)
// ============================================================================
void onCanBaudrateChange(uint32_t newBaudrate) {
    Serial.printf("[Config] Changing CAN baudrate to %lu...\n", newBaudrate);
    
    // CAN stoppen
    canDriver.stop();
    delay(100);
    
    // Neu initialisieren mit neuer Baudrate
    if (canDriver.init(AppConfig::CAN_TX_PIN, AppConfig::CAN_RX_PIN, newBaudrate)) {
        canDriver.setMessageCallback(onCanMessageReceived);
        canDriver.setErrorCallback(onCanError);
        
        if (canDriver.start()) {
            Serial.println("[Config] CAN restarted with new baudrate");
        } else {
            Serial.println("[Config] ERROR: CAN start failed!");
        }
    } else {
        Serial.println("[Config] ERROR: CAN init failed!");
    }
}

void onCanAutoDetectChange(bool enabled) {
    Serial.printf("[Config] Auto-Detection %s\n", enabled ? "enabled" : "disabled");
    protocolManager.setAutoDetect(enabled);
}

void onCanProtocolChange(bms_type_t type) {
    Serial.printf("[Config] Manually selecting protocol: %s\n", getBmsTypeName(type));
    protocolManager.selectProtocol(type);
}

// ============================================================================
// Display Initialisierung
// ============================================================================

bool initDisplay() {
    Serial.println("[Display] Initializing ESP_Panel...");
    
    // ESP Panel erstellen und initialisieren
    panel = new ESP_Panel();
    
    if (!panel) {
        Serial.println("[Display] ERROR: Failed to create panel!");
        return false;
    }
    
    // Panel initialisieren
    panel->init();
    
    // Hinweis: Frame Buffer Anzahl wird automatisch durch die Board-Konfiguration gesetzt
    // Die esp_panel_board_custom_conf.h definiert bereits die korrekten Buffer
    Serial.println("[Display] Using board-configured frame buffers (2x for Direct Mode)");
    
    // Panel starten
    if (!panel->begin()) {
        Serial.println("[Display] ERROR: Panel begin failed!");
        return false;
    }
    
    Serial.println("[Display] Panel initialized successfully");
    
    // LVGL Port initialisieren
    Serial.println("[Display] Initializing LVGL port...");
    if (!lvgl_port_init(panel->getLcd(), panel->getTouch())) {
        Serial.println("[Display] ERROR: LVGL port init failed!");
        return false;
    }
    
    Serial.println("[Display] LVGL port initialized successfully");
    return true;
}

// ============================================================================
// System Initialisierung
// ============================================================================

bool initializeSystem() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n========================================");
    Serial.println("    BMS Monitor System");
    Serial.println("    Version: 1.0.0 FINAL");
    Serial.println("    Waveshare ESP32-S3-Touch-LCD-4.3B");
    Serial.println("========================================\n");
    
    // Schritt 1: Display initialisieren
    Serial.println("[Init] Step 1: Display...");
    if (!initDisplay()) {
        Serial.println("[Init] ERROR: Display initialization failed!");
        return false;
    }
    Serial.println("[Init] Step 1: Display OK");
    
    // Schritt 2: Protokolle registrieren
    Serial.println("[Init] Step 2: Registering protocols...");
    protocolManager.registerProtocol(&pylontechProtocol);
    protocolManager.registerProtocol(&jkBmsProtocol);
    protocolManager.registerProtocol(&dalyProtocol);
    Serial.printf("[Init] Registered %d protocols\n", protocolManager.getProtocolCount());
    
    // Schritt 3: Protokolle initialisieren
    Serial.println("[Init] Step 3: Initializing protocols...");
    if (!protocolManager.initializeAll()) {
        Serial.println("[Init] ERROR: Protocol initialization failed!");
        return false;
    }
    Serial.println("[Init] Step 3: Protocols OK");
    
    // Schritt 4: CAN Bus initialisieren
    Serial.println("[Init] Step 4: Initializing CAN...");
    if (!canDriver.init(AppConfig::CAN_TX_PIN, AppConfig::CAN_RX_PIN, AppConfig::CAN_BAUDRATE)) {
        Serial.println("[Init] ERROR: CAN initialization failed!");
        return false;
    }
    
    canDriver.setMessageCallback(onCanMessageReceived);
    canDriver.setErrorCallback(onCanError);
    Serial.println("[Init] Step 4: CAN OK");
    
    // Schritt 5: CAN starten
    Serial.println("[Init] Step 5: Starting CAN...");
    if (!canDriver.start()) {
        Serial.println("[Init] ERROR: CAN start failed!");
        return false;
    }
    Serial.println("[Init] Step 5: CAN Started");
    
    // Schritt 6: Protokolle starten
    Serial.println("[Init] Step 6: Starting protocols...");
    if (!protocolManager.startAll()) {
        Serial.println("[Init] ERROR: Protocol start failed!");
        return false;
    }
    protocolManager.setAutoDetect(true);
    Serial.println("[Init] Step 6: Protocols Started");
    
    // Schritt 7: UI initialisieren
    Serial.println("[Init] Step 7: Initializing UI...");
    uiManager = UIManager::getInstance();
    if (!uiManager || !uiManager->init()) {
        Serial.println("[Init] ERROR: UI initialization failed!");
        return false;
    }
    
    // Panel-Referenz an UI Manager übergeben für Backlight-Steuerung
    uiManager->setPanel(panel);
    
    // Hardware-Konfigurations-Callbacks registrieren
    uiManager->setCanBaudrateCallback(onCanBaudrateChange);
    uiManager->setCanAutoDetectCallback(onCanAutoDetectChange);
    uiManager->setCanProtocolCallback(onCanProtocolChange);
    
    // Initial-Helligkeit setzen
    uiManager->setBrightness(80);
    
    Serial.println("[Init] Step 7: UI OK");
    
    Serial.println("\n========================================");
    Serial.println("    System Ready!");
    Serial.println("    Waiting for BMS messages...");
    Serial.println("    Type 'help' for commands");
    Serial.println("========================================\n");
    
    return true;
}

// ============================================================================
// Serial Commands
// ============================================================================
void displayBmsData() {
    if (!dataValid) {
        Serial.println("[BMS] No valid data available");
        return;
    }
    
    uint32_t age = millis() - currentBmsData.last_update;
    if (age > AppConfig::DATA_TIMEOUT) {
        Serial.println("[BMS] Data timeout!");
        dataValid = false;
        return;
    }
    
    Serial.println("\n╔═══════════════════════════════════════╗");
    Serial.println("║         BMS LIVE DATA                 ║");
    Serial.println("╠═══════════════════════════════════════╣");
    Serial.printf  ("║ Type:       %-25s ║\n", getBmsTypeName(currentBmsData.type));
    Serial.printf  ("║ Voltage:    %6.2f V                  ║\n", currentBmsData.voltage);
    Serial.printf  ("║ Current:    %6.1f A                  ║\n", currentBmsData.current);
    Serial.printf  ("║ SOC:        %6.1f %%                 ║\n", currentBmsData.soc);
    Serial.printf  ("║ Temp:       %6.1f °C                 ║\n", currentBmsData.temperature);
    Serial.printf  ("║ Cycles:     %6u                      ║\n", currentBmsData.cycles);
    Serial.printf  ("║ Status:     %-25s ║\n", currentBmsData.status_text);
    Serial.printf  ("║ Age:        %6lu ms                  ║\n", age);
    Serial.println("╚═══════════════════════════════════════╝\n");
}


void displayStatistics() {
    Serial.println("\n========================================");
    Serial.println("           STATISTICS");
    Serial.println("========================================");
    
    // CAN Stats
    uint32_t rx, tx, err;
    canDriver.getStats(rx, tx, err);
    Serial.printf("CAN: RX=%lu, TX=%lu, ERR=%lu\n", rx, tx, err);
    
    // Protocol Detection Stats
    protocolManager.printDetectionStats();
    
    // Aktives Protokoll
    auto* active = protocolManager.getActiveProtocol();
    if (active) {
        Serial.printf("Active Protocol: %s\n", active->getName());
    } else {
        Serial.println("Active Protocol: None");
    }
    
    Serial.println("========================================\n");
}

void handleSerialCommands() {
    if (!Serial.available()) return;
    
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toLowerCase();
    
    if (cmd == "help") {
        Serial.println("\n=== BMS Monitor Commands ===");
        Serial.println("data       - Show current BMS data");
        Serial.println("stats      - Show statistics");
        Serial.println("reset      - Reset statistics");
        Serial.println("detect     - Enable auto-detection");
        Serial.println("pylontech  - Select Pylontech protocol");
        Serial.println("jk         - Select JK BMS protocol");
        Serial.println("daly       - Select DALY protocol");
        Serial.println("debug      - Toggle debug output");
        Serial.println("help       - Show this help");
        Serial.println("============================\n");
    }
    else if (cmd == "data") {
        displayBmsData();
    }
    else if (cmd == "stats") {
        displayStatistics();
    }
    else if (cmd == "reset") {
        canDriver.resetStats();
        protocolManager.resetStats();
        Serial.println("[CMD] Statistics reset");
    }
    else if (cmd == "detect") {
        protocolManager.setAutoDetect(true);
        Serial.println("[CMD] Auto-detection enabled");
    }
    else if (cmd == "pylontech") {
        if (protocolManager.selectProtocol(BMS_PYLONTECH)) {
            Serial.println("[CMD] Pylontech protocol selected");
        } else {
            Serial.println("[CMD] ERROR: Failed to select Pylontech");
        }
    }
    else if (cmd == "jk") {
        if (protocolManager.selectProtocol(BMS_JK_BMS)) {
            Serial.println("[CMD] JK BMS protocol selected");
        } else {
            Serial.println("[CMD] ERROR: Failed to select JK BMS");
        }
    }
    else if (cmd == "daly") {
        if (protocolManager.selectProtocol(BMS_DALY)) {
            Serial.println("[CMD] DALY protocol selected");
        } else {
            Serial.println("[CMD] ERROR: Failed to select DALY");
        }
    }
    else if (cmd == "debug") {
        #ifdef DEBUG_CAN_MESSAGES
        Serial.println("[CMD] Debug mode is currently ON");
        Serial.println("[CMD] To disable: Comment out #define DEBUG_CAN_MESSAGES and recompile");
        #else
        Serial.println("[CMD] Debug mode is currently OFF");
        Serial.println("[CMD] To enable: Uncomment #define DEBUG_CAN_MESSAGES and recompile");
        #endif
    }
    else {
        Serial.printf("[CMD] Unknown command: '%s'\n", cmd.c_str());
        Serial.println("[CMD] Type 'help' for available commands");
    }
}

// ============================================================================
// Setup & Loop
// ============================================================================

void setup() {
    // System initialisieren
    if (!initializeSystem()) {
        Serial.println("\n*** FATAL ERROR: System initialization failed! ***");
        Serial.println("*** System halted ***");
        while (1) {
            delay(1000);
        }
    }
    
    Serial.println("[System] Ready and running!");
    Serial.println("[System] Type 'help' for available commands\n");
}

void loop() {
    static uint32_t lastDisplay = 0;
    static uint32_t lastStats = 0;
    static uint32_t lastStatusUpdate = 0;
    uint32_t now = millis();
    
    // Serial-Kommandos verarbeiten
    handleSerialCommands();
    
    // Screen Timeout überwachen (Option 4)
    if (uiManager) {
        uiManager->checkInactivityTimeout();
    }
    
    // Periodische Status-Updates (Option 5)
    if (now - lastStatusUpdate >= AppConfig::STATUS_UPDATE_INTERVAL) {
        lastStatusUpdate = now;
        
        if (uiManager && uiManager->getCurrentScreen() == SCREEN_CAN) {
            // CAN Status aktualisieren
            char statusText[100];
            uint32_t rx, tx, err;
            canDriver.getStats(rx, tx, err);
            
            auto* active = protocolManager.getActiveProtocol();
            if (active) {
                snprintf(statusText, sizeof(statusText), 
                        "Active: %s | RX: %lu | TX: %lu | ERR: %lu",
                        active->getName(), rx, tx, err);
            } else {
                snprintf(statusText, sizeof(statusText), 
                        "Detecting... | RX: %lu | TX: %lu | ERR: %lu",
                        rx, tx, err);
            }
            
            uiManager->updateCanStatus(statusText);
        }
    }
    
    // Periodische Daten-Anzeige auf Serial
    if (now - lastDisplay >= AppConfig::DATA_DISPLAY_INTERVAL) {
        lastDisplay = now;
        
        if (dataValid) {
            // Nur auf Serial ausgeben, nicht im Debug-Modus
            #ifndef DEBUG_CAN_MESSAGES
            displayBmsData();
            #endif
            
            // UI aktualisieren wenn auf BMS Data Screen
            if (uiManager && uiManager->getCurrentScreen() == SCREEN_BMS_DATA) {
                uiManager->updateBmsData(currentBmsData);
            }
        } else {
            // Keine Verbindung
            if (uiManager && uiManager->getCurrentScreen() == SCREEN_BMS_DATA) {
                uiManager->showNoConnection();
            }
        }
    }
    
    // Periodische Statistik-Ausgabe
    if (now - lastStats >= AppConfig::STATS_INTERVAL) {
        lastStats = now;
        
        #ifndef DEBUG_CAN_MESSAGES
        displayStatistics();
        #endif
    }
    
    // Kleine Pause für andere Tasks
    delay(5);
}
