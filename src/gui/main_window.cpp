#include "main_window.h"
#include "pricer/black_scholes.h"
#include "pricer/monte_carlo.h"
#include "pricer/binomial.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QHeaderView>
#include <QTextStream>
#include <QFile>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , optionTypeCombo_(new QComboBox(this))
    , optionStyleCombo_(new QComboBox(this))
    , pricingMethodCombo_(new QComboBox(this))
    , spotPriceSpin_(new QDoubleSpinBox(this))
    , strikePriceSpin_(new QDoubleSpinBox(this))
    , timeToExpirySpin_(new QDoubleSpinBox(this))
    , riskFreeRateSpin_(new QDoubleSpinBox(this))
    , volatilitySpin_(new QDoubleSpinBox(this))
    , dividendYieldSpin_(new QDoubleSpinBox(this))
    , monteCarloGroup_(new QGroupBox("Monte Carlo Settings", this))
    , numPathsSpin_(new QDoubleSpinBox(this))
    , numStepsSpin_(new QDoubleSpinBox(this))
    , varianceReductionCombo_(new QComboBox(this))
    , binomialGroup_(new QGroupBox("Binomial Tree Settings", this))
    , numTreeStepsSpin_(new QDoubleSpinBox(this))
    , treeMethodCombo_(new QComboBox(this))
    , resultsTable_(new QTableWidget(this))
{
    setWindowTitle("Options Pricer");

    // Create central widget and main layout
    auto* centralWidget = new QWidget(this);
    auto* mainLayout = new QHBoxLayout(centralWidget);

    // Create left panel for inputs and right panel for results
    auto* leftPanel = new QWidget(centralWidget);
    auto* rightPanel = new QWidget(centralWidget);
    auto* leftLayout = new QVBoxLayout(leftPanel);
    auto* rightLayout = new QVBoxLayout(rightPanel);

    // Setup input widgets
    createInputLayout();
    createEngineSpecificControls();

    // Create input group
    auto* inputGroup = new QGroupBox("Option Parameters", centralWidget);
    auto* inputForm = new QFormLayout(inputGroup);

    // Add inputs to form
    inputForm->addRow("Option Type:", optionTypeCombo_);
    inputForm->addRow("Option Style:", optionStyleCombo_);
    inputForm->addRow("Pricing Method:", pricingMethodCombo_);
    inputForm->addRow("Spot Price:", spotPriceSpin_);
    inputForm->addRow("Strike Price:", strikePriceSpin_);
    inputForm->addRow("Time to Expiry (years):", timeToExpirySpin_);
    inputForm->addRow("Risk-free Rate:", riskFreeRateSpin_);
    inputForm->addRow("Volatility:", volatilitySpin_);
    inputForm->addRow("Dividend Yield:", dividendYieldSpin_);

    // Create calculate button
    auto* calculateButton = new QPushButton("Calculate", centralWidget);
    connect(calculateButton, &QPushButton::clicked, this, &MainWindow::calculateOption);

    // Create labels for sections
    auto* resultsLabel = new QLabel("Results", rightPanel);
    QFont labelFont = resultsLabel->font();
    labelFont.setBold(true);
    labelFont.setPointSize(labelFont.pointSize() + 2);
    resultsLabel->setFont(labelFont);

    // Setup results table
    createResultsLayout();

    // Add widgets to left panel
    leftLayout->addWidget(inputGroup);
    leftLayout->addWidget(monteCarloGroup_);
    leftLayout->addWidget(binomialGroup_);
    leftLayout->addWidget(calculateButton);
    leftLayout->addStretch();

    // Add widgets to right panel
    rightLayout->addWidget(resultsLabel);
    rightLayout->addWidget(resultsTable_);

    // Add panels to main layout
    mainLayout->addWidget(leftPanel, 1);
    mainLayout->addWidget(rightPanel, 2);

    setCentralWidget(centralWidget);
    createMenuBar();

    // Set initial state
    updateEngineControls();

    // Restore window geometry
    QSettings settings;
    restoreGeometry(settings.value("mainWindowGeometry").toByteArray());

    // Set default size if no previous geometry exists
    if (size().isEmpty()) {
        resize(1200, 800);
    }
}

MainWindow::~MainWindow() = default;

