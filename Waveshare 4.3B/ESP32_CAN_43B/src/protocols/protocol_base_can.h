/**
 * @file protocol_base_can.h
 * @brief Basis-Klasse für alle CAN-basierten BMS-Protokolle
 * @author BMS Monitor Team
 * @date 2025
 * 
 * SPEICHERN ALS: src/protocols/protocol_base_can.h
 */

#ifndef PROTOCOL_BASE_CAN_H
#define PROTOCOL_BASE_CAN_H

#include <Arduino.h>
#include "../core/bms_data_types.h"

/**
 * @brief Abstrakte Basis-Klasse für CAN-Protokolle
 */
class CanProtocolBase {
protected:
    bms_data_t m_data;
    bool m_connected;
    uint32_t m_lastUpdate;
    uint32_t m_messageCount;
    uint32_t m_errorCount;
    
    // Helper-Funktionen
    uint16_t extractUint16(const uint8_t* data, size_t offset, bool bigEndian = true) {
        if (bigEndian) {
            return (uint16_t)(data[offset] << 8) | data[offset + 1];
        } else {
            return (uint16_t)(data[offset + 1] << 8) | data[offset];
        }
    }
    
    int16_t extractInt16(const uint8_t* data, size_t offset, bool bigEndian = true) {
        return (int16_t)extractUint16(data, offset, bigEndian);
    }
    
    uint32_t extractUint32(const uint8_t* data, size_t offset, bool bigEndian = true) {
        if (bigEndian) {
            return ((uint32_t)data[offset] << 24) |
                   ((uint32_t)data[offset + 1] << 16) |
                   ((uint32_t)data[offset + 2] << 8) |
                   ((uint32_t)data[offset + 3]);
        } else {
            return ((uint32_t)data[offset + 3] << 24) |
                   ((uint32_t)data[offset + 2] << 16) |
                   ((uint32_t)data[offset + 1] << 8) |
                   ((uint32_t)data[offset]);
        }
    }
    
    int32_t extractInt32(const uint8_t* data, size_t offset, bool bigEndian = true) {
        return (int32_t)extractUint32(data, offset, bigEndian);
    }
    
    bool validateRange(float value, float min, float max) const {
        return (value >= min && value <= max);
    }
    
    void markUpdated() {
        m_lastUpdate = millis();
        m_connected = true;
        m_messageCount++;
    }
    
    void markError() {
        m_errorCount++;
    }

public:
    CanProtocolBase() 
        : m_connected(false)
        , m_lastUpdate(0)
        , m_messageCount(0)
        , m_errorCount(0)
    {
        memset(&m_data, 0, sizeof(m_data));
        m_data.type = BMS_NONE;
    }
    
    virtual ~CanProtocolBase() = default;
    
    // Abstrakte Methoden
    virtual const char* getName() const = 0;
    virtual bms_type_t getType() const = 0;
    virtual bool canAcceptMessage(uint32_t canId) const = 0;
    virtual bool parseMessage(uint32_t canId, const uint8_t* data, uint8_t length) = 0;
    
    // Standard-Implementation
    virtual bool initialize() {
        m_connected = false;
        m_lastUpdate = 0;
        m_messageCount = 0;
        m_errorCount = 0;
        memset(&m_data, 0, sizeof(m_data));
        m_data.type = getType();
        return true;
    }
    
    virtual bool start() {
        Serial.printf("[%s] Protocol started\n", getName());
        return true;
    }
    
    virtual bool stop() {
        Serial.printf("[%s] Protocol stopped\n", getName());
        m_connected = false;
        return true;
    }
    
    bool isConnected(uint32_t timeoutMs = 5000) const { 
        if (!m_connected) {
            return false;
        }
        return ((millis() - m_lastUpdate) < timeoutMs);
    }
    
    bool getData(bms_data_t& data) const {
        if (!isConnected()) {
            return false;
        }
        data = m_data;
        data.connected = true;
        data.last_update = m_lastUpdate;
        return true;
    }
    
    uint32_t getDataAge() const {
        return millis() - m_lastUpdate;
    }
    
    void getStats(uint32_t& msgCount, uint32_t& errCount) const {
        msgCount = m_messageCount;
        errCount = m_errorCount;
    }
    
    void resetStats() {
        m_messageCount = 0;
        m_errorCount = 0;
    }
    
    void printDebug() const {
        Serial.printf("[%s] Connected: %s, Age: %lu ms, Msgs: %lu, Errors: %lu\n",
                     getName(),
                     isConnected() ? "YES" : "NO",
                     getDataAge(),
                     m_messageCount,
                     m_errorCount);
    }
};

#endif // PROTOCOL_BASE_CAN_H
