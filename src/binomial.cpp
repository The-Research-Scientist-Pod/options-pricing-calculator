#include "pricer/binomial.h"
#include <cmath>
#include <stdexcept>

namespace pricer {

BinomialTreeEngine::BinomialTreeEngine(size_t num_steps, bool use_bbs)
    : num_steps_(num_steps)
    , use_bbs_(use_bbs) {
    if (num_steps_ == 0) {
        throw std::invalid_argument("Number of steps must be positive");
    }
}

double BinomialTreeEngine::calculate(const Option& option) const {
    double value = calculateWithParameters(option, num_steps_);

    if (use_bbs_) {
        value = applyBBSExtrapolation(option, value);
    }

    return value;
}

double BinomialTreeEngine::calculateWithParameters(const Option& option, size_t steps) const {
    // Store original number of steps
    size_t original_steps = num_steps_;
    const_cast<BinomialTreeEngine*>(this)->num_steps_ = steps;

    // Build price tree
    auto price_tree = buildPriceTree(option);

    // Calculate option values
    bool is_american = dynamic_cast<const AmericanOption*>(&option) != nullptr;
    auto option_values = calculateOptionValues(option, price_tree, is_american);

    // Restore original number of steps
    const_cast<BinomialTreeEngine*>(this)->num_steps_ = original_steps;

    // Return root node value
    return option_values[0];
}

std::vector<double> BinomialTreeEngine::buildPriceTree(const Option& option) const {
    const double dt = option.getExpiry() / num_steps_;
    auto [u, d, p] = calculateParameters(option, dt);

    std::vector<double> price_tree((num_steps_ + 1) * (num_steps_ + 2) / 2);
    double S = option.getSpot();

    // Build tree from bottom to top
    for (size_t step = 0; step <= num_steps_; ++step) {
        for (size_t node = 0; node <= step; ++node) {
            // Calculate price at this node
            price_tree[getIndex(step, node)] = S * std::pow(u, node) * std::pow(d, step - node);
        }
    }

    return price_tree;
}

std::vector<double> BinomialTreeEngine::calculateOptionValues(
    const Option& option,
    const std::vector<double>& price_tree,
    bool american) const {

    const double dt = option.getExpiry() / num_steps_;
    auto [u, d, p] = calculateParameters(option, dt);
    const double df = std::exp(-option.getRate() * dt);

    std::vector<double> values((num_steps_ + 1) * (num_steps_ + 2) / 2);

    // Initialize terminal values
    for (size_t node = 0; node <= num_steps_; ++node) {
        values[getIndex(num_steps_, node)] =
            calculatePayoff(option, price_tree[getIndex(num_steps_, node)]);
    }

    // Work backwards through the tree
    for (size_t step = num_steps_ - 1; step != size_t(-1); --step) {
        for (size_t node = 0; node <= step; ++node) {
            // Get option value from backwards induction
            double continuation = df * (
                p * values[getIndex(step + 1, node + 1)] +
                (1 - p) * values[getIndex(step + 1, node)]
            );

            if (american) {
                // Check for early exercise
                double exercise = calculatePayoff(option, price_tree[getIndex(step, node)]);
                values[getIndex(step, node)] = std::max(continuation, exercise);
            } else {
                values[getIndex(step, node)] = continuation;
            }
        }
    }

    return values;
}

std::tuple<double, double, double> BinomialTreeEngine::calculateParameters(
    const Option& option,
    const double dt) {
    const double sigma = option.getVolatility();
    const double r = option.getRate();
    const double q = option.getDividend();

    // Calculate up and down factors
    double u = std::exp(sigma * std::sqrt(dt));
    double d = 1.0 / u;

    // Risk-neutral probability
    double p = (std::exp((r - q) * dt) - d) / (u - d);

    return {u, d, p};
}

double BinomialTreeEngine::applyBBSExtrapolation(
    const Option& option,
    const double value) const {

    // Calculate with double the steps
    const size_t double_steps = 2 * num_steps_;
    const double value2 = calculateWithParameters(option, double_steps);

    // Apply Richardson extrapolation
    return 2.0 * value2 - value;
}

double BinomialTreeEngine::calculatePayoff(
    const Option& option,
    double spot_price) {

    double strike = option.getStrike();

    if (option.getType() == OptionType::Call) {
        return std::max(spot_price - strike, 0.0);
    } else {
        return std::max(strike - spot_price, 0.0);
    }
}

size_t BinomialTreeEngine::getIndex(size_t step, size_t node) const {
    return (step * (step + 1)) / 2 + node;
}

double BinomialTreeEngine::calculateDelta(const Option& option) const {
    const double h = 0.01 * option.getSpot();
    double spot = option.getSpot();

    const_cast<Option&>(option).setSpot(spot + h);
    double up_price = calculate(option);

    const_cast<Option&>(option).setSpot(spot - h);
    double down_price = calculate(option);

    const_cast<Option&>(option).setSpot(spot);

    return (up_price - down_price) / (2.0 * h);
}

double BinomialTreeEngine::calculateGamma(const Option& option) const {
    const double h = 0.01 * option.getSpot();
    double spot = option.getSpot();

    const_cast<Option&>(option).setSpot(spot + h);
    double up_price = calculate(option);

    const_cast<Option&>(option).setSpot(spot - h);
    double down_price = calculate(option);

    const_cast<Option&>(option).setSpot(spot);
    double mid_price = calculate(option);

    return (up_price - 2.0 * mid_price + down_price) / (h * h);
}

double BinomialTreeEngine::calculateTheta(const Option& option) const {
    const double h = 1.0 / 365.0;  // One day
    double expiry = option.getExpiry();

    const_cast<Option&>(option).setExpiry(expiry - h);
    double new_price = calculate(option);

    const_cast<Option&>(option).setExpiry(expiry);
    double original_price = calculate(option);

    return -(original_price - new_price) / h;
}

double BinomialTreeEngine::calculateVega(const Option& option) const {
    const double h = 0.0001;
    double vol = option.getVolatility();

    const_cast<Option&>(option).setVolatility(vol + h);
    double up_price = calculate(option);

    const_cast<Option&>(option).setVolatility(vol - h);
    double down_price = calculate(option);

    const_cast<Option&>(option).setVolatility(vol);

    return (up_price - down_price) / (2.0 * h);
}

double BinomialTreeEngine::calculateRho(const Option& option) const {
    const double h = 0.0001;
    double rate = option.getRate();

    const_cast<Option&>(option).setRate(rate + h);
    double up_price = calculate(option);

    const_cast<Option&>(option).setRate(rate - h);
    double down_price = calculate(option);

    const_cast<Option&>(option).setRate(rate);

    return (up_price - down_price) / (2.0 * h);
}

} // namespace pricer