void MainWindow::createInputLayout() {
    // Option type combo
    optionTypeCombo_->addItem("Call", QVariant::fromValue(pricer::OptionType::Call));
    optionTypeCombo_->addItem("Put", QVariant::fromValue(pricer::OptionType::Put));

    // Option style combo
    optionStyleCombo_->addItem("European");
    optionStyleCombo_->addItem("American");

    // Pricing method combo
    pricingMethodCombo_->addItem("Black-Scholes");
    pricingMethodCombo_->addItem("Monte Carlo");
    pricingMethodCombo_->addItem("Binomial Tree");
    connect(pricingMethodCombo_, &QComboBox::currentTextChanged,
            this, &MainWindow::updateEngineControls);

    // Numeric inputs
    spotPriceSpin_->setRange(0.01, MAX_PRICE);
    spotPriceSpin_->setDecimals(PRICE_DECIMALS);
    spotPriceSpin_->setValue(100.0);

    strikePriceSpin_->setRange(0.01, MAX_PRICE);
    strikePriceSpin_->setDecimals(PRICE_DECIMALS);
    strikePriceSpin_->setValue(100.0);

    timeToExpirySpin_->setRange(0.01, MAX_TIME);
    timeToExpirySpin_->setDecimals(RATE_DECIMALS);
    timeToExpirySpin_->setValue(1.0);

    riskFreeRateSpin_->setRange(0.0, MAX_RATE);
    riskFreeRateSpin_->setDecimals(RATE_DECIMALS);
    riskFreeRateSpin_->setValue(0.05);

    volatilitySpin_->setRange(0.01, MAX_RATE);
    volatilitySpin_->setDecimals(RATE_DECIMALS);
    volatilitySpin_->setValue(0.2);

    dividendYieldSpin_->setRange(0.0, MAX_RATE);
    dividendYieldSpin_->setDecimals(RATE_DECIMALS);
    dividendYieldSpin_->setValue(0.0);
}

void MainWindow::createEngineSpecificControls() {
    // Monte Carlo controls
    auto* mcLayout = new QFormLayout(monteCarloGroup_);

    numPathsSpin_->setRange(1000, 1000000);
    numPathsSpin_->setValue(100000);
    numPathsSpin_->setDecimals(0);

    numStepsSpin_->setRange(10, 1000);
    numStepsSpin_->setValue(252);
    numStepsSpin_->setDecimals(0);

    varianceReductionCombo_->addItem("None");
    varianceReductionCombo_->addItem("Antithetic Variates");

    mcLayout->addRow("Number of Paths:", numPathsSpin_);
    mcLayout->addRow("Steps per Path:", numStepsSpin_);
    mcLayout->addRow("Variance Reduction:", varianceReductionCombo_);

    // Binomial tree controls
    auto* btLayout = new QFormLayout(binomialGroup_);

    numTreeStepsSpin_->setRange(10, 10000);
    numTreeStepsSpin_->setValue(1000);
    numTreeStepsSpin_->setDecimals(0);

    treeMethodCombo_->addItem("CRR");
    treeMethodCombo_->addItem("CRR with Richardson");

    btLayout->addRow("Number of Steps:", numTreeStepsSpin_);
    btLayout->addRow("Method:", treeMethodCombo_);
}

