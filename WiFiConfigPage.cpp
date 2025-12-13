/*
 * Copyright (c) 2025 Tomosawa
 * All rights reserved.
 *
 * This code is part of the WiFi2JJY project.
 * GitHub: https://github.com/Tomosawa/WIFI2JJY
 */
#include "WiFiConfigPage.h"

// HTML头部模板 - 存储在PROGMEM中
const char header_html[] PROGMEM = R"(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>MYNOVA Clock - WiFi配置</title>
    <style>
)";

// CSS样式 - 存储在PROGMEM中
const char css_style[] PROGMEM = R"(
/* 基础样式 */
* {
    box-sizing: border-box;
    margin: 0;
    padding: 0;
}

body {
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    line-height: 1.6;
    color: #333;
    background-color: #f5f7fa;
    padding: 0;
    margin: 0;
}

.container {
    max-width: 800px;
    margin: 0 auto;
    padding: 20px;
    min-height: 100vh;
    display: flex;
    flex-direction: column;
}

/* 头部样式 */
.header {
    text-align: center;
    margin-bottom: 30px;
    padding: 20px;
    background: linear-gradient(135deg, #6a11cb 0%, #2575fc 100%);
    border-radius: 10px;
    color: white;
    box-shadow: 0 4px 15px rgba(0, 0, 0, 0.1);
}

.header h1 {
    font-size: 2.5rem;
    margin-bottom: 10px;
}

.header p {
    font-size: 1.2rem;
    opacity: 0.9;
}

/* 主要内容区域 */
.main-content {
    flex: 1;
    display: flex;
    flex-direction: column;
    gap: 25px;
}

/* 区块样式 */
section {
    background-color: white;
    border-radius: 10px;
    padding: 25px;
    box-shadow: 0 2px 10px rgba(0, 0, 0, 0.05);
}

h2 {
    color: #2c3e50;
    margin-bottom: 20px;
    font-size: 1.5rem;
    border-bottom: 2px solid #eaeaea;
    padding-bottom: 10px;
}

/* 状态卡片 */
.status-card {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
    gap: 15px;
}

.status-item {
    display: flex;
    flex-direction: column;
    padding: 15px;
    background-color: #f8f9fa;
    border-radius: 8px;
    border-left: 4px solid #3498db;
}

.status-label {
    font-weight: 600;
    color: #7f8c8d;
    margin-bottom: 5px;
    font-size: 0.9rem;
}

.status-value {
    font-weight: 700;
    color: #2c3e50;
    word-break: break-all;
}

/* 扫描控制 */
.scan-controls {
    display: flex;
    align-items: center;
    gap: 15px;
    margin-bottom: 20px;
}

/* WiFi列表 */
.wifi-list {
    display: grid;
    gap: 10px;
}

.wifi-item {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 15px;
    border: 1px solid #eaeaea;
    border-radius: 8px;
    cursor: pointer;
    transition: all 0.3s ease;
}

.wifi-item:hover {
    background-color: #f8f9fa;
    border-color: #3498db;
    transform: translateY(-2px);
    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.05);
}

.wifi-info {
    display: flex;
    flex-direction: column;
    gap: 5px;
}

.wifi-ssid {
    font-weight: 600;
    color: #2c3e50;
}

.wifi-details {
    display: flex;
    gap: 10px;
    font-size: 0.85rem;
    color: #7f8c8d;
}

.wifi-signal {
    display: flex;
    align-items: center;
    gap: 5px;
}

.signal-strength {
    width: 20px;
    height: 20px;
    display: inline-block;
}

/* 表单样式 */
.connect-form {
    display: flex;
    flex-direction: column;
    gap: 20px;
}

.form-group {
    display: flex;
    flex-direction: column;
    gap: 8px;
}

/* 下拉选择框样式 */
select {
    padding: 12px 15px;
    border: 1px solid #ddd;
    border-radius: 6px;
    font-size: 1rem;
    background-color: white;
    color: #333;
    transition: border-color 0.3s;
    width: 100%;
    -webkit-appearance: none;
    -moz-appearance: none;
    appearance: none;
    background-image: url("data:image/svg+xml;charset=US-ASCII,%3Csvg%20xmlns%3D%22http%3A%2F%2Fwww.w3.org%2F2000%2Fsvg%22%20width%3D%22292.4%22%20height%3D%22292.4%22%3E%3Cpath%20fill%3D%22%23333%22%20d%3D%22M287%2069.4a17.6%2017.6%200%200%200-13-5.4H18.4c-5%200-9.3%201.8-12.9%205.4A17.6%2017.6%200%200%200%200%2082.2c0%205%201.8%209.3%205.4%2012.9l128%20127.9c3.6%203.6%207.8%205.4%2012.8%205.4s9.2-1.8%2012.8-5.4L287%2095c3.5-3.5%205.4-7.8%205.4-12.8%200-5-1.9-9.2-5.5-12.8z%22%2F%3E%3C%2Fsvg%3E");
    background-repeat: no-repeat;
    background-position: right 15px center;
    background-size: 12px auto;
    padding-right: 40px;
}

