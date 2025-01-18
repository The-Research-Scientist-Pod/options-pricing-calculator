set(SOURCES
        black_scholes.cpp
        monte_carlo.cpp
        binomial.cpp
        option.cpp
        utils.cpp
)

add_library(options_pricer_lib ${SOURCES})

target_include_directories(options_pricer_lib
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# GUI subdirectory
add_subdirectory(gui)