void MainWindow::createResultsLayout() {
    resultsTable_->setColumnCount(2);
    resultsTable_->setHorizontalHeaderLabels({"Metric", "Value"});
    resultsTable_->horizontalHeader()->setStretchLastSection(true);
    resultsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::createMenuBar() {
    auto* menuBar = new QMenuBar(this);
    auto* fileMenu = menuBar->addMenu("File");
    auto* actionMenu = menuBar->addMenu("Action");

    auto* resetAction = new QAction("Reset", this);
    connect(resetAction, &QAction::triggered, this, &MainWindow::resetFields);

    auto* exportAction = new QAction("Export Results", this);
    connect(exportAction, &QAction::triggered, this, &MainWindow::exportResults);

    auto* exitAction = new QAction("Exit", this);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    auto* calculateAction = new QAction("Calculate", this);
    connect(calculateAction, &QAction::triggered, this, &MainWindow::calculateOption);

    fileMenu->addAction(resetAction);
    fileMenu->addAction(exportAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    actionMenu->addAction(calculateAction);

    setMenuBar(menuBar);
}

void MainWindow::calculateOption() {
    try {
        auto option = createOption();
        option->setPricingEngine(createPricingEngine());
        displayResults(*option);
    }
    catch (const std::exception& e) {
        showErrorMessage(QString("Calculation error: ") + e.what());
    }
}

std::unique_ptr<pricer::Option> MainWindow::createOption() const {
    auto type = optionTypeCombo_->currentData().value<pricer::OptionType>();
    bool isAmerican = optionStyleCombo_->currentText() == "American";

    if (isAmerican) {
        return std::make_unique<pricer::AmericanOption>(
            type,
            strikePriceSpin_->value(),
            timeToExpirySpin_->value(),
            spotPriceSpin_->value(),
            riskFreeRateSpin_->value(),
            volatilitySpin_->value(),
            dividendYieldSpin_->value()
        );
    } else {
        return std::make_unique<pricer::EuropeanOption>(
            type,
            strikePriceSpin_->value(),
            timeToExpirySpin_->value(),
            spotPriceSpin_->value(),
            riskFreeRateSpin_->value(),
            volatilitySpin_->value(),
            dividendYieldSpin_->value()
        );
    }
}

std::shared_ptr<pricer::PricingEngine> MainWindow::createPricingEngine() const {
    QString method = pricingMethodCombo_->currentText();

    if (method == "Black-Scholes") {
        return pricer::makeBlackScholesPricingEngine();
    }
    else if (method == "Monte Carlo") {
        return pricer::makeMonteCarloEngine(
            static_cast<size_t>(numPathsSpin_->value()),
            static_cast<size_t>(numStepsSpin_->value()),
            varianceReductionCombo_->currentText().contains("Antithetic")
        );
    }
    else {  // Binomial Tree
        return pricer::makeBinomialTreeEngine(
            static_cast<size_t>(numTreeStepsSpin_->value()),
            treeMethodCombo_->currentText().contains("Richardson")
        );
    }
}

void MainWindow::displayResults(const pricer::Option& option) {
    resultsTable_->setRowCount(0);

    // Add results to table
    auto addRow = [this](const QString& metric, double value) {
        int row = resultsTable_->rowCount();
        resultsTable_->insertRow(row);
        resultsTable_->setItem(row, 0, new QTableWidgetItem(metric));
        resultsTable_->setItem(row, 1, new QTableWidgetItem(QString::number(value, 'f', 6)));
    };

    addRow("Option Price", option.price());
    addRow("Delta", option.delta());
    addRow("Gamma", option.gamma());
    addRow("Theta", option.theta());
    addRow("Vega", option.vega());
    addRow("Rho", option.rho());

    // If Monte Carlo, add confidence interval
    if (auto* mc_engine = dynamic_cast<const pricer::MonteCarloEngine*>(option.getEngine())) {
        auto ci = mc_engine->getConfidenceInterval();
        addRow("95% CI Lower", ci.first);
        addRow("95% CI Upper", ci.second);
    }

    resultsTable_->resizeColumnsToContents();
}

void MainWindow::updateEngineControls() {
    QString method = pricingMethodCombo_->currentText();

    monteCarloGroup_->setVisible(method == "Monte Carlo");
    binomialGroup_->setVisible(method == "Binomial Tree");

    // Enable/disable option style based on pricing method
    optionStyleCombo_->setEnabled(method != "Black-Scholes");
}

void MainWindow::resetFields() {
    // Reset to default values
    spotPriceSpin_->setValue(100.0);
    strikePriceSpin_->setValue(100.0);
    timeToExpirySpin_->setValue(1.0);
    riskFreeRateSpin_->setValue(0.05);
    volatilitySpin_->setValue(0.2);
    dividendYieldSpin_->setValue(0.0);

    optionTypeCombo_->setCurrentIndex(0);
    optionStyleCombo_->setCurrentIndex(0);
    pricingMethodCombo_->setCurrentIndex(0);

    numPathsSpin_->setValue(100000);
    numStepsSpin_->setValue(252);
    varianceReductionCombo_->setCurrentIndex(0);

    numTreeStepsSpin_->setValue(1000);
    treeMethodCombo_->setCurrentIndex(0);

    resultsTable_->setRowCount(0);
}

void MainWindow::exportResults() {
    if (resultsTable_->rowCount() == 0) {
        showErrorMessage("No results to export.");
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this,
        "Export Results", "", "CSV Files (*.csv);;All Files (*)");

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        showErrorMessage("Could not open file for writing.");
        return;
    }

    QTextStream out(&file);

    // Write header
    out << "Metric,Value\n";

    // Write data
    for (int row = 0; row < resultsTable_->rowCount(); ++row) {
        out << resultsTable_->item(row, 0)->text() << ","
            << resultsTable_->item(row, 1)->text() << "\n";
    }
}

void MainWindow::showErrorMessage(const QString& message) {
    QMessageBox::critical(this, "Error", message);
}

void MainWindow::closeEvent(QCloseEvent* event) {
    // Save window geometry
    QSettings settings;
    settings.setValue("mainWindowGeometry", saveGeometry());
    QMainWindow::closeEvent(event);
}