//
// Created by Yusufu Shehu on 18/01/2025.
//

#ifndef BLACK_SCHOLES_H
#define BLACK_SCHOLES_H
#include "option.h"
#include <memory>

namespace pricer {

/**
 * @brief Abstract base class for all pricing engines
 */
class PricingEngine {
public:
    virtual ~PricingEngine() = default;

    /**
     * @brief Calculate the option price
     * @param option The option to price
     * @return The calculated price
     */
    virtual double calculate(const Option& option) const = 0;

    /**
     * @brief Calculate option's delta
     * @param option The option to calculate delta for
     * @return The calculated delta
     */
    virtual double calculateDelta(const Option& option) const = 0;

    /**
     * @brief Calculate option's gamma
     * @param option The option to calculate gamma for
     * @return The calculated gamma
     */
    virtual double calculateGamma(const Option& option) const = 0;

    /**
     * @brief Calculate option's theta
     * @param option The option to calculate theta for
     * @return The calculated theta
     */
    virtual double calculateTheta(const Option& option) const = 0;

    /**
     * @brief Calculate option's vega
     * @param option The option to calculate vega for
     * @return The calculated vega
     */
    virtual double calculateVega(const Option& option) const = 0;

    /**
     * @brief Calculate option's rho
     * @param option The option to calculate rho for
     * @return The calculated rho
     */
    virtual double calculateRho(const Option& option) const = 0;
};

/**
 * @brief Black-Scholes pricing engine implementation
 *
 * This class implements the Black-Scholes-Merton model for European option pricing.
 * It provides analytical solutions for option prices and Greeks.
 */
class BlackScholesPricingEngine : public PricingEngine {
public:
    double calculate(const Option& option) const override;
    double calculateDelta(const Option& option) const override;
    double calculateGamma(const Option& option) const override;
    double calculateTheta(const Option& option) const override;
    double calculateVega(const Option& option) const override;
    double calculateRho(const Option& option) const override;

private:
    /**
     * @brief Calculate d1 parameter from Black-Scholes formula
     * @param S Spot price
     * @param K Strike price
     * @param r Risk-free rate
     * @param q Dividend yield
     * @param sigma Volatility
     * @param T Time to expiry
     * @return The d1 parameter
     */
    static double calculateD1(double S, double K, double r, double q,
                            double sigma, double T);

    /**
     * @brief Calculate d2 parameter from Black-Scholes formula
     * @param d1 The d1 parameter
     * @param sigma Volatility
     * @param T Time to expiry
     * @return The d2 parameter
     */
    static double calculateD2(double d1, double sigma, double T);

    /**
     * @brief Calculate the cumulative normal distribution function
     * @param x Input value
     * @return N(x)
     */
    static double normalCDF(double x);

    /**
     * @brief Calculate the normal probability density function
     * @param x Input value
     * @return The probability density at x
     */
    static double normalPDF(double x);
};

// Factory function to create a Black-Scholes pricing engine
inline std::shared_ptr<PricingEngine> makeBlackScholesPricingEngine() {
    return std::make_shared<BlackScholesPricingEngine>();
}

} // namespace pricer
#endif //BLACK_SCHOLES_H
