# U3LogicAnalyzer（CH32H417 逻辑分析仪上位机）

[English](README.md) | **中文**

U3LogicAnalyzer 是配套 CH32H417 / CH569 USB 逻辑分析仪的 PC 上位机软件，基于
PulseView 0.5.0 修改，内置裁剪后的 sigrok 软件栈（libsigrok + libsigrokdecode），
支持 **130 种协议解码**（I²C / SPI / UART 等）。

## 功能特性

- **硬件支持**：CH32H417（USB3.0，最高 200MHz 采样）、CH569 热插拔检测
  - Windows：原厂 CH375 驱动模式（CH375DLL）
  - macOS / Linux：驱动内置 **libusb 传输层**，无需任何内核驱动（协议参考
    WCH 官方 CH37X_LINUX SDK）
- **固件升级**：IAP 对话框（HID 模式全平台可用；CH375 模式仅 Windows）
- **解码通道分配**对话框
- 中文界面、深色主题、全新工具栏图标
- 130 个协议解码器
- **解码器自动发现**：无需设置 `SIGROKDECODE_DIR` 环境变量
- macOS 应用打包（.app）
- 应用名：U3LogicAnalyzer

## 构建流程

### 第 1 步：获取源码（仓库自包含）

sigrok 后端 `libsigrok`（含 wch-ch32h417 驱动与 libusb 传输层）和协议解码器
`libsigrokdecode`（130 个）**随仓库一同分发**，clone 后即可构建：

```bash
git clone <仓库地址> U3LogicAnalyzer
cd U3LogicAnalyzer
./build_macos.sh
```

构建脚本仍支持环境变量 `LIBSIGROK_SRC` / `LIBSIGROKDECODE_SRC` 指向其他
源码目录，或在前一级目录放置同名目录，便于复用自定义版本。

目录结构：

```
U3LogicAnalyzer/
├── main.cpp / CMakeLists.txt / pv/  # 上位机源码
├── libsigrok/                       # sigrok 后端（含 wch-ch32h417 驱动）
├── libsigrokdecode/                 # 130 个协议解码器
├── build_macos.sh                   # macOS / Linux 构建脚本
├── build_windows.sh                 # Windows（MSYS2）构建脚本
├── package_app.sh                   # macOS .app / DMG 打包
└── .github/workflows/build.yml      # CI：三平台构建 + tag 发布
```

### 第 2 步：安装构建依赖

macOS（Homebrew）：

```bash
brew install pkgconf cmake ninja qt@5 glibmm@2.66 glib libusb hidapi \
             libzip boost python@3.14
```

Linux（Debian/Ubuntu 示例，包名以发行版为准）：

```bash
sudo apt install build-essential cmake ninja-build pkg-config \
  qtbase5-dev qttools5-dev libqt5svg5-dev \
  libglib2.0-dev libglibmm-2.4-dev libusb-1.0-0-dev \
  libhidapi-dev libzip-dev libboost-dev libboost-filesystem-dev \
  libboost-serialization-dev python3-dev
```

### 第 3 步：一键构建

```bash
cd U3LogicAnalyzer
./build_macos.sh            # 构建（产物在 build/，已被 .gitignore 忽略）
./build_macos.sh --clean    # 全量重编（改过驱动或 CMake 后建议先 clean）
```

脚本按顺序构建并安装到 `build/install/`：libsigrok（含 wch-ch32h417 驱动）
→ libsigrokcxx → libsigrokdecode（130 个解码器）→ LogicAnalyzer。

产物：`build/pulseview_build/LogicAnalyzer`

### 第 4 步：运行

```bash
./build_macos.sh --run      # 构建后直接启动
# 或手动：
./build/pulseview_build/LogicAnalyzer
```

解码器会自动发现（优先 .app 内 `Contents/Resources/decoders`，其次安装前缀、
构建树），无需任何环境变量。

真机验证建议带日志级别运行：

```bash
./build/pulseview_build/LogicAnalyzer -l 5
```

正常时日志应出现 `Scan found 1 devices (wch-ch32h417)`，采集时出现连续的
`Read ... bytes from pipe`。

### 第 5 步（可选）：打包 macOS 应用与 DMG

```bash
./package_app.sh           # 生成 build/U3LogicAnalyzer.app
./package_app.sh --dmg     # 再生成 build/U3LogicAnalyzer.dmg
open build/U3LogicAnalyzer.dmg   # 打开后拖到 Applications 即完成安装
```

`build/U3LogicAnalyzer.app` 完全自包含（Qt5 框架、sigrok 库及其全部依赖、
Python 运行时、130 个解码器、应用图标），可在 Finder 中双击运行。
`build/U3LogicAnalyzer.dmg`（约 70-80MB）内含 app 和 /Applications 快捷链接。

### Windows 构建

