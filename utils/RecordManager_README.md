# RecordManager 单例类使用说明

## 概述

`RecordManager` 是一个单例模式的记录管理器类，用于处理电池管理系统中异常记录和充放电记录的数据库操作。该类提供了统一的接口来记录电池异常信息和充放电状态变化。

## 功能特性

### 1. 异常记录功能 (`exception_record`)
- **防重复记录**：同一设备在1小时内相同类型的异常只记录一次
- **自动时间戳**：自动添加记录时间
- **WebSocket告警**：支持发送WebSocket告警消息
- **异常类型支持**：
  - 0: 一般异常
  - 1: 通讯异常  
  - 2: 电池异常

### 2. 充放电记录功能 (`cfd_record`)
- **自动时间计算**：自动计算充放电时间间隔
- **状态记录**：记录充电(1)和放电(0)状态
- **格式化输出**：时间间隔以"X天X时X分X秒"格式显示

## 使用方法

### 基本使用

```cpp
#include "utils/RecordManager.h"

// 获取单例实例
RecordManager* recordManager = RecordManager::instance();

// 记录异常
recordManager->exception_record("BAT001", "机房A", 1, "通讯超时");

// 记录充放电状态
QDateTime startTime = QDateTime::currentDateTime().addSecs(-3600);
recordManager->cfd_record("BAT001", "机房A", 1, startTime);
```

### 在通信模块中的集成

```cpp
// 在SerialWorker或ModbusTcpWorker中
void onCommunicationTimeout(const QString &powerId, const QString &site)
{
    RecordManager::instance()->exception_record(powerId, site, 1, "通信超时");
}

void onBatteryStatusChanged(const QString &powerId, const QString &site, 
                           int oldStatus, int newStatus, const QDateTime &startTime)
{
    if (oldStatus != newStatus)
    {
        RecordManager::instance()->cfd_record(powerId, site, newStatus, startTime);
    }
}
```

### WebSocket告警集成

```cpp
// 连接WebSocket信号
QObject::connect(RecordManager::instance(), &RecordManager::sig_ws_send,
                 this, &YourClass::handleWebSocketMessage);

// 处理WebSocket消息
void YourClass::handleWebSocketMessage(int type, const QString &request)
{
    if (type == 3) // 告警消息
    {
        // 处理告警消息
        qDebug() << "收到告警消息:" << request;
    }
}
```

## 数据库表结构

### power_exception_record 表
```sql
CREATE TABLE power_exception_record (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    power_id TEXT NOT NULL,           -- 设备ID
    site TEXT NOT NULL,               -- 安装地点
    exception_type INTEGER NOT NULL,  -- 异常类型 (0,1,2)
    exception_info TEXT NOT NULL,     -- 异常信息
    generate_time DATETIME NOT NULL   -- 生成时间
);
```

### power_status_record 表
```sql
CREATE TABLE power_status_record (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    power_id TEXT NOT NULL,           -- 设备ID
    site TEXT NOT NULL,               -- 安装地点
    charge_status INTEGER NOT NULL,   -- 充放电状态 (0=放电, 1=充电)
    start_time DATETIME NOT NULL,     -- 开始时间
    generate_time DATETIME NOT NULL,  -- 结束时间
    interval TEXT NOT NULL            -- 时间间隔
);
```

## 配置说明

### 运行模式控制
通过 `myApp::run_mode` 控制是否发送WebSocket告警消息：
- `run_mode = 0`: 不发送WebSocket消息
- `run_mode != 0`: 发送WebSocket告警消息

### 异常记录防重复机制
- 检查时间窗口：1小时（3600秒）
- 检查条件：相同设备ID + 相同异常类型 + 相同异常信息
- 如果1小时内已有相同异常记录，则跳过插入

## 注意事项

1. **线程安全**：RecordManager是单例模式，在多线程环境中使用需要注意线程安全
2. **数据库连接**：确保在使用前数据库连接已建立
3. **内存管理**：单例对象在程序结束时自动销毁，无需手动删除
4. **错误处理**：所有数据库操作都有错误日志输出，可通过qDebug查看

## 示例代码

完整的使用示例请参考 `RecordManagerExample.cpp` 文件。

## 版本信息

- 创建时间：2022-05-30
- 作者：Wei.Liu
- 版本：1.0.0 