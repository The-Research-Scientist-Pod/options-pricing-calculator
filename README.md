# Options Pricing Calculator

A modern C++ library and GUI application for pricing financial options using various methodologies including Black-Scholes, Monte Carlo simulation, and binomial tree models.

## Features

- Multiple pricing models:
  - Black-Scholes analytical solution
  - Monte Carlo simulation with variance reduction
  - Binomial tree model with Richardson extrapolation
- Support for both European and American options
- Complete Greeks calculations (Delta, Gamma, Theta, Vega, Rho)
- Modern, Qt-based graphical interface
- Fast, parallel Monte Carlo simulations
- Confidence interval calculations
- Export capabilities for results

## Prerequisites

### Required Libraries

- C++ compiler supporting C++20
- CMake (>= 3.15)
- Qt6
- Google Test (for unit tests)

### Installation on macOS

Using Homebrew:
```bash
# Install basic requirements
brew install cmake
brew install qt@6
brew install googletest

# Make sure Qt is linked
brew link qt@6
```

### Installation on Linux (Ubuntu/Debian)

```bash
# Install basic requirements
sudo apt-get update
sudo apt-get install cmake
sudo apt-get install qt6-base-dev
sudo apt-get install libgtest-dev
```

## Building the Project

1. Clone the repository:
```bash
git clone https://github.com/yourusername/options-pricing-calculator.git
cd options-pricing-calculator
```

2. Create and enter build directory:
```bash
mkdir build
cd build
```

3. Configure with CMake:
```bash
cmake ..
```

4. Build:
```bash
make
```

## Running the Application

After building, you can find the executable in the build directory:

```bash
# Run the GUI application
./src/gui/options_pricer_gui

# Run the tests
./tests/options_pricer_tests
```

## Usage

1. Select option type (Call/Put)
2. Choose option style (European/American)
3. Select pricing method:
   - Black-Scholes (European options only)
   - Monte Carlo simulation
   - Binomial tree
4. Enter parameters:
   - Spot price
   - Strike price
   - Time to expiry
   - Risk-free rate
   - Volatility
   - Dividend yield (if any)
5. Configure method-specific settings if applicable
6. Click "Calculate" to get results

## Project Structure

```
options_pricer/
├── CMakeLists.txt                 # Main CMake configuration
├── README.md
├── include/
│   └── pricer/
│       ├── black_scholes.h
│       ├── monte_carlo.h
│       ├── binomial.h
│       ├── option.h
│       └── utils.h
├── src/
│   ├── CMakeLists.txt            # Source build configuration
│   ├── black_scholes.cpp
│   ├── monte_carlo.cpp
│   ├── binomial.cpp
│   ├── option.cpp
│   ├── utils.cpp
│   └── gui/
│       ├── CMakeLists.txt        # GUI build configuration
│       ├── main_window.h
│       ├── main_window.cpp
│       └── main.cpp
├── tests/
│   ├── CMakeLists.txt           # Test build configuration
│   ├── test_main.cpp
│   ├── test_black_scholes.cpp
│   ├── test_monte_carlo.cpp
│   └── test_binomial.cpp
└── examples/
    ├── CMakeLists.txt           # Examples build configuration
    └── basic_usage.cpp
```

## Additional Resources

### Interactive Calculators
For quick calculations and comparison with our implementation, you can use these online calculators:

- **Black-Scholes Calculator**: An interactive tool for calculating European option prices and Greeks using the Black-Scholes model. Useful for validating our implementation and quick price checks. [Try it here](https://researchdatapod.com/data-science-tools/calculators/black-scholes-option-pricing-calculator/)

- **Monte Carlo Options Calculator**: Demonstrates the Monte Carlo simulation method for option pricing, allowing you to understand how different numbers of simulations affect pricing accuracy. [Try it here](https://researchdatapod.com/data-science-tools/calculators/monte-carlo-option-pricing-calculator/)

- **Binomial Option Calculator**: Interactive tool for pricing options using the binomial tree model, particularly useful for understanding early exercise features of American options. [Try it here](https://researchdatapod.com/data-science-tools/calculators/binomial-option-pricing-calculator/)

- **Implied Volatility Calculator**: Helpful tool for calculating implied volatility from market prices, which can be used as input for our pricing models. [Try it here](https://researchdatapod.com/data-science-tools/calculators/implied-volatility-calculator/)

### Implementation Guides
If you're interested in understanding the implementation details or want to compare with other programming languages:

- **C++ Implementation**: Detailed guide on implementing Black-Scholes in C++, which forms the basis of our analytical pricing engine. [Read more](https://researchdatapod.com/black-scholes-option-pricing-in-c/)

- **Python Implementation**: For those interested in Python implementations, this guide provides a great comparison point and can help in validating our results. [Read more](https://researchdatapod.com/black-scholes-option-pricing-in-python/)

- **R Implementation**: Provides another perspective on options pricing implementation, useful for cross-validation and understanding different approaches. [Read more](https://researchdatapod.com/black-scholes-option-pricing-in-r/)

### Theoretical Background

- **Options Greeks Derivation**: Comprehensive guide explaining the mathematical derivation of option Greeks from the Black-Scholes formula. Essential for understanding our Greeks calculations implementation. [Read more](https://researchdatapod.com/how-to-derive-options-greeks-from-the-black-scholes-formula/)

These resources provide valuable reference points for understanding both the theoretical foundations and practical implementations of options pricing. They can be particularly helpful when extending or modifying our implementation, or when validating results against established tools.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request. For major changes, please open an issue first to discuss what you would like to change.

## Acknowledgments

- Built with Qt6 and modern C++
- Uses Google Test for unit testing
- Special thanks to all contributors
