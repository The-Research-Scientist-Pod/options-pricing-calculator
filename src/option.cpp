#include "pricer/option.h"
#include "pricer/engine.h"
#include "pricer/utils.h"
#include <stdexcept>

namespace pricer {

Option::Option(const OptionType type,
               const double strike,
               const double expiry,
               const double spot,
               const double rate,
               const double volatility,
               const double dividend)
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

void Option::checkEngine() const {
    if (!engine_) {
        throw std::runtime_error("No pricing engine set");
    }
}

double Option::price() const {
    checkEngine();
    return engine_->calculate(*this);
}

double Option::delta() const {
    checkEngine();
    return engine_->calculateDelta(*this);
}

double Option::gamma() const {
    checkEngine();
    return engine_->calculateGamma(*this);
}

double Option::theta() const {
    checkEngine();
    return engine_->calculateTheta(*this);
}

double Option::vega() const {
    checkEngine();
    return engine_->calculateVega(*this);
}

double Option::rho() const {
    checkEngine();
    return engine_->calculateRho(*this);
}

void Option::setPricingEngine(std::shared_ptr<PricingEngine> engine) {
    engine_ = std::move(engine);
}

void Option::setSpot(const double spot) {
    spot_ = spot;
    validateParameters();
}

void Option::setRate(const double rate) {
    rate_ = rate;
    validateParameters();
}

void Option::setVolatility(const double volatility) {
    volatility_ = volatility;
    validateParameters();
}

void Option::setDividend(const double dividend) {
    dividend_ = dividend;
    validateParameters();
}

void Option::setExpiry(const double expiry) {
    expiry_ = expiry;
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
    checkEngine();
    return engine_->calculate(*this);
}

} // namespace pricer