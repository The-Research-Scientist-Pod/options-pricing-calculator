//
// Created by Yusufu Shehu on 18/01/2025.
//

#include "pricer/option.h"
#include <cmath>
#include <stdexcept>

namespace pricer {

Option::Option(OptionType type,
               double strike,
               double expiry,
               double spot,
               double rate,
               double volatility,
               double dividend)
    : type_(type)
    , strike_(strike)
    , expiry_(expiry)
    , spot_(spot)
    , rate_(rate)
    , volatility_(volatility)
    , dividend_(dividend)
    , engine_(nullptr)
{
    validateParameters();
}

double Option::price() const {
    if (!engine_) {
        throw std::runtime_error("No pricing engine set");
    }
    return engine_->calculate(*this);
}

double Option::delta() const {
    // Use finite difference method for now
    // Will be overridden by specific pricing engines later
    const double h = 0.01;
    const double spot = spot_;

    setSpot(spot + h);
    double price_up = price();

    setSpot(spot - h);
    double price_down = price();

    setSpot(spot);  // Reset spot price

    return (price_up - price_down) / (2 * h);
}

double Option::gamma() const {
    // Use finite difference method for now
    const double h = 0.01;
    const double spot = spot_;

    setSpot(spot + h);
    double price_up = price();

    setSpot(spot - h);
    double price_down = price();

    setSpot(spot);
    double price_mid = price();

    return (price_up - 2 * price_mid + price_down) / (h * h);
}

double Option::theta() const {
    // Implement basic finite difference
    // Will be overridden by specific pricing engines
    const double h = 1.0 / 365.0;  // One day
    const double expiry = expiry_;

    double original_price = price();
    expiry_ -= h;
    double new_price = price();
    expiry_ = expiry;  // Reset expiry

    return -(new_price - original_price) / h;
}

double Option::vega() const {
    // Use finite difference method
    const double h = 0.0001;
    const double vol = volatility_;

    setVolatility(vol + h);
    double price_up = price();

    setVolatility(vol - h);
    double price_down = price();

    setVolatility(vol);  // Reset volatility

    return (price_up - price_down) / (2 * h);
}

double Option::rho() const {
    // Use finite difference method
    const double h = 0.0001;
    const double rate = rate_;

    setRate(rate + h);
    double price_up = price();

    setRate(rate - h);
    double price_down = price();

    setRate(rate);  // Reset rate

    return (price_up - price_down) / (2 * h);
}

void Option::setPricingEngine(std::shared_ptr<PricingEngine> engine) {
    engine_ = engine;
}

void Option::setSpot(double spot) {
    spot_ = spot;
    validateParameters();
}

void Option::setRate(double rate) {
    rate_ = rate;
    validateParameters();
}

void Option::setVolatility(double volatility) {
    volatility_ = volatility;
    validateParameters();
}

void Option::setDividend(double dividend) {
    dividend_ = dividend;
    validateParameters();
}

void Option::validateParameters() const {
    if (strike_ <= 0.0) {
        throw std::invalid_argument("Strike price must be positive");
    }
    if (expiry_ <= 0.0) {
        throw std::invalid_argument("Time to expiry must be positive");
    }
    if (spot_ <= 0.0) {
        throw std::invalid_argument("Spot price must be positive");
    }
    if (volatility_ <= 0.0) {
        throw std::invalid_argument("Volatility must be positive");
    }
    if (dividend_ < 0.0) {
        throw std::invalid_argument("Dividend yield cannot be negative");
    }
}

double AmericanOption::price() const {
    if (!engine_) {
        throw std::runtime_error("No pricing engine set");
    }
    // The pricing engine will need to handle American options differently
    return engine_->calculate(*this);
}

} // namespace pricer