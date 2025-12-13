/*
 * Copyright (c) 2025 Tomosawa
 * All rights reserved.
 *
 * This code is part of the WiFi2JJY project.
 * GitHub: https://github.com/Tomosawa/WIFI2JJY
 */
#include "JJYSender.h"
#include "IOPin.h"

JJYSender::JJYSender(int daPin) : m_daPin(daPin) {
    // 初始化DA引脚
    pinMode(m_daPin, OUTPUT);
    digitalWrite(m_daPin, HIGH);
    Serial.printf("[JJYSender] DA pin configured as OUTPUT, HIGH (Pin: %d)\n", m_daPin);
}

// 编码JJY时间格式
void JJYSender::encodeJJY(int jjyBits[60], struct tm* timeinfo) {
  // 位置标记（Marker，低电平0.2秒）
  jjyBits[0] = 2;   // 分钟标记
  jjyBits[9] = 2;   // 位置标记P1
  jjyBits[19] = 2;  // 位置标记P2
  jjyBits[29] = 2;  // 位置标记P3
  jjyBits[39] = 2;  // 位置标记P4
  jjyBits[49] = 2;  // 位置标记P5
  jjyBits[59] = 2;  // 位置标记P0
  
  // 分钟（1-8位）：BCD编码
  int minute = timeinfo->tm_min;
  int min_tens = minute / 10;  // 十位（0-5）
  int min_ones = minute % 10;  // 个位（0-9）
  
  // 十位分钟（1-3位）：最大值5，需要3位
  jjyBits[1] = (min_tens & 0x04) ? 1 : 0;  // 40分
  jjyBits[2] = (min_tens & 0x02) ? 1 : 0;  // 20分
  jjyBits[3] = (min_tens & 0x01) ? 1 : 0;  // 10分
  
  // 第4位保留
  jjyBits[4] = 0;
  
  // 个位分钟（5-8位）：0-9，需要4位
  jjyBits[5] = (min_ones & 0x08) ? 1 : 0;  // 8分
  jjyBits[6] = (min_ones & 0x04) ? 1 : 0;  // 4分
  jjyBits[7] = (min_ones & 0x02) ? 1 : 0;  // 2分
  jjyBits[8] = (min_ones & 0x01) ? 1 : 0;  // 1分
  
  // 小时（12-18位）：BCD编码
  int hour = timeinfo->tm_hour;
  int hour_tens = hour / 10;  // 十位（0-2）
  int hour_ones = hour % 10;  // 个位（0-9）
  
  // 十位小时（12-13位）：最大值2，需要2位
  jjyBits[12] = (hour_tens & 0x02) ? 1 : 0;  // 20时
  jjyBits[13] = (hour_tens & 0x01) ? 1 : 0;  // 10时
  
  // 第14位保留
  jjyBits[14] = 0;
  
  // 个位小时（15-18位）：0-9，需要4位
  jjyBits[15] = (hour_ones & 0x08) ? 1 : 0;  // 8时
  jjyBits[16] = (hour_ones & 0x04) ? 1 : 0;  // 4时
  jjyBits[17] = (hour_ones & 0x02) ? 1 : 0;  // 2时
  jjyBits[18] = (hour_ones & 0x01) ? 1 : 0;  // 1时
  
  // 当年第几天（22-33位）：1-366，需要9位（实际用12位）
  int dayOfYear = timeinfo->tm_yday + 1;  // tm_yday从0开始
  
  // 百位天数（22-23位）：最大值3，需要2位
  int day_hundreds = dayOfYear / 100;
  jjyBits[22] = (day_hundreds & 0x02) ? 1 : 0;  // 200天
  jjyBits[23] = (day_hundreds & 0x01) ? 1 : 0;  // 100天
  
  // 第24位保留
  jjyBits[24] = 0;
  
  // 十位天数（25-28位）：0-9，需要4位
  int day_tens = (dayOfYear % 100) / 10;
  jjyBits[25] = (day_tens & 0x08) ? 1 : 0;  // 80天
  jjyBits[26] = (day_tens & 0x04) ? 1 : 0;  // 40天
  jjyBits[27] = (day_tens & 0x02) ? 1 : 0;  // 20天
  jjyBits[28] = (day_tens & 0x01) ? 1 : 0;  // 10天
  
  // 个位天数（30-33位）：0-9，需要4位
  int day_ones = dayOfYear % 10;
  jjyBits[30] = (day_ones & 0x08) ? 1 : 0;  // 8天
  jjyBits[31] = (day_ones & 0x04) ? 1 : 0;  // 4天
  jjyBits[32] = (day_ones & 0x02) ? 1 : 0;  // 2天
  jjyBits[33] = (day_ones & 0x01) ? 1 : 0;  // 1天
  
  // 奇偶校验位1（36位）：小时和分钟的奇偶校验
  int parity1 = 0;
  for (int i = 12; i <= 18; i++) {
    if (jjyBits[i] == 1) parity1++;
  }
  for (int i = 1; i <= 8; i++) {
    if (jjyBits[i] == 1) parity1++;
  }
  jjyBits[36] = (parity1 % 2 == 1) ? 1 : 0;
  
  // 奇偶校验位2（37位）：年积日的奇偶校验
  int parity2 = 0;
  for (int i = 22; i <= 33; i++) {
    if (jjyBits[i] == 1) parity2++;
  }
  jjyBits[37] = (parity2 % 2 == 1) ? 1 : 0;
  
  // 年份（41-48位）：BCD编码，只编码后两位（00-99）
  int year = (timeinfo->tm_year + 1900) % 100;
  int year_tens = year / 10;
  int year_ones = year % 10;
  
  // 十位年份（41-44位）：0-9，需要4位
  jjyBits[41] = (year_tens & 0x08) ? 1 : 0;  // 80年
  jjyBits[42] = (year_tens & 0x04) ? 1 : 0;  // 40年
  jjyBits[43] = (year_tens & 0x02) ? 1 : 0;  // 20年
  jjyBits[44] = (year_tens & 0x01) ? 1 : 0;  // 10年
  
  // 个位年份（45-48位）：0-9，需要4位
  jjyBits[45] = (year_ones & 0x08) ? 1 : 0;  // 8年
  jjyBits[46] = (year_ones & 0x04) ? 1 : 0;  // 4年
  jjyBits[47] = (year_ones & 0x02) ? 1 : 0;  // 2年
  jjyBits[48] = (year_ones & 0x01) ? 1 : 0;  // 1年
  
  // 星期（50-52位）：0=星期日，1=星期一，...，6=星期六
  int dayOfWeek = timeinfo->tm_wday;
  jjyBits[50] = (dayOfWeek & 0x04) ? 1 : 0;  // 4
  jjyBits[51] = (dayOfWeek & 0x02) ? 1 : 0;  // 2
  jjyBits[52] = (dayOfWeek & 0x01) ? 1 : 0;  // 1
  
  // 其他位保留为0或用于特殊功能
  // 第10,11,20,21,34,35,38,40,53-58位保留

}

