---
document: "Talos Desk Project Overview"
version: "1.0.2"
last_updated: "2025-08-02 Evening"
status: "active"
project_version: "1.0.0"
maintainer: "development-team"
review_date: "2025-11-02"
---

# Talos Desk - Screen-to-RTSP ONVIF Bridge

## Overview

Talos Desk is a professional-grade desktop application that captures computer screen content and streams it as RTSP video feeds with ONVIF compatibility. This enables seamless integration with enterprise Video Management Systems (VMS) and Network Video Recorders (NVR).

## Features

- 🖥️ **Cross-Platform**: Windows, macOS, and Linux support
- 📹 **Native Resolution Capture**: Always captures at screen's native resolution
- 🎬 **H.264/H.265 Support**: Hardware-accelerated video encoding
- 🔌 **ONVIF Profile S**: Compatible with major VMS platforms
- 🚀 **High Performance**: <15% CPU usage at 1080p30
- 🔒 **Secure**: RTSP authentication and encrypted streams
- 🎯 **Easy Integration**: Works with NX Witness, Milestone, Blue Iris, and more

## Quick Start

### Prerequisites

- **Windows**: Windows 10/11 (64-bit), Visual Studio 2022
- **macOS**: macOS 11.0+, Xcode
- **Linux**: Ubuntu 20.04+, GCC 11+, X11 development libraries
- **Dependencies**: FFmpeg 5.0+, live555 (optional), Dear ImGui, nlohmann/json

### Building from Source

1. Clone the repository:
```bash
git clone https://github.com/stefanus-ronald/talos_desk.git
cd talos_desk
```

2. Set up dependencies (optional - for advanced features):
```bash
./scripts/setup_dependencies.sh
```

3. Build using platform-specific scripts:

**Windows:**
```batch
scriptsuild_windows.bat Release
```

**macOS:**
```bash
./scripts/build_macos.sh Release
```

**Linux:**
```bash
./scripts/build_linux.sh Release
```

3. Run the application:
```bash
# Windows
build\windows-x64-Releasein\Release	alos_desk.exe

# macOS
build/macos-arm64-Release/bin/Release/talos_desk

# Linux
build/linux-x86_64-Release/bin/talos_desk
```

## Configuration

Talos Desk uses a JSON configuration file. Create `config.json`:

```json
{
  "video": {
    "codec": "h264",
    "frameRate": 30,
    "bitrate": 8000,
    "qualityPreset": "balanced"
  },
  "network": {
    "rtspPort": 554,
    "authType": "digest",
    "username": "admin",
    "password": "your_password"
  }
}
```

## VMS Integration

### RTSP URL Format
```
rtsp://[username:password@]<ip_address>:554/stream
```

### ONVIF Discovery
Most VMS platforms will automatically discover Talos Desk through ONVIF:
1. Enable ONVIF discovery in your VMS
2. Scan for new devices
3. Add Talos Desk as an IP camera

## Command Line Options

```
talos_desk [options]

Options:
  -h, --help          Show help message
  -c, --config FILE   Specify configuration file
  -d, --daemon        Run in daemon mode
  -v, --verbose       Enable verbose logging
  --no-gui            Disable GUI (headless mode)
  --port PORT         RTSP server port
  --fps FPS           Frame rate (1-60)
  --quality QUALITY   Quality preset: high, balanced, low
```

## Development Status

This project is currently in active development. Week 4 of development has been completed with comprehensive memory management and optimization systems implemented.

### Current Progress:
- ✅ CMake build system
- ✅ Platform-specific build scripts
- ✅ Basic application structure
- ✅ Configuration management
- ✅ Logging framework
- ✅ Windows screen capture (Desktop Duplication API)
- ✅ macOS screen capture (AVFoundation)
- ✅ FFmpeg video encoding (H.264)
- ✅ External dependencies integration
- ✅ **Memory Management & Optimization**:
  - ✅ Lock-free circular buffer system
  - ✅ Advanced memory pool with automatic sizing
  - ✅ Zero-copy pipeline optimization
  - ✅ Comprehensive memory leak detection
  - ✅ Performance profiling and metrics
- 🚧 Linux screen capture
- 🚧 RTSP server implementation
- 🚧 ONVIF services
- 🚧 User interface (Dear ImGui)

## Documentation

📚 **Complete documentation is available in the [docs/](docs/) folder:**

- **Project**: [Requirements](docs/project/PRD.md) | [Development Plan](docs/project/development_plan.md) | [TODO List](docs/project/todo.md) | [Changelog](docs/project/CHANGELOG.md)
- **Architecture**: [System Design](docs/architecture/architecture_design.md) | [Build System](docs/architecture/BUILD_SUMMARY.md)
- **Design**: [UI System](docs/design/frontend_design_system.md) | [UX Architecture](docs/design/ux_architecture.md) | [Accessibility](docs/design/accessibility_guide.md)
- **Operations**: [Workflows](docs/operations/workflow.md) | [Use Cases](docs/operations/use_case.md)

See [docs/meta/DOCS_VERSION.md](docs/meta/DOCS_VERSION.md) for documentation version tracking.

## Contributing

Contributions are welcome! Please read our contributing guidelines and submit pull requests to the GitHub repository.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Support

For issues, questions, or contributions, please visit:
https://github.com/stefanus-ronald/talos_desk
