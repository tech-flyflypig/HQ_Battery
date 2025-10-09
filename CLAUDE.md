# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

HQ_Battery 是一个基于 Qt 5.15.2 的电池管理系统（BMS）桌面应用程序，使用 C++17 开发。系统支持多种通信协议（串口、Modbus TCP）监控电池状态，提供实时数据显示、历史记录查询、告警管理等功能。

## 构建和运行

### 环境要求

- Qt 5.15.2（MSVC 2019 64位）
- MSVC 2019 编译器
- qmake 路径: `D:\Qt\5.15.2\msvc2019_64\bin\qmake.exe`

### 构建命令

```bash
# 生成 Makefile
qmake HQ_Battery.pro

# 编译 Debug 版本
nmake debug

# 编译 Release 版本
nmake release

# 清理构建产物
nmake clean
```

**注意**：
- Release 模式自动禁用 qDebug（定义 QT_NO_DEBUG_OUTPUT）
- MSVC 编译器添加 `/utf-8` 标志确保 UTF-8 编码
- Qt 模块依赖：core, gui, widgets, serialport, sql, xlsx, charts, network

## 代码架构

### 目录结构

- `component/` - UI 组件和表单（电池列表、统计面板、记录查询等）
- `protocols/` - 通信协议层，支持多种电池类型和通信方式
- `utils/` - 工具类（配置管理、记录管理、统计等）
- `mainwindow.h/cpp` - 主窗口，管理界面切换和用户权限
- `battery.db` - SQLite 数据库文件

### 核心架构设计

#### 1. 协议层（protocols/）- 工厂模式 + 策略模式

**电池协议抽象**：
- `BatteryInterface` - 抽象基类，定义协议接口
  - `processSerialData()` - 处理原始数据
  - `getQueryCommand()` - 返回查询命令
  - `batteryDataProcessed(BMS_1)` - 数据处理完成信号
- `BatteryInterfaceFactory::createBattery()` - 根据产品类型创建实例
- `BMS1Battery` - BMS_1 协议具体实现（protocols/bms1battery.h:1）

**通信层抽象**：
- `CommunicationWorker` - 通信接口基类
  - `startCommunication()` / `stopCommunication()`
  - `sendControlCommand()` - 发送控制命令
  - `asQObject()` - 返回 QObject* 用于信号槽连接
- `CommunicationWorkerFactory::createWorker()` - 创建通信实例
  - `CommunicationType::Serial` → `SerialWorker`
  - `CommunicationType::TCP` → `ModbusTcpWorker`

**扩展方式**：
- 新增电池类型：
  1. 继承 `BatteryInterface` 实现协议解析
  2. 在 `BatteryInterfaceFactory::createBattery()` 注册类型
  3. 在 `utils/Struct.h` 定义数据结构
- 新增通信方式：
  1. 继承 `CommunicationWorker` 实现通信逻辑
  2. 在 `CommunicationWorkerFactory::createWorker()` 注册枚举

#### 2. 组件层（component/）- UI 组件和表单

**核心组件**：
- `BatteryListForm` - 单个电池项组件（component/batterylistform.h:1）
  - 使用 `std::enable_shared_from_this<BatteryListForm>` 支持智能指针管理
  - 管理通信线程、状态监控、充放电追踪
  - 充放电状态：`Unknown(-1)`, `Discharging(0)`, `Charging(1)`, `Idle(2)`
  - 监控状态：`Stopped(0)`, `Running(1)`, `Error(2)`
  - 状态缓存：`m_lastAlarmStatus`, `m_lastProtectStatus`, `m_lastFaultStatus`
- `BatteryGridWidget` - 网格布局容器，管理多个 `BatteryListForm` 实例
- `BMS1InfoShowForm` - 详细信息展示界面（切换详细视图）
- 记录查询表单：
  - `ChargeAndDischargeRecordForm` - 充放电历史
  - `ExceptionRecordForm` - 异常记录查询

#### 3. 数据管理（utils/）- 单例模式

**RecordManager**（utils/RecordManager.h:1）：
- `RecordManager::instance()` - 线程安全的单例实例
- `exception_record()` - 异常记录入库（power_id, site, type, info）
- `cfd_record()` - 充放电状态变化记录（power_id, site, status, start_time）

