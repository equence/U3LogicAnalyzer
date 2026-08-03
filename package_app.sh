#!/bin/bash
# ============================================================================
# package_app.sh - 把构建产物打包成 macOS 的 LogicAnalyzer.app
#
# 用法: 先 ./build_macos.sh，再 ./package_app.sh
# 产物: build/LogicAnalyzer.app（可拖入 /Applications 或直接运行）
# 加 --dmg 参数还会生成 build/U3LogicAnalyzer.dmg 安装镜像
# ============================================================================

set -e

MAKE_DMG=0
for arg in "$@"; do
	case "$arg" in
		--dmg) MAKE_DMG=1 ;;
	esac
done

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${BUILD_DIR:-$SCRIPT_DIR/build}"
BIN="$BUILD_DIR/pulseview_build/LogicAnalyzer"
PREFIX="$BUILD_DIR/install"
APP="$BUILD_DIR/U3LogicAnalyzer.app"
CONTENTS="$APP/Contents"
MACOS_DIR="$CONTENTS/MacOS"
FRAMEWORKS="$CONTENTS/Frameworks"
RESOURCES="$CONTENTS/Resources"

if [ ! -x "$BIN" ]; then
	echo "[错误] 找不到 $BIN，请先运行 ./build_macos.sh"
	exit 1
fi
if [ "$(uname -s)" != "Darwin" ]; then
	echo "[错误] 打包仅支持 macOS"
	exit 1
fi

# 必须用 qt@5 自带的 macdeployqt；机器上若同时装有 Qt6，
# PATH 里的 macdeployqt 很可能是 Qt6 版本，会把 Qt6 插件打进 app。
MACDEPLOYQT="/opt/homebrew/opt/qt@5/bin/macdeployqt"
[ -x "$MACDEPLOYQT" ] || MACDEPLOYQT="$(command -v macdeployqt 2>/dev/null)"
if [ -z "$MACDEPLOYQT" ]; then
	echo "[错误] 找不到 macdeployqt，请安装 qt@5"
	exit 1
fi

echo "============================================"
echo " 打包 U3LogicAnalyzer.app"
echo "============================================"

echo "==> 清理旧的 .app"
rm -rf "$APP"
rm -rf "$BUILD_DIR/LogicAnalyzer.app"   # 旧名遗留清理
mkdir -p "$MACOS_DIR" "$FRAMEWORKS" "$RESOURCES"

echo "==> 复制主程序"
cp "$BIN" "$MACOS_DIR/LogicAnalyzer"

echo "==> 复制协议解码器"
cp -R "$PREFIX/share/libsigrokdecode/decoders" "$RESOURCES/decoders"
echo "    $(ls "$RESOURCES/decoders" | wc -l | tr -d ' ') 个解码器"

echo "==> 生成应用图标"
ICONSET="$BUILD_DIR/AppIcon.iconset"
rm -rf "$ICONSET"
mkdir -p "$ICONSET"
for s in 16 32 128 256 512; do
	sips -z "$s" "$s" "$SCRIPT_DIR/icons/pulseview.png" \
		--out "$ICONSET/icon_${s}x${s}.png" >/dev/null 2>&1
	sips -z "$((s*2))" "$((s*2))" "$SCRIPT_DIR/icons/pulseview.png" \
		--out "$ICONSET/icon_${s}x${s}@2x.png" >/dev/null 2>&1
done
iconutil -c icns "$ICONSET" -o "$RESOURCES/AppIcon.icns"

echo "==> 生成 Info.plist"
cat > "$CONTENTS/Info.plist" <<'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>CFBundleDevelopmentRegion</key><string>zh_CN</string>
	<key>CFBundleDisplayName</key><string>U3LogicAnalyzer</string>
	<key>CFBundleExecutable</key><string>LogicAnalyzer</string>
	<key>CFBundleIconFile</key><string>AppIcon</string>
	<key>CFBundleIdentifier</key><string>com.equence.u3logicanalyzer</string>
	<key>CFBundleInfoDictionaryVersion</key><string>6.0</string>
	<key>CFBundleName</key><string>U3LogicAnalyzer</string>
	<key>CFBundlePackageType</key><string>APPL</string>
	<key>CFBundleShortVersionString</key><string>1.1</string>
	<key>CFBundleVersion</key><string>1.1</string>
	<key>LSMinimumSystemVersion</key><string>11.0</string>
	<key>NSHighResolutionCapable</key><true/>
	<key>NSPrincipalClass</key><string>NSApplication</string>
</dict>
</plist>
EOF

echo "==> 收集并复制非 Qt 动态库"

# Python 框架整体复制（libsigrokdecode 依赖它）
PY_FRAMEWORK="/opt/homebrew/opt/python@3.14/Frameworks/Python.framework"
if [ -d "$PY_FRAMEWORK" ]; then
	cp -R "$PY_FRAMEWORK" "$FRAMEWORKS/Python.framework"
	echo "    + Python.framework"
fi

copied_names=""
already_copied() {
	[ "${copied_names#*" $1 "}" != "$copied_names" ]
}
mark_copied() {
	copied_names="$copied_names $1 "
}

