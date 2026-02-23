#include "american_option.hpp"
#include "option_parameters.hpp"
#include "utils.hpp"
#include <iostream>



int main() {
    financial::OptionParameters optParams{100, 100, 1, 0.05, 0.1, finutils::OptionType::Call};
    financial::BinominalCalculation calcOpt(1000);

    auto result = calcOpt.calcPrice(optParams);
    std::cout << "Best price: " << result << "\n" << "For option: \n" << optParams << std::endl;

    return EXIT_SUCCESS;
}