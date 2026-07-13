# Qt C++ 接入示例

本目录只提供接入代码和项目配置，不要求 RaceBear SDK 开发机安装 Qt。建议第三方使用 Qt 5/Qt 6 的 MSVC 2019 x64 Kit，以便直接链接 SDK 提供的 `RaceBearSDK.lib`。

## 1. CMake 配置

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyRaceBearQtApp LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)

# 同时兼容 Qt 5 和 Qt 6，但必须选择 MSVC x64 Kit。
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)

set(RACEBEAR_SDK_ROOT "D:/SDK/RaceBearSDK")

add_executable(MyRaceBearQtApp
    main.cpp
    RaceBearBackend.cpp
    RaceBearBackend.h
)

# Qt 前端只包含公开头，不引用 SDK 的 src 或 SimRacebear 适配层。
target_include_directories(MyRaceBearQtApp PRIVATE
    "${RACEBEAR_SDK_ROOT}/include"
)

target_link_libraries(MyRaceBearQtApp PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    "${RACEBEAR_SDK_ROOT}/bin/x64/Release/RaceBearSDK.lib"
)

# 示例包含 UTF-8 无 BOM 中文注释，VS2019 必须显式使用 UTF-8 源码编码。
target_compile_options(MyRaceBearQtApp PRIVATE /utf-8)

# DLL 必须部署到 Qt EXE 同目录，否则程序启动时无法解析导入函数。
add_custom_command(TARGET MyRaceBearQtApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${RACEBEAR_SDK_ROOT}/bin/x64/Release/RaceBearSDK.dll"
        "$<TARGET_FILE_DIR:MyRaceBearQtApp>"
)
```

生成 VS2019 x64 工程：

```powershell
cmake -S . -B build -G "Visual Studio 16 2019" -A x64
cmake --build build --config Release
```

## 2. Qt 后端适配类

建议只创建一个 Qt 后端对象集中管理 SDK 生命周期。UI 页面不要各自初始化或关闭 SDK。

`RaceBearBackend.h`：

```cpp
#pragma once

#include "RaceBearSDK.h"

#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QTimer>

class RaceBearBackend final : public QObject
{
    Q_OBJECT

public:
    explicit RaceBearBackend(QObject* parent = nullptr);
    ~RaceBearBackend() override;

    // frequencyHz 单位为 Hz，不是毫秒；默认 100 表示目标周期约 10 ms。
    // 整个应用只调用一次 initialize/shutdown，页面之间共享该后端对象。
    bool initialize(const QString& appDataName, int frequencyHz = 100);
    void shutdown();

    // state() 返回最近一次定时器读取的副本，UI 不直接高频调用 DLL。
    const RB_RuntimeState& state() const { return state_; }

    // 目录/配置使用低频 JSON 接口；动作统一使用 Command 接口。
    QJsonObject readJson(const char* key, bool* ok = nullptr) const;
    QJsonObject runCommand(
        const char* command,
        const QJsonObject& args = QJsonObject(),
        bool* ok = nullptr) const;

signals:
    // 只传递刷新通知，页面收到后从 state() 读取同一份稳定快照。
    void stateChanged();
    void sdkLogReceived(const QString& text);
    void sdkError(const QString& text);

private slots:
    void refreshState();
    void drainLogs();

private:
    bool initialized_ = false;
    // 结构体只含 POD 字段，可以安全复制保存为 UI 当前快照。
    RB_RuntimeState state_{};
    QTimer stateTimer_;
    QTimer logTimer_;
};
```

`RaceBearBackend.cpp`：

```cpp
#include "RaceBearBackend.h"

#include <QByteArray>
#include <QJsonDocument>
#include <QVector>

RaceBearBackend::RaceBearBackend(QObject* parent)
    : QObject(parent)
{
    // 60 Hz 用于视觉状态刷新，日志无需同样高频。
    stateTimer_.setInterval(16); // 约 60 Hz UI 刷新
    logTimer_.setInterval(200);

    connect(&stateTimer_, &QTimer::timeout,
        this, &RaceBearBackend::refreshState);
    connect(&logTimer_, &QTimer::timeout,
        this, &RaceBearBackend::drainLogs);
}

