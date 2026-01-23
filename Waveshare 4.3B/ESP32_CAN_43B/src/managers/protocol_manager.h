/**
 * @file protocol_manager.h
 * @brief Protocol Manager mit Auto-Detection
 * @date 2025
 * 
 * SPEICHERN ALS: src/managers/protocol_manager.h
 */

#ifndef PROTOCOL_MANAGER_H
#define PROTOCOL_MANAGER_H

#include "../protocols/protocol_base_can.h"
#include <vector>

class ProtocolManager {
private:
    struct DetectionStats {
        CanProtocolBase* protocol;
        uint32_t matchCount;
        uint32_t lastMatch;
        
        DetectionStats() : protocol(nullptr), matchCount(0), lastMatch(0) {}
    };
    
    std::vector<CanProtocolBase*> m_protocols;
    std::vector<DetectionStats> m_detectionStats;
    CanProtocolBase* m_activeProtocol;
    bool m_autoDetect;
    
    static constexpr uint32_t DETECTION_THRESHOLD = 5;

public:
    ProtocolManager() 
        : m_activeProtocol(nullptr)
        , m_autoDetect(true)
    {
        Serial.println("[ProtocolMgr] Initialized");
    }
    
    ~ProtocolManager() {
        m_protocols.clear();
        m_detectionStats.clear();
    }
    
    void registerProtocol(CanProtocolBase* protocol) {
        if (!protocol) {
            Serial.println("[ProtocolMgr] ERROR: Null protocol!");
            return;
        }
        
        m_protocols.push_back(protocol);
        
        DetectionStats stats;
        stats.protocol = protocol;
        stats.matchCount = 0;
        stats.lastMatch = 0;
        m_detectionStats.push_back(stats);
        
        Serial.printf("[ProtocolMgr] Registered: %s\n", protocol->getName());
    }
    
    bool initializeAll() {
        Serial.println("[ProtocolMgr] Initializing all protocols...");
        
        bool success = true;
        for (auto* protocol : m_protocols) {
            if (!protocol->initialize()) {
                Serial.printf("[ProtocolMgr] ERROR: Failed to initialize: %s\n", 
                            protocol->getName());
                success = false;
            }
        }
        
        Serial.printf("[ProtocolMgr] Initialization %s\n", 
                     success ? "successful" : "FAILED");
        return success;
    }
    
    bool startAll() {
        Serial.println("[ProtocolMgr] Starting all protocols...");
        
        bool success = true;
        for (auto* protocol : m_protocols) {
            if (!protocol->start()) {
                Serial.printf("[ProtocolMgr] ERROR: Failed to start: %s\n", 
                            protocol->getName());
                success = false;
            }
        }
        
        Serial.printf("[ProtocolMgr] Start %s\n", 
                     success ? "successful" : "FAILED");
        return success;
    }
    
    bool stopAll() {
        Serial.println("[ProtocolMgr] Stopping all protocols...");
        
        bool success = true;
        for (auto* protocol : m_protocols) {
            if (!protocol->stop()) {
                Serial.printf("[ProtocolMgr] ERROR: Failed to stop: %s\n", 
                            protocol->getName());
                success = false;
            }
        }
        
        m_activeProtocol = nullptr;
        return success;
    }
    
    void setAutoDetect(bool enable) {
        m_autoDetect = enable;
        if (enable) {
            m_activeProtocol = nullptr;
            Serial.println("[ProtocolMgr] Auto-detection ENABLED");
        } else {
            Serial.println("[ProtocolMgr] Auto-detection DISABLED");
        }
    }
    
    bool isAutoDetectEnabled() const {
        return m_autoDetect;
    }
    
    bool selectProtocol(bms_type_t type) {
        for (auto* protocol : m_protocols) {
            if (protocol->getType() == type) {
                m_activeProtocol = protocol;
                m_autoDetect = false;
                Serial.printf("[ProtocolMgr] Manually selected: %s\n", 
                            protocol->getName());
                return true;
            }
        }
        
        Serial.printf("[ProtocolMgr] ERROR: Protocol type %d not found!\n", type);
        return false;
    }
    
    bool routeMessage(uint32_t canId, const uint8_t* data, uint8_t length) {
        // Wenn ein Protokoll aktiv ist und Auto-Detect aus
        if (m_activeProtocol && !m_autoDetect) {
            if (m_activeProtocol->canAcceptMessage(canId)) {
                return m_activeProtocol->parseMessage(canId, data, length);
            }
            return false;
        }
        
        // Auto-Detection
        for (size_t i = 0; i < m_protocols.size(); i++) {
            auto* protocol = m_protocols[i];
            
            if (protocol->canAcceptMessage(canId)) {
                bool success = protocol->parseMessage(canId, data, length);
                
                if (success) {
                    m_detectionStats[i].matchCount++;
                    m_detectionStats[i].lastMatch = millis();
                    
                    if (m_detectionStats[i].matchCount >= DETECTION_THRESHOLD && !m_activeProtocol) {
                        m_activeProtocol = protocol;
                        Serial.printf("\n*** [ProtocolMgr] AUTO-DETECTED: %s ***\n\n", 
                                    protocol->getName());
                    }
                    
                    return true;
                }
            }
        }
        
        return false;
    }
    
    CanProtocolBase* getActiveProtocol() const {
        return m_activeProtocol;
    }
    
    bool isConnected() const {
        if (m_activeProtocol) {
            return m_activeProtocol->isConnected();
        }
        
        for (auto* protocol : m_protocols) {
            if (protocol->isConnected()) {
                return true;
            }
        }
        
        return false;
    }
    
    bool getData(bms_data_t& data) const {
        if (m_activeProtocol) {
            return m_activeProtocol->getData(data);
        }
        
        for (auto* protocol : m_protocols) {
            if (protocol->isConnected() && protocol->getData(data)) {
                return true;
            }
        }
        
        return false;
    }
    
    size_t getProtocolCount() const {
        return m_protocols.size();
    }
    
    void printDetectionStats() const {
        Serial.println("\n=== Protocol Detection Stats ===");
        
        for (const auto& stats : m_detectionStats) {
            uint32_t age = (stats.lastMatch > 0) ? (millis() - stats.lastMatch) : 0;
            
            Serial.printf("%-20s: %4lu matches, last: %5lu ms ago %s\n",
                         stats.protocol->getName(),
                         stats.matchCount,
                         age,
                         (stats.protocol == m_activeProtocol) ? "[ACTIVE]" : "");
        }
        
        Serial.println("================================\n");
    }
    
    void printProtocolInfo() const {
        Serial.println("\n=== Registered Protocols ===");
        
        for (const auto* protocol : m_protocols) {
            Serial.printf("%-20s: %s %s\n",
                         protocol->getName(),
                         protocol->isConnected() ? "CONNECTED" : "disconnected",
                         (protocol == m_activeProtocol) ? "[ACTIVE]" : "");
            
            uint32_t msgCount, errCount;
            protocol->getStats(msgCount, errCount);
            Serial.printf("  Messages: %lu, Errors: %lu, Age: %lu ms\n",
                         msgCount, errCount, protocol->getDataAge());
        }
        
        Serial.println("============================\n");
    }
    
    void resetStats() {
        Serial.println("[ProtocolMgr] Resetting all statistics...");
        
        for (auto& stats : m_detectionStats) {
            stats.matchCount = 0;
            stats.lastMatch = 0;
        }
        
        for (auto* protocol : m_protocols) {
            protocol->resetStats();
        }
        
        m_activeProtocol = nullptr;
        Serial.println("[ProtocolMgr] Statistics reset complete");
    }
};

#endif // PROTOCOL_MANAGER_H
