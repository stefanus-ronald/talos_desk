# Talos Desk - TODO List

## Project Status: Planning Phase
**Last Updated**: Development Plan Created
**Target Release**: Version 1.0 in 7 months

## High Priority Tasks 🔴

### Immediate Setup (Week 1)
- [ ] Create GitHub/GitLab repository
- [ ] Set up CMake build system
- [ ] Configure CI/CD pipeline (GitHub Actions/GitLab CI)
- [ ] Create development Docker containers
- [ ] Set up development wiki/documentation site
- [ ] Create CONTRIBUTING.md guidelines
- [ ] Set up issue templates

### Core Architecture
- [ ] Design abstract base classes for platform independence
- [ ] Create ICaptureEngine interface
- [ ] Create IEncoder interface  
- [ ] Create IStreamServer interface
- [ ] Design configuration schema (JSON)
- [ ] Implement logging framework (spdlog)
- [ ] Create error handling system

### Critical Dependencies
- [ ] Download and build FFmpeg 5.0+ static libs
- [ ] Download and build live555 2023.07+
- [ ] Integrate Dear ImGui
- [ ] Set up nlohmann/json
- [ ] Configure platform SDKs (DirectX, AVFoundation, X11)

## Medium Priority Tasks 🟡

### Development Infrastructure
- [ ] Set up unit testing framework (Google Test)
- [ ] Create performance benchmarking suite
- [ ] Set up memory leak detection (Valgrind/AddressSanitizer)
- [ ] Configure code formatting (clang-format)
- [ ] Set up static analysis (clang-tidy, PVS-Studio)
- [ ] Create build scripts for all platforms
- [ ] Set up cross-compilation environment

### Documentation
- [ ] Create API documentation template
- [ ] Set up Doxygen configuration
- [ ] Write coding standards document
- [ ] Create architecture diagrams
- [ ] Document build instructions for each platform
- [ ] Create troubleshooting guide template

### Testing Infrastructure
- [ ] Set up automated testing framework
- [ ] Create RTSP client test suite
- [ ] Design performance test scenarios
- [ ] Create VMS compatibility test plan
- [ ] Set up continuous integration tests

## Low Priority Tasks 🟢

### Future Enhancements
- [ ] Research audio capture feasibility
- [ ] Investigate GPU-accelerated filters
- [ ] Plan mobile viewer app
- [ ] Research cloud relay architecture
- [ ] Evaluate WebRTC as additional protocol
- [ ] Plan plugin system architecture
- [ ] Research AI-based features

### Community Building
- [ ] Create project website
- [ ] Set up Discord/Slack community
- [ ] Create demo videos
- [ ] Write blog posts about development
- [ ] Create contributor recognition system
- [ ] Plan beta testing program

## Phase-Specific TODOs

### Phase 1: Foundation (Weeks 1-4)
- [ ] **Week 1**: Project setup and architecture
  - [ ] CMake configuration
  - [ ] Git repository
  - [ ] Basic logging
  - [ ] Configuration manager
  
- [ ] **Week 2**: Windows screen capture
  - [ ] Desktop Duplication API
  - [ ] Native resolution detection
  - [ ] Multi-monitor support
  - [ ] Performance monitoring
  
- [ ] **Week 3**: Basic video encoding
  - [ ] FFmpeg integration
  - [ ] H.264 software encoding
  - [ ] Quality presets
  - [ ] Bitrate control
  
- [ ] **Week 4**: Memory management
  - [ ] Circular buffers
  - [ ] Memory pools
  - [ ] Zero-copy pipeline
  - [ ] Leak detection

### Phase 2: RTSP Server (Weeks 5-8)
- [ ] **Week 5**: live555 integration
- [ ] **Week 6**: Multi-client support
- [ ] **Week 7**: Network optimization
- [ ] **Week 8**: RTSP compliance testing

### Phase 3: Multi-Platform (Weeks 9-12)
- [ ] **Week 9**: macOS implementation
- [ ] **Week 10**: Linux implementation
- [ ] **Week 11**: Platform abstraction
- [ ] **Week 12**: NVIDIA hardware acceleration

