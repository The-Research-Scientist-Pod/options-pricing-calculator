//
// Created by Yusufu Shehu on 18/01/2025.
//

#ifndef OPTIONS_PRICER_MAIN_WINDOW_H
#define OPTIONS_PRICER_MAIN_WINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QTableWidget>
#include <QGroupBox>
#include <memory>

namespace pricer {
class Option;
class PricingEngine;
}

class MainWindow : public QMainWindow {
    Q_OBJECT  // Must be first declaration in class

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void calculateOption();
    void updateEngineControls();
    void resetFields();
    void exportResults();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    // Input widgets
    QComboBox* optionTypeCombo_;
    QComboBox* optionStyleCombo_;
    QComboBox* pricingMethodCombo_;
    QDoubleSpinBox* spotPriceSpin_;
    QDoubleSpinBox* strikePriceSpin_;
    QDoubleSpinBox* timeToExpirySpin_;
    QDoubleSpinBox* riskFreeRateSpin_;
    QDoubleSpinBox* volatilitySpin_;
    QDoubleSpinBox* dividendYieldSpin_;

    // Monte Carlo specific controls
    QGroupBox* monteCarloGroup_;
    QDoubleSpinBox* numPathsSpin_;
    QDoubleSpinBox* numStepsSpin_;
    QComboBox* varianceReductionCombo_;

    // Binomial tree specific controls
    QGroupBox* binomialGroup_;
    QDoubleSpinBox* numTreeStepsSpin_;
    QComboBox* treeMethodCombo_;

    // Results display
    QTableWidget* resultsTable_;

    // Create layout functions
    void createInputLayout();
    void createEngineSpecificControls();
    void createResultsLayout();
    void createMenuBar();

    // Helper functions
    [[nodiscard]] std::unique_ptr<pricer::Option> createOption() const;
    [[nodiscard]] std::shared_ptr<pricer::PricingEngine> createPricingEngine() const;
    void displayResults(const pricer::Option& option);
    void showErrorMessage(const QString& message);

    // Constants for input validation
    static constexpr double MAX_PRICE = 1000000.0;
    static constexpr double MAX_RATE = 1.0;  // 100%
    static constexpr double MAX_TIME = 30.0;  // 30 years
    static constexpr int PRICE_DECIMALS = 2;
    static constexpr int RATE_DECIMALS = 4;
};

#endif // OPTIONS_PRICER_MAIN_WINDOW_H