/*
 * Copyright (c) 2025 Tomosawa
 * All rights reserved.
 *
 * This code is part of the WiFi2JJY project.
 * GitHub: https://github.com/Tomosawa/WIFI2JJY
 */
#ifndef WEBSERVICE_H
#define WEBSERVICE_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "WiFiManager.h"
#include "WiFiConfigPage.h"


class WebService {
public:
    WebService(WiFiManager* wifiManager);
    ~WebService();
    
    // 初始化Web服务器
    void begin();
    
    // 处理客户端请求
    void handleClient();
    
    // 检查服务器是否正在运行
    bool isRunning();

private:  
    
    WiFiManager* m_wifiManager;
    WebServer* m_server;
    WiFiConfigPage* m_wifiConfigPage;
    bool m_running;
    const int m_port = 80;
    
    // 路由处理函数
    void handleRoot();
    void handleScan();
    void handleConnect();
    void handleStatus();
    void handleReset();
    void handleReboot();
    void handleNotFound();
    
    // 辅助函数
    String getContentType(const String& path);
    void sendResponse(int code, const String& type, const String& content);
    String getNetworkListJSON();
    String getStatusJSON();
    String escapeJSON(const String& input);
    
    // HTML页面生成
    String generateHTMLPage();
    String generateWiFiList();
    String generateConnectionStatus();
};

#endif // WEBSERVICE_H