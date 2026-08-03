#!/bin/bash
# ============================================================================
# build_macos.sh - 在 macOS（或 Linux）上构建 CH32H417 LogicAnalyzer
#
# 依赖（Homebrew）:
#   brew install pkgconf cmake ninja qt@5 glibmm@2.66 glib libusb hidapi \
#                libzip boost python@3.14
#
# 源码获取: libsigrok / libsigrokdecode 随仓库分发（仓库内置）。
# 若目录缺失，解析顺序: 环境变量显式指定 → 仓库内目录 → 上一级目录 →
# 自动从 GitHub clone（可用 LIBSIGROK_URL / LIBSIGROKDECODE_URL 覆盖）。
#
# 用法:
#   ./build_macos.sh              # 构建（增量）
#   ./build_macos.sh --clean      # 全量重编
#   ./build_macos.sh --run        # 构建后启动 LogicAnalyzer
# ============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PARENT_DIR="$(dirname "$SCRIPT_DIR")"
LIBSIGROK_URL="${LIBSIGROK_URL:-https://github.com/equence/libsigrok.git}"
LIBSIGROKDECODE_URL="${LIBSIGROKDECODE_URL:-https://github.com/equence/libsigrokdecode.git}"

# 源码解析顺序：环境变量显式指定 → 仓库内目录 → 上一级目录 → 自动 clone
resolve_sigrok_src() {
	local var="$1" dirname="$2" url="$3" current
	eval current=\${$var:-}
	if [ -n "$current" ] && [ -d "$current" ]; then
		return
	fi
	if [ -d "$SCRIPT_DIR/$dirname" ]; then
		eval $var="$SCRIPT_DIR/$dirname"
		return
	fi
	if [ -d "$PARENT_DIR/$dirname" ]; then
		eval $var="$PARENT_DIR/$dirname"
		echo "[提示] 使用 $PARENT_DIR/$dirname（上一级目录）"
		return
	fi
	echo "==> 从 $url 获取 $dirname ..."
	if ! git clone --depth 1 "$url" "$SCRIPT_DIR/$dirname" >/dev/null 2>&1; then
		echo "[错误] 无法从 $url 获取 $dirname"
		echo "配套的 $dirname 需要包含 wch-ch32h417 驱动和 CMake 构建文件，"
		echo "官方 sigrokproject 上游无法直接使用。请："
		echo "  1) 把配套 $dirname 源码推到你的 GitHub（如 equence/$dirname），或"
		echo "  2) 设置环境变量 $var 指向本地源码目录"
		exit 1
	fi
	eval $var="$SCRIPT_DIR/$dirname"
}

resolve_sigrok_src LIBSIGROK_SRC libsigrok "$LIBSIGROK_URL"
resolve_sigrok_src LIBSIGROKDECODE_SRC libsigrokdecode "$LIBSIGROKDECODE_URL"

# 校验配套驱动存在
if [ ! -f "$LIBSIGROK_SRC/src/hardware/wch-ch32h417/api.c" ]; then
	echo "[错误] $LIBSIGROK_SRC 缺少 wch-ch32h417 驱动，这不是配套的 libsigrok 源码"
	exit 1
fi
# 构建产物放在项目目录下（.gitignore 已忽略 build/）
BUILD_DIR="${BUILD_DIR:-$SCRIPT_DIR/build}"
PREFIX="$BUILD_DIR/install"

CLEAN=0
RUN=0
for arg in "$@"; do
    case $arg in
        --clean) CLEAN=1 ;;
        --run) RUN=1 ;;
    esac
done

JOBS="$(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)"

echo "============================================"
echo " LogicAnalyzer (macOS/Linux) Build"
echo "============================================"
echo "Repo:        $SCRIPT_DIR"
echo "libsigrok:   $LIBSIGROK_SRC"
echo "libsigrokdecode: $LIBSIGROKDECODE_SRC"
echo "Build:       $BUILD_DIR"
echo ""

if ! command -v pkg-config >/dev/null 2>&1; then
    echo "[错误] 缺少 pkg-config，请先执行: brew install pkgconf"
    exit 1
fi

