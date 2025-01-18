//
// Created by Yusufu Shehu on 18/01/2025.
//

#ifndef OPTION_H
#define OPTION_H


#include <string>
#include <memory>
#include <stdexcept>

namespace pricer {

// Forward declarations
class PricingEngine;

/**
 * @brief Enumeration for option types
 */
enum class OptionType {
    Call,
    Put
};

/**
 * @brief Base class for all options
 *
 * This class provides the interface for all option types and stores
 * common parameters used in option pricing.
 */
class Option {
public:
    /**
     * @brief Constructor for Option
     * @param type The type of option (Call or Put)
     * @param strike The strike price of the option
     * @param expiry Time to expiration in years
     * @param spot Current price of the underlying asset
     * @param rate Risk-free interest rate (as a decimal, e.g., 0.05 for 5%)
     * @param volatility Volatility of the underlying asset (as a decimal)
     * @param dividend Dividend yield (as a decimal)
     */
    Option(OptionType type,
           double strike,
           double expiry,
           double spot,
           double rate,
           double volatility,
           double dividend = 0.0);

    // Virtual destructor for proper inheritance
    virtual ~Option() = default;

    // Prevent copying
    Option(const Option&) = delete;
    Option& operator=(const Option&) = delete;

    // Allow moving
    Option(Option&&) noexcept = default;
    Option& operator=(Option&&) noexcept = default;

    /**
     * @brief Calculate the price of the option
     * @return The calculated price
     * @throws std::runtime_error if no pricing engine is set
     */
    virtual double price() const;

    /**
     * @brief Calculate the option's delta
     * @return The calculated delta
     */
    virtual double delta() const;

    /**
     * @brief Calculate the option's gamma
     * @return The calculated gamma
     */
    virtual double gamma() const;

    /**
     * @brief Calculate the option's theta
     * @return The calculated theta
     */
    virtual double theta() const;

    /**
     * @brief Calculate the option's vega
     * @return The calculated vega
     */
    virtual double vega() const;

    /**
     * @brief Calculate the option's rho
     * @return The calculated rho
     */
    virtual double rho() const;

    /**
     * @brief Set the pricing engine for this option
     * @param engine Shared pointer to the pricing engine
     */
    void setPricingEngine(std::shared_ptr<PricingEngine> engine);

    // Getters
    OptionType getType() const { return type_; }
    double getStrike() const { return strike_; }
    double getExpiry() const { return expiry_; }
    double getSpot() const { return spot_; }
    double getRate() const { return rate_; }
    double getVolatility() const { return volatility_; }
    double getDividend() const { return dividend_; }

    // Setters with validation
    void setSpot(double spot);
    void setRate(double rate);
    void setVolatility(double volatility);
    void setDividend(double dividend);

protected:
    // Option parameters
    OptionType type_;
    double strike_;
    double expiry_;
    double spot_;
    double rate_;
    double volatility_;
    double dividend_;

    // Pricing engine
    std::shared_ptr<PricingEngine> engine_;

    /**
     * @brief Validate option parameters
     * @throws std::invalid_argument if parameters are invalid
     */
    void validateParameters() const;
};

/**
 * @brief European option class
 */
class EuropeanOption : public Option {
public:
    using Option::Option;  // Inherit constructors
};

/**
 * @brief American option class
 */
class AmericanOption : public Option {
public:
    using Option::Option;  // Inherit constructors

    /**
     * @brief Override price calculation for American options
     * @return The calculated price
     */
    double price() const override;
};

} // namespace pricer

#endif //OPTION_H
