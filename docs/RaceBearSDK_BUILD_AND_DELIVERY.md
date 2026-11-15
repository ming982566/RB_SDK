# 构建与交付核对

当前正式交付版本为 1.0.4（Windows x64 / VS2019 v142），对象协议 v1，
十入口 ABI。正式包的 SDK_DEPENDENCIES.json 标记为 release；本地开发预览包
仍使用 development-preview，不能与 GitHub Release 附件混淆。

## 工具集范围

SDK DLL 的正式构建与回归固定使用 VS2019 Community、v142、x64。
第三方宿主不需要编译 SDK 私有源码。示例只使用标准公开结构和十入口对象 ABI，
不需要定义任何 LEGACY 宏。

| 宿主工具集 | 示例配置 | 验证级别 |
|---|---|---|
| VS2019 v142 x64 | Visual Studio 16 2019，-A x64 -T v142 | 官方 Debug/Release 验证基线 |
| VS2022 v143 x64 | Visual Studio 17 2022，-A x64 -T v143；或对应开发命令行 Ninja | 已收到第三方动态加载成功反馈；本轮不将其冒充官方双配置回归 |
| x86、ARM、其它编译器 | 不在当前 x64 交付范围 | 未验证 |

示例 CMake 允许 v142/v143。VS2022 可选 v142 工具集时仍可使用正式基线。
Ninja 单配置必须显式设置 CMAKE_BUILD_TYPE；Debug 和 Release 使用不同构建目录。

```powershell
cmake -S examples -B obj/examples-v143 -G "Visual Studio 17 2022" -A x64 -T v143
cmake --build obj/examples-v143 --config Release
# 在已初始化相应 x64 开发工具链的命令行中：
cmake -S examples -B obj/examples-ninja-release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build obj/examples-ninja-release
```

Debug 宿主部署 bin/x64/Debug 的 DLL/LIB/依赖；Release 宿主部署 Release。
头文件是同一次构建的唯一公开头，两种配置均使用它，不存在 Debug 专用头。
动态 LoadLibrary 宿主无需链接 LIB，但仍需校验 DLL 和头的版本、布局及目录能力；
DLL 依赖和资源部署要求不因此消失。Debug DLL 用于开发环境，不作为普通用户运行包。

## 清单与身份

交付根目录包含 SHA256SUMS.txt 和 SDK_DEPENDENCIES.json。
清单覆盖全部包内容（清单自身除外），包括头、两配置 DLL/LIB、libusb、文档、
示例和资源；SDK_DEPENDENCIES 记录版本、架构、配置、路径和关键文件哈希。
SHA256SUMS 包含 SDK_DEPENDENCIES 的哈希，清单不循环包含自身。

DLL 的 FileVersion/ProductVersion 与 RB_Runtime_GetVersion 一致。
版本相同不能证明同一次构建，仍须核对 SHA256；未加载前可读取版本资源，
加载后记录实际模块绝对路径与运行时版本。

在交付目录执行：

```powershell
powershell -NoProfile -File tools/Verify-SdkDelivery.ps1
```

清单提供包内部一致性基准，不是数字签名；对外发布还应核对 Release 附件的
ZIP SHA256，可信来源由发布渠道保证。开发预览包不是已正式发布版本。

## UI 参考与素材

SDK 提供功能合同和 C++ 参考实现，不提供 SimRacebear 专有 UI 图标或品牌素材授权。
第三方自行选择有使用许可的图标库；添加、删除、复制、上下移动等采用通用图标，
提供工具提示与可访问名称，不要求视觉复制官方前端。
随包 DashboardAssets 是仪表运行资源，不是控制面板图标库。
参考截图见 [界面参考](RaceBearSDK_UI_REFERENCE.md)，术语见 [glossary.csv](glossary.csv)。