select:focus {
    border-color: #3498db;
    outline: none;
    box-shadow: 0 0 0 2px rgba(52, 152, 219, 0.2);
}

label {
    font-weight: 600;
    color: #2c3e50;
}

input[type="text"],
input[type="password"] {
    padding: 12px 15px;
    border: 1px solid #ddd;
    border-radius: 6px;
    font-size: 1rem;
    transition: border-color 0.3s;
    width: 100%;
}

input[type="text"]:focus,
input[type="password"]:focus {
    border-color: #3498db;
    outline: none;
    box-shadow: 0 0 0 2px rgba(52, 152, 219, 0.2);
}

.ssid-field {
    position: relative;
    width: 100%;
}

.ssid-input-wrap {
    display: flex;
    align-items: stretch;
    gap: 6px;
}

.ssid-toggle {
    border: 1px solid #ddd;
    border-radius: 6px;
    background: #f5f7fa;
    cursor: pointer;
    padding: 0 12px;
    font-size: 1rem;
    color: #2c3e50;
    transition: border-color 0.3s, box-shadow 0.3s, background 0.3s;
}

.ssid-toggle:hover {
    background: #eef2f7;
    border-color: #3498db;
    box-shadow: 0 0 0 2px rgba(52, 152, 219, 0.15);
}

.ssid-dropdown {
    position: absolute;
    top: 100%;
    left: 0;
    right: 0;
    background: #fff;
    border: 1px solid #ddd;
    border-radius: 6px;
    box-shadow: 0 4px 12px rgba(0,0,0,0.08);
    max-height: 220px;
    overflow-y: auto;
    margin-top: 6px;
    z-index: 20;
}

.ssid-item {
    padding: 10px 12px;
    cursor: pointer;
    display: flex;
    justify-content: space-between;
    align-items: center;
    gap: 8px;
}

.ssid-item:hover {
    background-color: #f5f7fa;
}

.ssid-signal {
    color: #7f8c8d;
    font-size: 0.9rem;
}

.password-toggle {
    display: flex;
    align-items: center;
    gap: 8px;
    margin-top: 5px;
}

.form-actions {
    display: flex;
    gap: 15px;
    margin-top: 10px;
}

/* 按钮样式 */
.btn {
    padding: 12px 20px;
    border: none;
    border-radius: 6px;
    font-size: 1rem;
    font-weight: 600;
    cursor: pointer;
    transition: all 0.3s ease;
    display: inline-flex;
    align-items: center;
    justify-content: center;
}

.btn-primary {
    background-color: #3498db;
    color: white;
}

.btn-primary:hover {
    background-color: #2980b9;
    transform: translateY(-2px);
    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
}

.btn-success {
    background-color: #2ecc71;
    color: white;
}

.btn-success:hover {
    background-color: #27ae60;
    transform: translateY(-2px);
    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
}

.btn-danger {
    background-color: #e74c3c;
    color: white;
}

.btn-danger:hover {
    background-color: #c0392b;
    transform: translateY(-2px);
    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
}

/* 消息样式 */
.message {
    padding: 15px;
    border-radius: 6px;
    margin-top: 15px;
    font-weight: 500;
}

.message.success {
    background-color: rgba(46, 204, 113, 0.1);
    border: 1px solid #2ecc71;
    color: #27ae60;
}

.message.error {
    background-color: rgba(231, 76, 60, 0.1);
    border: 1px solid #e74c3c;
    color: #c0392b;
}

/* 加载状态 */
.loading {
    display: inline-flex;
    align-items: center;
    gap: 8px;
    color: #7f8c8d;
    font-style: italic;
}

.loading::before {
    content: '';
    width: 16px;
    height: 16px;
    border: 2px solid #ddd;
    border-top: 2px solid #3498db;
    border-radius: 50%;
    animation: spin 1s linear infinite;
}

@keyframes spin {
    0% { transform: rotate(0deg); }
    100% { transform: rotate(360deg); }
}

