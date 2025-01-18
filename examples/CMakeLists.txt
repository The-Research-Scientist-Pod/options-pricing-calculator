# Create the example executable
add_executable(basic_usage basic_usage.cpp)

# Link against the library
target_include_directories(basic_usage
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(basic_usage
        PRIVATE
        options_pricer_lib
)