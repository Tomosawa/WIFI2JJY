/*
 * Copyright (c) 2025 Tomosawa
 * All rights reserved.
 *
 * This code is part of the WiFi2JJY project.
 * GitHub: https://github.com/Tomosawa/WIFI2JJY
 */
#include "WebService.h"
#include "JJYSender.h"
#include "TimeSync.h"
#include "WiFiManager.h"
#include "esp_system.h"
#include <Arduino.h>
#include <WiFi.h>
#include <driver/gpio.h>
#include <esp_sleep.h>
#include <time.h>
#include "IOPin.h"

// WiFi管理器和Web服务器
WiFiManager wifiManager;
WebService webService(&wifiManager);

// JJYSender对象
JJYSender jjySender(PIN_DA);

// TimeSync对象
TimeSync timeSync;

// 系统状态
bool wifiConnected = false;

void setup() {
  Serial.begin(115200);
  Serial.println("=== JJY Clock Initialization ===");

  pinMode(PIN_PON, INPUT);
  Serial.print("[Setup] PON pin configured as INPUT, current value: ");
  Serial.println(digitalRead(PIN_PON) ? "HIGH" : "LOW");

  // 初始化WiFi管理器
  Serial.println("[WiFi] Initializing WiFi Manager...");
  wifiManager.begin();
  // 启动Web服务器
  webService.begin();
  Serial.printf("[WebServer] Configuration server started at %s\n",
                wifiManager.getLocalIP().c_str());

  // 如果没有保存的配置，直接跳过等待，保持 AP 模式
  if (!wifiManager.hasSavedConfig()) {
    Serial.println("[WiFi] No saved WiFi config, stay in AP mode for setup.");
    Serial.println("=== Initialization Complete ===");
    return;
  }
  
  // 等待WiFi连接（仅在已有配置时尝试）
  Serial.println("[WiFi] Waiting for saved WiFi connection...");
  unsigned long wifiStartTime = millis();
  const unsigned long wifiTimeout = 30000; // 30秒超时

  while (!wifiManager.isConnected() &&
         (millis() - wifiStartTime < wifiTimeout)) {
    wifiManager.handleConnection();
    webService.handleClient();
    delay(100);

    // 每5秒打印一次状态
    if ((millis() - wifiStartTime) % 5000 < 100) {
      Serial.printf("[WiFi] Status: %s\n",
                    wifiManager.getConnectionStatus().c_str());
    }
  }

  if (wifiManager.isConnected()) {
    wifiConnected = true;
    Serial.println("[WiFi] Connected successfully!");
    Serial.printf("[WiFi] IP address: %s\n", wifiManager.getLocalIP().c_str());

    // 同步NTP时间
    timeSync.startNTPSyncTask();
  } else {
    Serial.println(
        "[WiFi] Connection timeout or failed, continuing in AP mode...");
  }

  Serial.println("=== Initialization Complete ===");
}

void loop() {
  // 处理Web服务器请求
  webService.handleClient();
  // 启动时WIFI未连接过，但现在已连接，保存配置
  if (!wifiConnected && wifiManager.isConnected()) {
    wifiConnected = true;
    Serial.println("[WiFi] WIFI Connected, Saving Configuration...");
    wifiManager.saveWiFiConfig();
  }

  // WIFI连接了，但是时间还没有同步，则尝试同步
  if (wifiConnected && wifiManager.isConnected() && !timeSync.timeSynced) {
    Serial.println("[WiFi] WIFI Connected, Time synchronization...");
    timeSync.startNTPSyncTask();
    delay(3000); // 等待3秒后重试
    return; // 退出当前循环，避免继续执行后续代码
  }

  // WiFi 已连接且时间同步后，启动高优先级任务发送JJY，不阻塞主循环
  if (wifiConnected && timeSync.timeSynced && jjySender.getTaskHandle() == nullptr && !jjySender.isAsyncDone()) {
    Serial.println("\n=== Starting JJY send task ===");
    jjySender.startAsyncSend(&timeSync);
  }

  // 如果发送任务完成，准备休眠
  if (jjySender.isAsyncDone()) {
    Serial.println("\n=== Exiting Main Loop ===");
    Serial.println("[System] Configuring deep sleep with PON pin wakeup...");
    esp_deep_sleep_enable_gpio_wakeup((1ULL << PIN_PON),
                                      ESP_GPIO_WAKEUP_GPIO_LOW);
    Serial.printf("[System] GPIO%d configured for GPIO wakeup\n", PIN_PON);
    Serial.println("[System] Entering deep sleep...");
    Serial.flush();

    WiFi.scanDelete();
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(50);
    esp_deep_sleep_start();
  }

  // 继续处理Web请求 / 连接维护
  delay(50);
}
