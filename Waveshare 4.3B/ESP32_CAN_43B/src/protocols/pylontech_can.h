/**
 * @file pylontech_can.h
 * @brief Pylontech BMS CAN-Protokoll
 * @date 2025
 * 
 * SPEICHERN ALS: src/protocols/pylontech_can.h
 */

#ifndef PYLONTECH_CAN_H
#define PYLONTECH_CAN_H

#include "protocol_base_can.h"

class PylontechCan : public CanProtocolBase {
private:
    static constexpr uint32_t ID_VOLTAGE = 0x359;
    static constexpr uint32_t ID_CURRENT = 0x35C;
    static constexpr uint32_t ID_SOC     = 0x355;
    static constexpr uint32_t ID_TEMP    = 0x356;
    static constexpr uint32_t ID_STATUS  = 0x35E;
    static constexpr uint32_t ID_ALARM   = 0x35A;
    
    bool m_voltageReceived;
    bool m_currentReceived;
    bool m_socReceived;
    
public:
    PylontechCan() 
        : CanProtocolBase()
        , m_voltageReceived(false)
        , m_currentReceived(false)
        , m_socReceived(false)
    {}
    
    const char* getName() const override { 
        return "Pylontech CAN"; 
    }
    
    bms_type_t getType() const override { 
        return BMS_PYLONTECH; 
    }
    
    bool canAcceptMessage(uint32_t canId) const override {
        return (canId == ID_VOLTAGE || 
                canId == ID_CURRENT || 
                canId == ID_SOC || 
                canId == ID_TEMP ||
                canId == ID_STATUS ||
                canId == ID_ALARM);
    }
    
    bool parseMessage(uint32_t canId, const uint8_t* data, uint8_t length) override {
        if (length < 8) {
            markError();
            return false;
        }
        
        bool parsed = false;
        
        switch (canId) {
            case ID_VOLTAGE: {
                uint16_t rawVoltage = extractUint16(data, 0, true);
                m_data.voltage = rawVoltage * 0.01f;
                
                if (validateRange(m_data.voltage, 40.0f, 60.0f)) {
                    m_voltageReceived = true;
                    parsed = true;
                    Serial.printf("[Pylontech] Voltage: %.2f V\n", m_data.voltage);
                }
                break;
            }
            
            case ID_CURRENT: {
                int16_t rawCurrent = extractInt16(data, 0, true);
                m_data.current = rawCurrent * 0.1f;
                
                m_data.charging = (m_data.current > 0.5f);
                m_data.discharging = (m_data.current < -0.5f);
                
                m_currentReceived = true;
                parsed = true;
                
                const char* status = m_data.charging ? "Charging" : 
                                    (m_data.discharging ? "Discharging" : "Idle");
                Serial.printf("[Pylontech] Current: %.1f A (%s)\n", m_data.current, status);
                break;
            }
            
            case ID_SOC: {
                uint16_t rawSoc = extractUint16(data, 0, true);
                m_data.soc = rawSoc * 0.1f;
                
                if (validateRange(m_data.soc, 0.0f, 100.0f)) {
                    m_socReceived = true;
                    parsed = true;
                    Serial.printf("[Pylontech] SOC: %.1f %%\n", m_data.soc);
                }
                break;
            }
            
            case ID_TEMP: {
                int16_t rawTemp = extractInt16(data, 0, true);
                m_data.temperature = rawTemp * 0.1f;
                
                if (validateRange(m_data.temperature, -20.0f, 60.0f)) {
                    parsed = true;
                    Serial.printf("[Pylontech] Temperature: %.1f °C\n", m_data.temperature);
                }
                break;
            }
            
            case ID_STATUS: {
                m_data.cycles = extractUint16(data, 0, true);
                snprintf(m_data.status_text, sizeof(m_data.status_text), 
                         "Online - %u Zyklen", m_data.cycles);
                parsed = true;
                Serial.printf("[Pylontech] Cycles: %u\n", m_data.cycles);
                break;
            }
            
            case ID_ALARM: {
                uint8_t alarmByte = data[0];
                
                if (alarmByte != 0) {
                    snprintf(m_data.status_text, sizeof(m_data.status_text), 
                             "ALARM 0x%02X", alarmByte);
                    Serial.printf("[Pylontech] ALARM: 0x%02X\n", alarmByte);
                } else {
                    snprintf(m_data.status_text, sizeof(m_data.status_text), 
                             "Online");
                }
                
                parsed = true;
                break;
            }
            
            default:
                return false;
        }
        
        if (parsed) {
            markUpdated();
        } else {
            markError();
        }
        
        return parsed;
    }
    
    bool hasCompleteData() const {
        return m_voltageReceived && m_currentReceived && m_socReceived;
    }
    
    void resetReceivedFlags() {
        m_voltageReceived = false;
        m_currentReceived = false;
        m_socReceived = false;
    }
};

#endif // PYLONTECH_CAN_H
