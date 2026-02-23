#pragma once
#include "option_parameters.hpp"
#include "utils.hpp"
#include <algorithm>
#include <vector>
#include <cmath>

namespace financial {
class BinominalCalculation {
 private:
    std::vector<double> optionPrices_;
    size_t calcSteps_;
 public:
    explicit BinominalCalculation(size_t calcSteps) : calcSteps_(calcSteps) {
        optionPrices_.resize(calcSteps_ + 1);
    }

    double calcPrice(const OptionParameters& optionParams) {
        AmericanParameters americanParams(optionParams, calcSteps_);

        for (size_t i = 0; i <= calcSteps_; ++i) {
            auto tmp = calcCurPrice(optionParams, americanParams, i, calcSteps_);
            optionPrices_[i] = calcFinalPrice(optionParams, tmp);
        }

        for (size_t curStep = calcSteps_; curStep --> 0;) {
            for (size_t i = 0; i <= curStep; ++i) {
                auto holdValue = calcHoldPrice(americanParams, optionPrices_[i], optionPrices_[i + 1]);

                auto execPrice = calcCurPrice(optionParams, americanParams, i, curStep);
                auto execValue = calcFinalPrice(optionParams, execPrice);
                optionPrices_[i] = std::max(execValue, holdValue);
            }
        }
        return optionPrices_[0];
    }

 private:
    double calcFinalPrice(const OptionParameters& optionParams, const double& payoff) {
        auto tmp = payoff - optionParams.strikePrice_;
        tmp = (optionParams.optionType_ == finutils::OptionType::Call) ? tmp : -tmp;
        return std::max(0., tmp);
    }

    double calcCurPrice(const OptionParameters& optionParams, const AmericanParameters& americanParams, const size_t iteration, const size_t step) {
        return optionParams.spotPrice_ * std::pow(americanParams.upFactor, step - iteration)
                                               * std::pow(americanParams.downFactor, iteration);
    }

    double calcHoldPrice(const AmericanParameters& americanParams, const double nextUp, const double nextDown) {
        return americanParams.discountFactor * (americanParams.riskFactor * nextUp + (1 - americanParams.riskFactor) * nextDown);
    }
};

} // namespace financial