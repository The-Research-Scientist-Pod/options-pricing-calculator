set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify sources
set(SOURCES
        main.cpp
        main_window.cpp
)

set(HEADERS
        main_window.h
)

# Tell CMake to run moc when necessary
qt_wrap_cpp(MOC_SOURCES ${HEADERS})

# Create executable
add_executable(options_pricer_gui
        ${SOURCES}
        ${HEADERS}
        ${MOC_SOURCES}
)

# Link Qt libraries
target_link_libraries(options_pricer_gui
        PRIVATE
        options_pricer_lib
        Qt6::Widgets
)

target_include_directories(options_pricer_gui
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)