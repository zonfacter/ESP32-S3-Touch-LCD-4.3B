/**
 * @file can_driver.h
 * @brief Hardware-Abstraktion für CAN-Bus (TWAI)
 * @author BMS Monitor Team
 * @date 2025
 * 
 * Kapselt die ESP32 TWAI-Hardware und bietet eine einfache
 * Callback-basierte Schnittstelle für CAN-Kommunikation.
 * 
 * SPEICHERN ALS: src/hardware/can_driver.h
 */

#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#include <Arduino.h>
#include "driver/twai.h"
#include <functional>

// ============================================================================
// Callback-Typen
// ============================================================================

/**
 * @brief Callback für empfangene CAN-Nachrichten
 * @param canId CAN-Identifier
 * @param data Zeiger auf Daten-Array
 * @param length Länge der Daten
 */
using CanMessageCallback = std::function<void(uint32_t canId, const uint8_t* data, uint8_t length)>;

/**
 * @brief Callback für CAN-Fehler
 * @param state TWAI State
 */
using CanErrorCallback = std::function<void(twai_state_t state)>;

// ============================================================================
// CAN Driver Klasse
// ============================================================================

/**
 * @brief Hardware-Abstraktions-Layer für CAN (TWAI)
 * 
 * Diese Klasse kapselt die ESP32 TWAI-Hardware und bietet eine
 * einfache, Callback-basierte Schnittstelle für CAN-Kommunikation.
 */
class CanDriver {
private:
    // ========================================================================
    // Member-Variablen
    // ========================================================================
    
    bool m_initialized;                 ///< Initialisierungs-Status
    bool m_running;                     ///< Lauf-Status
    uint8_t m_txPin;                    ///< TX GPIO Pin
    uint8_t m_rxPin;                    ///< RX GPIO Pin
    uint32_t m_baudrate;                ///< Baudrate in bps
    
    // Callbacks
    CanMessageCallback m_messageCallback;   ///< Callback für Nachrichten
    CanErrorCallback m_errorCallback;       ///< Callback für Fehler
    
    // Task für Message-Reception
    TaskHandle_t m_rxTask;              ///< Handle für RX-Task
    
    // Statistik
    uint32_t m_rxCount;                 ///< Empfangene Nachrichten
    uint32_t m_txCount;                 ///< Gesendete Nachrichten
    uint32_t m_errorCount;              ///< Anzahl Fehler
    
    // ========================================================================
    // Statische Task-Funktion
    // ========================================================================
    
    /**
     * @brief Task-Funktion für CAN-Empfang
     * @param parameter Zeiger auf CanDriver-Instanz
     */
    static void rxTaskFunction(void* parameter) {
        CanDriver* driver = static_cast<CanDriver*>(parameter);
        twai_message_t message;
        
        Serial.println("[CAN Task] Started");
        
        while (driver->m_running) {
            // Warte auf Nachricht (mit Timeout)
            esp_err_t err = twai_receive(&message, pdMS_TO_TICKS(100));
            
            if (err == ESP_OK) {
                driver->m_rxCount++;
                
                // Callback aufrufen
                if (driver->m_messageCallback) {
                    driver->m_messageCallback(
                        message.identifier,
                        message.data,
                        message.data_length_code
                    );
                }
            } else if (err == ESP_ERR_TIMEOUT) {
                // Timeout ist normal, einfach weiter warten
                continue;
            } else {
                driver->m_errorCount++;
                
                // Error Callback
                if (driver->m_errorCallback) {
                    twai_status_info_t status;
                    twai_get_status_info(&status);
                    driver->m_errorCallback(status.state);
                }
            }
            
            // Kleine Pause um Watchdog zu füttern
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        
        Serial.println("[CAN Task] Stopped");
        vTaskDelete(nullptr);
    }

public:
    // ========================================================================
    // Konstruktor und Destruktor
    // ========================================================================
    
    /**
     * @brief Konstruktor
     */
    CanDriver()
        : m_initialized(false)
        , m_running(false)
        , m_txPin(0)
        , m_rxPin(0)
        , m_baudrate(0)
        , m_messageCallback(nullptr)
        , m_errorCallback(nullptr)
        , m_rxTask(nullptr)
        , m_rxCount(0)
        , m_txCount(0)
        , m_errorCount(0)
    {
        Serial.println("[CAN] Driver created");
    }
    
    /**
     * @brief Destruktor
     */
    ~CanDriver() {
        deinit();
    }
    
    // ========================================================================
    // Initialisierung
    // ========================================================================
    
    /**
     * @brief Initialisiert CAN-Hardware
     * @param txPin TX-Pin GPIO Nummer
     * @param rxPin RX-Pin GPIO Nummer
     * @param baudrate Baudrate (125000, 250000, 500000, 1000000)
     * @return true bei Erfolg
     */
    bool init(uint8_t txPin, uint8_t rxPin, uint32_t baudrate) {
        if (m_initialized) {
            Serial.println("[CAN] Already initialized");
            return false;
        }
        
        m_txPin = txPin;
        m_rxPin = rxPin;
        m_baudrate = baudrate;
        
        Serial.printf("[CAN] Initializing on TX=%d, RX=%d @ %lu bps...\n", 
                     m_txPin, m_rxPin, m_baudrate);
        
        // TWAI General Configuration
        twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
            (gpio_num_t)m_txPin, 
            (gpio_num_t)m_rxPin, 
            TWAI_MODE_NORMAL
        );
        g_config.rx_queue_len = 20;  // Größere Queue für mehr Nachrichten
        g_config.tx_queue_len = 10;
        
        // TWAI Timing Configuration
        twai_timing_config_t t_config;
        switch (baudrate) {
            case 125000:  t_config = TWAI_TIMING_CONFIG_125KBITS(); break;
            case 250000:  t_config = TWAI_TIMING_CONFIG_250KBITS(); break;
            case 500000:  t_config = TWAI_TIMING_CONFIG_500KBITS(); break;
            case 1000000: t_config = TWAI_TIMING_CONFIG_1MBITS(); break;
            default:
                Serial.printf("[CAN] ERROR: Invalid baudrate: %lu\n", baudrate);
                return false;
        }
        
        // TWAI Filter Configuration (accept all messages)
        twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
        
        // Install TWAI driver
        esp_err_t err = twai_driver_install(&g_config, &t_config, &f_config);
        if (err != ESP_OK) {
            Serial.printf("[CAN] ERROR: Driver install failed: %d\n", err);
            return false;
        }
        
        m_initialized = true;
        Serial.println("[CAN] Initialization successful");
        return true;
    }
    
