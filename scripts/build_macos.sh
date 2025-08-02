#!/bin/bash
# Talos Desk - macOS Build Script
# Requires: Xcode, CMake 3.20+, Git

set -e

echo "========================================"
echo "Talos Desk - macOS Build Script"
echo "========================================"
echo

# Set build configuration
BUILD_TYPE=${1:-Release}
ARCH=${2:-$(uname -m)}

# Set project root
PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$PROJECT_ROOT"

# Create build directory
BUILD_DIR="build/macos-${ARCH}-${BUILD_TYPE}"
mkdir -p "$BUILD_DIR"

echo "Configuration:"
echo "  Build Type: $BUILD_TYPE"
echo "  Architecture: $ARCH"
echo "  Build Directory: $BUILD_DIR"
echo

# Check for required tools
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake not found"
    echo "Please install CMake 3.20 or later:"
    echo "  brew install cmake"
    exit 1
fi

if ! command -v clang &> /dev/null; then
    echo "Error: Xcode command line tools not found"
    echo "Please install Xcode command line tools:"
    echo "  xcode-select --install"
    exit 1
fi

# Configure with CMake
echo "Configuring project with CMake..."
cd "$BUILD_DIR"

# Set deployment target for better compatibility
export MACOSX_DEPLOYMENT_TARGET=11.0

cmake -G "Xcode" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_OSX_ARCHITECTURES="$ARCH" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET="$MACOSX_DEPLOYMENT_TARGET" \
    -DBUILD_TESTS=ON \
    -DUSE_HARDWARE_ACCEL=ON \
    -DENABLE_ONVIF=ON \
    ../..

# Build the project
echo
echo "Building project..."
cmake --build . --config "$BUILD_TYPE" -- -jobs $(sysctl -n hw.ncpu)

# Sign the application if in Release mode
if [ "$BUILD_TYPE" = "Release" ]; then
    echo
    echo "Signing application..."
    # Use ad-hoc signing if no developer certificate is available
    codesign --force --deep --sign - "bin/$BUILD_TYPE/talos_desk"
fi

# Run tests if built
if [ -f "bin/$BUILD_TYPE/talos_desk_tests" ]; then
    echo
    echo "Running tests..."
    "bin/$BUILD_TYPE/talos_desk_tests" || echo "Warning: Some tests failed"
fi

# Create .app bundle structure
if [ "$BUILD_TYPE" = "Release" ]; then
    echo
    echo "Creating app bundle..."
    APP_BUNDLE="bin/$BUILD_TYPE/TalosDesk.app"
    mkdir -p "$APP_BUNDLE/Contents/MacOS"
    mkdir -p "$APP_BUNDLE/Contents/Resources"
    
    # Copy executable
    cp "bin/$BUILD_TYPE/talos_desk" "$APP_BUNDLE/Contents/MacOS/TalosDesk"
    
    # Create Info.plist
    cat > "$APP_BUNDLE/Contents/Info.plist" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>TalosDesk</string>
    <key>CFBundleIdentifier</key>
    <string>com.talosdesk.app</string>
    <key>CFBundleName</key>
    <string>TalosDesk</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0.0</string>
    <key>CFBundleVersion</key>
    <string>1</string>
    <key>LSMinimumSystemVersion</key>
    <string>11.0</string>
    <key>LSUIElement</key>
    <true/>
    <key>NSHighResolutionCapable</key>
    <true/>
    <key>NSRequiresAquaSystemAppearance</key>
    <false/>
</dict>
</plist>
EOF
    
    # Sign the app bundle
    codesign --force --deep --sign - "$APP_BUNDLE"
fi

echo
echo "========================================"
echo "Build completed successfully!"
echo "Executable: $BUILD_DIR/bin/$BUILD_TYPE/talos_desk"
if [ "$BUILD_TYPE" = "Release" ]; then
    echo "App Bundle: $BUILD_DIR/bin/$BUILD_TYPE/TalosDesk.app"
fi
echo "========================================"