#!/bin/bash
# Talos Desk - Linux Build Script
# Requires: GCC/Clang, CMake 3.20+, Git, X11 development libraries

set -e

echo "========================================"
echo "Talos Desk - Linux Build Script"
echo "========================================"
echo

# Set build configuration
BUILD_TYPE=${1:-Release}
COMPILER=${2:-gcc}

# Set project root
PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$PROJECT_ROOT"

# Create build directory
BUILD_DIR="build/linux-$(uname -m)-${BUILD_TYPE}"
mkdir -p "$BUILD_DIR"

echo "Configuration:"
echo "  Build Type: $BUILD_TYPE"
echo "  Compiler: $COMPILER"
echo "  Architecture: $(uname -m)"
echo "  Build Directory: $BUILD_DIR"
echo

# Check for required tools
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake not found"
    echo "Please install CMake 3.20 or later:"
    echo "  sudo apt-get install cmake          # Debian/Ubuntu"
    echo "  sudo dnf install cmake              # Fedora"
    echo "  sudo pacman -S cmake                # Arch"
    exit 1
fi

if ! command -v "$COMPILER" &> /dev/null; then
    echo "Error: Compiler $COMPILER not found"
    echo "Please install a C++ compiler:"
    echo "  sudo apt-get install build-essential    # Debian/Ubuntu"
    echo "  sudo dnf install gcc-c++                # Fedora"
    echo "  sudo pacman -S gcc                      # Arch"
    exit 1
fi

# Check for X11 development libraries
if ! pkg-config --exists x11 2>/dev/null; then
    echo "Error: X11 development libraries not found"
    echo "Please install X11 development libraries:"
    echo "  sudo apt-get install libx11-dev libxfixes-dev libxrandr-dev    # Debian/Ubuntu"
    echo "  sudo dnf install libX11-devel libXfixes-devel libXrandr-devel  # Fedora"
    echo "  sudo pacman -S libx11 libxfixes libxrandr                      # Arch"
    exit 1
fi

# Set compiler
if [ "$COMPILER" = "gcc" ]; then
    export CC=gcc
    export CXX=g++
elif [ "$COMPILER" = "clang" ]; then
    export CC=clang
    export CXX=clang++
fi

# Configure with CMake
echo "Configuring project with CMake..."
cd "$BUILD_DIR"

cmake -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DBUILD_TESTS=ON \
    -DUSE_HARDWARE_ACCEL=ON \
    -DENABLE_ONVIF=ON \
    -DPORTABLE_BUILD=ON \
    ../..

# Build the project
echo
echo "Building project..."
cmake --build . --parallel $(nproc)

# Run tests if built
if [ -f "bin/talos_desk_tests" ]; then
    echo
    echo "Running tests..."
    "bin/talos_desk_tests" || echo "Warning: Some tests failed"
fi

# Strip symbols for release builds
if [ "$BUILD_TYPE" = "Release" ]; then
    echo
    echo "Stripping debug symbols..."
    strip "bin/talos_desk"
fi

# Create portable package
if [ "$BUILD_TYPE" = "Release" ]; then
    echo
    echo "Creating portable package..."
    PACKAGE_DIR="talos_desk_portable"
    mkdir -p "$PACKAGE_DIR"
    
    # Copy executable
    cp "bin/talos_desk" "$PACKAGE_DIR/"
    
    # Create run script
    cat > "$PACKAGE_DIR/run_talos_desk.sh" <<'EOF'
#!/bin/bash
# Talos Desk portable launcher
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
export LD_LIBRARY_PATH="$SCRIPT_DIR/lib:$LD_LIBRARY_PATH"
"$SCRIPT_DIR/talos_desk" "$@"
EOF
    chmod +x "$PACKAGE_DIR/run_talos_desk.sh"
    
    # Copy any required libraries (if needed)
    # TODO: Use patchelf or similar to make truly portable
    
    # Create desktop entry
    cat > "$PACKAGE_DIR/talos_desk.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=Talos Desk
Comment=Screen-to-RTSP ONVIF Bridge
Exec=$PACKAGE_DIR/run_talos_desk.sh
Icon=talos_desk
Categories=AudioVideo;Network;
Terminal=false
StartupNotify=false
EOF
    
    # Create tarball
    tar -czf "talos_desk_portable_$(uname -m).tar.gz" "$PACKAGE_DIR"
    echo "Portable package created: talos_desk_portable_$(uname -m).tar.gz"
fi

echo
echo "========================================"
echo "Build completed successfully!"
echo "Executable: $BUILD_DIR/bin/talos_desk"
echo "========================================"

# Print library dependencies
echo
echo "Library dependencies:"
ldd "bin/talos_desk" | grep -E "=> /" | awk '{print "  " $1 " => " $3}'