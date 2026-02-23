#pragma once
#include <fstream>
#include <iostream>

namespace finutils {
static const int OPTIONS_AMOUNT = 1;

enum class OptionType {
    Call,
    Put
};

inline std::ostream& operator<<(std::ostream& outStream, const finutils::OptionType type) {
    std::string res = (type == finutils::OptionType::Call) ? "Call" : "Put";
    outStream << res;
    return outStream;
}

// void dataGenerator() {


// }

// void getOptionParameters() {

// }
} // namespace finutils