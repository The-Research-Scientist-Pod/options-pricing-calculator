#include "pricer/monte_carlo.h"
#include "pricer/black_scholes.h"
#include "pricer/option.h"
#include <gtest/gtest.h>
#include <memory>
#include <chrono>
#include <vector>
#include <numeric>

class MonteCarloTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create engines with high number of paths for accuracy
        mc_engine = std::make_shared<pricer::MonteCarloEngine>(1000000, 252, true, 8);
        bs_engine = pricer::makeBlackScholesPricingEngine();
    }

    // Helper function to create a standard European call option
    static std::unique_ptr<pricer::Option> makeEuropeanCall() {
        return std::make_unique<pricer::EuropeanOption>(
            pricer::OptionType::Call,
            100.0,  // Strike
            1.0,    // Expiry (1 year)
            100.0,  // Spot
            0.05,   // Risk-free rate (5%)
            0.2,    // Volatility (20%)
            0.0     // No dividend
        );
    }

    // Helper function to create a standard European put option
    static std::unique_ptr<pricer::Option> makeEuropeanPut() {
        return std::make_unique<pricer::EuropeanOption>(
            pricer::OptionType::Put,
            100.0,  // Strike
            1.0,    // Expiry
            100.0,  // Spot
            0.05,   // Rate
            0.2,    // Volatility
            0.0     // No dividend
        );
    }

    // Helper function to create an American put option
    static std::unique_ptr<pricer::Option> makeAmericanPut() {
        return std::make_unique<pricer::AmericanOption>(
            pricer::OptionType::Put,
            100.0,  // Strike
            1.0,    // Expiry
            100.0,  // Spot
            0.05,   // Rate
            0.2,    // Volatility
            0.0     // No dividend
        );
    }

    std::shared_ptr<pricer::MonteCarloEngine> mc_engine;
    std::shared_ptr<pricer::PricingEngine> bs_engine;
    const double tolerance = 0.1;  // Higher tolerance due to Monte Carlo randomness
};

// Test European call option against Black-Scholes
TEST_F(MonteCarloTest, EuropeanCallVsBlackScholes) {
    const auto option = makeEuropeanCall();

    // Black-Scholes engine
    option->setPricingEngine(bs_engine);
    const double bs_price = option->price();
    std::cout << "Debug: Black-Scholes Price: " << bs_price << std::endl;

    // Monte Carlo engine
    mc_engine->setNumPaths(100000); // Increase paths
    option->setPricingEngine(mc_engine);
    const double mc_price = option->price();
    std::cout << "Debug: Monte Carlo Price: " << mc_price << std::endl;

    // Confidence interval
    auto [fst, snd] = mc_engine->getConfidenceInterval(*option);
    std::cout << "Debug: Monte Carlo Confidence Interval: [" << fst << ", " << snd << "]" << std::endl;

    EXPECT_NEAR(mc_price, bs_price, 0.1);
    EXPECT_TRUE(bs_price >= fst && bs_price <= snd);
}


// Test European put option against Black-Scholes
TEST_F(MonteCarloTest, EuropeanPutVsBlackScholes) {
    auto option = makeEuropeanPut();

    option->setPricingEngine(bs_engine);
    double bs_price = option->price();

    option->setPricingEngine(mc_engine);
    double mc_price = option->price();

    EXPECT_NEAR(mc_price, bs_price, tolerance);
}

// Test Greeks calculation
TEST_F(MonteCarloTest, GreeksVsBlackScholes) {
    auto option = makeEuropeanCall();

    option->setPricingEngine(bs_engine);
    double bs_delta = option->delta();
    double bs_gamma = option->gamma();
    double bs_theta = option->theta();

    option->setPricingEngine(mc_engine);
    double mc_delta = option->delta();
    double mc_gamma = option->gamma();
    double mc_theta = option->theta();

    EXPECT_NEAR(mc_delta, bs_delta, tolerance);
    EXPECT_NEAR(mc_gamma, bs_gamma, tolerance);
    EXPECT_NEAR(mc_theta, bs_theta, tolerance * 10); // Wider tolerance for theta
}

