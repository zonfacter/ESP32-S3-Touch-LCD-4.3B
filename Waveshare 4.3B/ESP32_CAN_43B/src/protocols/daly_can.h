/**
 * @file daly_can.h
 * @brief DALY BMS CAN-Protokoll
 * @date 2025
 * 
 * SPEICHERN ALS: src/protocols/daly_can.h
 */

#ifndef DALY_CAN_H
#define DALY_CAN_H

#include "protocol_base_can.h"

class DalyCan : public CanProtocolBase {
private:
    static constexpr uint32_t ID_VOLTAGE = 0x18FF50E5;
    static constexpr uint32_t ID_CURRENT = 0x18FF51E5;
    static constexpr uint32_t ID_SOC     = 0x18FF52E5;
    static constexpr uint32_t ID_TEMP    = 0x18FF53E5;
    static constexpr uint32_t ID_STATUS  = 0x18FF54E5;
    static constexpr uint32_t ID_CELLS   = 0x18FF55E5;
    
public:
    DalyCan() : CanProtocolBase() {}
    
    const char* getName() const override { 
        return "DALY BMS CAN"; 
    }
    
    bms_type_t getType() const override { 
        return BMS_DALY; 
    }
    
    bool canAcceptMessage(uint32_t canId) const override {
        return (canId == ID_VOLTAGE || 
                canId == ID_CURRENT || 
                canId == ID_SOC || 
                canId == ID_TEMP ||
                canId == ID_STATUS ||
                canId == ID_CELLS);
    }
    
    bool parseMessage(uint32_t canId, const uint8_t* data, uint8_t length) override {
        if (length < 8) {
            markError();
            return false;
        }
        
        bool parsed = false;
        
        switch (canId) {
            case ID_VOLTAGE: {
                uint16_t voltageRaw = extractUint16(data, 0, false);  // Little-Endian!
                m_data.voltage = voltageRaw * 0.1f;
                
                if (validateRange(m_data.voltage, 40.0f, 60.0f)) {
                    parsed = true;
                    Serial.printf("[DALY] Voltage: %.2f V\n", m_data.voltage);
                }
                break;
            }
            
            case ID_CURRENT: {
                int16_t currentRaw = extractInt16(data, 0, false);  // Little-Endian!
                m_data.current = currentRaw * 0.1f;
                
                m_data.charging = (m_data.current > 0.5f);
                m_data.discharging = (m_data.current < -0.5f);
                
                parsed = true;
                Serial.printf("[DALY] Current: %.1f A\n", m_data.current);
                break;
            }
            
            case ID_SOC: {
                uint16_t socRaw = extractUint16(data, 0, false);  // Little-Endian!
                m_data.soc = socRaw * 0.1f;
                
                if (validateRange(m_data.soc, 0.0f, 100.0f)) {
                    parsed = true;
                    Serial.printf("[DALY] SOC: %.1f %%\n", m_data.soc);
                }
                break;
            }
            
            case ID_TEMP: {
                int16_t tempRaw = extractInt16(data, 0, false);  // Little-Endian!
                m_data.temperature = tempRaw * 0.1f;
                
                if (validateRange(m_data.temperature, -20.0f, 60.0f)) {
                    parsed = true;
                    Serial.printf("[DALY] Temperature: %.1f °C\n", m_data.temperature);
                }
                break;
            }
            
            case ID_STATUS: {
                uint8_t statusFlags = data[0];
                uint8_t alarmFlags = data[1];
                m_data.cycles = extractUint16(data, 4, false);  // Little-Endian!
                
                if (alarmFlags != 0) {
                    snprintf(m_data.status_text, sizeof(m_data.status_text),
                            "ALARM 0x%02X - %u Zyklen", alarmFlags, m_data.cycles);
                } else {
                    snprintf(m_data.status_text, sizeof(m_data.status_text),
                            "Online - %u Zyklen", m_data.cycles);
                }
                
                parsed = true;
                Serial.printf("[DALY] Status: 0x%02X, Alarm: 0x%02X, Cycles: %u\n",
                            statusFlags, alarmFlags, m_data.cycles);
                break;
            }
            
            case ID_CELLS: {
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
};

#endif // DALY_CAN_H
