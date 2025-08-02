#!/bin/bash

# Setup script for Talos Desk external dependencies
# This script downloads and builds FFmpeg, live555, and Dear ImGui

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
DEPS_DIR="$PROJECT_ROOT/third_party"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Create dependencies directory
mkdir -p "$DEPS_DIR"
cd "$DEPS_DIR"

# Detect platform
OS="unknown"
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macos"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    OS="windows"
fi

print_info "Detected platform: $OS"
print_info "Dependencies directory: $DEPS_DIR"

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check prerequisites
print_info "Checking prerequisites..."

if ! command_exists git; then
    print_error "git is not installed. Please install git first."
    exit 1
fi

if ! command_exists cmake; then
    print_error "cmake is not installed. Please install cmake first."
    exit 1
fi

# Platform-specific prerequisites
if [[ "$OS" == "linux" ]]; then
    if ! command_exists pkg-config; then
        print_warning "pkg-config not found. Installing dependencies may fail."
    fi
elif [[ "$OS" == "macos" ]]; then
    if ! command_exists brew; then
        print_warning "Homebrew not found. Some dependencies may need manual installation."
    fi
fi

# Function to download and extract archive
download_and_extract() {
    local url=$1
    local filename=$2
    local extract_dir=$3
    
    if [ ! -f "$filename" ]; then
        print_info "Downloading $filename..."
        curl -L "$url" -o "$filename" || wget "$url" -O "$filename"
    fi
    
    # Check if file was downloaded successfully
    if [ ! -f "$filename" ] || [ ! -s "$filename" ]; then
        print_error "Failed to download $filename"
        rm -f "$filename"
        return 1
    fi
    
    print_info "Extracting $filename..."
    case "$filename" in
        *.tar.gz|*.tgz)
            tar -xzf "$filename"
            ;;
        *.tar.bz2)
            tar -xjf "$filename"
            ;;
        *.zip)
            unzip -q "$filename"
            ;;
        *)
            print_error "Unknown archive format: $filename"
            return 1
            ;;
    esac
}

# 1. Setup FFmpeg
print_info "Setting up FFmpeg..."

FFMPEG_VERSION="5.1.4"
FFMPEG_DIR="$DEPS_DIR/ffmpeg"

if [ ! -d "$FFMPEG_DIR" ]; then
    # Check if system FFmpeg is available and sufficient
    if command_exists ffmpeg && command_exists pkg-config; then
        SYSTEM_FFMPEG_VERSION=$(pkg-config --modversion libavcodec 2>/dev/null || echo "0")
        print_info "Found system FFmpeg version: $SYSTEM_FFMPEG_VERSION"
        
        # Create symlink to system FFmpeg
        if pkg-config --exists libavcodec libavformat libavutil libswscale; then
            print_info "Using system FFmpeg installation"
            mkdir -p "$FFMPEG_DIR"
            echo "System FFmpeg" > "$FFMPEG_DIR/USING_SYSTEM"
        fi
    else
        # Download prebuilt FFmpeg or build from source
        mkdir -p "$FFMPEG_DIR"
        
        if [[ "$OS" == "macos" ]]; then
            # On macOS, use Homebrew
            if command_exists brew; then
                print_info "Installing FFmpeg via Homebrew..."
                brew install ffmpeg || true
                echo "Homebrew FFmpeg" > "$FFMPEG_DIR/USING_HOMEBREW"
            fi
        elif [[ "$OS" == "linux" ]]; then
            # On Linux, provide build instructions
            print_warning "FFmpeg not found. Please install using your package manager:"
            print_warning "  Ubuntu/Debian: sudo apt-get install libavcodec-dev libavformat-dev libavutil-dev libswscale-dev"
            print_warning "  Fedora: sudo dnf install ffmpeg-devel"
            print_warning "  Arch: sudo pacman -S ffmpeg"
        elif [[ "$OS" == "windows" ]]; then
            # On Windows, download prebuilt binaries
            print_info "Downloading prebuilt FFmpeg for Windows..."
            FFMPEG_WIN_URL="https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-master-latest-win64-gpl-shared.zip"
            download_and_extract "$FFMPEG_WIN_URL" "ffmpeg-win64.zip" "$FFMPEG_DIR"
        fi
    fi