// Test convergence with increasing number of paths
TEST_F(MonteCarloTest, Convergence) {
    auto option = makeEuropeanCall();

    option->setPricingEngine(bs_engine);
    double bs_price = option->price();

    std::vector<size_t> path_counts = {1000, 10000, 100000, 1000000};
    std::vector<double> errors;

    for (size_t paths : path_counts) {
        auto mc = std::make_shared<pricer::MonteCarloEngine>(paths, 252, true, 8);
        option->setPricingEngine(mc);
        double price = option->price();
        errors.push_back(std::abs(price - bs_price));
    }

    // Verify that error decreases with more paths
    for (size_t i = 1; i < errors.size(); ++i) {
        EXPECT_LT(errors[i], errors[i-1]);
    }
}

// Test impact of antithetic variance reduction
TEST_F(MonteCarloTest, AntitheticVarianceReduction) {
    const auto option = makeEuropeanCall();

    // Create engines with and without antithetic variates
    const auto mc_no_antithetic = std::make_shared<pricer::MonteCarloEngine>(100000, 252, false, 8);
    const auto mc_with_antithetic = std::make_shared<pricer::MonteCarloEngine>(100000, 252, true, 8);

    // Run multiple simulations and collect standard errors
    std::vector<double> errors_no_antithetic;
    std::vector<double> errors_with_antithetic;

    for (int i = 0; i < 10; ++i) {
        option->setPricingEngine(mc_no_antithetic);
        //option->price();  // Calculate price to update confidence interval
        auto ci1 = mc_no_antithetic->getConfidenceInterval(*option);
        errors_no_antithetic.push_back(ci1.second - ci1.first);

        option->setPricingEngine(mc_with_antithetic);
        //option->price();  // Calculate price to update confidence interval
        auto ci2 = mc_with_antithetic->getConfidenceInterval(*option);
        errors_with_antithetic.push_back(ci2.second - ci2.first);
    }

    // Calculate average confidence interval width
    const double avg_error_no_antithetic = std::accumulate(errors_no_antithetic.begin(),
                                                           errors_no_antithetic.end(), 0.0)
                                           / errors_no_antithetic.size();

    const double avg_error_with_antithetic = std::accumulate(errors_with_antithetic.begin(),
                                                             errors_with_antithetic.end(), 0.0)
                                             / errors_with_antithetic.size();

    // Antithetic variates should reduce variance
    EXPECT_LT(avg_error_with_antithetic, avg_error_no_antithetic);
}

// Test parallel execution performance
TEST_F(MonteCarloTest, ParallelPerformance) {
    const auto option = makeEuropeanCall();

    // Create single-threaded and multi-threaded engines
    const auto mc_single = std::make_shared<pricer::MonteCarloEngine>(1000000, 252, true, 1);
    const auto mc_multi = std::make_shared<pricer::MonteCarloEngine>(1000000, 252, true, 8);

    // Measure single-threaded time and price
    auto start = std::chrono::high_resolution_clock::now();
    option->setPricingEngine(mc_single);
    double single_thread_price = option->price();
    auto end = std::chrono::high_resolution_clock::now();
    auto single_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Measure multi-threaded time and price
    start = std::chrono::high_resolution_clock::now();
    option->setPricingEngine(mc_multi);
    const double multi_thread_price = option->price();
    end = std::chrono::high_resolution_clock::now();
    const auto multi_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Prices should be similar
    EXPECT_NEAR(single_thread_price, multi_thread_price, tolerance);
    
    // Multi-threaded should be faster
    EXPECT_LT(multi_time.count(), single_time.count());
}

// Test the impact of time steps
TEST_F(MonteCarloTest, TimeStepSensitivity) {
    auto option = makeEuropeanCall();
    option->setPricingEngine(bs_engine);
    double bs_price = option->price();
    
    std::vector<size_t> step_counts = {52, 252, 504};  // Weekly, Daily, Twice-daily
    std::vector<double> errors;
    
    for (size_t steps : step_counts) {
        auto mc = std::make_shared<pricer::MonteCarloEngine>(1000000, steps, true, 8);
        option->setPricingEngine(mc);
        double mc_price = option->price();
        errors.push_back(std::abs(mc_price - bs_price));
    }
    
    // More time steps should generally improve accuracy
    for (size_t i = 1; i < errors.size(); ++i) {
        EXPECT_LE(errors[i], errors[i-1] * 1.1);  // Allow 10% tolerance
    }
}