RaceBearBackend::~RaceBearBackend()
{
    shutdown();
}

bool RaceBearBackend::initialize(
    const QString& appDataName,
    int frequencyHz)
{
    if (initialized_) {
        return true;
    }

    // SDK 所有 char* 文本均为 UTF-8，不能直接传 QString 内部 UTF-16 数据。
    const QByteArray appNameUtf8 = appDataName.toUtf8();
    if (RB_Runtime_SetAppDataName(appNameUtf8.constData()) != RB_OK) {
        emit sdkError(QStringLiteral("无效的应用目录名"));
        return false;
    }
    if (RB_Runtime_Initialize() != RB_OK) {
        emit sdkError(QStringLiteral("RaceBear SDK 初始化失败"));
        return false;
    }

    initialized_ = true;

    // frequencyHz 是每秒计算次数；100 Hz 对应约 10 ms 周期。
    // SDK 内部循环负责后端计算；Qt 定时器只读取结果，不能重复驱动计算。
    if (RB_Runtime_StartLoop(frequencyHz) != RB_OK) {
        shutdown();
        emit sdkError(QStringLiteral("RaceBear SDK 计算循环启动失败"));
        return false;
    }

    stateTimer_.start();
    logTimer_.start();
    return true;
}

void RaceBearBackend::shutdown()
{
    if (!initialized_) {
        return;
    }

    // 先阻止 Qt 定时器继续调用 SDK，再关闭 SDK。
    stateTimer_.stop();
    logTimer_.stop();
    RB_Runtime_Shutdown();
    initialized_ = false;
    state_ = {};
}

void RaceBearBackend::refreshState()
{
    // 先读入临时结构体，成功后再替换 UI 快照，失败时保留上一次有效状态。
    RB_RuntimeState newState{};
    if (RB_State_Read(&newState) != RB_OK) {
        return;
    }

    state_ = newState;
    emit stateChanged();
}

void RaceBearBackend::drainLogs()
{
    // 每次调用只弹出一条日志，读取到空字符串表示当前队列已清空。
    char buffer[4096]{};
    for (;;) {
        const int rc = RB_Log_Read(buffer, sizeof(buffer));
        if (rc != RB_OK || buffer[0] == '\0') {
            return;
        }
        emit sdkLogReceived(QString::fromUtf8(buffer));
    }
}

QJsonObject RaceBearBackend::readJson(const char* key, bool* ok) const
{
    if (ok != nullptr) {
        *ok = false;
    }

    // JSON 大小不固定，收到 RB_BUFFER_TOO_SMALL 后翻倍重试。
    QVector<char> buffer(64 * 1024);
    while (buffer.size() <= 16 * 1024 * 1024) {
        const int rc = RB_Data_ReadJson(key, buffer.data(), buffer.size());
        if (rc == RB_BUFFER_TOO_SMALL) {
            buffer.resize(buffer.size() * 2);
            continue;
        }
        if (rc != RB_OK) {
            return {};
        }

        // 使用 Qt JSON 解析器，不能用字符串截取读取字段。
        QJsonParseError error{};
        const QJsonDocument document = QJsonDocument::fromJson(
            QByteArray(buffer.data()), &error);
        if (error.error != QJsonParseError::NoError || !document.isObject()) {
            return {};
        }
        if (ok != nullptr) {
            *ok = true;
        }
        return document.object();
    }
    return {};
}

QJsonObject RaceBearBackend::runCommand(
    const char* command,
    const QJsonObject& args,
    bool* ok) const
{
    if (ok != nullptr) {
        *ok = false;
    }

    // QJsonDocument 负责正确转义用户输入，避免手工拼接产生非法 JSON。
    const QByteArray argsJson = QJsonDocument(args).toJson(
        QJsonDocument::Compact);
    QVector<char> result(4096);

    for (;;) {
        const int rc = RB_Command_Run(
            command,
            argsJson.constData(),
            result.data(),
            result.size());
        if (rc == RB_BUFFER_TOO_SMALL) {
            result.resize(result.size() * 2);
            continue;
        }
        if (rc != RB_OK) {
            return {};
        }

        const QJsonDocument document = QJsonDocument::fromJson(
            QByteArray(result.data()));
        if (!document.isObject()) {
            return {};
        }
        if (ok != nullptr) {
            *ok = true;
        }
        return document.object();
    }
}
```

## 3. 在窗口中使用

```cpp
backend_ = new RaceBearBackend(this);

