/*
 * Copyright (c) 2025 Tomosawa
 * All rights reserved.
 *
 * This code is part of the WiFi2JJY project.
 * GitHub: https://github.com/Tomosawa/WIFI2JJY
 */
#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>

class WiFiManager {
public:
    WiFiManager();
    ~WiFiManager();
    
    // 初始化WiFi管理器
    void begin();
    
    // 检查WiFi连接状态
    bool isConnected();
    
    // 尝试连接到保存的WiFi网络
    bool connectToSavedNetwork();
    
    // 连接到指定的WiFi网络
    bool connectToNetwork(const String& ssid, const String& password);
    
    // 启动AP模式
    void startAPMode();
    
    // 获取本地IP地址
    String getLocalIP();
    
    // 保存WiFi配置
    bool saveWiFiConfig(const String& ssid, const String& password);
    bool saveWiFiConfig();
    
    // 清除保存的WiFi配置
    void clearWiFiConfig();
    
    // 获取已保存的SSID
    String getSavedSSID();
    // 获取已保存的密码
    String getSavedPassword();
    
    // 检查是否有保存的WiFi配置
    bool hasSavedConfig();
    
    // 开始异步扫描网络
    void startScan();
    
    // 获取扫描到的网络数量
    int getScannedNetwork();
    
    // 获取扫描到的网络SSID
    String getScannedSSID(int index) const;
    
    // 获取扫描到的网络信号强度
    int getScannedRSSI(int index) const;
    
    // 获取扫描到的网络加密类型
    String getScannedEncryptionType(int index) const;
    
    // 处理WiFi连接状态
    void handleConnection();
    
    // 获取连接状态描述
    String getConnectionStatus();
    
    // 检查是否处于AP模式
    bool isAPMode() const { return m_apMode; }
    
    // 异步发起连接（不阻塞API）
    void startAsyncConnect(const String& ssid, const String& password);

private:
    Preferences m_preferences;
    bool m_apMode;
    bool m_stationMode;
    bool m_connecting;
    int m_scanResultCount;
    unsigned long m_lastConnectionAttempt;
    int m_connectionAttempts;
    const int MAX_CONNECTION_ATTEMPTS = 10;
    const unsigned long CONNECTION_TIMEOUT = 10000; // 10秒
    
    // AP模式配置
    const char* m_apSSID = "MYNOVA_CLOCK";
    const char* m_apPassword = "MYNOVA123";
    
    // 配置存储键名
    const char* WIFI_SSID_KEY = "wifi_ssid";
    const char* WIFI_PASSWORD_KEY = "wifi_password";
    
    // 内部方法
    bool attemptConnection(const String& ssid, const String& password);
    
    // 异步连接状态
    String m_targetSSID;
    String m_targetPassword;
    unsigned long m_connectStartTime = 0;
};

#endif // WIFIMANAGER_H