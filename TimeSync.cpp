/*
 * Copyright (c) 2025 Tomosawa
 * All rights reserved.
 *
 * This code is part of the WiFi2JJY project.
 * GitHub: https://github.com/Tomosawa/WIFI2JJY
 */
#include "TimeSync.h"
#include <Arduino.h>

// ESP32C3 特定实现：直接使用 esp_timer_get_time()
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// 时区偏移，这里采用东8区（UTC+8）北京时间
constexpr long UTC_OFFSET = 8 * 3600;

// esp_timer 回调：到点后通知等待的任务
static void rtcTimerNotifyTask(void* arg) {
    TaskHandle_t task = static_cast<TaskHandle_t>(arg);
    if (task == nullptr) return;

    BaseType_t higher_woken = pdFALSE;
    vTaskNotifyGiveFromISR(task, &higher_woken);
    if (higher_woken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

TimeSync::TimeSync()  {
}

void TimeSync::waitUntilNextMinuteRTC() {
    // 读取当前秒级时间
    time_t now_s = time(nullptr);
    struct tm* timeinfo = localtime(&now_s);
    if (timeinfo == nullptr) {
        Serial.println("[TimeSync] Failed to get localtime, fallback to immediate return");
        return;
    }

    int current_second = timeinfo->tm_sec;
    int seconds_to_next_minute = 60 - current_second;
    if (current_second == 0) {
        return;
    }

    int64_t wait_us = (int64_t)seconds_to_next_minute * 1000000LL;
    if (wait_us <= 0) return;

    Serial.printf("[TimeSync][RTC] Current %02d:%02d:%02d, arming timer for %d s\n",
                  timeinfo->tm_hour, timeinfo->tm_min, current_second, seconds_to_next_minute);

    // 创建一次性高精度定时器，回调里通知当前任务
    esp_timer_handle_t timer = nullptr;
    esp_timer_create_args_t args = {
        .callback = &rtcTimerNotifyTask,
        .arg = (void*)xTaskGetCurrentTaskHandle(),
        .dispatch_method = ESP_TIMER_TASK, // 任务上下文，允许使用 FreeRTOS API
        .name = "ts_next_minute"
    };

    esp_err_t err = esp_timer_create(&args, &timer);
    if (err != ESP_OK) {
        Serial.printf("[TimeSync][RTC] esp_timer_create failed: %d\n", err);
        return;
    }

    err = esp_timer_start_once(timer, wait_us);
    if (err != ESP_OK) {
        Serial.printf("[TimeSync][RTC] esp_timer_start_once failed: %d\n", err);
        esp_timer_delete(timer);
        return;
    }

    // 等待回调通知；超时时间略大于预计等待，防止异常卡死
    uint32_t wait_ticks = pdMS_TO_TICKS((wait_us / 1000LL) + 2000); // 额外+2s 余量
    ulTaskNotifyTake(pdTRUE, wait_ticks);

    // 清理定时器
    esp_timer_stop(timer);
    esp_timer_delete(timer);
}

bool TimeSync::resyncTime(time_t now_s, int64_t cur_us, time_t& next_min_s, int64_t& target_us, int seconds_to_next_minute) const {
    // 检查系统时间是否合理（不是默认值或未初始化值）
    if (now_s < 1000000000) { // 2001年之前的时间被认为是未初始化的
        return false;
    }
    
    // 重新获取当前时间，检查是否有显著变化
    time_t new_now_s = time(nullptr);
    struct tm* new_timeinfo = localtime(&new_now_s);
    int new_current_second = new_timeinfo->tm_sec;
    
    // 计算新的等待秒数
    int new_seconds_to_next_minute = 60 - new_current_second;
    if (new_seconds_to_next_minute >= 60) {
        new_seconds_to_next_minute = 60;
    }
    
    // 如果等待时间变化超过1秒，重新计算目标时间
    if (abs(new_seconds_to_next_minute - seconds_to_next_minute) > 1) {
        next_min_s = new_now_s + new_seconds_to_next_minute;
        target_us = cur_us + (int64_t)new_seconds_to_next_minute * 1000000LL;
        return true;
    }
    
    return false;
}

void TimeSync::syncNTPTime() {
    // 配置NTP
    Serial.println("[NTP] Configuring NTP servers...");
    configTime(UTC_OFFSET, 0, "ntp1.aliyun.com", "ntp1.tencent.com", "cn.pool.ntp.org");

    // 等待NTP同步
    Serial.print("[NTP] Waiting for NTP synchronization...");
    time_t now = time(nullptr);
    int ntpAttempts = 0;
    while (now < 1000000000 && ntpAttempts < 30) { // 最多等待15秒
      delay(500);
      now = time(nullptr);
      ntpAttempts++;
      if (ntpAttempts % 2 == 0) { // 每秒打印一次
        Serial.print(".");
      }
    }
    
    if (now >= 1000000000) {
      timeSynced = true;
      Serial.println(" Done!");
      
      // 打印同步后的系统时间
      struct tm* t = localtime(&now);
      Serial.printf("[Time] Current system time: %04d-%02d-%02d %02d:%02d:%02d\n", 
                    t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                    t->tm_hour, t->tm_min, t->tm_sec);
    } else {
      Serial.println(" Failed!");
      Serial.println("[NTP] Time synchronization failed, but continuing...");
    }
}

bool TimeSync::startNTPSyncTask() {
    // 检查任务是否已经在运行
    if (m_ntpSyncTaskHandle != nullptr) {
        Serial.println("[TimeSync] NTP sync task is already running.");
        return false;
    }

    // 创建NTP同步任务
    BaseType_t result = xTaskCreatePinnedToCore(
        &TimeSync::ntpSyncTask,  // 任务函数
        "NTPSyncTask",          // 任务名称
        4096,                   // 任务栈大小
        this,                   // 传递给任务函数的参数
        5,                      // 任务优先级
        &m_ntpSyncTaskHandle,   // 任务句柄
        0                       // 核心编号（0或1）
    );

    if (result == pdPASS) {
        Serial.println("[TimeSync] NTP sync task created successfully.");
        return true;
    } else {
        Serial.printf("[TimeSync] Failed to create NTP sync task, error: %d\n", result);
        m_ntpSyncTaskHandle = nullptr;
        return false;
    }
}

void TimeSync::ntpSyncTask(void *pvParameters) {
    TimeSync *timeSync = static_cast<TimeSync*>(pvParameters);
    if (timeSync == nullptr) {
        Serial.println("[TimeSync] Invalid task parameter, exiting task.");
        vTaskDelete(nullptr);
        return;
    }

    // 执行NTP同步
    timeSync->syncNTPTime();

    // 同步完成后清理任务句柄
    timeSync->m_ntpSyncTaskHandle = nullptr;
    
    // 退出任务
    vTaskDelete(nullptr);
}