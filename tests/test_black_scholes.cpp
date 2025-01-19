#include "pricer/black_scholes.h"
#include "pricer/option.h"
#include <gtest/gtest.h>
#include <memory>
#include <cmath>

class BlackScholesTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = pricer::makeBlackScholesPricingEngine();
    }

    // Helper function to create a standard European call option
    static std::unique_ptr<pricer::Option> makeStandardCall() {
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
    static std::unique_ptr<pricer::Option> makeStandardPut() {
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

    std::shared_ptr<pricer::PricingEngine> engine;
    const double tolerance = 1e-2;  // Tolerance for floating-point comparisons
};

// Test at-the-money call option pricing
TEST_F(BlackScholesTest, AtTheMoneyCall) {
    auto option = makeStandardCall();
    option->setPricingEngine(engine);

    double expected_price = 10.4505;  // Updated with erfc implementation
    EXPECT_NEAR(option->price(), expected_price, tolerance);
}

// Test at-the-money put option pricing
TEST_F(BlackScholesTest, AtTheMoneyPut) {
    auto option = makeStandardPut();
    option->setPricingEngine(engine);

    double expected_price = 5.5735;  // Updated with erfc implementation
    EXPECT_NEAR(option->price(), expected_price, tolerance);
}

// Test in-the-money call option
TEST_F(BlackScholesTest, InTheMoneyCall) {
    auto option = makeStandardCall();
    option->setSpot(120.0);  // Spot > Strike
    option->setPricingEngine(engine);

    double expected_price = 26.17;  // Updated with erfc implementation
    EXPECT_NEAR(option->price(), expected_price, tolerance);
}

// Test out-of-the-money call option
TEST_F(BlackScholesTest, OutOfTheMoneyCall) {
    auto option = makeStandardCall();
    option->setSpot(80.0);  // Spot < Strike
    option->setPricingEngine(engine);

    double expected_price = 1.86;  // Updated with erfc implementation
    EXPECT_NEAR(option->price(), expected_price, tolerance);
}

// Test call option Greeks
// Test call option Greeks
TEST_F(BlackScholesTest, CallGreeks) {
    const auto option = makeStandardCall();
    option->setPricingEngine(engine);

    EXPECT_NEAR(option->delta(), 0.6368, tolerance);
    EXPECT_NEAR(option->gamma(), 0.0195, tolerance);
    EXPECT_NEAR(option->vega(), 0.375, tolerance);
    EXPECT_NEAR(option->theta(), -0.018, tolerance);  // Updated to daily theta
    EXPECT_NEAR(option->rho(), 0.5323, tolerance);
}

// Test put option Greeks
TEST_F(BlackScholesTest, PutGreeks) {
    const auto option = makeStandardPut();
    option->setPricingEngine(engine);

    EXPECT_NEAR(option->delta(), -0.3632, tolerance);
    EXPECT_NEAR(option->gamma(), 0.0195, tolerance);
    EXPECT_NEAR(option->vega(), 0.375, tolerance);
    EXPECT_NEAR(option->theta(), -0.003, tolerance);  // Updated to daily theta
    EXPECT_NEAR(option->rho(), -0.4189, tolerance);
}

// Test option with dividend yield
TEST_F(BlackScholesTest, DividendEffect) {
    auto option = makeStandardCall();
    option->setDividend(0.03);  // 3% dividend yield
    option->setPricingEngine(engine);

    double expected_price = 8.652529;  // Updated with erfc implementation
    EXPECT_NEAR(option->price(), expected_price, tolerance);
}

// Test impact of volatility changes
TEST_F(BlackScholesTest, VolatilityEffect) {
    auto option = makeStandardCall();
    option->setPricingEngine(engine);

    double base_price = option->price();
    option->setVolatility(0.3);  // Increase volatility to 30%

    // Price should increase with higher volatility
    EXPECT_GT(option->price(), base_price);
}

// Test invalid parameters
TEST_F(BlackScholesTest, InvalidParameters) {
    EXPECT_THROW({
        pricer::EuropeanOption option(
            pricer::OptionType::Call,
            -100.0,  // Negative strike price
            1.0,
            100.0,
            0.05,
            0.2,
            0.0
        );
    }, std::invalid_argument);

    EXPECT_THROW({
        pricer::EuropeanOption option(
            pricer::OptionType::Call,
            100.0,
            1.0,
            100.0,
            0.05,
            -0.2,  // Negative volatility
            0.0
        );
    }, std::invalid_argument);
}

// Test put-call parity
TEST_F(BlackScholesTest, PutCallParity) {
    auto call = makeStandardCall();
    auto put = makeStandardPut();
    call->setPricingEngine(engine);
    put->setPricingEngine(engine);

    double S = call->getSpot();
    double K = call->getStrike();
    double r = call->getRate();
    double T = call->getExpiry();

    // Put-call parity formula: C - P = S - K*exp(-rT)
    double lhs = call->price() - put->price();
    double rhs = S - K * std::exp(-r * T);

    EXPECT_NEAR(lhs, rhs, tolerance);
}

// Test time decay effect
TEST_F(BlackScholesTest, TimeDecay) {
    const auto option = makeStandardCall();
    option->setPricingEngine(engine);

    const double original_price = option->price();

    // Create same option with less time to expiry
    const auto shorter_option = std::make_unique<pricer::EuropeanOption>(
        pricer::OptionType::Call,
        100.0,
        0.5,   // 6 months instead of 1 year
        100.0,
        0.05,
        0.2,
        0.0
    );
    shorter_option->setPricingEngine(engine);

    // Option with less time value should be worth less (all else equal)
    EXPECT_LT(shorter_option->price(), original_price);
}