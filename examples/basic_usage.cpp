//
// Created by Yusufu Shehu on 18/01/2025.
//
#include "pricer/option.h"
#include "pricer/black_scholes.h"
#include "pricer/monte_carlo.h"
#include "pricer/binomial.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>

// Helper function to print option results
void printResults(const std::string& method, const pricer::Option& option) {
    std::cout << "\n" << method << " Results:\n";
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Price: " << option.price() << "\n";
    std::cout << "Delta: " << option.delta() << "\n";
    std::cout << "Gamma: " << option.gamma() << "\n";
    std::cout << "Theta: " << option.theta() << "\n";
    std::cout << "Vega:  " << option.vega() << "\n";
    std::cout << "Rho:   " << option.rho() << "\n";
    std::cout << "----------------------------\n";
}

int main() {
    try {
        // Create a European call option
        pricer::EuropeanOption euro_call(
            pricer::OptionType::Call,
            100.0,      // Strike
            1.0,        // Time to expiry (1 year)
            100.0,      // Spot price
            0.05,       // Risk-free rate (5%)
            0.2,        // Volatility (20%)
            0.02        // Dividend yield (2%)
        );

        // Create an American put option
        pricer::AmericanOption amer_put(
            pricer::OptionType::Put,
            100.0,      // Strike
            1.0,        // Time to expiry
            100.0,      // Spot price
            0.05,       // Risk-free rate
            0.2,        // Volatility
            0.02        // Dividend yield
        );

        // Create pricing engines
        const auto bs_engine = pricer::makeBlackScholesPricingEngine();
        const auto mc_engine = pricer::makeMonteCarloEngine(100000, 252, true);  // 100k paths, 252 steps
        const auto bin_engine = pricer::makeBinomialTreeEngine(1000);  // 1000 steps

        // Price European call with all methods
        std::cout << "\nEuropean Call Option:\n";
        std::cout << "==================\n";

        // Black-Scholes pricing
        euro_call.setPricingEngine(bs_engine);
        printResults("Black-Scholes", euro_call);

        // Monte Carlo pricing
        euro_call.setPricingEngine(mc_engine);
        printResults("Monte Carlo", euro_call);

        // Binomial Tree pricing
        euro_call.setPricingEngine(bin_engine);
        printResults("Binomial Tree", euro_call);

        // Price American put with supported methods
        std::cout << "\nAmerican Put Option:\n";
        std::cout << "=================\n";

        // Monte Carlo pricing
        amer_put.setPricingEngine(mc_engine);
        printResults("Monte Carlo", amer_put);

        // Binomial Tree pricing (more suitable for American options)
        amer_put.setPricingEngine(bin_engine);
        printResults("Binomial Tree", amer_put);

        // Demonstrate parameter sensitivity
        std::cout << "\nParameter Sensitivity Analysis:\n";
        std::cout << "============================\n";

        euro_call.setPricingEngine(bs_engine);  // Use Black-Scholes for speed

        // Vary spot price
        std::vector<double> spots = {90.0, 95.0, 100.0, 105.0, 110.0};
        std::cout << "\nSpot Price Sensitivity:\n";
        for (double spot : spots) {
            euro_call.setSpot(spot);
            std::cout << "Spot = " << spot << ", Price = " << euro_call.price()
                     << ", Delta = " << euro_call.delta() << "\n";
        }

        // Reset spot price
        euro_call.setSpot(100.0);

        // Vary volatility
        const std::vector<double> vols = {0.1, 0.2, 0.3, 0.4, 0.5};
        std::cout << "\nVolatility Sensitivity:\n";
        for (double vol : vols) {
            euro_call.setVolatility(vol);
            std::cout << "Vol = " << vol << ", Price = " << euro_call.price()
                     << ", Vega = " << euro_call.vega() << "\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}