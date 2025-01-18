cmake_minimum_required(VERSION 3.15)
project(options_pricer VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

# Find required packages
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(GTest REQUIRED)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(tests)

# Installation rules
install(TARGETS options_pricer_lib
        EXPORT options_pricer-targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)

install(DIRECTORY include/pricer
        DESTINATION include
        FILES_MATCHING PATTERN "*.hpp"
)

# Export targets
install(EXPORT options_pricer-targets
        FILE options_pricer-targets.cmake
        NAMESPACE options_pricer::
        DESTINATION lib/cmake/options_pricer
)