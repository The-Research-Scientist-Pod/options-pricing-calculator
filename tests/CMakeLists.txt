enable_testing()

# Set the test sources
set(TEST_SOURCES
        test_main.cpp
        test_black_scholes.cpp
        test_monte_carlo.cpp
        test_binomial.cpp
)

# Create the test executable
add_executable(options_pricer_tests ${TEST_SOURCES})

# Link with options_pricer_lib and GTest
target_link_libraries(options_pricer_tests
        PRIVATE
        options_pricer_lib
        GTest::gtest
        GTest::gtest_main
)

target_include_directories(options_pricer_tests
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# Add test to CTest
add_test(NAME options_pricer_tests COMMAND options_pricer_tests)