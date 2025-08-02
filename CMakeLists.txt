# Talos Desk - Screen-to-RTSP ONVIF Bridge
# Root CMakeLists.txt

cmake_minimum_required(VERSION 3.20)
project(TalosDesk 
    VERSION 1.0.0
    DESCRIPTION "Screen-to-RTSP ONVIF Bridge Application"
    LANGUAGES CXX C
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

# Add module path for custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Options
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(USE_HARDWARE_ACCEL "Enable hardware acceleration support" ON)
option(ENABLE_ONVIF "Enable ONVIF support" ON)
option(PORTABLE_BUILD "Build portable executable" OFF)

# Compiler flags
if(MSVC)
    add_compile_options(/W4 /WX)
    add_compile_options(/MP)  # Multi-processor compilation
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_definitions(NOMINMAX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    else()
        add_compile_options(-O3)
    endif()
endif()

# Platform detection
if(WIN32)
    add_compile_definitions(PLATFORM_WINDOWS)
    set(PLATFORM_NAME "windows")
elseif(APPLE)
    add_compile_definitions(PLATFORM_MACOS)
    set(PLATFORM_NAME "macos")
elseif(UNIX)
    add_compile_definitions(PLATFORM_LINUX)
    set(PLATFORM_NAME "linux")
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find required packages
find_package(Threads REQUIRED)

# Check for dependencies configuration
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/TalosDeskDependencies.cmake")
    include("${CMAKE_CURRENT_SOURCE_DIR}/third_party/TalosDeskDependencies.cmake")
endif()

# Find FFmpeg
find_package(FFmpeg COMPONENTS avcodec avformat avutil swscale avfilter)
if(FFMPEG_FOUND)
    message(STATUS "Found FFmpeg")
    include_directories(${FFMPEG_INCLUDE_DIRS})
else()
    message(WARNING "FFmpeg not found. Video encoding will be disabled.")
    add_compile_definitions(NO_FFMPEG)
endif()

# Find Dear ImGui
if(DEFINED IMGUI_ROOT)
    set(IMGUI_SOURCES
        ${IMGUI_ROOT}/imgui.cpp
        ${IMGUI_ROOT}/imgui_demo.cpp
        ${IMGUI_ROOT}/imgui_draw.cpp
        ${IMGUI_ROOT}/imgui_tables.cpp
        ${IMGUI_ROOT}/imgui_widgets.cpp
    )
    
    # Platform-specific ImGui backends
    if(WIN32)
        list(APPEND IMGUI_SOURCES
            ${IMGUI_ROOT}/backends/imgui_impl_win32.cpp
            ${IMGUI_ROOT}/backends/imgui_impl_dx11.cpp
        )
    elseif(APPLE)
        list(APPEND IMGUI_SOURCES
            ${IMGUI_ROOT}/backends/imgui_impl_osx.mm
            ${IMGUI_ROOT}/backends/imgui_impl_metal.mm
        )
    else()
        list(APPEND IMGUI_SOURCES
            ${IMGUI_ROOT}/backends/imgui_impl_glfw.cpp
            ${IMGUI_ROOT}/backends/imgui_impl_opengl3.cpp
        )
    endif()
    
    include_directories(${IMGUI_ROOT} ${IMGUI_ROOT}/backends)
endif()

# Find nlohmann/json
if(DEFINED JSON_INCLUDE_DIR)
    include_directories(${JSON_INCLUDE_DIR})
endif()

# Find Live555
find_package(Live555 COMPONENTS BasicUsageEnvironment groupsock liveMedia UsageEnvironment)
if(Live555_FOUND)
    message(STATUS "Found Live555")
    include_directories(${Live555_INCLUDE_DIRS})
else()
    message(WARNING "Live555 not found. RTSP server functionality will be disabled.")
    add_compile_definitions(NO_LIVE555)
endif()

# External dependencies (to be configured)
# add_subdirectory(external)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(COMMON_SOURCES
    src/main.cpp
    src/core/application.cpp
    src/core/configuration.cpp
    src/core/logger.cpp
    src/core/frame_buffer.cpp
    src/core/memory_pool.cpp
    src/core/zero_copy_buffer.cpp
    src/core/memory_tracker.cpp
    src/core/performance_profiler.cpp
    src/capture/capture_engine.cpp
    src/capture/frame_buffer.cpp
    src/encoder/video_encoder.cpp
    src/encoder/codec_manager.cpp
    src/encoder/ffmpeg_encoder.cpp
    src/network/rtsp_server.cpp
    src/network/rtsp_session.cpp
    src/ui/tray_application.cpp
    src/ui/configuration_window.cpp
)

# Platform-specific sources
set(PLATFORM_SOURCES)
if(WIN32)
    list(APPEND PLATFORM_SOURCES
        src/capture/desktop_duplication_api.cpp
        src/capture/windows_capture_engine.cpp
    )
elseif(APPLE)
    list(APPEND PLATFORM_SOURCES
        src/capture/macos_capture_engine.mm
    )
    # Enable Objective-C++ for .mm files
    set_source_files_properties(src/capture/macos_capture_engine.mm PROPERTIES
        COMPILE_FLAGS "-x objective-c++"
    )
elseif(UNIX)
    # TODO: Add Linux capture sources
endif()

# Add ImGui sources if available
if(DEFINED IMGUI_SOURCES)
    list(APPEND COMMON_SOURCES ${IMGUI_SOURCES})
endif()

# Create executable
add_executable(talos_desk 
    ${COMMON_SOURCES}
    ${PLATFORM_SOURCES}
)

# Link libraries
target_link_libraries(talos_desk PRIVATE
    Threads::Threads
)

# Link FFmpeg if found
if(FFMPEG_FOUND)
    target_link_libraries(talos_desk PRIVATE ${FFMPEG_LIBRARIES})
endif()

# Link Live555 if found
if(Live555_FOUND)
    target_link_libraries(talos_desk PRIVATE ${Live555_LIBRARIES})
endif()

# Platform-specific libraries
if(WIN32)
    target_link_libraries(talos_desk PRIVATE
        d3d11
        dxgi
        ws2_32
        winmm
        shell32
        user32
        gdi32
    )
elseif(APPLE)
    find_library(FOUNDATION_FRAMEWORK Foundation)
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(AVFOUNDATION_FRAMEWORK AVFoundation)
    find_library(COREGRAPHICS_FRAMEWORK CoreGraphics)
    find_library(COREMEDIA_FRAMEWORK CoreMedia)
    find_library(COREVIDEO_FRAMEWORK CoreVideo)
    
    target_link_libraries(talos_desk PRIVATE
        ${FOUNDATION_FRAMEWORK}
        ${COCOA_FRAMEWORK}
        ${AVFOUNDATION_FRAMEWORK}
        ${COREGRAPHICS_FRAMEWORK}
        ${COREMEDIA_FRAMEWORK}
        ${COREVIDEO_FRAMEWORK}
    )
elseif(UNIX)
    find_package(X11 REQUIRED)
    target_link_libraries(talos_desk PRIVATE
        ${X11_LIBRARIES}
        ${X11_Xfixes_LIB}
        ${X11_Xrandr_LIB}
    )
endif()

# ONVIF support
if(ENABLE_ONVIF)
    add_compile_definitions(ENABLE_ONVIF)
    # TODO: Add gSOAP configuration
endif()

# Hardware acceleration
if(USE_HARDWARE_ACCEL)
    add_compile_definitions(USE_HARDWARE_ACCEL)
    # TODO: Add hardware acceleration library detection
endif()

# Tests
if(BUILD_TESTS)
    enable_testing()
    # TODO: Add tests directory when tests are implemented
    # add_subdirectory(tests)
endif()

# Documentation
if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        add_subdirectory(docs)
    endif()
endif()

# Installation rules
install(TARGETS talos_desk
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# CPack configuration for packaging
set(CPACK_PACKAGE_NAME "TalosDesk")
set(CPACK_PACKAGE_VENDOR "TalosDesk Team")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Screen-to-RTSP ONVIF Bridge")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}-${PLATFORM_NAME}")

if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop;TGZ")
elseif(UNIX)
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "TalosDesk Team")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.17)")
endif()

include(CPack)

# Print configuration summary
message(STATUS "")
message(STATUS "=== Talos Desk Configuration Summary ===")
message(STATUS "Version:                 ${PROJECT_VERSION}")
message(STATUS "Build type:              ${CMAKE_BUILD_TYPE}")
message(STATUS "Platform:                ${PLATFORM_NAME}")
message(STATUS "C++ Compiler:            ${CMAKE_CXX_COMPILER}")
message(STATUS "Build tests:             ${BUILD_TESTS}")
message(STATUS "Build documentation:     ${BUILD_DOCS}")
message(STATUS "Hardware acceleration:   ${USE_HARDWARE_ACCEL}")
message(STATUS "ONVIF support:           ${ENABLE_ONVIF}")
message(STATUS "Portable build:          ${PORTABLE_BUILD}")
message(STATUS "Install prefix:          ${CMAKE_INSTALL_PREFIX}")
message(STATUS "========================================")
message(STATUS "")