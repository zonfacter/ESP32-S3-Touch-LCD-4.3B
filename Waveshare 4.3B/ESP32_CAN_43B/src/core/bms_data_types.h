/**
 * @file bms_data_types.h
 * @brief Gemeinsame Datentypen für das BMS Monitor System
 * @author BMS Monitor Team
 * @date 2025
 * 
 * Diese Datei enthält alle gemeinsamen Datenstrukturen und Enums,
 * die von allen Modulen verwendet werden.
 */

#ifndef BMS_DATA_TYPES_H
#define BMS_DATA_TYPES_H

#include <stdint.h>
#include <string.h>

//=============================================================================
// BMS Typen
//=============================================================================

/**
 * @brief Unterstützte BMS-Typen
 */
enum bms_type_t {
    BMS_NONE = 0,           ///< Kein BMS erkannt
    BMS_PYLONTECH,          ///< Pylontech BMS
    BMS_JK_BMS,             ///< JK BMS
    BMS_DALY,               ///< DALY BMS
    BMS_SEPLOS,             ///< Seplos BMS
    BMS_EVE,                ///< EVE LiFePO4 BMS
    BMS_LIFEPO4_POWER       ///< LiFePO4 Power BMS
};

//=============================================================================
// BMS Datenstruktur
//=============================================================================

/**
 * @brief Zentrale BMS-Datenstruktur
 * 
 * Enthält alle relevanten Daten eines Battery Management Systems.
 * Wird von allen Protokollen gefüllt und vom UI angezeigt.
 */
struct bms_data_t {
    // Identifikation
    bms_type_t type;                ///< BMS-Typ
    bool connected;                 ///< Verbindungsstatus
    
    // Elektrische Werte
    float voltage;                  ///< Spannung in Volt
    float current;                  ///< Strom in Ampere (+ = Laden, - = Entladen)
    float soc;                      ///< State of Charge in Prozent (0-100)
    float temperature;              ///< Temperatur in Celsius
    
    // Batterie-Zustand
    uint16_t cycles;                ///< Anzahl Ladezyklen
    bool charging;                  ///< true = wird geladen
    bool discharging;               ///< true = wird entladen
    
    // Status
    char status_text[64];           ///< Status-Text für UI
    uint32_t last_update;           ///< Zeitstempel des letzten Updates (millis())
    
    // Konstruktor für Initialisierung
    bms_data_t() {
        type = BMS_NONE;
        connected = false;
        voltage = 0.0f;
        current = 0.0f;
        soc = 0.0f;
        temperature = 0.0f;
        cycles = 0;
        charging = false;
        discharging = false;
        memset(status_text, 0, sizeof(status_text));
        last_update = 0;
    }
};

//=============================================================================
// CAN Konfiguration
//=============================================================================

/**
 * @brief CAN Baudrate Optionen
 */
enum can_baudrate_t {
    CAN_125K = 0,           ///< 125 kBit/s
    CAN_250K,               ///< 250 kBit/s
    CAN_500K,               ///< 500 kBit/s (Standard für BMS)
    CAN_1M                  ///< 1 MBit/s
};

/**
 * @brief CAN Konfigurationsstruktur
 */
struct can_config_t {
    can_baudrate_t baudrate;        ///< CAN Baudrate
    bool termination;               ///< 120 Ohm Terminierung aktiv
    bool auto_detect_bms;           ///< Automatische BMS-Erkennung
    uint8_t tx_pin;                 ///< TX GPIO Pin
    uint8_t rx_pin;                 ///< RX GPIO Pin
    
    // Konstruktor mit Standardwerten
    can_config_t() {
        baudrate = CAN_500K;
        termination = true;
        auto_detect_bms = true;
        tx_pin = 12;
        rx_pin = 13;
    }
};

//=============================================================================
// RS485/Modbus Konfiguration
//=============================================================================

/**
 * @brief RS485 Baudrate Optionen
 */
enum rs485_baudrate_t {
    RS485_9600 = 0,         ///< 9600 Baud (Standard für Modbus)
    RS485_19200,            ///< 19200 Baud
    RS485_38400,            ///< 38400 Baud
    RS485_115200            ///< 115200 Baud
};

/**
 * @brief RS485 Konfigurationsstruktur
 */
struct rs485_config_t {
    rs485_baudrate_t baudrate;      ///< RS485 Baudrate
    uint8_t slave_id;               ///< Modbus Slave ID
    bool auto_detect_bms;           ///< Automatische BMS-Erkennung
    uint8_t tx_pin;                 ///< TX GPIO Pin
    uint8_t rx_pin;                 ///< RX GPIO Pin
    uint8_t de_pin;                 ///< Driver Enable Pin
    uint8_t re_pin;                 ///< Receiver Enable Pin
    
    // Konstruktor mit Standardwerten
    rs485_config_t() {
        baudrate = RS485_9600;
        slave_id = 1;
        auto_detect_bms = true;
        tx_pin = 16;
        rx_pin = 15;
        de_pin = 4;
        re_pin = 4;
    }
};

//=============================================================================
// Helper Funktionen
//=============================================================================

/**
 * @brief Gibt den Namen eines BMS-Typs zurück
 * @param type BMS-Typ
 * @return Name als String
 */
inline const char* getBmsTypeName(bms_type_t type) {
    switch (type) {
        case BMS_PYLONTECH:     return "Pylontech";
        case BMS_JK_BMS:        return "JK BMS";
        case BMS_DALY:          return "DALY BMS";
        case BMS_SEPLOS:        return "Seplos";
        case BMS_EVE:           return "EVE LiFePO4";
        case BMS_LIFEPO4_POWER: return "LiFePO4 Power";
        case BMS_NONE:          
        default:                return "Unknown";
    }
}

/**
 * @brief Konvertiert CAN Baudrate Enum zu Wert
 * @param rate Baudrate Enum
 * @return Baudrate in bps
 */
inline uint32_t getCanBaudrateValue(can_baudrate_t rate) {
    switch (rate) {
        case CAN_125K:  return 125000;
        case CAN_250K:  return 250000;
        case CAN_500K:  return 500000;
        case CAN_1M:    return 1000000;
        default:        return 500000;
    }
}

/**
 * @brief Konvertiert RS485 Baudrate Enum zu Wert
 * @param rate Baudrate Enum
 * @return Baudrate in bps
 */
inline uint32_t getRs485BaudrateValue(rs485_baudrate_t rate) {
    switch (rate) {
        case RS485_9600:    return 9600;
        case RS485_19200:   return 19200;
        case RS485_38400:   return 38400;
        case RS485_115200:  return 115200;
        default:            return 9600;
    }
}

/**
 * @brief Gibt CAN Baudrate Namen zurück
 * @param rate Baudrate Enum
 * @return Name als String
 */
inline const char* getCanBaudrateName(can_baudrate_t rate) {
    switch (rate) {
        case CAN_125K:  return "125 kBit/s";
        case CAN_250K:  return "250 kBit/s";
        case CAN_500K:  return "500 kBit/s";
        case CAN_1M:    return "1 MBit/s";
        default:        return "Unknown";
    }
}

/**
 * @brief Gibt RS485 Baudrate Namen zurück
 * @param rate Baudrate Enum
 * @return Name als String
 */
inline const char* getRs485BaudrateName(rs485_baudrate_t rate) {
    switch (rate) {
        case RS485_9600:    return "9600 Baud";
        case RS485_19200:   return "19200 Baud";
        case RS485_38400:   return "38400 Baud";
        case RS485_115200:  return "115200 Baud";
        default:            return "Unknown";
    }
}

#endif // BMS_DATA_TYPES_H