/* 隐藏元素 */
.hidden {
    display: none !important;
}

/* 页脚 */
.footer {
    text-align: center;
    margin-top: 30px;
    padding: 20px;
    color: #7f8c8d;
    font-size: 0.9rem;
}

/* 响应式设计 */
@media (max-width: 768px) {
    .container {
        padding: 15px;
    }

    .header h1 {
        font-size: 2rem;
    }

    .status-card {
        grid-template-columns: 1fr;
    }

    .form-actions {
        flex-direction: column;
    }

    .wifi-item {
        flex-direction: column;
        align-items: flex-start;
        gap: 10px;
    }
}
)";

// HTML主体开始 - 存储在PROGMEM中
const char body_start[] PROGMEM = R"(
    </style>
</head>
<body>
    <div class="container">
        <header class="header">
            <h1>MYNOVA Clock</h1>
            <p>WiFi配置管理</p>
        </header>

        <main class="main-content">
            <section class="connect-section">
                <h2>连接WiFi</h2>
                <form id="connect-form" class="connect-form">
                    <div class="form-group">
                        <label for="ssid">网络名称(SSID):</label>
                        <div class="ssid-field">
                            <div class="ssid-input-wrap">
                                <input type="text" id="ssid" name="ssid" placeholder="选择或输入WiFi名称" autocomplete="off" required>
                                <button type="button" id="ssid-toggle" class="ssid-toggle">▼</button>
                            </div>
                            <div id="ssid-dropdown" class="ssid-dropdown hidden"></div>
                            <div id="ssid-dropdown-all" class="ssid-dropdown hidden"></div>
                        </div>
                        <div style="margin-top: 5px; font-size: 0.9rem; color: #7f8c8d;">
                            扫描状态: <span id="scan-status">初始化中</span>
                        </div>
                    </div>
                    <div class="form-group">
                        <label for="password">密码:</label>
                        <input type="password" id="password" name="password">
                        <div class="password-toggle">
                            <input type="checkbox" id="show-password">
                            <label for="show-password">显示密码</label>
                        </div>
                    </div>
                    <div class="form-actions">
                        <button type="submit" class="btn btn-success">连接</button>
                        <button type="button" id="reset-btn" class="btn btn-danger">重置配置</button>
                    </div>
                </form>
                <div id="connect-message" class="message hidden"></div>
            </section>

            <section class="status-section">
                <h2>当前状态</h2>
                <div class="status-card">
                    <div class="status-item">
                        <span class="status-label">连接状态:</span>
                        <span id="connection-status" class="status-value">检查中...</span>
                    </div>
                    <div id="ip-address-container" class="status-item" style="display: none;">
                        <span class="status-label">IP地址:</span>
                        <span id="ip-address" class="status-value"></span>
                    </div>
                    <div class="status-item">
                        <span class="status-label">当前SSID:</span>
                        <span id="current-ssid" class="status-value">获取中...</span>
                    </div>
                </div>
            </section>
        </main>

        <footer class="footer">
            <p>MYNOVA Clock WiFi配置页面</p>
        </footer>
    </div>

    <script>
)";

