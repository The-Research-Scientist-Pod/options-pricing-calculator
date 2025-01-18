//
// Created by Yusufu Shehu on 18/01/2025.
//

#ifndef OPTIONS_PRICER_MONTE_CARLO_H
#define OPTIONS_PRICER_MONTE_CARLO_H

#include "engine.h"
#include <random>
#include <vector>

namespace pricer {

/**
 * @brief Monte Carlo simulation engine for option pricing
 */
class MonteCarloEngine final : public PricingEngine {
public:
    /**
     * @brief Constructor with simulation parameters
     * @param num_paths Number of simulation paths
     * @param num_steps Number of time steps per path
     * @param use_antithetic Whether to use antithetic variates
     * @param num_threads Number of threads (0 for auto)
     */
    explicit MonteCarloEngine(size_t num_paths = 100000,
                            size_t num_steps = 252,
                            bool use_antithetic = true,
                            size_t num_threads = 0);

    double calculate(const Option& option) const override;
    double calculateDelta(const Option& option) const override;
    double calculateGamma(const Option& option) const override;
    double calculateTheta(const Option& option) const override;
    double calculateVega(const Option& option) const override;
    double calculateRho(const Option& option) const override;

    // Getters
    size_t getNumPaths() const { return num_paths_; }
    size_t getNumSteps() const { return num_steps_; }
    bool getUseAntithetic() const { return use_antithetic_; }
    size_t getNumThreads() const { return num_threads_; }

    // Setters
    void setNumPaths(size_t paths) { num_paths_ = paths; }
    void setNumSteps(size_t steps) { num_steps_ = steps; }
    void setUseAntithetic(bool use) { use_antithetic_ = use; }
    void setNumThreads(size_t threads) { num_threads_ = threads; }

    std::pair<double, double> getConfidenceInterval() const;

private:
    size_t num_paths_;
    size_t num_steps_;
    bool use_antithetic_;
    size_t num_threads_;

    mutable double last_mean_;
    mutable double last_stderr_;

    std::vector<double> generatePath(const Option& option,
                                   std::mt19937& rng,
                                   bool antithetic = false) const;

    static double calculatePayoff(const Option& option, double final_price);

    std::pair<double, double> simulateBatch(const Option& option,
                                          unsigned int seed,
                                          size_t num_paths) const;
};

// Factory function
inline std::shared_ptr<PricingEngine> makeMonteCarloEngine(
    size_t num_paths = 100000,
    size_t num_steps = 252,
    bool use_antithetic = true,
    size_t num_threads = 0) {
    return std::make_shared<MonteCarloEngine>(
        num_paths, num_steps, use_antithetic, num_threads);
}

} // namespace pricer

#endif // OPTIONS_PRICER_MONTE_CARLO_H