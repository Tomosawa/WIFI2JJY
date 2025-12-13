/*
 * Copyright (c) 2025 Tomosawa
 * All rights reserved.
 *
 * This code is part of the WiFi2JJY project.
 * GitHub: https://github.com/Tomosawa/WIFI2JJY
 */
#ifndef JJYSENDER_H
#define JJYSENDER_H

#include <Arduino.h>
#include <time.h>
#include "TimeSync.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class JJYSender {
public:
  
    explicit JJYSender(int daPin);

    void encodeJJY(int jjyBits[60], struct tm* timeinfo);
    void sendJJYSignal(int jjyBits[60]);

    // 异步发送任务
    bool startAsyncSend(TimeSync* timeSync);
    bool isAsyncDone() const { return m_taskDone; }
    TaskHandle_t getTaskHandle() const { return m_taskHandle; }
    void clearAsyncDone() { m_taskDone = false; }
    
private:
    int m_daPin;  // DA引脚号
    TimeSync* m_timeSync = nullptr;
 
    TaskHandle_t m_taskHandle = nullptr;
    volatile bool m_taskDone = false;

    static void sendTask(void* param);
};

#endif // JJYSENDER_H