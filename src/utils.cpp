//
// Created by Yusufu Shehu on 18/01/2025.
//
#include "pricer/utils.h"
#include <cmath>
#include <stdexcept>


namespace pricer::utils {

double normalCDF(double x) {
    // Using Abramowitz and Stegun approximation (1.26.2.32)
    constexpr double a1 = 0.254829592;
    constexpr double a2 = -0.284496736;
    constexpr double a3 = 1.421413741;
    constexpr double a4 = -1.453152027;
    constexpr double a5 = 1.061405429;
    constexpr double p = 0.3275911;

    double sign = 1.0;
    if (x < 0) {
        sign = -1.0;
        x = -x;
    }

    double t = 1.0 / (1.0 + p * x);
    double y = 1.0 - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * std::exp(-x * x / 2.0);

    return 0.5 * (1.0 + sign * y);
}

double normalPDF(double x) {
    static const double sqrt2pi = std::sqrt(2.0 * M_PI);
    return std::exp(-x * x / 2.0) / sqrt2pi;
}

double inverseNormalCDF(double p) {
    if (p <= 0.0 || p >= 1.0) {
        throw std::invalid_argument("Probability must be between 0 and 1");
    }

    // Beasley-Springer-Moro algorithm
    static const double a[4] = {
        2.50662823884,
        -18.61500062529,
        41.39119773534,
        -25.44106049637
    };

    static const double b[4] = {
        -8.47351093090,
        23.08336743743,
        -21.06224101826,
        3.13082909833
    };

    static const double c[9] = {
        0.3374754822726147,
        0.9761690190917186,
        0.1607979714918209,
        0.0276438810333863,
        0.0038405729373609,
        0.0003951896511919,
        0.0000321767881768,
        0.0000002888167364,
        0.0000003960315187
    };

    double q = p - 0.5;
    double r;

    if (std::abs(q) <= 0.42) {
        r = q * q;
        double num = (((a[3] * r + a[2]) * r + a[1]) * r + a[0]) * q;
        double den = ((((b[3] * r + b[2]) * r + b[1]) * r + b[0]) * r + 1.0);
        return num / den;
    }

    r = (q > 0) ? (1.0 - p) : p;
    r = std::sqrt(-std::log(r));
    double x = ((((((((c[8] * r + c[7]) * r + c[6]) * r + c[5]) * r + c[4]) * r + c[3]) * r + c[2]) * r + c[1]) * r + c[0]);

    return (q > 0) ? x : -x;
}

std::vector<double> generateNormalVariates(size_t n, std::mt19937& rng) {
    std::normal_distribution<double> normal(0.0, 1.0);
    std::vector<double> variates(n);

    for (size_t i = 0; i < n; ++i) {
        variates[i] = normal(rng);
    }

    return variates;
}

std::vector<double> generateAntitheticNormalVariates(size_t n, std::mt19937& rng) {
    std::normal_distribution<double> normal(0.0, 1.0);
    std::vector<double> variates(2 * n);

    for (size_t i = 0; i < n; ++i) {
        double z = normal(rng);
        variates[i] = z;
        variates[i + n] = -z;  // Antithetic pair
    }

    return variates;
}

double discountFactor(double rate, double time) {
    return std::exp(-rate * time);
}

double forwardPrice(double spot, double rate, double dividend, double time) {
    return spot * std::exp((rate - dividend) * time);
}

    double impliedVolatility(double marketPrice, double spot, double strike,
                            double rate, double time, double dividend, bool isCall) {
    // Newton-Raphson implementation
    const double ACCURACY = 1.0e-5;
    const int MAX_ITERATIONS = 100;
    double sigma = 0.5;  // Initial guess

    for (int i = 0; i < MAX_ITERATIONS; i++) {
        // TODO: Implement proper Black-Scholes calculation here
        // For now, return placeholder to avoid unused parameter warnings
        (void)marketPrice;
        (void)spot;
        (void)strike;
        (void)rate;
        (void)time;
        (void)dividend;
        (void)isCall;
        (void)ACCURACY;
    }
    return sigma;
}
double historicalVolatility(const std::vector<double>& prices, const double timeStep) {
    if (prices.size() < 2) {
        throw std::invalid_argument("Need at least two prices to calculate volatility");
    }

    // Calculate log returns
    std::vector<double> logReturns;
    logReturns.reserve(prices.size() - 1);

    for (size_t i = 1; i < prices.size(); ++i) {
        logReturns.push_back(std::log(prices[i] / prices[i-1]));
    }

    // Calculate mean
    double sum = std::accumulate(logReturns.begin(), logReturns.end(), 0.0);
    double mean = sum / logReturns.size();

    // Calculate variance
    double squaredSum = 0.0;
    for (double r : logReturns) {
        squaredSum += (r - mean) * (r - mean);
    }
    double variance = squaredSum / (logReturns.size() - 1);

    // Annualize volatility
    return std::sqrt(variance / timeStep);
}

double presentValue(const std::vector<double>& cashFlows,
                   const std::vector<double>& times,
                   double rate) {
    if (cashFlows.size() != times.size()) {
        throw std::invalid_argument("Cash flows and times vectors must have same size");
    }

    double pv = 0.0;
    for (size_t i = 0; i < cashFlows.size(); ++i) {
        pv += cashFlows[i] * discountFactor(rate, times[i]);
    }

    return pv;
}

bool isApproxEqual(const double a, const double b, const double epsilon) {
    return std::abs(a - b) <= epsilon * std::max(std::abs(a), std::abs(b));
}
    // Add to end of file...

double utils::finiteDifferenceDelta(const Option& option, const double h) {
    const double spot = option.getSpot();

    const_cast<Option&>(option).setSpot(spot + h);
    const double price_up = option.price();

    const_cast<Option&>(option).setSpot(spot - h);
    const double price_down = option.price();

    const_cast<Option&>(option).setSpot(spot);  // Reset

    return (price_up - price_down) / (2 * h);
}

double utils::finiteDifferenceGamma(const Option& option, const double h) {
    const double spot = option.getSpot();

    const_cast<Option&>(option).setSpot(spot + h);
    const double price_up = option.price();

    const_cast<Option&>(option).setSpot(spot - h);
    const double price_down = option.price();

    const_cast<Option&>(option).setSpot(spot);
    const double price_mid = option.price();

    return (price_up - 2 * price_mid + price_down) / (h * h);
}

double utils::finiteDifferenceTheta(const Option& option, const double h) {
    const double expiry = option.getExpiry();

    const double original_price = option.price();
    const_cast<Option&>(option).setExpiry(expiry - h);
    const double new_price = option.price();
    const_cast<Option&>(option).setExpiry(expiry);  // Reset

    return -(new_price - original_price) / h;
}

double utils::finiteDifferenceVega(const Option& option, const double h) {
    const double vol = option.getVolatility();

    const_cast<Option&>(option).setVolatility(vol + h);
    const double price_up = option.price();

    const_cast<Option&>(option).setVolatility(vol - h);
    const double price_down = option.price();

    const_cast<Option&>(option).setVolatility(vol);  // Reset

    return (price_up - price_down) / (2 * h);
}

double utils::finiteDifferenceRho(const Option& option, const double h) {
    const double rate = option.getRate();

    const_cast<Option&>(option).setRate(rate + h);
    const double price_up = option.price();

    const_cast<Option&>(option).setRate(rate - h);
    const double price_down = option.price();

    const_cast<Option&>(option).setRate(rate);  // Reset

    return (price_up - price_down) / (2 * h);
}

} // namespace pricer::utils
