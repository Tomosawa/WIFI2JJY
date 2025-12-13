/*
 * Copyright (c) 2025 Tomosawa
 * All rights reserved.
 *
 * This code is part of the WiFi2JJY project.
 * GitHub: https://github.com/Tomosawa/WIFI2JJY
 */
#include "WebService.h"
#include <WebServer.h>

WebService::WebService(WiFiManager* wifiManager) : m_wifiManager(wifiManager), m_running(false) {
    m_server = new WebServer(m_port);
    m_wifiConfigPage = new WiFiConfigPage();
}

WebService::~WebService() {
    if (m_running) {
        m_server->stop();
        m_running = false;
    }
    delete m_server;
    delete m_wifiConfigPage;
}

void WebService::begin() {
    if (m_running) {
        return;
    }
    
    // 设置路由处理函数
    m_server->on("/", [this]() { handleRoot(); });
    m_server->on("/scan", HTTP_GET, [this]() { handleScan(); });
    m_server->on("/connect", HTTP_POST, [this]() { handleConnect(); });
    m_server->on("/status", HTTP_GET, [this]() { handleStatus(); });
    m_server->on("/reset", HTTP_POST, [this]() { handleReset(); });
    m_server->on("/reboot", HTTP_POST, [this]() { handleReboot(); });
    
    // 404处理
    m_server->onNotFound([this]() { handleNotFound(); });
    
    // 启动服务器
    m_server->begin();
    m_running = true;
    
    Serial.printf("[WebService] Server started on port %d\n", m_port);
}

void WebService::handleClient() {
    if (m_running) {
        m_server->handleClient();
    }
}

bool WebService::isRunning() {
    return m_running;
}

void WebService::handleRoot() {
    m_wifiConfigPage->sendPage(&m_server->client());
}

void WebService::handleScan() {
    // 获取扫描结果
    int networkCount = m_wifiManager->getScannedNetwork();

    if (networkCount == -1) {
        sendResponse(202, "application/json", "{\"status\":\"scanning\", \"message\":\"WiFi scan in progress\"}");
        return;
    }

    String json = "{\"status\":\"success\",\"networks\":[";
    
    bool first = true;
    for (int i = 0; i < networkCount; i++) {
        String ssid = m_wifiManager->getScannedSSID(i);
        if (ssid.length() == 0) continue; // 跳过无效的网络
        
        if (!first) json += ",";
        first = false;
        
        json += "{";
        json += "\"ssid\":\"" + escapeJSON(ssid) + "\",";
        json += "\"rssi\":" + String(m_wifiManager->getScannedRSSI(i)) + ",";
        json += "\"encryption\":\"" + m_wifiManager->getScannedEncryptionType(i) + "\"";
        json += "}";
    }
    
    json += "],\"count\":" + String(networkCount) + "}";
    sendResponse(200, "application/json", json);
}

void WebService::handleConnect() {
    String ssid = m_server->arg("ssid");
    String password = m_server->arg("password");
    Serial.println("ssid = " + ssid);
    Serial.println("password = " + password);
    if (ssid.length() == 0) {
        sendResponse(400, "application/json", "{\"status\":\"error\",\"message\":\"SSID is required\"}");
        return;
    }
    
    Serial.printf("[WebService] Connecting to WiFi (async): %s\n", ssid.c_str());
    m_wifiManager->startAsyncConnect(ssid, password);
    
    // 立即返回，前端可轮询 /status
    sendResponse(202, "application/json", "{\"status\":\"pending\",\"message\":\"Connecting...\"}");
}

void WebService::handleStatus() {
    String json = getStatusJSON();
    sendResponse(200, "application/json", json);
}

void WebService::handleReset() {
    m_wifiManager->clearWiFiConfig();
    sendResponse(200, "application/json", 
        "{\"status\":\"success\",\"message\":\"WiFi configuration reset\"}");
}

void WebService::handleReboot() {
    sendResponse(200, "application/json", "{\"status\":\"success\",\"message\":\"Rebooting\"}");
    Serial.println("[WebService] Reboot requested via /reboot");
    delay(200);
    esp_restart();
}

void WebService::handleNotFound() {
    sendResponse(404, "text/plain", "Not Found");
}

String WebService::getContentType(const String& path) {
    if (path.endsWith(".html")) return "text/html";
    else if (path.endsWith(".css")) return "text/css";
    else if (path.endsWith(".js")) return "application/javascript";
    else if (path.endsWith(".png")) return "image/png";
    else if (path.endsWith(".jpg")) return "image/jpeg";
    else if (path.endsWith(".gif")) return "image/gif";
    else if (path.endsWith(".ico")) return "image/x-icon";
    return "text/plain";
}

void WebService::sendResponse(int code, const String& type, const String& content) {
    m_server->sendHeader("Access-Control-Allow-Origin", "*");
    m_server->sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    m_server->sendHeader("Access-Control-Allow-Headers", "Content-Type");
    m_server->send(code, type, content);
}

String WebService::getNetworkListJSON() {
    int networkCount = m_wifiManager->getScannedNetwork();
    if(networkCount == 0) return "";
    String json = "[";

    for (int i = 0; i < networkCount; i++) {
        String ssid = m_wifiManager->getScannedSSID(i);
        if (ssid.length() == 0) continue; // Skip invalid networks
        
        if (i > 0) json += ",";
        
        json += "{";
        json += "\"ssid\":\"" + escapeJSON(ssid) + "\",";
        json += "\"rssi\":" + String(m_wifiManager->getScannedRSSI(i)) + ",";
        json += "\"encryption\":\"" + m_wifiManager->getScannedEncryptionType(i) + "\"";
        json += "}";
    }
    
    json += "]";
    return json;
}

String WebService::getStatusJSON() {
    String json = "{";
    json += "\"connected\":" + String(m_wifiManager->isConnected() ? "true" : "false") + ",";
    json += "\"status\":\"" + m_wifiManager->getConnectionStatus() + "\",";
    json += "\"ip\":\"" + m_wifiManager->getLocalIP() + "\",";
    json += "\"ssid\":\"" + m_wifiManager->getSavedSSID() + "\",";
    json += "\"saved\":{";
    json += "\"ssid\":\"" + m_wifiManager->getSavedSSID() + "\",";
    json += "\"password\":\"" + m_wifiManager->getSavedPassword() + "\"";
    json += "}";
    json += "}";
    return json;
}

String WebService::escapeJSON(const String& input) {
    String output;
    output.reserve(input.length() * 1.1); // Reserve some extra space for escape characters
    
    for (size_t i = 0; i < input.length(); i++) {
        char c = input.charAt(i);
        switch (c) {
            case '"': output += "\\\""; break;
            case '\\': output += "\\\\"; break;
            case '\b': output += "\\b"; break;
            case '\f': output += "\\f"; break;
            case '\n': output += "\\n"; break;
            case '\r': output += "\\r"; break;
            case '\t': output += "\\t"; break;
            default:
                if (c >= ' ' && c <= '~') {
                    // Printable ASCII character
                    output += c;
                } else {
                    // Non-printable or non-ASCII character, escape as \uXXXX
                    char buf[7];
                    snprintf(buf, sizeof(buf), "\\u%04x", c);
                    output += buf;
                }
        }
    }
    
    return output;
}