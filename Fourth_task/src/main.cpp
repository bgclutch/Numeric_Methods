#include "monte_carlo_pi.hpp"
#include <stdfloat>
#include <iostream>
#include <fstream>

int main() {
    std::ofstream file("pi_calculation.txt");
    pi_bench::runBenchmark<std::float32_t>(file, pi_bench::ITERS,  "float32");
    pi_bench::runBenchmark<std::float64_t>(file, pi_bench::ITERS, "float64");
    return 0;
}