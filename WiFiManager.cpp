/*
 * Copyright (c) 2025 Tomosawa
 * All rights reserved.
 *
 * This code is part of the WiFi2JJY project.
 * GitHub: https://github.com/Tomosawa/WIFI2JJY
 */
#include "WiFiManager.h"

WiFiManager::WiFiManager() : m_apMode(false), m_stationMode(false), m_scanResultCount(0), m_lastConnectionAttempt(0), m_connectionAttempts(0) {
}

WiFiManager::~WiFiManager() {
}

void WiFiManager::begin() {
    m_preferences.begin("wifi-config", false);
    m_connecting = false;
    Serial.println("[WiFiManager] Initialized");
    // 确保在AP_STA模式下
    WiFi.mode(WIFI_AP_STA);
    delay(200);
    // 无论何时都需要启动AP
    startAPMode();
    delay(100);
   
    // 检查是否有保存的WiFi配置，有则连接
    if (hasSavedConfig()) {
        Serial.println("[WiFiManager] Found saved WiFi configuration");
        connectToSavedNetwork();
    } else {
        Serial.println("[WiFiManager] No saved WiFi configuration found");
    }

     // 开始扫描网络
    startScan();
    // 等待扫描完成
    while (WiFi.scanComplete() == -1) {
        delay(100);
    }
    Serial.println("[WiFiManager] WIFI Scan complete");
}

bool WiFiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

bool WiFiManager::connectToSavedNetwork() {
    String ssid = getSavedSSID();
    String password = m_preferences.getString(WIFI_PASSWORD_KEY, "");
    
    if (ssid.length() == 0) {
        Serial.println("[WiFiManager] No saved SSID found");
        return false;
    }
    
    Serial.printf("[WiFiManager] Attempting to connect to saved network: %s\n", ssid.c_str());
    return connectToNetwork(ssid, password);
}

bool WiFiManager::connectToNetwork(const String& ssid, const String& password) {
    
    Serial.printf("[WiFiManager] Connecting to SSID: %s\n", ssid.c_str());
    WiFi.begin(ssid.c_str(), password.c_str());
    
    m_connectionAttempts = 0;
    m_lastConnectionAttempt = millis();
    
    // 等待连接
    while (WiFi.status() != WL_CONNECTED && m_connectionAttempts < MAX_CONNECTION_ATTEMPTS) {
        delay(500);
        m_connectionAttempts++;
        
        if (m_connectionAttempts % 2 == 0) {
            Serial.print(".");
        }
        
        // 检查超时
        if (millis() - m_lastConnectionAttempt > CONNECTION_TIMEOUT) {
            Serial.println("\n[WiFiManager] Connection timeout");
            return false;
        }
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[WiFiManager] Connected successfully!");
        Serial.printf("[WiFiManager] IP address: %s\n", WiFi.localIP().toString().c_str());
        
        // 保存成功连接的配置
        saveWiFiConfig(ssid, password);
        return true;
    } else {
        Serial.println("\n[WiFiManager] Failed to connect");
        return false;
    }
}

void WiFiManager::startAPMode() {
    Serial.printf("[WiFiManager] Starting AP mode with SSID: %s\n", m_apSSID);
    WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
    WiFi.softAP(m_apSSID, m_apPassword);
    
    IPAddress apIP = WiFi.softAPIP();
    Serial.printf("[WiFiManager] AP started, IP address: %s\n", apIP.toString().c_str());
    
    m_apMode = true;
}

String WiFiManager::getLocalIP() {
    if (isConnected()) {
        // 只有在真正连接到WiFi网络时才返回IP地址
        IPAddress ip = WiFi.localIP();
        if (ip != IPAddress(0, 0, 0, 0)) {
            return ip.toString();
        }
    }
    // 未连接或IP地址无效时返回空字符串
    return "";
}

bool WiFiManager::saveWiFiConfig() {
    return saveWiFiConfig(m_targetSSID, m_targetPassword);
}

bool WiFiManager::saveWiFiConfig(const String& ssid, const String& password) {
    m_preferences.putString(WIFI_SSID_KEY, ssid);
    m_preferences.putString(WIFI_PASSWORD_KEY, password);
    
    Serial.printf("[WiFiManager] WiFi configuration saved: %s\n", ssid.c_str());
    return true;
}

