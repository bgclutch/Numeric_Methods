#pragma once
#include <fstream>
#include <random>
#include <chrono>
#include <iomanip>
#include <omp.h>
#include <cmath>

namespace pi_bench {
static const size_t ITERS = 100000000;
static const double PI_REF = 3.14159265358979323846;

template <typename ElemType>
void runBenchmark(std::ofstream& output, size_t iterations, const std::string& typeName) {
    size_t inCircleUni  = 0;
    const ElemType ZERO = static_cast<ElemType>(0.0);
    const ElemType ONE = static_cast<ElemType>(1.0);
    const ElemType RADIUS = static_cast<ElemType>(1.0);
    const ElemType QUARTER_COEF = static_cast<ElemType>(4.0);

    // EDITED general seed for all threads
    std::random_device rd;
    // EDITED getting start set point from non-determined generator
    unsigned int global_seed = rd();

    /* method rd() in std::randon_device implemented as CPU transistors heat noise interpretation
       (on Unix /dev/urandom and in x86-64 chips implemented an RDRAND instruction)
       Here we are getting good seed by slow method and then use fast determined algorithm mt19937

       Random Seed vs Fixed Seed(eg 42):
       While a fixed seed is useful for debugging and perfect reproducibility,
       a random seed is preferred for this statistical experiment to:
       1. Prove that the estimation of Pi is unbiased regardless of the entry point.
       2. Demonstrate that the discard() method works on any sequence.
    */

    #pragma omp parallel
    {
        // EDITED all threads initializes with common seed
        std::mt19937 genUni(global_seed);

        int threadId   = omp_get_thread_num();
        int numThreads = omp_get_num_threads();

        // ADD block-splitting method for all threads
        size_t itersPerThread = (iterations / static_cast<size_t>(numThreads)) + 1;

        // ADD guarantee that each thread will starts computing from its own unique number
        unsigned long long skipCount = itersPerThread * 2 * static_cast<size_t>(threadId);

        // ADD DISCARD(), skip until current thread gets needed part of data set, no thread intersections
        genUni.discard(skipCount);

        std::uniform_real_distribution<ElemType> distUni(ZERO, ONE);

        #pragma omp for reduction(+:inCircleUni)
        for (size_t i = 0; i < iterations; ++i) {
            ElemType x = distUni(genUni);
            ElemType y = distUni(genUni);
            if (x * x + y * y <= RADIUS) {
                ++inCircleUni;
            }
        }
    }

    ElemType calculatedPi = QUARTER_COEF * static_cast<ElemType>(inCircleUni) / static_cast<ElemType>(iterations);
    double absError = std::abs(static_cast<double>(calculatedPi) - PI_REF);

    // ADD error estimation
    double stdDev = std::sqrt(PI_REF * (4.0 - PI_REF) / static_cast<double>(iterations));
    double confidenceLimit = 3.0 * stdDev;

    output << "[" << typeName << " with discard() Block-Splitting]\n";
    output << std::fixed << std::setprecision(15);
    output << "  Calculated Pi: " << calculatedPi << "\n";
    output << "  Reference Pi:  " << PI_REF << "\n";
    output << "  Abs Error:     " << std::scientific << absError << "\n";
    output << "  Std Deviation: " << stdDev << "\n";
    output << "  3-Sigma Limit: " << confidenceLimit << "\n";

    if (absError <= confidenceLimit)
        output << "  [SUCCESS] Error IS in 3 SD. Generator is OK.\n\n";
    else
        output << "  [WARNING] Error IS NOT in 3 SD! Generator might be broken.\n\n";
}
} // namespace pi_bench