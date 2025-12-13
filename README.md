# WIFI授时转JJY信号

一个基于ESP32-C3 Super mini开发板的JJY标准时间信号发送器。该项目通过WiFi连接NTP服务器同步时间，然后生成并发送JJY格式的标准时间信号，可用于校准支持JJY信号的钟表设备。

## 项目概述

JJY是日本的标准时间信号，由日本信息通信研究机构(NICT)运营，用于广播精确的标准时间。该项目利用ESP32-C3 Super mini开发板，通过以下功能实现JJY信号的生成与发送：

- WiFi自动连接与配置（通过Web界面）
- NTP时间同步（支持多服务器配置）
- JJY格式时间信号生成
- 高效的电源管理（深度睡眠模式）
- Web配置界面（用于WiFi设置）

## 硬件要求

- **主控制器**：ESP32-C3 Super mini开发板
- **电源**：5V电源或3.3V供电

### 引脚定义

根据项目中的`IOPin.h`文件，使用以下引脚配置：

| 引脚名称 | GPIO编号 | 功能描述 |
|---------|---------|---------|
| PIN_DA  | GPIO_NUM_3 | JJY信号输出引脚 |
| PIN_PON | GPIO_NUM_4 | 电源控制引脚 |

## 软件依赖

- **Arduino IDE**：用于代码编译和烧录
- **ESP32 Arduino Core**：ESP32-C3开发板支持
- **FreeRTOS**：用于任务管理和多线程处理
- **WiFi库**：用于WiFi连接
- **WebServer库**：用于提供Web配置界面
- **Preferences库**：用于存储WiFi配置信息

## 功能特性

### 1. WiFi连接管理

- 首次启动时自动进入AP模式，提供Web配置界面
- 支持保存WiFi配置到非易失性存储(NVS)
- 自动重连到上次连接的WiFi网络
- 连接超时自动切换到AP模式

### 2. NTP时间同步

- 支持多个NTP服务器配置
- 使用FreeRTOS任务进行异步时间同步
- 时间同步完成后自动更新系统时间
- 支持时区设置（默认东京时区）

### 3. JJY信号发送

- 生成符合JJY格式的时间信号
- 使用PWM技术模拟JJY信号的AM调制
- 高优先级任务确保信号发送的精确性
- 信号发送完成后自动进入深度睡眠

### 4. 电源管理

- 支持深度睡眠模式，降低功耗
- 通过GPIO引脚唤醒设备
- 发送完JJY信号后自动进入睡眠状态

### 5. Web配置界面

- 提供WiFi网络扫描功能
- 支持WiFi SSID和密码配置
- 实时显示设备状态
- 提供重启和重置功能

## 使用说明

### 1. 首次启动配置

1. 将ESP32-C3 Super mini正确接入电路并接入供电
2. 设备启动后，搜索并连接名为"MYNOVA-CLOCK"的WiFi网络
3. 打开浏览器，访问`192.168.1.1`进入配置界面
4. 选择您的WiFi网络，输入密码`MYNOVA123`，点击连接
5. 设备将自动连接到WiFi，连接成功后，您可以在浏览器中查看设备状态

### 2. 正常运行

1. 设备启动后，自动连接到保存的WiFi网络
2. 连接成功后，启动NTP时间同步任务
3. 时间同步完成后，开始发送JJY信号
4. 信号发送完成后，设备进入深度睡眠模式
5. 可通过PON引脚唤醒设备，重新开始流程

### 3. 重新配置

如果需要重新配置WiFi：

1. 断开设备电源
2. 按住PON引脚（如果有物理按钮）或通过串口发送重置命令
3. 设备将清除保存的配置并重新进入AP模式
4. 重复首次启动配置步骤

## 代码结构

项目采用模块化设计，主要文件结构如下：

```
WiFi2JJY/
├── WiFi2JJY.ino          # 主程序入口
├── WiFiManager.h         # WiFi连接管理头文件
├── WiFiManager.cpp       # WiFi连接管理实现
├── WebService.h          # Web服务器头文件
├── WebService.cpp        # Web服务器实现
├── WiFiConfigPage.cpp    # WiFi配置页面（HTML/JavaScript）
├── TimeSync.h            # 时间同步头文件
├── TimeSync.cpp          # 时间同步实现
├── JJYSender.h           # JJY信号发送头文件
├── JJYSender.cpp         # JJY信号发送实现
├── IOPin.h               # 引脚定义
└── README.md             # 项目说明文档
```

### 核心组件说明

1. **WiFiManager**：负责WiFi连接的建立、配置保存和状态管理
2. **WebService**：提供Web配置界面和API接口
3. **TimeSync**：使用NTP协议同步系统时间
4. **JJYSender**：生成并发送JJY格式的时间信号
5. **Web配置界面**：通过HTML和JavaScript提供用户交互界面


### WiFi配置界面

提供用户友好的Web界面进行WiFi配置：

- 扫描可用WiFi网络
- 输入WiFi密码
- 显示连接状态
- 提供重置功能

## 编译和烧录

源码编译烧录

1. 打开Arduino IDE，选择`文件 > 打开`，导航到项目目录
2. 选择`工具 > 开发板 > ESP32 Arduino > ESP32C3 Dev Module`
3. 选择`工具 > 端口`，选择连接的ESP32-C3开发板端口
4. 点击`项目 > 上传`，开始编译和烧录代码

直接烧录预编译二进制文件

1. 下载最新的预编译二进制文件（`WiFi2JJY.bin`）
2. 使用ESP官方flash_download工具（[下载链接](https://www.espressif.com/en/support/download/other-tools)）
3. 连接ESP32-C3开发板到电脑
4. 打开flash_download工具，选择正确的端口和开发板型号（ESP32-C3）
5. 点击"Add File"，添加下载的二进制文件
6. 设置下载地址为0x0000，其他参数保持默认
7. 点击"Start"开始烧录


## 电源管理

项目实现了高效的电源管理策略：

1. 设备启动后，尽快连接WiFi并同步时间
2. 完成JJY信号发送后，立即进入深度睡眠模式
3. 支持通过PON引脚唤醒设备

## 故障排除

### WiFi连接问题

- 确保输入的WiFi密码正确
- 确保WiFi信号强度足够
- 尝试重启设备
- 检查串口输出的错误信息

### 时间同步问题

- 确保设备已连接到WiFi
- 检查NTP服务器配置
- 确保网络可以访问NTP服务器

### JJY信号问题

- 确保DA引脚连接正确
- 检查JJY信号发送任务是否正常启动
- 查看串口输出的调试信息

## 许可证

本项目采用MIT许可证，详见LICENSE文件。

## 贡献

欢迎提交Issue和Pull Request！

## 参考资料

- [JJY时间信号规范](https://www.nict.go.jp/JST/ntp/jjy.html)
- [ESP32-C3 技术参考手册](https://www.espressif.com/sites/default/files/documentation/esp32-c3_technical_reference_manual_en.pdf)
- [Arduino ESP32 Core](https://github.com/espressif/arduino-esp32)
- [FreeRTOS官方文档](https://www.freertos.org/documentation.html)

