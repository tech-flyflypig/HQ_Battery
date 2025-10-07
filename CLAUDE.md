# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

HQ_Battery 是一个基于 Qt 5.15.2 的电池管理系统（BMS）桌面应用程序，使用 C++17 开发。系统支持多种通信协议（串口、Modbus TCP）监控电池状态，提供实时数据显示、历史记录查询、告警管理等功能。

## 构建和运行

### 构建项目

使用 qmake 和 MSVC 2019 编译：

```bash
# 生成 Makefile
qmake HQ_Battery.pro

# 编译 Debug 版本
nmake debug

# 编译 Release 版本
nmake release
```

注意：
- 项目使用 Qt 5.15.2 和 MSVC 2019 64位编译器
- qmake 路径: `D:\Qt\5.15.2\msvc2019_64\bin\qmake.exe`
- Release 模式下会自动禁用 qDebug 输出（通过 QT_NO_DEBUG_OUTPUT 宏）
- 项目依赖：serialport, sql, xlsx, charts, network 模块

### 清理构建

```bash
nmake clean
```

## 代码架构

### 目录结构

- `component/` - UI 组件和表单（电池列表、统计面板、记录查询等）
- `protocols/` - 通信协议层，支持多种电池类型和通信方式
- `utils/` - 工具类（配置管理、记录管理、统计等）
- `mainwindow.h/cpp` - 主窗口，管理界面切换和用户权限
- `battery.db` - SQLite 数据库文件

### 核心架构设计

#### 1. 协议层（protocols/）

**工厂模式 + 策略模式**：
- `BatteryInterface` - 抽象电池协议接口
- `BatteryInterfaceFactory` - 工厂类，根据产品类型创建具体电池实例
- `BMS1Battery` - BMS_1 型号电池的具体实现
- `CommunicationWorker` - 通信接口基类
- `CommunicationWorkerFactory` - 创建串口或 TCP 通信实例
- `SerialWorker` - 串口通信实现
- `ModbusTcpWorker` - Modbus TCP 通信实现

新增电池类型时：
1. 继承 `BatteryInterface` 实现新协议
2. 在 `BatteryInterfaceFactory::createBattery()` 中注册
3. 定义对应的数据结构在 `utils/Struct.h`

新增通信方式时：
1. 继承 `CommunicationWorker`
2. 在 `CommunicationWorkerFactory::createWorker()` 中注册

#### 2. 组件层（component/）

**核心组件**：
- `BatteryListForm` - 电池项组件，使用 `std::enable_shared_from_this` 支持智能指针管理
  - 管理单个电池的通信、状态监控、充放电状态追踪
  - 充放电状态枚举：`Unknown(-1)`, `Discharging(0)`, `Charging(1)`, `Idle(2)`
  - 监控状态枚举：`Stopped(0)`, `Running(1)`, `Error(2)`
- `BatteryGridWidget` - 网格布局管理多个电池项
- `BMS1InfoShowForm` - 详细信息展示（切换到详细视图时）
- 记录表单：`ChargeAndDischargeRecordForm`, `ExceptionRecordForm`

#### 3. 数据管理（utils/）

**单例模式**：
- `RecordManager::instance()` - 统一管理异常记录和充放电记录
  - `exception_record()` - 记录异常到数据库
  - `cfd_record()` - 记录充放电状态变化
- `myApp` - 全局配置管理（数据库连接、Modbus IP、日志配置等）
  - `ReadConfig()` / `WriteConfig()` - 配置文件读写
  - `SaveSql()` - 保存到数据库

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

使用 SQLite (`battery.db`)，主要表：
- 用户表 - 登录和权限管理
- 电池配置表 - 存储 `battery_info` (ID, 地址, 型号, 通信类型等)
- 充放电记录表 - 时间戳、状态、持续时间
- 异常记录表 - 异常类型、信息、时间

## 代码规范

- **C++ 标准**：严格使用 C++17（不使用 C++20/23 特性）
- **代码风格**：遵循 Google C++ 风格指南
  - 类名使用 PascalCase
  - 成员变量使用 `m_` 前缀
  - 常量使用 kCamelCase
- **字符编码**：UTF-8（MSVC 需添加 `/utf-8` 编译选项）
- **智能指针**：优先使用 `std::shared_ptr` 和 `std::unique_ptr`

## 特殊注意事项

### 通信机制
- 所有通信工作在独立线程中，避免阻塞 UI
- 超时机制：通信无响应时触发 `communicationTimeout()` 信号
- 错误处理：通过 `communicationError()` 信号通知上层

### 状态管理
- `BatteryListForm` 缓存上次状态值避免重复记录异常：
  - `m_lastAlarmStatus`, `m_lastProtectStatus`, `m_lastFaultStatus`
- 充放电状态变化时立即记录到数据库（带时间戳）

### 日志管理
- 使用自定义日志函数 `outputMessage()`
- 日志文件自动轮换（由 `ManageLogFile()` 管理）
- Release 版本禁用 qDebug 输出

### Modbus TCP 模式
- 可选启用 Modbus TCP 服务器对接第三方系统（龙软大屏）
- 配置在 `myApp::modbus_mode` 和 `myApp::ModbusIp`
