# Lua Script Engine

## How to Run

### 一键启动 (推荐)

macOS / Linux:
```bash
./run.sh           # 执行脚本后退出
./run.sh -i        # 交互模式，可输入 Lua 代码
./run.sh -d        # 调试模式，显示详细日志
```

Windows:
```cmd
run.bat
run.bat -i
```

脚本会自动检测并安装缺失的依赖（cmake、lua 等）。

### Docker 启动
```bash
docker-compose up --build -d
```

查看日志:
```bash
docker-compose logs -f lua-engine
```

停止:
```bash
docker-compose down
```

### 本地启动

依赖安装 (macOS):
```bash
brew install lua pkg-config
```

依赖安装 (Ubuntu/Debian):
```bash
sudo apt-get install liblua5.4-dev pkg-config
```

编译运行:
```bash
cd backend
mkdir build && cd build
cmake ..
make
./lua_engine ../scripts
```

命令行参数:
```bash
./lua_engine [script_dir] [-i|--interactive] [-d|--debug]

# 示例
./lua_engine scripts -i      # 交互模式
./lua_engine -d              # 调试日志级别
```

配置文件 `engine.conf`:
```ini
script_dir=scripts
log_level=info
interactive=false
```

### 运行单元测试
```bash
cd backend/build
cmake -DBUILD_TESTS=ON ..
make
ctest --output-on-failure
```

## Services

| 服务 | 描述 |
|------|------|
| lua-engine | Lua 脚本引擎，加载并执行 scripts 目录下的 .lua 文件 |

## 测试账号

本项目为纯后端脚本引擎，无需账号认证。

## 题目内容

建个cpp工程，实现一个lua脚本的引擎，可以通过编写lua脚本文件扩展程序功能

---

## 项目介绍

基于 C++ 和 Lua 5.4 的脚本引擎，支持:

- 加载执行 Lua 脚本
- C++ 原生函数注册到 Lua
- 从 C++ 调用 Lua 函数
- 跨平台 Docker 镜像 (ARM64/AMD64)

### 内置原生函数

| 函数 | 说明 |
|------|------|
| `native_print(msg)` | 打印消息 |
| `native_add(a, b)` | 返回 a + b |
| `native_timestamp()` | 返回当前时间戳(ms) |
| `native_sleep(ms)` | 休眠指定毫秒 |

### 扩展方式

在 `backend/scripts/` 目录添加 `.lua` 文件即可自动加载执行。

### 示例脚本

| 脚本 | 功能 |
|------|------|
| `demo.lua` | 基础演示 |
| `config_loader.lua` | 配置管理模块 |
| `string_utils.lua` | 字符串工具函数 |
| `task_scheduler.lua` | 任务调度器 |
| `event_system.lua` | 事件发布/订阅系统 |
| `data_validator.lua` | 数据验证器 |