void WiFiManager::clearWiFiConfig() {
    m_preferences.remove(WIFI_SSID_KEY);
    m_preferences.remove(WIFI_PASSWORD_KEY);
    Serial.println("[WiFiManager] WiFi configuration cleared");
}

String WiFiManager::getSavedSSID() {
    return m_preferences.getString(WIFI_SSID_KEY, "");
}

String WiFiManager::getSavedPassword() {
    return m_preferences.getString(WIFI_PASSWORD_KEY, "");
}

bool WiFiManager::hasSavedConfig() {
    String ssid = getSavedSSID();
    return ssid.length() > 0;
}

void WiFiManager::startScan() {
    WiFi.scanNetworks(true, true); // 开始异步扫描
    Serial.println("[WiFiManager] Starting WiFi scan...");
}

int WiFiManager::getScannedNetwork() {
    int8_t scanStatus = WiFi.scanComplete();
    if (scanStatus >= 0) {
        // 扫描完成
        m_scanResultCount = scanStatus;
        Serial.printf("[WiFiManager] Scan completed, found %d networks\n", m_scanResultCount);
    } else if (scanStatus == -1) {
        // 扫描仍在进行中
        Serial.println("[WiFiManager] Scanning...");
        m_scanResultCount = 0;
    }
    return scanStatus;
}


String WiFiManager::getScannedSSID(int index) const {
    if (index >= 0 && index < m_scanResultCount) {
        return WiFi.SSID(index);
    }
    return "";
}

int WiFiManager::getScannedRSSI(int index) const {
    if (index >= 0 && index < m_scanResultCount) {
        return WiFi.RSSI(index);
    }
    return 0;
}

String WiFiManager::getScannedEncryptionType(int index) const {
    switch (WiFi.encryptionType(index)) {
        case WIFI_AUTH_OPEN:
            return "Open";
        case WIFI_AUTH_WEP:
            return "WEP";
        case WIFI_AUTH_WPA_PSK:
            return "WPA";
        case WIFI_AUTH_WPA2_PSK:
            return "WPA2";
        case WIFI_AUTH_WPA_WPA2_PSK:
            return "WPA/WPA2";
        case WIFI_AUTH_WPA2_ENTERPRISE:
            return "WPA2 Enterprise";
        case WIFI_AUTH_WPA3_PSK:
            return "WPA3";
        case WIFI_AUTH_WPA2_WPA3_PSK:
            return "WPA2/WPA3";
        default:
            return "Unknown";
    }
}

void WiFiManager::handleConnection() {
    // 优先处理正在进行的异步连接
    if (m_connecting) {
        wl_status_t st = WiFi.status();
        if (st == WL_CONNECTED) {
            m_connecting = false;
            Serial.println("[WiFiManager] Async connected!");
            saveWiFiConfig(m_targetSSID, m_targetPassword);
        } else if (millis() - m_connectStartTime > CONNECTION_TIMEOUT) {
            m_connecting = false;
            Serial.println("[WiFiManager] Async connect timeout");
        }
        return;
    }

    // 如果已连接，无需处理
    if (isConnected()) {
        return;
    }
}

String WiFiManager::getConnectionStatus() {
    switch (WiFi.status()) {
        case WL_IDLE_STATUS:
            return "Idle";
        case WL_NO_SSID_AVAIL:
            return "No SSID Available";
        case WL_SCAN_COMPLETED:
            return "Scan Completed";
        case WL_CONNECTED:
            return "Connected";
        case WL_CONNECT_FAILED:
            return "Connection Failed";
        case WL_CONNECTION_LOST:
            return "Connection Lost";
        case WL_DISCONNECTED:
            return "Disconnected";
        default:
            return "Unknown";
    }
}


bool WiFiManager::attemptConnection(const String& ssid, const String& password) {
    WiFi.begin(ssid.c_str(), password.c_str());
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        attempts++;
    }
    
    return WiFi.status() == WL_CONNECTED;
}

void WiFiManager::startAsyncConnect(const String& ssid, const String& password) {
    m_targetSSID = ssid;
    m_targetPassword = password;
    m_connectStartTime = millis();
    m_lastConnectionAttempt = m_connectStartTime;
    m_connecting = true;
    Serial.printf("[WiFiManager] Async connect start: %s\n", ssid.c_str());
    WiFi.begin(ssid.c_str(), password.c_str());
}