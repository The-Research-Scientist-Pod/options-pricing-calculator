#include "pricer/binomial.h"
#include "pricer/black_scholes.h"
#include "pricer/option.h"
#include <gtest/gtest.h>
#include <memory>
#include <vector>

class BinomialTreeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create engines with sufficient number of steps for accuracy
        bin_engine = std::make_shared<pricer::BinomialTreeEngine>(1000);
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

    std::shared_ptr<pricer::BinomialTreeEngine> bin_engine;
    std::shared_ptr<pricer::PricingEngine> bs_engine;
    const double tolerance = 0.01;  // 1 cent accuracy
};

// Test European call option against Black-Scholes
TEST_F(BinomialTreeTest, EuropeanCallVsBlackScholes) {
    auto option = makeEuropeanCall();

    option->setPricingEngine(bs_engine);
    double bs_price = option->price();

    option->setPricingEngine(bin_engine);
    double bin_price = option->price();

    EXPECT_NEAR(bin_price, bs_price, tolerance);
}

// Test European put option against Black-Scholes
TEST_F(BinomialTreeTest, EuropeanPutVsBlackScholes) {
    auto option = makeEuropeanPut();

    option->setPricingEngine(bs_engine);
    double bs_price = option->price();

    option->setPricingEngine(bin_engine);
    double bin_price = option->price();

    EXPECT_NEAR(bin_price, bs_price, tolerance);
}

// Test convergence with increasing number of steps
TEST_F(BinomialTreeTest, Convergence) {
    auto option = makeEuropeanCall();

    option->setPricingEngine(bs_engine);
    double bs_price = option->price();

    std::vector<size_t> steps = {50, 100, 200, 400, 800};
    std::vector<double> errors;

    for (size_t n : steps) {
        auto bin = std::make_shared<pricer::BinomialTreeEngine>(n);
        option->setPricingEngine(bin);
        double price = option->price();
        errors.push_back(std::abs(price - bs_price));
    }

    // Verify that error decreases with more steps
    for (size_t i = 1; i < errors.size(); ++i) {
        EXPECT_LT(errors[i], errors[i-1]);
    }
}

// Test early exercise premium for American put
TEST_F(BinomialTreeTest, AmericanPutPremium) {
    auto euro_put = std::make_unique<pricer::EuropeanOption>(
        pricer::OptionType::Put,
        100.0,  // Strike
        1.0,    // Expiry
        90.0,   // Spot (in-the-money)
        0.05,   // Rate
        0.2,    // Volatility
        0.0     // No dividend
    );

    auto amer_put = std::make_unique<pricer::AmericanOption>(
        pricer::OptionType::Put,
        100.0,  // Strike
        1.0,    // Expiry
        90.0,   // Spot
        0.05,   // Rate
        0.2,    // Volatility
        0.0     // No dividend
    );

    euro_put->setPricingEngine(bin_engine);
    amer_put->setPricingEngine(bin_engine);

    // American put should be worth more due to early exercise
    EXPECT_GT(amer_put->price(), euro_put->price());
}

// Test Greeks calculation
TEST_F(BinomialTreeTest, Greeks) {
    const auto option = makeEuropeanCall();

    option->setPricingEngine(bs_engine);
    const double bs_delta = option->delta();
    const double bs_gamma = option->gamma();
    const double bs_theta = option->theta();

    option->setPricingEngine(bin_engine);
    const double bin_delta = option->delta();
    const double bin_gamma = option->gamma();
    const double bin_theta = option->theta();

    EXPECT_NEAR(bin_delta, bs_delta, tolerance);
    EXPECT_NEAR(bin_gamma, bs_gamma, tolerance);
    //EXPECT_NEAR(bin_theta, bs_theta, tolerance * 10);  // Wider tolerance for theta
}

// Test put-call parity for European options
TEST_F(BinomialTreeTest, PutCallParity) {
    const auto call = makeEuropeanCall();
    auto put = makeEuropeanPut();

    call->setPricingEngine(bin_engine);
    put->setPricingEngine(bin_engine);

    double S = call->getSpot();
    double K = call->getStrike();
    double r = call->getRate();
    double T = call->getExpiry();

    // Put-call parity: C - P = S - K*exp(-rT)
    double lhs = call->price() - put->price();
    double rhs = S - K * std::exp(-r * T);

    EXPECT_NEAR(lhs, rhs, tolerance);
}

// Test the impact of number of steps
TEST_F(BinomialTreeTest, StepCountSensitivity) {
    auto option = makeEuropeanCall();
    option->setPricingEngine(bs_engine);
    double bs_price = option->price();

    std::vector<size_t> step_counts = {100, 500, 1000};
    std::vector<double> errors;

    for (size_t steps : step_counts) {
        auto bin = std::make_shared<pricer::BinomialTreeEngine>(steps);
        option->setPricingEngine(bin);
        double bin_price = option->price();
        errors.push_back(std::abs(bin_price - bs_price));
    }

    // More steps should generally improve accuracy
    for (size_t i = 1; i < errors.size(); ++i) {
        EXPECT_LE(errors[i], errors[i-1]);
    }
}

// Test dividend effect
TEST_F(BinomialTreeTest, DividendEffect) {
    auto eur_call = makeEuropeanCall();
    auto amer_call = std::make_unique<pricer::AmericanOption>(
        pricer::OptionType::Call,
        100.0,  // Strike
        1.0,    // Expiry
        100.0,  // Spot
        0.05,   // Rate
        0.2,    // Volatility
        0.06    // High dividend yield (6%)
    );

    eur_call->setDividend(0.06);

    eur_call->setPricingEngine(bin_engine);
    amer_call->setPricingEngine(bin_engine);

    // With high dividends, American call should be worth more than European
    EXPECT_GT(amer_call->price(), eur_call->price());
}