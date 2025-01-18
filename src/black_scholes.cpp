//
// Created by Yusufu Shehu on 18/01/2025.
//
#include "pricer/black_scholes.h"
#include <cmath>
#include <stdexcept>

namespace pricer {

double BlackScholesPricingEngine::calculate(const Option& option) const {
    // Extract parameters from the option
    const double S = option.getSpot();
    const double K = option.getStrike();
    const double T = option.getExpiry();
    const double r = option.getRate();
    const double sigma = option.getVolatility();
    const double q = option.getDividend();

    // Calculate d1 and d2
    const double d1 = calculateD1(S, K, r, q, sigma, T);
    const double d2 = calculateD2(d1, sigma, T);

    // Calculate option price based on type
    double price = 0.0;
    if (option.getType() == OptionType::Call) {
        price = S * exp(-q * T) * normalCDF(d1) - K * exp(-r * T) * normalCDF(d2);
    } else {  // Put option
        price = K * exp(-r * T) * normalCDF(-d2) - S * exp(-q * T) * normalCDF(-d1);
    }

    return price;
}

double BlackScholesPricingEngine::calculateDelta(const Option& option) const {
    const double S = option.getSpot();
    const double K = option.getStrike();
    const double T = option.getExpiry();
    const double r = option.getRate();
    const double sigma = option.getVolatility();
    const double q = option.getDividend();

    const double d1 = calculateD1(S, K, r, q, sigma, T);

    if (option.getType() == OptionType::Call) {
        return exp(-q * T) * normalCDF(d1);
    } else {
        return exp(-q * T) * (normalCDF(d1) - 1.0);
    }
}

double BlackScholesPricingEngine::calculateGamma(const Option& option) const {
    const double S = option.getSpot();
    const double K = option.getStrike();
    const double T = option.getExpiry();
    const double r = option.getRate();
    const double sigma = option.getVolatility();
    const double q = option.getDividend();

    const double d1 = calculateD1(S, K, r, q, sigma, T);

    // Gamma is the same for both calls and puts
    return exp(-q * T) * normalPDF(d1) / (S * sigma * sqrt(T));
}

double BlackScholesPricingEngine::calculateTheta(const Option& option) const {
    double S = option.getSpot();
    double K = option.getStrike();
    double T = option.getExpiry();
    double r = option.getRate();
    double sigma = option.getVolatility();
    double q = option.getDividend();

    double d1 = calculateD1(S, K, r, q, sigma, T);
    double d2 = calculateD2(d1, sigma, T);

    double theta = 0.0;
    if (option.getType() == OptionType::Call) {
        theta = -exp(-q * T) * S * normalPDF(d1) * sigma / (2 * sqrt(T))
                - r * K * exp(-r * T) * normalCDF(d2)
                + q * S * exp(-q * T) * normalCDF(d1);
    } else {
        theta = -exp(-q * T) * S * normalPDF(d1) * sigma / (2 * sqrt(T))
                + r * K * exp(-r * T) * normalCDF(-d2)
                - q * S * exp(-q * T) * normalCDF(-d1);
    }

    return theta;
}

double BlackScholesPricingEngine::calculateVega(const Option& option) const {
    const double S = option.getSpot();
    const double K = option.getStrike();
    const double T = option.getExpiry();
    const double r = option.getRate();
    const double sigma = option.getVolatility();
    const double q = option.getDividend();

    const double d1 = calculateD1(S, K, r, q, sigma, T);

    // Vega is the same for both calls and puts
    return S * exp(-q * T) * sqrt(T) * normalPDF(d1);
}

double BlackScholesPricingEngine::calculateRho(const Option& option) const {
    const double S = option.getSpot();
    const double K = option.getStrike();
    const double T = option.getExpiry();
    const double r = option.getRate();
    const double sigma = option.getVolatility();
    const double q = option.getDividend();

    const double d1 = calculateD1(S, K, r, q, sigma, T);
    const double d2 = calculateD2(d1, sigma, T);

    if (option.getType() == OptionType::Call) {
        return K * T * exp(-r * T) * normalCDF(d2);
    } else {
        return -K * T * exp(-r * T) * normalCDF(-d2);
    }
}

double BlackScholesPricingEngine::calculateD1(double S, double K, double r,
                                            double q, double sigma, double T) {
    if (T <= 0.0) {
        throw std::invalid_argument("Time to expiry must be positive");
    }
    if (sigma <= 0.0) {
        throw std::invalid_argument("Volatility must be positive");
    }

    return (log(S/K) + (r - q + sigma * sigma / 2.0) * T) / (sigma * sqrt(T));
}

double BlackScholesPricingEngine::calculateD2(double d1, double sigma, double T) {
    return d1 - sigma * sqrt(T);
}

double BlackScholesPricingEngine::normalCDF(double x) {
    // Using the approximation from M. Abramowitz and I.A. Stegun
    // Handbook of Mathematical Functions, Dover Publications
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

    const double t = 1.0 / (1.0 + p * x);
    const double y = 1.0 - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * exp(-x * x / 2.0);

    return 0.5 * (1.0 + sign * y);
}

double BlackScholesPricingEngine::normalPDF(double x) {
    return exp(-x * x / 2.0) / sqrt(2.0 * M_PI);
}

} // namespace pricer