// JavaScript代码 - 存储在PROGMEM中
const char javascript_code[] PROGMEM = R"(
// 页面加载完成后执行
document.addEventListener('DOMContentLoaded', function() {
    // 获取DOM元素
    const ssidSelect = document.getElementById('ssid');
    const ssidDropdown = document.getElementById('ssid-dropdown'); // 自动完成列表
    const ssidDropdownAll = document.getElementById('ssid-dropdown-all'); // 全量列表
    const ssidToggle = document.getElementById('ssid-toggle');
    const scanStatus = document.getElementById('scan-status');
    const connectForm = document.getElementById('connect-form');
    const resetBtn = document.getElementById('reset-btn');
    const connectMessage = document.getElementById('connect-message');
    const showPasswordCheckbox = document.getElementById('show-password');
    const passwordInput = document.getElementById('password');

    // 状态元素
    const connectionStatus = document.getElementById('connection-status');
    const ipAddress = document.getElementById('ip-address');
    const currentSSID = document.getElementById('current-ssid');

    // 扫描计时器（需在调用前定义，避免TDZ）
    let scanTimer = null;
    // 状态快速轮询定时器
    let statusPollTimer = null;
    // 扫描状态轮询定时器（处理HTTP 202状态时使用）
    let scanStatusTimer = null;
    // 当前可用网络列表
    let availableNetworks = [];

    // 初始化页面状态
    updateStatus();
    
    // 启动自动扫描
    startAutoScan();

    // 密码显示/隐藏切换
    showPasswordCheckbox.addEventListener('change', function() {
        passwordInput.type = this.checked ? 'text' : 'password';
    });

    // SSID输入/焦点事件
    ssidSelect.addEventListener('input', function() {
        passwordInput.value = '';
        renderSSIDDropdown(this.value);
        showSSIDDropdown();
        hideSSIDDropdownAll();
    });
    ssidSelect.addEventListener('focus', function() {
        renderSSIDDropdown(this.value);
        showSSIDDropdown();
        hideSSIDDropdownAll();
    });
    ssidSelect.addEventListener('blur', function() {
        setTimeout(hideSSIDDropdown, 150); // 给点击选择留时间
    });

    ssidToggle.addEventListener('click', function() {
        // 单独的全量列表，与自动完成分开
        if (ssidDropdownAll.classList.contains('hidden')) {
            renderFullSSIDDropdown();
            showSSIDDropdownAll();
            hideSSIDDropdown();
            ssidSelect.focus();
        } else {
            hideSSIDDropdownAll();
        }
    });

    // 连接表单提交事件
    connectForm.addEventListener('submit', function(e) {
        e.preventDefault();
        connectToWiFi();
    });

    // 重置按钮点击事件
    resetBtn.addEventListener('click', function() {
        if (confirm('确定要重置WiFi配置吗？')) {
            resetConfig();
        }
    });

    // 更新状态信息
    function updateStatus() {
        fetch('/status')
            .then(response => response.json())
            .then(data => {
                const isConnected = data.connected === true || data.status === 'connected';
                connectionStatus.textContent = isConnected ? '已连接' : '未连接';
                
                // 只在已连接时显示IP地址
                const ipContainer = document.getElementById('ip-address-container');
                if (isConnected && data.ip && data.ip !== '0.0.0.0') {
                    ipAddress.textContent = data.ip;
                    ipContainer.style.display = 'flex';
                } else {
                    ipContainer.style.display = 'none';
                }
                
                currentSSID.textContent = data.ssid || '无';
                
                // 更新状态样式
                if (isConnected) {
                    connectionStatus.style.color = '#2ecc71';
                } else {
                    connectionStatus.style.color = '#e74c3c';
                }

                // 如果有保存的配置，回填到输入框
                if (data.saved) {
                    if (data.saved.ssid) {
                        ssidSelect.value = data.saved.ssid;
                    }
                    if (data.saved.password) {
                        passwordInput.value = data.saved.password;
                    }
                    // 回填后刷新下拉匹配
                    renderSSIDDropdown(ssidSelect.value);
                }
            })
            .catch(error => {
                console.error('获取状态失败:', error);
                connectionStatus.textContent = '获取失败';
                ipAddress.textContent = '获取失败';
                currentSSID.textContent = '获取失败';
            });
    }
    
    // 短期快速轮询，连接过程加快反馈
    function startFastStatusPolling(intervalMs = 1000, maxTries = 30) {
        if (statusPollTimer) {
            clearInterval(statusPollTimer);
            statusPollTimer = null;
        }
        let tries = 0;
        statusPollTimer = setInterval(() => {
            tries++;
            updateStatus();
            if (tries >= maxTries) {
                clearInterval(statusPollTimer);
                statusPollTimer = null;
            }
        }, intervalMs);
    }

    // 自动扫描WiFi网络
    function startAutoScan() {
        // 立即执行一次扫描
        scanNetworks();
        
        // 每5秒扫描一次，直到成功拿到列表后停止
        scanTimer = setInterval(scanNetworks, 5000);
    }

    // 扫描WiFi网络
    function scanNetworks() {
        scanStatus.textContent = '正在检查WiFi扫描状态...';
        
        fetch('/scan')
            .then(response => {
                if (response.status === 202) {
                    // 扫描正在进行中
                    scanStatus.textContent = '正在扫描WiFi网络，请稍候...';
                    // 清除之前的状态轮询定时器
                    if (scanStatusTimer) {
                        clearTimeout(scanStatusTimer);
                    }
                    // 2秒后再次检查状态
                    scanStatusTimer = setTimeout(scanNetworks, 2000);
                    return null; // 不继续处理响应体
                }
                if (!response.ok) {
                    throw new Error(`HTTP error! status: ${response.status}`);
                }
                return response.json();
            })
            .then(data => {
                if (!data) return; // 如果是扫描中的情况，data为null，直接返回
                
                if (data.status === 'success') {
                    if (data.networks && data.networks.length > 0) {
                        // 已拿到列表，停止自动扫描，避免重复请求
                        if (scanTimer) {
                            clearInterval(scanTimer);
                            scanTimer = null;
                        }
                        scanStatus.textContent = `发现 ${data.networks.length} 个WiFi网络`;
                        updateSSIDDropdown(data.networks);
                    } else {
                        scanStatus.textContent = '未发现可用的WiFi网络，5秒后重试';
                        // 清除之前的状态轮询定时器
                        if (scanStatusTimer) {
                            clearTimeout(scanStatusTimer);
                        }
                        // 5秒后重试
                        scanStatusTimer = setTimeout(scanNetworks, 5000);
                    }
                } else {
                    // 处理其他状态或错误
                    const errorMsg = data.message || '扫描失败';
                    scanStatus.textContent = `错误: ${errorMsg}，5秒后重试`;
                    console.error('扫描WiFi网络失败:', errorMsg);
                    // 清除之前的状态轮询定时器
                    if (scanStatusTimer) {
                        clearTimeout(scanStatusTimer);
                    }
                    // 5秒后重试
                    scanStatusTimer = setTimeout(scanNetworks, 5000);
                }
            })
            .catch(error => {
                console.error('扫描WiFi网络失败:', error);
                scanStatus.textContent = `连接错误: ${error.message}，5秒后重试`;
                // 清除之前的状态轮询定时器
                if (scanStatusTimer) {
                    clearTimeout(scanStatusTimer);
                }
                // 5秒后重试
                scanStatusTimer = setTimeout(scanNetworks, 5000);
            });
    }

    // 更新SSID下拉列表
    function updateSSIDDropdown(networks) {
        // 保存当前输入的SSID
        const currentSSID = ssidSelect.value;
        
        // 按信号强度排序（从强到弱）
        networks.sort((a, b) => b.rssi - a.rssi);
        availableNetworks = networks;
        
        renderSSIDDropdown(currentSSID);
    }

    function renderSSIDDropdown(keyword = '') {
        ssidDropdown.innerHTML = '';
        const kw = keyword.trim().toLowerCase();
        const filtered = availableNetworks.filter(n => !kw || n.ssid.toLowerCase().includes(kw));

        if (filtered.length === 0) {
            const empty = document.createElement('div');
            empty.className = 'ssid-item';
            empty.textContent = '无匹配网络';
            ssidDropdown.appendChild(empty);
            return;
        }

        filtered.forEach(net => {
            const item = document.createElement('div');
            item.className = 'ssid-item';
            const signal = getSignalStrength(net.rssi);
            item.innerHTML = `<span>${net.ssid}</span><span class="ssid-signal">${signal}</span>`;
            item.addEventListener('click', () => {
                ssidSelect.value = net.ssid;
                hideSSIDDropdown();
                hideSSIDDropdownAll();
            });
            ssidDropdown.appendChild(item);
        });
    }

    function renderFullSSIDDropdown() {
        ssidDropdownAll.innerHTML = '';
        if (!availableNetworks || availableNetworks.length === 0) {
            const empty = document.createElement('div');
            empty.className = 'ssid-item';
            empty.textContent = '暂无可用网络';
            ssidDropdownAll.appendChild(empty);
            return;
        }
        availableNetworks.forEach(net => {
            const item = document.createElement('div');
            item.className = 'ssid-item';
            const signal = getSignalStrength(net.rssi);
            item.innerHTML = `<span>${net.ssid}</span><span class="ssid-signal">${signal}</span>`;
            item.addEventListener('click', () => {
                ssidSelect.value = net.ssid;
                hideSSIDDropdownAll();
            });
            ssidDropdownAll.appendChild(item);
        });
    }

    function showSSIDDropdown() {
        ssidDropdown.classList.remove('hidden');
    }

    function hideSSIDDropdown() {
        ssidDropdown.classList.add('hidden');
    }

    function showSSIDDropdownAll() {
        ssidDropdownAll.classList.remove('hidden');
    }

    function hideSSIDDropdownAll() {
        ssidDropdownAll.classList.add('hidden');
    }

    // 获取信号强度描述
    function getSignalStrength(rssi) {
        if (rssi > -50) return '信号极好';
        if (rssi > -60) return '信号良好';
        if (rssi > -70) return '信号一般';
        return '信号较弱';
    }

    // 连接到WiFi
    function connectToWiFi() {
        const ssid = document.getElementById('ssid').value;
        const password = document.getElementById('password').value;

        if (!ssid) {
            showMessage('请选择或输入WiFi名称', 'error');
            return;
        }

        // 禁用表单
        const submitBtn = connectForm.querySelector('button[type="submit"]');
        submitBtn.disabled = true;
        submitBtn.textContent = '连接中...';

        // 发送连接请求（使用urlencoded，兼容ESP32 WebServer）
        const params = new URLSearchParams();
        params.append('ssid', ssid);
        params.append('password', password);

        fetch('/connect', {
            method: 'POST',
            headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
            body: params.toString()
        })
            .then(response => response.json())
            .then(data => {
                if (data.status === 'success') {
                    showMessage(`连接成功！IP地址: ${data.ip}`, 'success');
                    // 更新状态并短期快速轮询
                    setTimeout(updateStatus, 500);
                    startFastStatusPolling(1000, 20);
                } else if (data.status === 'pending') {
                    showMessage('正在连接，请稍候...', 'success');
                    // 立即启动状态轮询，加快反馈
                    setTimeout(updateStatus, 500);
                    startFastStatusPolling(1000, 30);
                } else {
                    showMessage(`连接失败: ${data.message}`, 'error');
                }
            })
            .catch(error => {
                console.error('连接请求失败:', error);
                showMessage('连接请求失败，请重试', 'error');
            })
            .finally(() => {
                // 恢复表单状态
                submitBtn.disabled = false;
                submitBtn.textContent = '连接';
            });
    }

    // 重置配置
    function resetConfig() {
        fetch('/reset', {
            method: 'POST'
        })
            .then(response => response.json())
            .then(data => {
                if (data.status === 'success') {
                    showMessage('配置已重置', 'success');
                    setTimeout(() => {
                        if (confirm('配置已重置，是否现在重启设备？')) {
                            fetch('/reboot', { method: 'POST' })
                                .then(() => showMessage('正在重启，请稍候...', 'success'))
                                .catch(() => showMessage('重启请求失败，请手动重启', 'error'));
                        }
                    }, 300);
                } else {
                    showMessage(`重置失败: ${data.message}`, 'error');
                }
            })
            .catch(error => {
                console.error('重置请求失败:', error);
                showMessage('重置请求失败，请重试', 'error');
            });
    }

    // 显示消息
    function showMessage(message, type) {
        connectMessage.textContent = message;
        connectMessage.className = 'message ' + type;
        connectMessage.classList.remove('hidden');

        // 5秒后自动隐藏
        setTimeout(() => {
            connectMessage.classList.add('hidden');
        }, 5000);
    }

    // 定期更新状态
    setInterval(updateStatus, 10000); // 每10秒更新一次
});
)";

