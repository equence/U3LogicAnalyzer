# U3LogicAnalyzer

[中文文档](README_zh.md) | **English**

U3LogicAnalyzer is the PC host software for the CH32H417 / CH569 USB logic
analyzer, based on PulseView 0.5.0 with a trimmed sigrok stack
(libsigrok + libsigrokdecode). It supports 130 protocol decoders
(I2C / SPI / UART ...).

## Features

- CH32H417 (USB 3.0, up to 200 MHz sampling) and CH569 hot-plug detection
  - Windows: CH375 driver mode (CH375DLL)
  - macOS / Linux: libusb transport inside the driver, no kernel module
    required (protocol based on WCH's official CH37X_LINUX SDK)
- IAP firmware-upgrade dialog (HID mode on all platforms;
  CH375 mode is Windows-only)
- Protocol-decoder channel-assignment dialog
- Chinese UI, dark theme, new toolbar icons
- 130 protocol decoders
- Decoder auto-discovery - no `SIGROKDECODE_DIR` environment variable needed
- macOS .app / DMG packaging
- App name: U3LogicAnalyzer

## Complete flow (from scratch)

### Step 1 - Get the sources

The repository is self-contained: `libsigrok` (with the wch-ch32h417 driver
and the libusb transport) and `libsigrokdecode` (130 decoders) are included
in-tree:

```bash
git clone <repo-url> U3LogicAnalyzer
cd U3LogicAnalyzer
./build_macos.sh
```

The build scripts still accept `LIBSIGROK_SRC` / `LIBSIGROKDECODE_SRC` to
point at alternative source directories (or sibling dirs with the same names).

### Step 2 - Install dependencies

macOS (Homebrew):

```bash
brew install pkgconf cmake ninja qt@5 glibmm@2.66 glib libusb hidapi \
             libzip boost python@3.14
```

Linux (Debian/Ubuntu example):

```bash
sudo apt install build-essential cmake ninja-build pkg-config \
  qtbase5-dev qttools5-dev libqt5svg5-dev libglib2.0-dev \
  libglibmm-2.4-dev libusb-1.0-0-dev libhidapi-dev libzip-dev \
  libboost-dev python3-dev
```

### Step 3 - Build

```bash
./build_macos.sh            # output: build/pulseview_build/LogicAnalyzer
./build_macos.sh --clean    # full rebuild
```

### Step 4 - Run

```bash
./build_macos.sh --run
# or: ./build/pulseview_build/LogicAnalyzer
# hardware debugging: ./build/pulseview_build/LogicAnalyzer -l 5
```

Decoders are discovered automatically (no environment variables needed).
With a device attached, the log should show `Scan found 1 devices
(wch-ch32h417)` and continuous `Read ... bytes from pipe` lines.

### Step 5 (optional) - Package the macOS app / DMG

```bash
./package_app.sh
./package_app.sh --dmg
open build/U3LogicAnalyzer.dmg
```

## Windows

Build under MSYS2 MINGW64. The companion package's `README_CH32H417.md`
describes `setup_env.sh` / `build.sh`; the repository also ships
`build_windows.sh` which produces `build/LogicAnalyzer-win64.zip`
(exe + DLLs + decoders + run.bat).

## CI / releases

[`.github/workflows/build.yml`](.github/workflows/build.yml) builds macOS /
Linux / Windows on push to master and pull requests. Pushing a `v*` tag
(e.g. `git tag v1.1 && git push origin v1.1`) creates a GitHub Release with:

- `U3LogicAnalyzer.dmg` (macOS installer image)
- `U3LogicAnalyzer-linux.tar.gz` (Linux binary + decoders; needs system Qt5/glib)
- `LogicAnalyzer-win64.zip` (Windows portable package)

## Known limitations

- CH375 driver mode is Windows-only; macOS/Linux use the libusb transport.
  Capture with real hardware should be verified on the target platform.
- IAP CH375 mode is Windows-only; HID mode works on all platforms.
- CH32H417 hot-plug notification is a no-op on non-Windows platforms; the
  startup scan and the CH569 libusb polling thread cover device detection.
- The 16-channel mux zoom-buffer divide-by-zero crash has been fixed; verify
  with real hardware.

## Changes vs upstream PulseView

1. U3 customizations: renamed to U3LogicAnalyzer, Chinese UI, CH32H417/CH569
   support, IAP firmware-upgrade dialog, decoder-channel dialog, dark theme,
   new toolbar icons, zh_CN translations.
2. macOS / Linux enablement:
   - `build_macos.sh` / `build_windows.sh` / `package_app.sh`
   - libusb transport layer in `ch375_wrapper.c` (non-Windows), async buffered
     upload equivalent to the CH375DLL semantics
   - clang build fixes (missing return types, std::thread member pointer,
     unconditional Windows API usage, boost_system, -Wl,-Bdynamic)
   - crash fix: `unit_size_temp` divide-by-zero (16-channel case)
   - decoder auto-discovery
3. App renamed to U3LogicAnalyzer (.app bundle, Info.plist, window title).

## FAQ

- Decoder menu empty? Decoders are auto-discovered now; if it is still empty,
  check `build/install/share/libsigrokdecode/decoders` exists or set
  `SIGROKDECODE_DIR` and restart.
- Device not found? Run with `-l 5`; on macOS/Linux the driver uses libusb -
  confirm the device (VID 1a86, PID 5537) is visible to the OS.
- App fails to launch after packaging? Make sure the qt@5 `macdeployqt` is
  used (the script pins it) and no stale `build/LogicAnalyzer.app` remains.

## Licensing

The whole project, including the bundled `libsigrok` and `libsigrokdecode`
trees, is distributed under the GNU GPL v3 or later (GPLv3+); the
U3-specific driver code (wch-ch32h417, incl. the libusb transport) is
(C) Q2H2 under GPLv3+. Third-party resources (QDarkStyleSheet, DarkStyle,
QHexView, ExprTk, Tango icons) retain their own licenses and attributions -
see "Resource authors and licenses" in the upstream section below.

---

## Original upstream README

The sigrok project aims at creating a portable, cross-platform,
Free/Libre/Open-Source signal analysis software suite that supports various
device types (such as logic analyzers, oscilloscopes, multimeters, and more).

PulseView is a Qt-based LA/scope/MSO GUI for sigrok.

### Status

PulseView is in a usable state and has had official tarball releases.

### Copyright and license

PulseView is licensed under the terms of the GNU General Public License
(GPL), version 3 or later.

While some individual source code files are licensed under the GPLv2+, and
some files are licensed under the GPLv3+ or MIT, this doesn't change the fact
that the program as a whole is licensed under the terms of the GPLv3+ (e.g.
also due to the fact that it links against GPLv3+ libraries).

Please see the individual source files for the full list of copyright holders.

### Copyright notices

A copyright notice indicating a range of years, must be interpreted as having
had copyrightable material added in each of those years.

Example:

```text
Copyright (C) 2010-2013 Contributor Name
```

is to be interpreted as

```text
Copyright (C) 2010,2011,2012,2013 Contributor Name
```

### Resource authors and licenses

`icons/application-exit.png`, `icons/document-new.png`,
`icons/document-open.png`, `icons/document-save-as.png`,
`icons/edit-paste.svg`, `icons/help-browser.png`,
`icons/media-playback-pause.png`, `icons/media-playback-start.png`,
`icons/preferences-system.png`, `icons/search.svg`, `icons/window-new.png`,
`icons/zoom-fit-best.png`, `icons/zoom-in.png`, `icons/zoom-out.png`:
Tango Icon Library

- <http://tango.freedesktop.org/Tango_Desktop_Project>
- License: Public Domain

`icons/information.svg`: Bobarino

- <https://en.wikipedia.org/wiki/File:Information.svg>
- License: GFDL 1.2 or later / CC-BY-SA 3.0

`icons/add-math-channel.svg`: Inductiveload

- <https://en.wikipedia.org/wiki/File:Icon_Mathematical_Plot.svg>
- License: Public Domain

QDarkStyleSheet: Colin Duquesnoy

- <https://github.com/ColinDuquesnoy/QDarkStyleSheet>
- License: CC-BY 4.0

DarkStyle: Juergen Skrotzky

- <https://github.com/Jorgen-VikingGod/Qt-Frameless-Window-DarkStyle>
- License: MIT

QHexView: Victor Anjin

- <https://github.com/virinext/QHexView>
- License: MIT

ExprTk: Arash Partow

- <https://www.partow.net/programming/exprtk/index.html>
- License: MIT

### Mailing list

<https://lists.sourceforge.net/lists/listinfo/sigrok-devel>

### IRC

You can find the sigrok developers in the #sigrok IRC channel on Libera.Chat.

### Website

<http://sigrok.org/wiki/PulseView>
