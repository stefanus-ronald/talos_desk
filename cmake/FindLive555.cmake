# FindLive555.cmake - Find Live555 Streaming Media libraries
#
# This module finds Live555 libraries and headers
#
# Variables defined:
#   Live555_FOUND - System has Live555
#   Live555_INCLUDE_DIRS - Include directories
#   Live555_LIBRARIES - Libraries to link
#   Live555_VERSION - Version string
#
# Components:
#   BasicUsageEnvironment
#   groupsock
#   liveMedia
#   UsageEnvironment

include(FindPackageHandleStandardArgs)

# Initialize the search paths
set(Live555_SEARCH_PATHS
    ${Live555_ROOT}
    ${LIVE555_ROOT}
    $ENV{LIVE555_ROOT}
    $ENV{LIVE555_DIR}
    /usr/local
    /usr
    /opt/live555
    /opt/local
    ${CMAKE_SOURCE_DIR}/third_party/live555
    ${CMAKE_SOURCE_DIR}/external/live555
)

# Component list
set(Live555_COMPONENTS
    BasicUsageEnvironment
    groupsock
    liveMedia
    UsageEnvironment
)

# Find include directories
find_path(Live555_INCLUDE_DIR
    NAMES liveMedia/liveMedia.hh
    PATHS ${Live555_SEARCH_PATHS}
    PATH_SUFFIXES include live
    DOC "Live555 include directory"
)

# Find each component library
foreach(component ${Live555_COMPONENTS})
    find_library(Live555_${component}_LIBRARY
        NAMES ${component} lib${component} ${component}.lib
        PATHS ${Live555_SEARCH_PATHS}
        PATH_SUFFIXES lib lib64
        DOC "Live555 ${component} library"
    )
    
    if(Live555_${component}_LIBRARY)
        set(Live555_${component}_FOUND TRUE)
        list(APPEND Live555_LIBRARIES ${Live555_${component}_LIBRARY})
    else()
        set(Live555_${component}_FOUND FALSE)
    endif()
endforeach()

# Set include directories - Live555 requires multiple include paths
if(Live555_INCLUDE_DIR)
    set(Live555_INCLUDE_DIRS
        ${Live555_INCLUDE_DIR}/BasicUsageEnvironment
        ${Live555_INCLUDE_DIR}/groupsock
        ${Live555_INCLUDE_DIR}/liveMedia
        ${Live555_INCLUDE_DIR}/UsageEnvironment
        ${Live555_INCLUDE_DIR}
    )
endif()

# Find version from header if available
if(Live555_INCLUDE_DIR AND EXISTS "${Live555_INCLUDE_DIR}/liveMedia/liveMedia_version.hh")
    file(STRINGS "${Live555_INCLUDE_DIR}/liveMedia/liveMedia_version.hh" version_line
        REGEX "^#define[ \t]+LIVEMEDIA_LIBRARY_VERSION_STRING[ \t]+\"[^\"]+\"")
    if(version_line)
        string(REGEX REPLACE "^#define[ \t]+LIVEMEDIA_LIBRARY_VERSION_STRING[ \t]+\"([^\"]+)\".*" "\\1"
            Live555_VERSION "${version_line}")
    endif()
endif()

# Handle standard find package arguments
find_package_handle_standard_args(Live555
    REQUIRED_VARS Live555_INCLUDE_DIR Live555_LIBRARIES
    VERSION_VAR Live555_VERSION
    HANDLE_COMPONENTS
)

# Create imported targets
if(Live555_FOUND AND NOT TARGET Live555::Live555)
    add_library(Live555::Live555 INTERFACE IMPORTED)
    set_target_properties(Live555::Live555 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${Live555_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${Live555_LIBRARIES}"
    )
    
    # Create individual component targets
    foreach(component ${Live555_COMPONENTS})
        if(Live555_${component}_FOUND AND NOT TARGET Live555::${component})
            add_library(Live555::${component} UNKNOWN IMPORTED)
            set_target_properties(Live555::${component} PROPERTIES
                IMPORTED_LOCATION "${Live555_${component}_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${Live555_INCLUDE_DIRS}"
            )
        endif()
    endforeach()
endif()

# Additional platform-specific libraries that live555 might need
if(Live555_FOUND)
    if(WIN32)
        list(APPEND Live555_LIBRARIES ws2_32)
    elseif(UNIX)
        list(APPEND Live555_LIBRARIES pthread)
    endif()
endif()

# Print results
if(Live555_FOUND)
    message(STATUS "Found Live555: ${Live555_INCLUDE_DIR}")
    message(STATUS "Live555 libraries: ${Live555_LIBRARIES}")
    if(Live555_VERSION)
        message(STATUS "Live555 version: ${Live555_VERSION}")
    endif()
    foreach(component ${Live555_COMPONENTS})
        if(Live555_${component}_FOUND)
            message(STATUS "  Found ${component}")
        endif()
    endforeach()
else()
    message(STATUS "Live555 not found. Please set Live555_ROOT or install Live555")
endif()