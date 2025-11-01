# HALF 终端版本使用说明

## 概述
HALF (Hybrid Analysis Layer Framework) 终端版本是一个独立的网络数据包分析工具，基于原项目的C语言核心功能开发，移除了JNI依赖，可以直接在终端中使用。

## 功能特性
- 网络设备扫描和列表显示
- 实时数据包捕获和分析
- 以太网帧解析（MAC地址、类型、优先级）
- IP数据包解析（IP地址、版本、标志位、TTL等）
- 数据包内容十六进制和ASCII显示
- 支持优雅退出（Ctrl+C）

## 编译和运行

### 1. 安装依赖
```bash
# Ubuntu/Debian
sudo apt-get install libpcap-dev

# CentOS/RHEL
sudo yum install libpcap-devel
```

### 2. 编译
```bash
cd src
chmod +x compile_terminal.sh
./compile_terminal.sh
```

### 3. 运行
```bash
# 列出可用设备
./half_terminal -l

# 在指定设备上开始捕获（需要root权限）
sudo ./half_terminal -d eth0

# 显示帮助信息
./half_terminal -h
```

## 使用示例

### 列出网络设备
```bash
./half_terminal -l
```

输出示例：