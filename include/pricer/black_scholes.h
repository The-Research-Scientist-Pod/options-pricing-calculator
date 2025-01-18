//
// Created by Yusufu Shehu on 18/01/2025.
//

#ifndef OPTIONS_PRICER_BLACK_SCHOLES_H
#define OPTIONS_PRICER_BLACK_SCHOLES_H

#include "engine.h"
#include <memory>

namespace pricer {

 /**
  * @brief Black-Scholes pricing engine implementation
  */
 class BlackScholesPricingEngine : public PricingEngine {
 public:
  [[nodiscard]] double calculate(const Option& option) const override;
  [[nodiscard]] double calculateDelta(const Option& option) const override;
  [[nodiscard]] double calculateGamma(const Option& option) const override;
  [[nodiscard]] double calculateTheta(const Option& option) const override;
  [[nodiscard]] double calculateVega(const Option& option) const override;
  [[nodiscard]] double calculateRho(const Option& option) const override;

 private:
  static double calculateD1(double S, double K, double r, double q,
                          double sigma, double T);
  static double calculateD2(double d1, double sigma, double T);
  static double normalCDF(double x);
  static double normalPDF(double x);
 };

 // Factory function
 inline std::shared_ptr<PricingEngine> makeBlackScholesPricingEngine() {
  return std::make_shared<BlackScholesPricingEngine>();
 }

} // namespace pricer

#endif // OPTIONS_PRICER_BLACK_SCHOLES_H