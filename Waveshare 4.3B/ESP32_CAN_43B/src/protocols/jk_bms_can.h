/**
 * @file jk_bms_can.h
 * @brief JK BMS CAN-Protokoll
 * @date 2025
 * 
 * SPEICHERN ALS: src/protocols/jk_bms_can.h
 */

#ifndef JK_BMS_CAN_H
#define JK_BMS_CAN_H

#include "protocol_base_can.h"

class JkBmsCan : public CanProtocolBase {
private:
    static constexpr uint32_t ID_BASE = 0x02F4DA00;
    static constexpr uint32_t ID_MASK = 0xFFFFFF00;
    
    static constexpr uint8_t MSG_VOLTAGE = 0x01;
    static constexpr uint8_t MSG_CURRENT = 0x02;
    static constexpr uint8_t MSG_SOC = 0x03;
    static constexpr uint8_t MSG_TEMP = 0x04;
    static constexpr uint8_t MSG_STATUS = 0x05;
    static constexpr uint8_t MSG_CELLS = 0x10;
    
    uint8_t getMessageType(uint32_t canId) const {
        return (uint8_t)(canId & 0xFF);
    }
    
public:
    JkBmsCan() : CanProtocolBase() {}
    
    const char* getName() const override { 
        return "JK BMS CAN"; 
    }
    
    bms_type_t getType() const override { 
        return BMS_JK_BMS; 
    }
    
    bool canAcceptMessage(uint32_t canId) const override {
        return (canId & ID_MASK) == ID_BASE;
    }
    
    bool parseMessage(uint32_t canId, const uint8_t* data, uint8_t length) override {
        if (length < 8) {
            markError();
            return false;
        }
        
        uint8_t msgType = getMessageType(canId);
        bool parsed = false;
        
        switch (msgType) {
            case MSG_VOLTAGE: {
                uint32_t voltageMillivolts = extractUint32(data, 0, true);
                m_data.voltage = voltageMillivolts / 1000.0f;
                
                if (validateRange(m_data.voltage, 40.0f, 60.0f)) {
                    parsed = true;
                    Serial.printf("[JK BMS] Voltage: %.2f V\n", m_data.voltage);
                }
                break;
            }
            
            case MSG_CURRENT: {
                int32_t currentMilliamps = extractInt32(data, 0, true);
                m_data.current = currentMilliamps / 1000.0f;
                
                m_data.charging = (m_data.current > 0.5f);
                m_data.discharging = (m_data.current < -0.5f);
                
                parsed = true;
                Serial.printf("[JK BMS] Current: %.1f A\n", m_data.current);
                break;
            }
            
            case MSG_SOC: {
                uint16_t socRaw = extractUint16(data, 0, true);
                m_data.soc = socRaw * 0.01f;
                
                if (validateRange(m_data.soc, 0.0f, 100.0f)) {
                    parsed = true;
                    Serial.printf("[JK BMS] SOC: %.1f %%\n", m_data.soc);
                }
                break;
            }
            
            case MSG_TEMP: {
                int16_t tempRaw = extractInt16(data, 0, true);
                m_data.temperature = tempRaw * 0.1f;
                
                if (validateRange(m_data.temperature, -20.0f, 60.0f)) {
                    parsed = true;
                    Serial.printf("[JK BMS] Temperature: %.1f °C\n", m_data.temperature);
                }
                break;
            }
            
            case MSG_STATUS: {
                uint8_t statusByte = data[0];
                m_data.cycles = extractUint16(data, 2, true);
                
                snprintf(m_data.status_text, sizeof(m_data.status_text),
                        "Online - Status: 0x%02X - %u Zyklen", 
                        statusByte, m_data.cycles);
                
                parsed = true;
                Serial.printf("[JK BMS] Status: 0x%02X, Cycles: %u\n", 
                            statusByte, m_data.cycles);
                break;
            }
            
            case MSG_CELLS: {
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

#endif // JK_BMS_CAN_H
