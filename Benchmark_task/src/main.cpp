#include "benchlib.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

int main() {
    std::vector<double> data = {0.1, 0.00001, 1, 2, 100};

    auto stdlog = [](double x) {
        return std::log(x);
    };

    auto stdexp = [](double x){
        return std::exp(x);
    };

    std::cout << "stdlog res: " << "\n";
    benchlib::funcLatencyTest(stdlog, data);
    benchlib::funcThroughputTest(stdlog, data);
    std::cout << "-------------------------------\n";

    std::cout << "stdexp res: " << "\n";
    benchlib::funcLatencyTest(stdexp, data);
    benchlib::funcThroughputTest(stdexp, data);
    std::cout << "-------------------------------\n";

    return EXIT_SUCCESS;
}