cmake_minimum_required(VERSION 3.15)
project(starship-game VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
if(APPLE)
    # For Apple Silicon M4 (ARM64)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
endif()

# Add compile warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Library source files
set(STARSHIP_SOURCES
    src/game.cxx
)

# Create the library
add_library(starship STATIC ${STARSHIP_SOURCES})

# Include directories
target_include_directories(starship
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Example executable
add_executable(starship-terminal examples/main.cxx)

# Set Homebrew prefix for macOS
if(APPLE)
    set(HOMEBREW_PREFIX "/opt/homebrew")
    set(CMAKE_PREFIX_PATH "${HOMEBREW_PREFIX};${CMAKE_PREFIX_PATH}")
    # Manually add Homebrew include paths
    include_directories("${HOMEBREW_PREFIX}/include")
endif()

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Ensure include directories are set
target_include_directories(starship-terminal PRIVATE 
    ${SDL2_INCLUDE_DIRS}
    "${HOMEBREW_PREFIX}/include"
    /opt/homebrew/opt/sdl2_ttf/include
)
target_link_directories(starship-terminal PRIVATE 
    /opt/homebrew/lib
    /opt/homebrew/opt/sdl2_ttf/lib
)
target_link_libraries(starship-terminal PRIVATE starship ${SDL2_LIBRARIES} SDL2_ttf)

# Installation rules
install(TARGETS starship
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(DIRECTORY include/starship
    DESTINATION include
)

# Print build information
message(STATUS "Building Starship Game Library")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
if(APPLE)
    message(STATUS "Target Architecture: ${CMAKE_OSX_ARCHITECTURES}")
endif()
