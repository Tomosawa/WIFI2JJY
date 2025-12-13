/*
 * Copyright (c) 2025 Tomosawa
 * All rights reserved.
 *
 * This code is part of the WiFi2JJY project.
 * GitHub: https://github.com/Tomosawa/WIFI2JJY
 */
#ifndef WIFICONFIGPAGE_H
#define WIFICONFIGPAGE_H

#include <Arduino.h>
#include <pgmspace.h>
#include <WiFi.h>
#include <WebServer.h>

class WiFiConfigPage {
public:
    // 生成完整的HTML页面
    static void sendPage(WiFiClient* client);
    
private:
    // 发送HTML头部
    static void sendHeader(WiFiClient* client);
    
    // 发送HTML主体
    static void sendBody(WiFiClient* client);
    
    // 发送HTML尾部
    static void sendFooter(WiFiClient* client);
    
    // 发送CSS样式
    static void sendCSS(WiFiClient* client);
    
    // 发送JavaScript代码
    static void sendJavaScript(WiFiClient* client);
    
    // 辅助函数：从PROGMEM发送字符串
    static void sendProgmemString(WiFiClient* client, const char* progmemStr);
};

#endif // WIFICONFIGPAGE_H