Windows 使用 MSYS2 MINGW64 环境，完整流程见配套发布包中的
`README_CH32H417.md`：

```bash
./setup_env.sh             # 首次部署编译环境
./build.sh                 # 编译（或 ./build.sh --package 打包）
cd build_cmake/logicanalyzer_build && ./LogicAnalyzer.exe   # 运行
```

也可以直接用仓库内的 `build_windows.sh`（依赖用 pacman 安装，见脚本头部
注释），产物为 `build/LogicAnalyzer-win64.zip`（exe + DLL + 解码器 + run.bat）。

## 持续集成与发布（GitHub Actions）

`.github/workflows/build.yml` 会在以下时机自动构建 macOS / Linux / Windows
三个平台版本：

- push 到 `master` 或提交 PR：构建并上传构建产物（Actions artifacts）
- 推送 `v*` 标签（如 `git tag v1.1 && git push origin v1.1`）：构建后自动创建
  GitHub Release，附带三个平台的安装包：
  - `U3LogicAnalyzer.dmg`（macOS 安装镜像，拖拽安装）
  - `U3LogicAnalyzer-linux.tar.gz`（Linux 二进制 + 解码器，需系统 Qt5/glib 等依赖）
  - `LogicAnalyzer-win64.zip`（Windows 免安装包：exe + DLL + 解码器 + run.bat）

## 已知限制

- `wch-ch32h417` 驱动的 CH375 驱动模式仅 Windows；macOS / Linux 使用
  libusb 传输层，**实际采集功能需在真机上验证**（扫描、配置、连续采集、IAP）。
- IAP 的 CH375 模式仅 Windows；HID 模式全平台可用。
- CH32H417 热插拔通知在非 Windows 平台为 no-op，由启动扫描和 CH569 的
  libusb 轮询线程兜底。
- 16 通道模式下解码多路复用（mux）的缩放缓冲已修复除零崩溃，建议真机验证。

## 相对上游 PulseView 的改动摘要

1. **U3 定制全量同步**：品牌改名为 U3LogicAnalyzer、中文界面、CH32H417/CH569
   设备支持、IAP 固件升级对话框、解码通道分配对话框、深色主题、新图标、
   中文本地化翻译。
2. **macOS / Linux 适配**：
   - `build_macos.sh` 一键构建（libsigrok → libsigrokcxx → libsigrokdecode →
     LogicAnalyzer）
   - `package_app.sh` 打包 .app（macdeployqt + 依赖闭包 + ad-hoc 签名）
   - libsigrok 的 `ch375_wrapper.c` 新增非 Windows 的 libusb 传输层（异步
     缓冲上传模式，等价 Windows CH375DLL 语义）
   - 修复 clang 编译问题：缺返回类型声明、`std::thread` 成员函数指针、
     Windows API 无条件引用、`boost_system` 组件、`-Wl,-Bdynamic` 等
   - 修复崩溃：`unit_size_temp` 除零（16 通道场景）
   - 解码器自动发现（`main.cpp` 自动探测解码器目录；`libsigrokdecode` 的
     编译期 `DECODERS_DIR` 放开到所有平台）
3. 应用名统一为 **U3LogicAnalyzer**（.app 包名、Info.plist、窗口标题）。

## 常见问题

- **解码器菜单是空的？** 现在已自动发现解码器；若仍为空，检查
  `build/install/share/libsigrokdecode/decoders` 是否存在，或设置
  `SIGROKDECODE_DIR` 指向它后重启。
- **插上设备但扫描不到？** 先 `./LogicAnalyzer -l 5` 看日志；macOS/Linux 下
  驱动为 libusb 传输层，确认 `lsusb`/`system_profiler SPUSBDataType` 能看到
  1a86:5537。
- **打包后无法启动？** 确认使用的是 qt@5 的 macdeployqt（脚本已固定），且
  机器上没有残留的旧 `build/LogicAnalyzer.app`。

## 版权与许可

本项目基于 PulseView（sigrok 项目）修改，遵循 GNU 通用公共许可证（GPL）
第 3 版或更高版本（GPLv3+）。个别源码文件采用 GPLv2+、GPLv3+ 或 MIT 许可；
由于程序整体链接 GPLv3+ 库，程序整体按 GPLv3+ 授权。

随仓库分发的 `libsigrok` 与 `libsigrokdecode` 同样为 GPLv3+（树内均含
COPYING 文件）；其中新增的 wch-ch32h417 驱动（含 libusb 传输层）版权归
Q2H2，文件头已标注 GPLv3 许可。

请参阅各源码文件头部了解完整版权信息。第三方资源（Tango 图标、
QDarkStyleSheet、DarkStyle、QHexView、ExprTk 等）的作者与许可信息保留在
英文 [README.md](README.md) 的 "Resource authors and licenses" 一节。

sigrok 项目主页：<https://sigrok.org/wiki/PulseView>