collect_lib() {
	local p="$1" name
	case "$p" in
		/usr/lib/*|/System/*) return ;;
		*/Qt*.framework/*) return ;;
		*/Python.framework/*) return ;;
		@executable_path/*|@loader_path/*) return ;;
	esac
	if [ ! -f "$p" ]; then
		# @rpath 依赖：从 install 前缀解析
		p="$PREFIX/lib/$(basename "$p")"
	fi
	[ -f "$p" ] || { echo "    [跳过] 无法解析 $1"; return; }
	name="$(basename "$p")"
	if already_copied "$name"; then
		return
	fi
	mark_copied "$name"
	cp -L "$p" "$FRAMEWORKS/$name"
	echo "    + $name"
	local dep
	while IFS= read -r dep; do
		collect_lib "$dep"
	done < <(otool -L "$p" | tail -n +2 | awk '{print $1}')
}

local_dep_list() {
	otool -L "$1" | tail -n +2 | awk '{print $1}'
}

# 从主程序开始收集
while IFS= read -r dep; do
	collect_lib "$dep"
done < <(local_dep_list "$MACOS_DIR/LogicAnalyzer")

echo "==> 重写动态库引用"

fix_dep_ref() {
	local f="$1" dep name
	while IFS= read -r dep; do
		case "$dep" in
			/usr/lib/*|/System/*) continue ;;
			*/Qt*.framework/*) continue ;;
		esac
		case "$dep" in
			/opt/homebrew/opt/python@3.14/Frameworks/*)
				install_name_tool -change "$dep" \
					"@loader_path/Python.framework/Versions/3.14/Python" "$f" 2>/dev/null
				continue ;;
		esac
		name="$(basename "$dep")"
		install_name_tool -change "$dep" "@loader_path/$name" "$f" 2>/dev/null || true
	done < <(local_dep_list "$f")
}

# 主程序：库引用指向 app 内 Frameworks
while IFS= read -r dep; do
	case "$dep" in
		/usr/lib/*|/System/*|*/Qt*.framework/*|@executable_path/*) continue ;;
	esac
	if [[ "$dep" == /opt/homebrew/opt/python@3.14/Frameworks/* ]]; then
		install_name_tool -change "$dep" \
			"@executable_path/../Frameworks/Python.framework/Versions/3.14/Python" \
			"$MACOS_DIR/LogicAnalyzer"
		continue
	fi
	name="$(basename "$dep")"
	install_name_tool -change "$dep" \
		"@executable_path/../Frameworks/$name" "$MACOS_DIR/LogicAnalyzer" 2>/dev/null || true
done < <(local_dep_list "$MACOS_DIR/LogicAnalyzer")

# Frameworks 内的库：互相之间用 @loader_path
for f in "$FRAMEWORKS"/*.dylib; do
	[ -f "$f" ] || continue
	fix_dep_ref "$f"
done

echo "==> macdeployqt（打包 Qt 框架与插件）"
"$MACDEPLOYQT" "$APP" -always-overwrite 2>&1 | grep -v "Cannot resolve rpath" | grep -v "using QList" || true

echo "==> 验证残留的绝对引用"
LEFTOVER="$(otool -L "$MACOS_DIR/LogicAnalyzer" | grep -E '/opt/homebrew|@rpath' | grep -v Qt || true)"
if [ -n "$LEFTOVER" ]; then
	echo "    [注意] 仍有绝对引用:"
	echo "$LEFTOVER" | sed 's/^/    /'
else
	echo "    主程序引用已全部指向 app 内"
fi

echo "==> 代码签名（ad-hoc）"
# 逐个签名内部二进制，最后再签 app 外壳（避免 --deep 对嵌套框架如
# Python.framework 的密封问题）
find "$FRAMEWORKS" "$CONTENTS/PlugIns" -name '*.dylib' -print0 | \
	while IFS= read -r -d '' f; do
		codesign --force --sign - "$f" >/dev/null 2>&1 || true
	done
if [ -f "$FRAMEWORKS/Python.framework/Versions/3.14/Python" ]; then
	codesign --force --sign - \
		"$FRAMEWORKS/Python.framework/Versions/3.14/Python" >/dev/null 2>&1 || true
fi
for fw in "$FRAMEWORKS"/*.framework; do
	[ -d "$fw" ] && codesign --force --sign - "$fw" >/dev/null 2>&1 || true
done
codesign --force --sign - "$APP"

echo ""
echo "============================================"
echo " 打包完成: $APP"
echo " 运行: open $APP"
echo " 或:  cp -R $APP /Applications/"
echo "============================================"

if [ $MAKE_DMG -eq 1 ]; then
	echo "==> 制作 DMG 安装镜像"
	DMG_STAGE="$BUILD_DIR/dmg-staging"
	DMG="$BUILD_DIR/U3LogicAnalyzer.dmg"
	rm -rf "$DMG_STAGE"
	mkdir -p "$DMG_STAGE"
	cp -R "$APP" "$DMG_STAGE/"
	ln -s /Applications "$DMG_STAGE/Applications"
	rm -f "$DMG"
	hdiutil create -volname "U3LogicAnalyzer" -srcfolder "$DMG_STAGE" \
		-ov -format UDZO "$DMG"
	rm -rf "$DMG_STAGE"
	echo ""
	echo "============================================"
	echo " DMG 完成: $DMG"
	echo " 打开: open $DMG"
	echo "============================================"
fi
