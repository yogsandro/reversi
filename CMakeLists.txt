cmake_minimum_required(VERSION 3.20)
project(othello LANGUAGES CXX)

# Build options
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Position independent code for shared library
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Library sources (adjust as needed)
# Prefer globbing only for small projects; for larger projects list files explicitly.
file(GLOB_RECURSE OTHELLO_SOURCES
    src/*.cpp
    include/*.h
)

# Create shared library
add_library(othello SHARED ${OTHELLO_SOURCES})

# Include directories
target_include_directories(othello 
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Set library name and version (optional)
set_target_properties(othello PROPERTIES
    OUTPUT_NAME "othello"
    VERSION 1.0.0
    SOVERSION 1
)

if (MSVC)
  target_compile_options(othello PRIVATE /W4)
else()
  target_compile_options(othello PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Executable that links the library
file(GLOB_RECURSE OTHELLO_APP_SOURCES main/*.cpp)
add_executable(othello-console-app ${OTHELLO_APP_SOURCES})
target_link_libraries(othello-console-app PRIVATE othello)

target_include_directories(othello-console-app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Installation rules
install(TARGETS othello
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)

# Export CMake config for find_package support (optional)
include(CMakePackageConfigHelpers)
set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/othello")
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/othelloConfigVersion.cmake"
    VERSION 1.0.0
    COMPATIBILITY AnyNewerVersion
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/othelloConfigVersion.cmake"
    DESTINATION "${config_install_dir}"
)

# Build type defaults
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Optional: Position headers for consumers
target_include_directories(othello PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