// 后端只发出“状态已更新”通知；所有控件更新都在 Qt 主线程完成。
connect(backend_, &RaceBearBackend::stateChanged, this, [this]() {
    const RB_RuntimeState& state = backend_->state();
    gameNameLabel_->setText(QString::fromUtf8(state.Game.GameName));
    outputStateLabel_->setText(
        state.Output.AnyRuntimeActive
            ? QStringLiteral("已连接")
            : QStringLiteral("未连接"));
});

connect(backend_, &RaceBearBackend::sdkLogReceived,
    this, &MainWindow::appendSdkLog);

backend_->initialize(QStringLiteral("MyQtMotionApp"), 100); // 100 Hz，约 10 ms 周期。
```

连接输出：

```cpp
bool ok = false;
// 无参数命令传空 QJsonObject，序列化后就是 SDK 要求的 {}。
const QJsonObject result = backend_->runCommand(
    "output.connectAuto", QJsonObject(), &ok);
```

启动游戏：

```cpp
QJsonObject args;
// 使用游戏目录返回的稳定 gameKey，不要用界面显示名称代替。
args.insert("gameKey", selectedGameKey);
backend_->runCommand("game.launch", args);
```

## 4. qmake 配置

仍使用 qmake 的 Qt 5 项目可以加入：

```qmake
win32-msvc* {
    SDK_ROOT = D:/SDK/RaceBearSDK
    INCLUDEPATH += $$SDK_ROOT/include
    LIBS += -L$$SDK_ROOT/bin/x64/Release -lRaceBearSDK
    QMAKE_CXXFLAGS += /utf-8

    sdk_dll.files = $$SDK_ROOT/bin/x64/Release/RaceBearSDK.dll
    sdk_dll.path = $$OUT_PWD
    COPIES += sdk_dll
}
```

## 5. Qt MinGW

MinGW 不能直接链接 MSVC 格式的 `RaceBearSDK.lib`。有两个选择：

1. 推荐改用 Qt MSVC 2019 x64 Kit。
2. 使用 `QLibrary` 加载 `RaceBearSDK.dll`，并通过 `resolve()` 获取 `extern "C"` 导出函数。

动态加载示例：

```cpp
QLibrary library(QStringLiteral("RaceBearSDK.dll"));
if (!library.load()) {
    qWarning() << library.errorString();
    return;
}

// QLibrary::resolve 返回无类型地址，函数指针签名必须与公开头完全一致。
using InitializeFn = int (*)();
using ShutdownFn = void (*)();

auto initialize = reinterpret_cast<InitializeFn>(
    library.resolve("RB_Runtime_Initialize"));
auto shutdown = reinterpret_cast<ShutdownFn>(
    library.resolve("RB_Runtime_Shutdown"));

if (initialize == nullptr || shutdown == nullptr) {
    return;
}
```

使用动态加载时，仍需按 `RaceBearSDK.h` 定义全部结构体和函数指针。不要在 SDK 仍运行时卸载 `QLibrary`。

## 6. PySide/PyQt

PySide/PyQt 应直接复用 `examples/python/racebear_sdk.py`，然后使用 Qt `QTimer` 调用 `read_state()`。不要在工作线程或 SDK 回调中直接修改 Qt 控件，应发送 Qt Signal 回到主线程。

## 7. Qt 接入注意事项

- Qt、宿主程序和 RaceBear SDK 必须全部为 x64。
- 推荐使用 MSVC 2019 x64 Kit。
- SDK 文本通过 `QString::fromUtf8()` 转换。
- JSON 使用 `QJsonDocument` 和 `QJsonObject`，不要拼接 JSON 字符串。
- SDK 内部循环由 `RB_Runtime_StartLoop()` 驱动，Qt `QTimer` 只负责刷新 UI。
- 窗口退出前先停止所有 Qt 定时器和线程，再调用 `RB_Runtime_Shutdown()`。
- SDK 或串口回调不保证位于 Qt 主线程，必须通过信号槽转发。