# 平台相关的 pkg-config / CMake 搜索路径
EXTRA_PKGCONFIG=""
EXTRA_CMAKE_PREFIX=""
if [ "$(uname -s)" = "Darwin" ]; then
    for p in /opt/homebrew/opt/qt@5 /opt/homebrew/opt/glibmm@2.66; do
        if [ -d "$p" ]; then
            EXTRA_PKGCONFIG="$EXTRA_PKGCONFIG:$p/lib/pkgconfig"
            EXTRA_CMAKE_PREFIX="$EXTRA_CMAKE_PREFIX;$p"
        fi
    done
fi
export PKG_CONFIG_PATH="$PREFIX/lib/pkgconfig${EXTRA_PKGCONFIG}:/opt/homebrew/lib/pkgconfig:/usr/local/lib/pkgconfig"
export CMAKE_PREFIX_PATH="$PREFIX$EXTRA_CMAKE_PREFIX"

COMMON_OPTS=(
    -DCMAKE_INSTALL_PREFIX="$PREFIX"
    -DCMAKE_BUILD_TYPE=Release
    -DDISABLE_WERROR=ON
)

if [ $CLEAN -eq 1 ]; then
    echo "[CLEAN] 删除 $BUILD_DIR ..."
    rm -rf "$BUILD_DIR"
fi
mkdir -p "$BUILD_DIR"

# ============================================================================
# Step 1/4: libsigrok（含 wch-ch32h417 驱动；非 Windows 用 libusb 传输层）
# ============================================================================
echo ""
echo "===== Step 1/4: libsigrok ====="
cmake -S "$LIBSIGROK_SRC" -B "$BUILD_DIR/libsigrok_build" "${COMMON_OPTS[@]}"
cmake --build "$BUILD_DIR/libsigrok_build" -j"$JOBS"
cmake --install "$BUILD_DIR/libsigrok_build"
echo "[OK] libsigrok"

# ============================================================================
# Step 2/4: libsigrokcxx（C++ 绑定）
# ============================================================================
echo ""
echo "===== Step 2/4: libsigrokcxx ====="
cmake -S "$LIBSIGROK_SRC/bindings/cxx" -B "$BUILD_DIR/libsigrokcxx_build" \
    "${COMMON_OPTS[@]}"
cmake --build "$BUILD_DIR/libsigrokcxx_build" -j"$JOBS"
cmake --install "$BUILD_DIR/libsigrokcxx_build"
echo "[OK] libsigrokcxx"

# ============================================================================
# Step 3/4: libsigrokdecode（130 个协议解码器）
# ============================================================================
echo ""
echo "===== Step 3/4: libsigrokdecode ====="
cmake -S "$LIBSIGROKDECODE_SRC" -B "$BUILD_DIR/libsigrokdecode_build" \
    "${COMMON_OPTS[@]}"
cmake --build "$BUILD_DIR/libsigrokdecode_build" -j"$JOBS"
cmake --install "$BUILD_DIR/libsigrokdecode_build"
echo "[OK] libsigrokdecode ($(ls "$PREFIX/share/libsigrokdecode/decoders" 2>/dev/null | wc -l | tr -d ' ') 个解码器)"

# ============================================================================
# Step 4/4: LogicAnalyzer（本仓库）
# ============================================================================
echo ""
echo "===== Step 4/4: LogicAnalyzer ====="
cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR/pulseview_build" \
    "${COMMON_OPTS[@]}" \
    -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
    -DENABLE_DECODE=ON \
    -DENABLE_FLOW=OFF \
    -DENABLE_TESTS=OFF \
    -DENABLE_SIGNALS=OFF \
    -DSTATIC_PKGDEPS_LIBS=OFF
cmake --build "$BUILD_DIR/pulseview_build" -j"$JOBS"
echo "[OK] LogicAnalyzer"

echo ""
echo "============================================"
echo " 构建完成: $BUILD_DIR/pulseview_build/LogicAnalyzer"
echo "============================================"

if [ $RUN -eq 1 ]; then
    export SIGROKDECODE_DIR="$PREFIX/share/libsigrokdecode/decoders"
    export DYLD_LIBRARY_PATH="$PREFIX/lib"
    cd "$BUILD_DIR/pulseview_build"
    echo "启动 LogicAnalyzer ..."
    ./LogicAnalyzer "$@"
fi
