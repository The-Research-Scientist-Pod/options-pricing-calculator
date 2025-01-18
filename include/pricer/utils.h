//
// Created by Yusufu Shehu on 18/01/2025.
//

#ifndef OPTIONS_PRICER_UTILS_H
#define OPTIONS_PRICER_UTILS_H

#include "option.h"
#include <random>
#include <vector>

namespace pricer {
namespace utils {

/**
 * @brief Normal cumulative distribution function (N(x))
 * @param x Input value
 * @return The probability that a standard normal variable is less than x
 */
double normalCDF(double x);

/**
 * @brief Normal probability density function
 * @param x Input value
 * @return The value of the normal density at x
 */
double normalPDF(double x);

/**
 * @brief Inverse normal cumulative distribution function (N^(-1)(p))
 * @param p Probability value between 0 and 1
 * @return The value x such that N(x) = p
 */
double inverseNormalCDF(double p);

/**
 * @brief Generate standard normal random variables
 * @param n Number of variables to generate
 * @param rng Random number generator
 * @return Vector of n standard normal random variables
 */
std::vector<double> generateNormalVariates(size_t n, std::mt19937& rng);

/**
 * @brief Generate antithetic normal random variables
 * @param n Number of pairs to generate (result will have 2n variables)
 * @param rng Random number generator
 * @return Vector of 2n antithetic normal random variables
 */
std::vector<double> generateAntitheticNormalVariates(size_t n, std::mt19937& rng);

/**
 * @brief Calculate discount factor
 * @param rate Interest rate (as decimal)
 * @param time Time in years
 * @return Discount factor e^(-r*t)
 */
double discountFactor(double rate, double time);

/**
 * @brief Calculate forward price
 * @param spot Spot price
 * @param rate Risk-free rate
 * @param dividend Dividend yield
 * @param time Time to expiry
 * @return Forward price
 */
double forwardPrice(double spot, double rate, double dividend, double time);

/**
 * @brief Calculate implied volatility using Newton-Raphson method
 * @param marketPrice Observed market price
 * @param spot Spot price
 * @param strike Strike price
 * @param rate Risk-free rate
 * @param time Time to expiry
 * @param dividend Dividend yield
 * @param isCall Whether the option is a call (true) or put (false)
 * @return Implied volatility
 */
double impliedVolatility(double marketPrice, double spot, double strike,
                        double rate, double time, double dividend, bool isCall);

/**
 * @brief Calculate historical volatility from price series
 * @param prices Vector of historical prices
 * @param timeStep Time between observations in years
 * @return Annualized historical volatility
 */
double historicalVolatility(const std::vector<double>& prices, double timeStep);

/**
 * @brief Calculate the present value of cash flows
 * @param cashFlows Vector of future cash flows
 * @param times Vector of times (in years) when cash flows occur
 * @param rate Discount rate
 * @return Present value
 */
double presentValue(const std::vector<double>& cashFlows,
                   const std::vector<double>& times,
                   double rate);

/**
 * @brief Check if two doubles are approximately equal
 * @param a First value
 * @param b Second value
 * @param epsilon Maximum allowed difference
 * @return true if values are approximately equal
 */
bool isApproxEqual(double a, double b, double epsilon = 1e-10);

 /**
  * @brief Calculate delta using finite differences
  * @param option The option to calculate delta for
  * @param h Step size for finite difference
  * @return Calculated delta
  */
 double finiteDifferenceDelta(const Option& option, double h = 0.01);

 /**
  * @brief Calculate gamma using finite differences
  * @param option The option to calculate gamma for
  * @param h Step size for finite difference
  * @return Calculated gamma
  */
 double finiteDifferenceGamma(const Option& option, double h = 0.01);

 /**
  * @brief Calculate theta using finite differences
  * @param option The option to calculate theta for
  * @param h Step size for finite difference (default 1 day)
  * @return Calculated theta
  */
 double finiteDifferenceTheta(const Option& option, double h = 1.0/365.0);

 /**
  * @brief Calculate vega using finite differences
  * @param option The option to calculate vega for
  * @param h Step size for finite difference
  * @return Calculated vega
  */
 double finiteDifferenceVega(const Option& option, double h = 0.0001);

 /**
  * @brief Calculate rho using finite differences
  * @param option The option to calculate rho for
  * @param h Step size for finite difference
  * @return Calculated rho
  */
 double finiteDifferenceRho(const Option& option, double h = 0.0001);

} // namespace utils
} // namespace pricer

#endif // OPTIONS_PRICER_UTILS_H