// 发送 JJY 信号
void JJYSender::sendJJYSignal(int jjyBits[60]) {
   unsigned long startMillis = millis();
  
  for (int second = 0; second < 60; second++) {
    unsigned long targetMillis = startMillis + (second * 1000UL);
    
    // 先粗略等待
    long delayTime = targetMillis - millis();
    if (delayTime > 10) {
      delay(delayTime - 10);
    }
    // 等待到准确的秒数
    while (millis() < targetMillis) {
      yield();
    }
    
    // 发送脉冲（JJY是负逻辑：正常高电平，脉冲时低电平）
    digitalWrite(m_daPin, LOW);  // 开始脉冲
    
    int pulseWidth;
    if (jjyBits[second] == 2) {
      // Marker: 低电平0.2秒
      pulseWidth = 200;
    } else if (jjyBits[second] == 1) {
      // 数据位1: 低电平0.5秒
      pulseWidth = 500;
    } else {
      // 数据位0: 低电平0.8秒
      pulseWidth = 800;
    }
    
    delay(pulseWidth);
    digitalWrite(m_daPin, HIGH);  // 结束脉冲，回到高电平

    // 保证剩余时间保持高电平直到下一秒
    delayTime = targetMillis + 1000UL - millis();
    if (delayTime > 10) {
      delay(delayTime - 10);
    }
    while (millis() < targetMillis + 1000UL) {
      yield();
    }
  }
}

bool JJYSender::startAsyncSend(TimeSync* timeSync) {
  if (m_taskHandle != nullptr) {
    return false; // 已有任务在跑
  }
  m_timeSync = timeSync;
  m_taskDone = false;
  xTaskCreate(
      sendTask,
      "JJYSendTask",
      8192,
      this,
      3,
      &m_taskHandle);
  return m_taskHandle != nullptr;
}

void JJYSender::sendTask(void* param) {
  JJYSender* self = static_cast<JJYSender*>(param);
  int loopCount = 0;
  while (true) {
    loopCount++;
    Serial.printf("\n--- Loop Iteration %d ---\n", loopCount);

    // 检查当前PON状态
    Serial.print("[Loop] Current PON status: ");
    Serial.println(digitalRead(PIN_PON) ? "HIGH" : "LOW");

    // 每循环一次发送 60 秒时间码
    time_t now = time(nullptr);
    struct tm *timeinfo = localtime(&now);

    Serial.printf(
        "\nStart SendTime: %04d-%02d-%02d %02d:%02d Week %d (JJY Encode)\n",
        timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
        timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_wday);
    // 使用精确时间同步等待到下一个整分钟
    Serial.println(
        "[Loop] Waiting for next minute with precise synchronization...");
    if (self->m_timeSync) {
      self->m_timeSync->waitUntilNextMinuteRTC();
    }
    // 获取当前时间（刚到下一分钟）
    now = time(nullptr);
    timeinfo = localtime(&now);

    int frame[60] = {0};
    self->encodeJJY(frame, timeinfo);
    self->sendJJYSignal(frame);

    // 发送一帧后主板可能立即关闭 PON（校时成功）
    Serial.print("[Loop] Checking PON status after transmission... ");
    bool ponStatus = digitalRead(PIN_PON);
    Serial.println(ponStatus ? "HIGH" : "LOW");

    if (ponStatus) {
      Serial.println(
          "[Loop] PON is HIGH, Time synchronized, exiting loop to sleep");
      break;
    } else {
      Serial.println("[Loop] PON is still LOW, will continue sending");
    }
  }

  self->m_taskDone = true;
  self->m_taskHandle = nullptr;
  vTaskDelete(nullptr);
}