    /**
     * @brief Deinitialisiert CAN-Hardware
     * @return true bei Erfolg
     */
    bool deinit() {
        if (m_running) {
            stop();
        }
        
        if (m_initialized) {
            twai_driver_uninstall();
            m_initialized = false;
            Serial.println("[CAN] Deinitialized");
        }
        
        return true;
    }
    
    // ========================================================================
    // Start/Stop
    // ========================================================================
    
    /**
     * @brief Startet CAN-Kommunikation
     * @return true bei Erfolg
     */
    bool start() {
        if (!m_initialized || m_running) {
            Serial.println("[CAN] ERROR: Not initialized or already running");
            return false;
        }
        
        // TWAI starten
        esp_err_t err = twai_start();
        if (err != ESP_OK) {
            Serial.printf("[CAN] ERROR: Start failed: %d\n", err);
            return false;
        }
        
        // RX-Task erstellen
        BaseType_t result = xTaskCreate(
            rxTaskFunction,
            "can_rx_task",
            4096,           // Stack size
            this,           // Parameter (this pointer)
            5,              // Priority
            &m_rxTask
        );
        
        if (result != pdPASS) {
            Serial.println("[CAN] ERROR: Failed to create RX task");
            twai_stop();
            return false;
        }
        
        m_running = true;
        Serial.println("[CAN] Started successfully");
        return true;
    }
    
    /**
     * @brief Stoppt CAN-Kommunikation
     * @return true bei Erfolg
     */
    bool stop() {
        if (!m_running) {
            return false;
        }
        
        Serial.println("[CAN] Stopping...");
        
        // Task beenden
        m_running = false;  // Flag setzen damit Task sich beendet
        if (m_rxTask) {
            vTaskDelay(pdMS_TO_TICKS(200));  // Task Zeit zum Beenden geben
            m_rxTask = nullptr;
        }
        
        // TWAI stoppen
        twai_stop();
        
        Serial.println("[CAN] Stopped");
        return true;
    }
    
    // ========================================================================
    // Message Senden
    // ========================================================================
    
    /**
     * @brief Sendet eine CAN-Nachricht
     * @param canId CAN-Identifier
     * @param data Daten-Array
     * @param length Länge der Daten (max 8)
     * @return true bei Erfolg
     */
    bool sendMessage(uint32_t canId, const uint8_t* data, uint8_t length) {
        if (!m_running || length > 8) {
            return false;
        }
        
        twai_message_t message = {};
        message.identifier = canId;
        message.data_length_code = length;
        message.extd = (canId > 0x7FF) ? 1 : 0;  // Extended ID wenn > 11 Bit
        memcpy(message.data, data, length);
        
        esp_err_t err = twai_transmit(&message, pdMS_TO_TICKS(100));
        if (err == ESP_OK) {
            m_txCount++;
            return true;
        }
        
        m_errorCount++;
        return false;
    }
    
    // ========================================================================
    // Callbacks
    // ========================================================================
    
    /**
     * @brief Registriert Callback für empfangene Nachrichten
     * @param callback Callback-Funktion
     */
    void setMessageCallback(CanMessageCallback callback) {
        m_messageCallback = callback;
    }
    
    /**
     * @brief Registriert Callback für Fehler
     * @param callback Callback-Funktion
     */
    void setErrorCallback(CanErrorCallback callback) {
        m_errorCallback = callback;
    }
    
    // ========================================================================
    // Status und Statistiken
    // ========================================================================
    
    /**
     * @brief Gibt an ob CAN läuft
     */
    bool isRunning() const { 
        return m_running; 
    }
    
    /**
     * @brief Gibt an ob CAN initialisiert ist
     */
    bool isInitialized() const { 
        return m_initialized; 
    }
    
    /**
     * @brief Gibt Statistiken zurück
     * @param rx Referenz für RX-Counter
     * @param tx Referenz für TX-Counter
     * @param errors Referenz für Error-Counter
     */
    void getStats(uint32_t& rx, uint32_t& tx, uint32_t& errors) const {
        rx = m_rxCount;
        tx = m_txCount;
        errors = m_errorCount;
    }
    
    /**
     * @brief Setzt Statistiken zurück
     */
    void resetStats() {
        m_rxCount = 0;
        m_txCount = 0;
        m_errorCount = 0;
    }
    
    /**
     * @brief Gibt Statistiken auf Serial aus
     */
    void printStats() const {
        Serial.println("\n=== CAN Driver Stats ===");
        Serial.printf("RX Messages:  %lu\n", m_rxCount);
        Serial.printf("TX Messages:  %lu\n", m_txCount);
        Serial.printf("Errors:       %lu\n", m_errorCount);
        Serial.printf("Running:      %s\n", m_running ? "YES" : "NO");
        Serial.printf("Baudrate:     %lu bps\n", m_baudrate);
        Serial.println("========================\n");
    }
};

#endif // CAN_DRIVER_H
