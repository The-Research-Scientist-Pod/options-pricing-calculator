//
// Created by Yusufu Shehu on 18/01/2025.
//
#include "pricer/black_scholes.h"
#include <cmath>
#include <iomanip>
#include <stdexcept>
#include <iostream>

namespace pricer {
    namespace {
        // Helper function for debug output
        void debugPrint(const std::string& label, double value) {
            std::cout << std::setw(30) << std::left << label << " = "
                      << std::fixed << std::setprecision(6) << value << std::endl;
        }

    }

    double BlackScholesPricingEngine::calculate(const Option& option) const {
        // Extract parameters from the option
        const double S = option.getSpot();
        const double K = option.getStrike();
        const double T = option.getExpiry();
        const double r = option.getRate();
        const double sigma = option.getVolatility();
        const double q = option.getDividend();

        //std::cout << "\nBlack-Scholes Calculation:" << std::endl;
        //std::cout << "=========================" << std::endl;
        //debugPrint("Spot (S)", S);
        //debugPrint("Strike (K)", K);
        //debugPrint("Time (T)", T);
        //debugPrint("Rate (r)", r);
        //debugPrint("Volatility (σ)", sigma);
        //debugPrint("Dividend (q)", q);

        // Calculate d1 and d2
        const double d1 = calculateD1(S, K, r, q, sigma, T);
        const double d2 = calculateD2(d1, sigma, T);

        //debugPrint("d1", d1);
        //debugPrint("d2", d2);
        //debugPrint("N(d1)", normalCDF(d1));
        //debugPrint("N(d2)", normalCDF(d2));
        //debugPrint("n(d1)", normalPDF(d1));
        //debugPrint("n(d2)", normalPDF(d2));

        double price;
        if (option.getType() == OptionType::Call) {
            const double term1 = S * std::exp(-q * T) * normalCDF(d1);
            const double term2 = K * std::exp(-r * T) * normalCDF(d2);
            //debugPrint("Call Option Term 1", term1);
            //debugPrint("Call Option Term 2", term2);
            price = term1 - term2;
        } else {  // Put option
            const double term1 = K * std::exp(-r * T) * normalCDF(-d2);
            const double term2 = S * std::exp(-q * T) * normalCDF(-d1);
            //debugPrint("Put Option Term 1", term1);
            //debugPrint("Put Option Term 2", term2);
            price = term1 - term2;
        }

        //debugPrint("Final Price", price);
        //std::cout << "=========================\n" << std::endl;
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
        const double exp_term = std::exp(-q * T);

        return exp_term * (option.getType() == OptionType::Call ? normalCDF(d1) : normalCDF(d1) - 1.0);
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
    const double S = option.getSpot();
    const double K = option.getStrike();
    const double T = option.getExpiry();
    const double r = option.getRate();
    const double sigma = option.getVolatility();
    const double q = option.getDividend();

    const double d1 = calculateD1(S, K, r, q, sigma, T);
    const double d2 = calculateD2(d1, sigma, T);

    // Common term for both call and put
    const double common_term = -(S * exp(-q * T) * normalPDF(d1) * sigma) / (2 * sqrt(T));

    if (option.getType() == OptionType::Call) {
        return (common_term
                - r * K * exp(-r * T) * normalCDF(d2)
                + q * S * exp(-q * T) * normalCDF(d1)) / 365.0;  // Convert to daily theta
    } else {
        return (common_term
                + r * K * exp(-r * T) * normalCDF(-d2)
                - q * S * exp(-q * T) * normalCDF(-d1)) / 365.0;  // Convert to daily theta
    }
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
        const double vega = (S * exp(-q * T) * normalPDF(d1) * sqrt(T)) / 100.0;

        // Debug print statement (optional, remove if not needed)
        //std::cout << "Debug: calculateVega - Vega: " << vega << std::endl;

        return vega;
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

        // Calculate the discounted future value of the strike price
        const double fv_strike = K * exp(-r * T);
        const double delta_t = T;

        double rho = 0.0;
        if (option.getType() == OptionType::Call) {
            rho = (fv_strike * delta_t * normalCDF(d2)) / 100.0;
        } else {
            rho = (-1.0 * fv_strike * delta_t * normalCDF(-d2)) / 100.0;
        }

        // Debug print statement
        //std::cout << "Debug: calculateRho - Option Type: "
        //          << (option.getType() == OptionType::Call ? "Call" : "Put")
        //          << ", Rho: " << rho << std::endl;

        return rho;
    }


    double BlackScholesPricingEngine::calculateD1(const double S, const double K, const double r,
                                                  const double q, const double sigma, const double T) {
        if (T <= 0.0) {
            throw std::invalid_argument("Time to expiry must be positive");
        }
        if (sigma <= 0.0) {
            throw std::invalid_argument("Volatility must be positive");
        }

        return (std::log(S/K) + (r - q + sigma * sigma / 2.0) * T) / (sigma * std::sqrt(T));

    }

double BlackScholesPricingEngine::calculateD2(const double d1, const double sigma, const double T) {
    return d1 - sigma * sqrt(T);
}

    double BlackScholesPricingEngine::normalCDF(const double x) {
        return std::erfc(-x/std::sqrt(2.0))/2.0;
    }

    double BlackScholesPricingEngine::normalPDF(const double x) {
        static constexpr double sqrt2pi = 2.506628274631; // Pre-computed sqrt(2.0 * M_PI)
        return std::exp(-x * x / 2.0) / sqrt2pi;
    }

} // namespace pricer