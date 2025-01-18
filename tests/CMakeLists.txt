enable_testing()

set(TEST_SOURCES
        test_black_scholes.cpp
        test_monte_carlo.cpp
        test_binomial.cpp
)

add_executable(options_pricer_tests ${TEST_SOURCES})

target_link_libraries(options_pricer_tests
        PRIVATE
        options_pricer_lib
        GTest::GTest
        GTest::Main
)

add_test(NAME options_pricer_tests COMMAND options_pricer_tests)