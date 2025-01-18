//
// Created by Yusufu Shehu on 18/01/2025.
//

#ifndef OPTIONS_PRICER_ENGINE_H
#define OPTIONS_PRICER_ENGINE_H

#include "option.h"

namespace pricer {

    /**
     * @brief Abstract base class for all pricing engines
     *
     * This class defines the interface that all option pricing engines
     * must implement, including price calculation and Greeks.
     */
    class PricingEngine {
    public:
        virtual ~PricingEngine() = default;

        /**
         * @brief Calculate the option price
         * @param option The option to price
         * @return The calculated price
         */
        [[nodiscard]] virtual double calculate(const Option& option) const = 0;

        /**
         * @brief Calculate option's delta
         * @param option The option to calculate delta for
         * @return The calculated delta
         */
        [[nodiscard]] virtual double calculateDelta(const Option& option) const = 0;

        /**
         * @brief Calculate option's gamma
         * @param option The option to calculate gamma for
         * @return The calculated gamma
         */
        [[nodiscard]] virtual double calculateGamma(const Option& option) const = 0;

        /**
         * @brief Calculate option's theta
         * @param option The option to calculate theta for
         * @return The calculated theta
         */
        [[nodiscard]] virtual double calculateTheta(const Option& option) const = 0;

        /**
         * @brief Calculate option's vega
         * @param option The option to calculate vega for
         * @return The calculated vega
         */
        [[nodiscard]] virtual double calculateVega(const Option& option) const = 0;

        /**
         * @brief Calculate option's rho
         * @param option The option to calculate rho for
         * @return The calculated rho
         */
        [[nodiscard]] virtual double calculateRho(const Option& option) const = 0;
    };

} // namespace pricer

#endif // OPTIONS_PRICER_ENGINE_H
