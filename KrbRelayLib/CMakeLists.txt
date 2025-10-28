cmake_minimum_required(VERSION 3.15)
project(KrbRelayLib VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to build as shared (DLL) or static library
option(BUILD_SHARED_LIBS "Build shared libraries (DLL)" ON)

# Source files
set(KRBRELAY_SOURCES
    src/SecurityBuffer.cpp
    src/ComUtilities.cpp
    src/KerberosRelay.cpp
    src/NativeWrappers.cpp
)

# Header files
set(KRBRELAY_HEADERS
    include/KrbRelay/SecurityBuffer.h
    include/KrbRelay/ComUtilities.h
    include/KrbRelay/KerberosRelay.h
    include/KrbRelay/NativeWrappers.h
    include/KrbRelay/Types.h
)

# Create library target
add_library(KrbRelay ${KRBRELAY_SOURCES} ${KRBRELAY_HEADERS})

# Set library output name based on build type
if(BUILD_SHARED_LIBS)
    set_target_properties(KrbRelay PROPERTIES
        OUTPUT_NAME "KrbRelay"
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
else()
    set_target_properties(KrbRelay PROPERTIES
        OUTPUT_NAME "KrbRelay_static"
    )
endif()

# Include directories
target_include_directories(KrbRelay
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Windows-specific libraries
if(WIN32)
    target_link_libraries(KrbRelay
        PUBLIC
            ws2_32
            secur32
            wldap32
            ole32
            oleaut32
            rpcrt4
            advapi32
            netapi32
    )
endif()

# Compiler definitions
if(BUILD_SHARED_LIBS)
    target_compile_definitions(KrbRelay
        PRIVATE KRBRELAY_EXPORTS
        PUBLIC KRBRELAY_SHARED
    )
else()
    target_compile_definitions(KrbRelay
        PUBLIC KRBRELAY_STATIC
    )
endif()

# Installation rules
install(TARGETS KrbRelay
    EXPORT KrbRelayTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/
    DESTINATION include
)

# Export targets
install(EXPORT KrbRelayTargets
    FILE KrbRelayTargets.cmake
    NAMESPACE KrbRelay::
    DESTINATION lib/cmake/KrbRelay
)

# Generate config file
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/KrbRelayConfig.cmake
    INSTALL_DESTINATION lib/cmake/KrbRelay
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/KrbRelayConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/KrbRelayConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/KrbRelayConfigVersion.cmake
    DESTINATION lib/cmake/KrbRelay
)

# Example executable (optional)
option(BUILD_EXAMPLES "Build example programs" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
