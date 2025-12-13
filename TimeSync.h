/*
 * Copyright (c) 2025 Tomosawa
 * All rights reserved.
 *
 * This code is part of the WiFi2JJY project.
 * GitHub: https://github.com/Tomosawa/WIFI2JJY
 */
#ifndef TIMESYNC_H
#define TIMESYNC_H

#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class TimeSync {
public:
    TimeSync();
    
    // 使用硬件 RTC 定时器，在不睡眠的情况下等到下一整分 0 秒
    void waitUntilNextMinuteRTC();

    //网络更新同步NTP系统时间
    void syncNTPTime();

    //启动NTP同步任务
    bool startNTPSyncTask();

    // 时间是否已同步
    bool timeSynced = false;
private:
    // NTP同步任务句柄
    TaskHandle_t m_ntpSyncTaskHandle;

    // 静态任务函数，用于FreeRTOS任务
    static void ntpSyncTask(void *pvParameters);
    
    //重新同步时间，避免系统时间和微秒时钟差异过大
    bool resyncTime(time_t now_s, int64_t cur_us, time_t& next_min_s, int64_t& target_us, int seconds_to_next_minute) const;
};

#endif // TIMESYNC_H