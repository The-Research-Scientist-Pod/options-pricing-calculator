set(GUI_SOURCES
        main.cpp
        main_window.cpp
)

add_executable(options_pricer_gui ${GUI_SOURCES})

target_link_libraries(options_pricer_gui
        PRIVATE
        options_pricer_lib
        Qt6::Widgets
)