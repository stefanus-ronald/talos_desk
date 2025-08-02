# FindFFmpeg.cmake - Find FFmpeg libraries
# This module finds FFmpeg libraries and headers
#
# It defines the following variables:
#   FFMPEG_FOUND - True if FFmpeg is found
#   FFMPEG_INCLUDE_DIRS - Include directories for FFmpeg
#   FFMPEG_LIBRARIES - Libraries to link against
#   FFMPEG_VERSION - Version of FFmpeg found

find_package(PkgConfig QUIET)

# Component libraries to find
set(FFMPEG_COMPONENTS avcodec avformat avutil swscale avfilter avdevice swresample)

# Find include directory
find_path(FFMPEG_INCLUDE_DIR
    NAMES libavcodec/avcodec.h
    PATHS
        /usr/include
        /usr/local/include
        /opt/homebrew/include
        ${FFMPEG_ROOT}/include
    PATH_SUFFIXES ffmpeg
)

# Find each component
foreach(component ${FFMPEG_COMPONENTS})
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(PC_${component} QUIET lib${component})
    endif()
    
    find_library(FFMPEG_${component}_LIBRARY
        NAMES ${component} ${component}.dll
        PATHS
            /usr/lib
            /usr/local/lib
            /opt/homebrew/lib
            ${FFMPEG_ROOT}/lib
        HINTS ${PC_${component}_LIBDIR}
    )
    
    if(FFMPEG_${component}_LIBRARY)
        list(APPEND FFMPEG_LIBRARIES ${FFMPEG_${component}_LIBRARY})
    endif()
endforeach()

# Version detection
if(FFMPEG_INCLUDE_DIR AND EXISTS "${FFMPEG_INCLUDE_DIR}/libavcodec/version.h")
    file(STRINGS "${FFMPEG_INCLUDE_DIR}/libavcodec/version.h" version_line
        REGEX "^#define[ \t]+LIBAVCODEC_VERSION_MAJOR[ \t]+[0-9]+")
    string(REGEX MATCH "[0-9]+" FFMPEG_VERSION_MAJOR "${version_line}")
endif()

# Handle REQUIRED and QUIET arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FFmpeg
    REQUIRED_VARS FFMPEG_INCLUDE_DIR FFMPEG_LIBRARIES
    VERSION_VAR FFMPEG_VERSION_MAJOR
)

if(FFMPEG_FOUND)
    set(FFMPEG_INCLUDE_DIRS ${FFMPEG_INCLUDE_DIR})
    
    # Create imported targets
    foreach(component ${FFMPEG_COMPONENTS})
        if(FFMPEG_${component}_LIBRARY AND NOT TARGET FFmpeg::${component})
            add_library(FFmpeg::${component} UNKNOWN IMPORTED)
            set_target_properties(FFmpeg::${component} PROPERTIES
                IMPORTED_LOCATION "${FFMPEG_${component}_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${FFMPEG_INCLUDE_DIR}"
            )
        endif()
    endforeach()
endif()

mark_as_advanced(FFMPEG_INCLUDE_DIR FFMPEG_LIBRARIES)