// HTML尾部 - 存储在PROGMEM中
const char footer_html[] PROGMEM = R"(
    </script>
</body>
</html>
)";

// 实现函数
void WiFiConfigPage::sendPage(WiFiClient* client) {
    // 发送HTTP响应头
    client->println("HTTP/1.1 200 OK");
    client->println("Content-Type: text/html");
    client->println("Cache-Control: no-cache, no-store, must-revalidate");
    client->println("Pragma: no-cache");
    client->println("Expires: -1");
    client->println("Connection: close");
    client->println();
    
    sendHeader(client);
    sendBody(client);
    sendFooter(client);
}

void WiFiConfigPage::sendHeader(WiFiClient* client) {
    sendProgmemString(client, header_html);
    sendCSS(client);
    sendProgmemString(client, body_start);
}

void WiFiConfigPage::sendBody(WiFiClient* client) {
    sendJavaScript(client);
}

void WiFiConfigPage::sendFooter(WiFiClient* client) {
    sendProgmemString(client, footer_html);
}

void WiFiConfigPage::sendCSS(WiFiClient* client) {
    sendProgmemString(client, css_style);
}

void WiFiConfigPage::sendJavaScript(WiFiClient* client) {
    sendProgmemString(client, javascript_code);
}

void WiFiConfigPage::sendProgmemString(WiFiClient* client, const char* progmemStr) {
    char buffer[256];
    size_t len = strlen_P(progmemStr);
    size_t pos = 0;
    
    while (pos < len) {
        size_t chunkSize = min(len - pos, sizeof(buffer) - 1);
        strncpy_P(buffer, progmemStr + pos, chunkSize);
        buffer[chunkSize] = '\0';
        client->print(buffer);
        pos += chunkSize;
    }
}