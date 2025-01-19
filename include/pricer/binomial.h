#ifndef OPTIONS_PRICER_BINOMIAL_H
#define OPTIONS_PRICER_BINOMIAL_H

#include "engine.h"
#include <vector>
#include <tuple>

namespace pricer {

/**
 * @brief Cox-Ross-Rubinstein binomial tree implementation
 */
class BinomialTreeEngine : public PricingEngine {
public:
    /**
     * @brief Constructor with tree parameters
     * @param num_steps Number of time steps in the tree
     * @param use_bbs Whether to use BBS extrapolation
     */
    explicit BinomialTreeEngine(size_t num_steps = 100, bool use_bbs = true);

    [[nodiscard]] double calculate(const Option& option) const override;
    [[nodiscard]] double calculateDelta(const Option& option) const override;
    [[nodiscard]] double calculateGamma(const Option& option) const override;
    [[nodiscard]] double calculateTheta(const Option& option) const override;
    [[nodiscard]] double calculateVega(const Option& option) const override;
    [[nodiscard]] double calculateRho(const Option& option) const override;

    // Getters
    [[nodiscard]] size_t getNumSteps() const { return num_steps_; }
    [[nodiscard]] bool getUseBBS() const { return use_bbs_; }

    // Setters
    void setNumSteps(const size_t steps) { num_steps_ = steps; }
    void setUseBBS(const bool use_bbs) { use_bbs_ = use_bbs; }

private:
    size_t num_steps_;
    bool use_bbs_;

    /**
     * @brief Calculate option price with specified number of steps
     * @param option Option being priced
     * @param steps Number of steps to use
     * @return Option price
     */
    [[nodiscard]] double calculateWithParameters(const Option& option, size_t steps) const;

    /**
     * @brief Build price tree for underlying asset
     * @param option Option parameters
     * @return Vector containing price tree nodes
     */
    [[nodiscard]] std::vector<double> buildPriceTree(const Option& option) const;

    /**
     * @brief Calculate option values at each node
     * @param option Option parameters
     * @param price_tree Underlying price tree
     * @param american Whether to check for early exercise
     * @return Vector containing option values at each node
     */
    [[nodiscard]] std::vector<double> calculateOptionValues(
        const Option& option,
        const std::vector<double>& price_tree,
        bool american) const;

    /**
     * @brief Calculate tree parameters (up, down, probability)
     * @param option Option parameters
     * @param dt Time step size
     * @return Tuple of (up_factor, down_factor, probability)
     */
    static std::tuple<double, double, double> calculateParameters(
        const Option& option,
        double dt) ;

    /**
     * @brief Apply Black-Scholes-Richardson extrapolation
     * @param option Option parameters
     * @param value Initial binomial value
     * @return Extrapolated option value
     */
    [[nodiscard]] double applyBBSExtrapolation(
        const Option& option,
        double value) const;

    /**
     * @brief Calculate payoff at given spot price
     * @param option Option parameters
     * @param spot_price Current spot price
     * @return Option payoff
     */
    static double calculatePayoff(
        const Option& option,
        double spot_price);

    /**
     * @brief Convert step and node numbers to array index
     * @param step Time step number
     * @param node Node number at that step
     * @return Index in flattened array
     */
    [[nodiscard]] static size_t getIndex(size_t step, size_t node);
};

// Factory function
inline std::shared_ptr<PricingEngine> makeBinomialTreeEngine(
    size_t num_steps = 1000,
    bool use_bbs = true) {
    return std::make_shared<BinomialTreeEngine>(num_steps, use_bbs);
}

} // namespace pricer

#endif // OPTIONS_PRICER_BINOMIAL_H