### Phase 4: ONVIF (Weeks 13-16)
- [ ] **Week 13**: Basic ONVIF
- [ ] **Week 14**: Media Service
- [ ] **Week 15**: Authentication
- [ ] **Week 16**: VMS testing

### Phase 5: User Interface (Weeks 17-20)
- [ ] **Week 17**: System tray
- [ ] **Week 18**: Dear ImGui setup
- [ ] **Week 19**: Configuration UI
- [ ] **Week 20**: UI polish

### Phase 6: Advanced Features (Weeks 21-24)
- [ ] **Week 21**: H.265 support
- [ ] **Week 22**: More hardware acceleration
- [ ] **Week 23**: Security enhancements
- [ ] **Week 24**: Performance optimization

### Phase 7: Release Prep (Weeks 25-28)
- [ ] **Week 25**: Testing
- [ ] **Week 26**: Documentation
- [ ] **Week 27**: Packaging
- [ ] **Week 28**: Release

## Bug Fixes & Issues 🐛

### Known Issues
- [ ] No known issues yet (pre-development)

### Performance Optimizations
- [ ] Profile screen capture performance
- [ ] Optimize frame copy operations
- [ ] Reduce encoding latency
- [ ] Minimize memory allocations
- [ ] Optimize network packet handling

## Technical Debt 💳

### Refactoring Needs
- [ ] TBD after initial implementation

### Code Quality
- [ ] Maintain >80% test coverage
- [ ] Zero compiler warnings
- [ ] Pass all static analysis checks
- [ ] Document all public APIs

## Research & Investigation 🔍

### Technical Research
- [ ] HDR video support feasibility
- [ ] 120Hz+ display support
- [ ] Variable refresh rate handling
- [ ] Multi-GPU encoding distribution
- [ ] AV1 codec evaluation
- [ ] SRT protocol as alternative to RTSP

### Platform Research  
- [ ] Windows 11 specific features
- [ ] macOS Sonoma optimizations
- [ ] Wayland compositor support
- [ ] Steam Deck compatibility
- [ ] ARM platform support (Apple Silicon, RPi)

## Release Checklist ✅

### Pre-Release
- [ ] All tests passing
- [ ] Performance targets met (<15% CPU)
- [ ] Memory leaks fixed
- [ ] Security audit complete
- [ ] Documentation complete
- [ ] VMS compatibility verified
- [ ] Installers tested

### Release Tasks
- [ ] Tag release in Git
- [ ] Build release binaries
- [ ] Sign binaries
- [ ] Create release notes
- [ ] Update website
- [ ] Announce release
- [ ] Monitor feedback

## Notes 📝

### Key Decisions Made
1. **Native Resolution Only**: Always capture at screen's native resolution
2. **C++ Desktop App**: Pure C++ with Dear ImGui (no web UI)
3. **Minimal Dependencies**: Avoid heavy frameworks
4. **Performance First**: Target <15% CPU from day one

### Important Reminders
- Always test with multiple monitors
- Test with 4K and high refresh displays  
- Verify VMS compatibility early and often
- Keep binary size under 25MB
- Maintain cross-platform compatibility
- Follow secure coding practices

### Useful Links
- [FFmpeg Documentation](https://ffmpeg.org/documentation.html)
- [live555 Documentation](http://www.live555.com/liveMedia/)
- [Dear ImGui Wiki](https://github.com/ocornut/imgui/wiki)
- [ONVIF Specifications](https://www.onvif.org/specifications/)

---

## Quick Commands

### Build Commands
```bash
# Windows
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Release

# macOS
cmake -B build -G Xcode
cmake --build build --config Release

# Linux
cmake -B build -G "Unix Makefiles"
cmake --build build --config Release
```

### Test Commands
```bash
# Run unit tests
ctest --test-dir build -C Release

# Run RTSP test client
./build/test/rtsp_client_test

# Memory check (Linux)
valgrind --leak-check=full ./build/talos_desk
```

### Common Tasks
```bash
# Format code
clang-format -i src/**/*.cpp src/**/*.h

# Run static analysis  
clang-tidy src/**/*.cpp

# Generate documentation
doxygen Doxyfile

# Create package
cpack --config build/CPackConfig.cmake
```

---

*This TODO list is a living document. Update it regularly as development progresses.*