else
    print_info "FFmpeg already set up in $FFMPEG_DIR"
fi

# 2. Setup live555
print_info "Setting up live555..."

LIVE555_VERSION="latest"
LIVE555_DIR="$DEPS_DIR/live555"

if [ ! -d "$LIVE555_DIR" ]; then
    LIVE555_URL="http://www.live555.com/liveMedia/public/live555-latest.tar.gz"
    LIVE555_ARCHIVE="live555-latest.tar.gz"
    
    download_and_extract "$LIVE555_URL" "$LIVE555_ARCHIVE" "$DEPS_DIR"
    mv live "$LIVE555_DIR"
    
    # Build live555
    cd "$LIVE555_DIR"
    
    # Generate Makefiles based on platform
    if [[ "$OS" == "linux" ]]; then
        ./genMakefiles linux
    elif [[ "$OS" == "macos" ]]; then
        ./genMakefiles macosx
    elif [[ "$OS" == "windows" ]]; then
        # For Windows, we'll need to handle this differently
        print_warning "live555 Windows build requires Visual Studio. Please build manually."
    fi
    
    # Build if not Windows
    if [[ "$OS" != "windows" ]]; then
        print_info "Building live555..."
        make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)
    fi
    
    cd "$DEPS_DIR"
else
    print_info "live555 already set up in $LIVE555_DIR"
fi

# 3. Setup Dear ImGui
print_info "Setting up Dear ImGui..."

IMGUI_DIR="$DEPS_DIR/imgui"

if [ ! -d "$IMGUI_DIR" ]; then
    print_info "Cloning Dear ImGui..."
    git clone https://github.com/ocornut/imgui.git "$IMGUI_DIR"
    cd "$IMGUI_DIR"
    # Use latest stable version
    git checkout docking  # Use docking branch for multi-window support
    cd "$DEPS_DIR"
else
    print_info "Dear ImGui already set up in $IMGUI_DIR"
fi

# 4. Setup nlohmann/json
print_info "Setting up nlohmann/json..."

JSON_DIR="$DEPS_DIR/json"

if [ ! -d "$JSON_DIR" ]; then
    print_info "Downloading nlohmann/json..."
    mkdir -p "$JSON_DIR/include/nlohmann"
    JSON_URL="https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp"
    curl -L "$JSON_URL" -o "$JSON_DIR/include/nlohmann/json.hpp"
else
    print_info "nlohmann/json already set up in $JSON_DIR"
fi

# Create CMake config file for dependencies
print_info "Creating dependencies CMake configuration..."

cat > "$DEPS_DIR/TalosDeskDependencies.cmake" << EOF
# Talos Desk Dependencies Configuration
# Auto-generated by setup_dependencies.sh

set(TALOS_DEPS_DIR "${DEPS_DIR}")

# FFmpeg
if(EXISTS "${FFMPEG_DIR}/USING_SYSTEM")
    # Use system FFmpeg
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(FFMPEG REQUIRED libavcodec libavformat libavutil libswscale libavfilter)
elseif(EXISTS "${FFMPEG_DIR}/USING_HOMEBREW")
    # Use Homebrew FFmpeg on macOS
    set(FFMPEG_ROOT "/opt/homebrew" CACHE PATH "FFmpeg root directory")
else
    # Use local FFmpeg
    set(FFMPEG_ROOT "${FFMPEG_DIR}" CACHE PATH "FFmpeg root directory")
endif()

# live555
set(LIVE555_ROOT "${LIVE555_DIR}" CACHE PATH "live555 root directory")

# Dear ImGui
set(IMGUI_ROOT "${IMGUI_DIR}" CACHE PATH "Dear ImGui root directory")

# nlohmann/json
set(JSON_INCLUDE_DIR "${JSON_DIR}/include" CACHE PATH "nlohmann/json include directory")
EOF

print_info "Dependencies setup complete!"
print_info ""
print_info "Summary:"
print_info "  FFmpeg: ${FFMPEG_DIR}"
print_info "  live555: ${LIVE555_DIR}"
print_info "  Dear ImGui: ${IMGUI_DIR}"
print_info "  nlohmann/json: ${JSON_DIR}"
print_info ""
print_info "To use these dependencies, add the following to your CMake command:"
print_info "  -DCMAKE_PREFIX_PATH=${DEPS_DIR}"