**myApp**（utils/myapp.h:1）- 全局配置类：
- 数据库配置：`HostName`, `Port`, `DatabaseName`, `UserName`, `Password`
- Modbus 配置：`ModbusIp`, `modbus_mode`（对接第三方系统）
- 日志管理：`LogFileCount`, `CurrentLogFileName`
- 配置操作：
  - `ReadConfig()` / `WriteConfig()` - 配置文件 I/O
  - `SaveSql()` - 配置持久化到数据库
  - `init_param()` - 初始化默认参数
- 告警配置：`alarm_ban`（一键屏蔽告警）

**日志管理**：
- `ManageLogFile()` - 日志文件轮换
- `outputMessage()` - 自定义消息处理器

#### 4. 主窗口管理

`MainWindow` 负责：
- 用户权限管理：`setUserPrivilege()`, `getUserPrivilege()`
- 界面切换：主视图（网格）、详细视图、历史记录、设置
- 动态布局：根据窗口状态调整边距（最大化/正常模式）
- 返回按钮逻辑：`updateWidget2Content()` 控制显示返回按钮或 Logo

### 信号槽机制

电池数据流：
```
CommunicationWorker (接收原始数据)
  → BatteryInterface::batteryDataProcessed(BMS_1)
    → BatteryListForm::onBatteryDataReceived()
      → 更新 UI + 状态检测 + 异常记录
      → dataReceived() 信号 → MainWindow 或其他组件
```

充放电状态变化：
```
BatteryListForm::determineChargeState() (根据 systemStatus 判断)
  → recordChargeStateChange() (状态改变时)
    → RecordManager::cfd_record()
    → chargeStateChanged() 信号
```

### 数据库设计

**SQLite 数据库**：`battery.db`

主要数据表：
- **用户表**：登录认证和权限管理
  - 字段：user_name, password_hash（MD5）, privilege（0=普通/1=高级/9=管理员）
  - 详见 `USER` 结构体（utils/Struct.h:287）
- **电池配置表**：存储 `battery_info` 结构
  - power_id, type（产品型号）, site（位置）
  - port_name, baud_rate, data_bits, stop_bits, parity（串口配置）
  - status（0=正常/1=停止/2=故障）, last_time（最后上报时间）
- **充放电记录表**：状态变化历史
  - power_id, site, status, start_time, duration
- **异常记录表**：告警/保护/故障记录
  - power_id, site, type（异常类型）, exception_info, timestamp

## 代码规范

- **C++ 标准**：严格使用 C++17（不使用 C++20/23 特性）
- **代码风格**：遵循 Google C++ 风格指南
  - 类名使用 PascalCase
  - 成员变量使用 `m_` 前缀
  - 常量使用 kCamelCase
- **字符编码**：UTF-8（MSVC 需添加 `/utf-8` 编译选项）
- **智能指针**：优先使用 `std::shared_ptr` 和 `std::unique_ptr`

## 关键实现细节

### 线程模型
- **通信线程分离**：所有通信操作（串口/TCP）在独立 QThread 中执行
- **UI 线程**：仅处理界面更新和用户交互
- **信号槽通信**：跨线程数据传递使用 Qt 信号槽机制（自动队列连接）

### 状态管理和去重
`BatteryListForm` 实现状态变化检测：
- 缓存上次状态：`m_lastAlarmStatus`, `m_lastProtectStatus`, `m_lastFaultStatus`
- **仅在状态变化时记录**，避免重复写入数据库
- 充放电状态变化立即调用 `RecordManager::cfd_record()` 持久化

### 异常检测逻辑
根据 `BMS_1` 结构体的状态字段判断：
- `alarmStatus`（告警状态，位标志，见附表1）
- `protectStatus`（保护状态，位标志，见附表2）
- `faultStatus`（故障状态，位标志，见附表3）
- `systemStatus`（系统状态，位标志，见附表4）
  - 用于判断充放电状态：bit0=放电，bit1=充电

### 通信超时和错误处理
- `communicationTimeout()` 信号：无响应时触发
- `communicationError(QString)` 信号：错误详情通知 UI
- 监控状态切换为 `Error(2)` 时停止通信并提示用户

### Modbus TCP 服务器模式
- 启用条件：`myApp::modbus_mode == true`
- 用途：对接第三方监控系统（如龙软大屏）
- 配置：`myApp::ModbusIp` 指定服务器监听地址
