#!/bin/bash
# ============================================================================
# build_windows.sh - 在 MSYS2 MINGW64 下构建 Windows 版 LogicAnalyzer
#
# 依赖（MSYS2 MINGW64 shell 中执行）:
#   pacman -S --needed --noconfirm \
#       mingw-w64-x86_64-{gcc,cmake,ninja,pkgconf,gdb} \
#       mingw-w64-x86_64-{glib2,glibmm,libusb,hidapi,libzip} \
#       mingw-w64-x86_64-{boost,qt5,python,zip}
#
# 产物: build/LogicAnalyzer-win64.zip
#
# 源码获取: libsigrok / libsigrokdecode 随仓库分发（仓库内置）。
# 若目录缺失，解析顺序: 环境变量显式指定 → 仓库内目录 → 上一级目录 →
# 自动从 GitHub clone（可用 LIBSIGROK_URL / LIBSIGROKDECODE_URL 覆盖）。
# ============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PARENT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$SCRIPT_DIR/build/windows"
PREFIX="$BUILD_DIR/install"
PVDIR="$BUILD_DIR/pulseview_build"
JOBS="$(nproc 2>/dev/null || echo 4)"
LIBSIGROK_URL="${LIBSIGROK_URL:-https://github.com/equence/libsigrok.git}"
LIBSIGROKDECODE_URL="${LIBSIGROKDECODE_URL:-https://github.com/equence/libsigrokdecode.git}"

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

if [ ! -f "$LIBSIGROK_SRC/src/hardware/wch-ch32h417/api.c" ]; then
	echo "[错误] $LIBSIGROK_SRC 缺少 wch-ch32h417 驱动，这不是配套的 libsigrok 源码"
	exit 1
fi

if [ -z "$MSYSTEM" ] || [ "$MSYSTEM" != "MINGW64" ]; then
    echo "[错误] 请在 MSYS2 MINGW64 shell 中运行（开始菜单 → MSYS2 MINGW64）"
    exit 1
fi

export PATH="/mingw64/bin:$PATH"
export PKG_CONFIG_PATH="$PREFIX/lib/pkgconfig"

echo "============================================"
echo " LogicAnalyzer (Windows/MSYS2) Build"
echo "============================================"

COMMON_OPTS=(
    -DCMAKE_INSTALL_PREFIX="$PREFIX"
    -DCMAKE_BUILD_TYPE=Release
    -DDISABLE_WERROR=ON
    -DWIN32=ON
)

# ============================================================================
# Step 1/4: libsigrok（CH32H417 + Demo；Windows 使用 CH375 驱动模式）
# ============================================================================
echo ""
echo "===== Step 1/4: libsigrok ====="
cmake -S "$SCRIPT_DIR/libsigrok" -B "$BUILD_DIR/libsigrok_build" "${COMMON_OPTS[@]}"
cmake --build "$BUILD_DIR/libsigrok_build" -j"$JOBS"
cmake --install "$BUILD_DIR/libsigrok_build"
echo "[OK] libsigrok"

# ============================================================================
# Step 2/4: libsigrokcxx（C++ 绑定）
# ============================================================================
echo ""
echo "===== Step 2/4: libsigrokcxx ====="
cmake -S "$SCRIPT_DIR/libsigrok/bindings/cxx" -B "$BUILD_DIR/libsigrokcxx_build" \
    "${COMMON_OPTS[@]}"
cmake --build "$BUILD_DIR/libsigrokcxx_build" -j"$JOBS"
cmake --install "$BUILD_DIR/libsigrokcxx_build"
echo "[OK] libsigrokcxx"

# ============================================================================
# Step 3/4: libsigrokdecode（130 个协议解码器）
# ============================================================================
echo ""
echo "===== Step 3/4: libsigrokdecode ====="
cmake -S "$SCRIPT_DIR/libsigrokdecode" -B "$BUILD_DIR/libsigrokdecode_build" \
    "${COMMON_OPTS[@]}"
cmake --build "$BUILD_DIR/libsigrokdecode_build" -j"$JOBS"
cmake --install "$BUILD_DIR/libsigrokdecode_build"
echo "[OK] libsigrokdecode"

# ============================================================================
# Step 4/4: LogicAnalyzer（本仓库）
# ============================================================================
echo ""
echo "===== Step 4/4: LogicAnalyzer ====="
cmake -S "$SCRIPT_DIR" -B "$PVDIR" \
    "${COMMON_OPTS[@]}" \
    -DCMAKE_PREFIX_PATH="$PREFIX;/mingw64" \
    -DENABLE_DECODE=ON \
    -DENABLE_FLOW=OFF \
    -DENABLE_TESTS=OFF \
    -DENABLE_SIGNALS=OFF \
    -DSTATIC_PKGDEPS_LIBS=OFF
cmake --build "$PVDIR" -j"$JOBS"
echo "[OK] LogicAnalyzer.exe"

# ============================================================================
# 部署 DLL 与解码器，生成 run.bat
# ============================================================================
echo ""
echo "===== 部署运行环境 ====="
for dll in \
    libsigrok.dll libsigrokcxx.dll libsigrokdecode.dll \
    libglib-2.0-0.dll libglibmm-2.4-1.dll libgobject-2.0-0.dll \
    libsigc-2.0-0.dll libstdc++-6.dll libwinpthread-1.dll \
    libgcc_s_seh-1.dll libgmodule-2.0-0.dll libusb-1.0.dll libhidapi-0.dll \
    libintl-8.dll libiconv-2.dll libpcre2-8-0.dll libffi-8.dll \
    libzip.dll libbz2-1.dll liblzma-5.dll zlib1.dll libzstd.dll \
    libpython3.14.dll; do
    cp "$PREFIX/bin/$dll" "$PVDIR/" 2>/dev/null || \
    cp "/mingw64/bin/$dll" "$PVDIR/" 2>/dev/null || true
done

cp -R "$PREFIX/share/libsigrokdecode/decoders" "$PVDIR/decoders"

cat > "$PVDIR/run.bat" << 'RBEOF'
@echo off
cd /d "%~dp0"
set "PATH=%~dp0;%PATH%"
set "SIGROKDECODE_DIR=%~dp0decoders"
start "" "LogicAnalyzer.exe"
RBEOF

echo "  $(ls "$PVDIR"/*.dll 2>/dev/null | wc -l) 个 DLL, run.bat 已生成"

# ============================================================================
# 打包 zip
# ============================================================================
echo ""
echo "===== 打包 zip ====="
cd "$PVDIR"
rm -f "$SCRIPT_DIR/build/LogicAnalyzer-win64.zip"
zip -r "$SCRIPT_DIR/build/LogicAnalyzer-win64.zip" . > /dev/null

echo ""
echo "============================================"
echo " 构建完成: $PVDIR/LogicAnalyzer.exe"
echo " 安装包:   $SCRIPT_DIR/build/LogicAnalyzer-win64.zip"
echo "============================================"
