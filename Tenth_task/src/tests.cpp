#include "tests.hpp"

#include <omp.h>

#include <bit>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <numeric>
#include <random>
#include <span>

#include "minstdrand.hpp"

void tests::isRNGCorrect(std::ofstream& output)
{
    rng::VectorMinstd rng;
    rng.seed(42);

    const size_t testSize = 128;
    std::vector<uint32_t> vecOut(testSize);
    rng.generateInt(vecOut);

    std::minstd_rand stdRNG(42);  // reference
    bool passed = true;
    for (size_t i = 0; i < testSize; ++i)
    {
        uint32_t stdVal = stdRNG();
        if (vecOut[i] != stdVal)
        {
            output << "Mismatch at " << i << ": vec =" << vecOut[i] << " std =" << stdVal << "\n";
            passed = false;
        }
    }
    if (passed)
    {
        output << "Functional correctness check PASSED\n";
    }
}

void tests::piBenchmark(std::ofstream& output)
{
    constexpr size_t ITERATIONS = 100000000;  // 100 млн точек, каждая требует 2 числа => нужно 200 млн float
    constexpr size_t BLOCKSIZE = 512;
    constexpr double PI_REF    = std::numbers::pi;
    size_t totalCatched        = 0;

    auto begin = std::chrono::high_resolution_clock::now();
#pragma omp parallel reduction(+ : totalCatched)
    {
        int tidTmp        = omp_get_thread_num();
        int numThreadsTmp = omp_get_num_threads();

        uint32_t tid        = std::bit_cast<uint32_t>(tidTmp);
        uint32_t numThreads = std::bit_cast<uint32_t>(numThreadsTmp);

        size_t ptsPerThread = ITERATIONS / numThreads;
        size_t beginPoint   = tid * ptsPerThread;
        size_t endPoint     = (tid == numThreads - 1) ? ITERATIONS : beginPoint + ptsPerThread;
        size_t count        = endPoint - beginPoint;

        // тк на 1 точку нужно 2 генерации мы смещаемся на 2 * beginPoint.
        rng::VectorMinstd rng;
        rng.seed(42, beginPoint * 2);

        // блок содержит 2 * BLOCKSIZE случайных чисел
        std::vector<float> block(BLOCKSIZE * 2);
        size_t threadCatched = 0;

        for (size_t b = 0; b < count; b += BLOCKSIZE)
        {
            size_t current_block = std::min(BLOCKSIZE, count - b);

            rng.generateFloat(block);

// Векторизованный подсчет x^2 + y^2
#pragma omp simd reduction(+ : threadCatched)
            for (size_t i = 0; i < current_block; ++i)
            {
                float x = block[i * 2];
                float y = block[i * 2 + 1];
                if (x * x + y * y <= 1.0f)
                {
                    threadCatched++;
                }
            }
        }
        totalCatched += threadCatched;
    }

    auto end                              = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - begin;

    double calculatedPi    = 4.0 * static_cast<double>(totalCatched) / ITERATIONS;
    double absError        = std::abs(calculatedPi - PI_REF);
    double stdDev          = std::sqrt(PI_REF * (4.0 - PI_REF) / static_cast<double>(ITERATIONS));
    double confidenceLimit = 3.0 * stdDev;

    // Вывод результатов
    output << "[fp32 Vectorized minstd_rand with Skip-Ahead Block-Splitting]\n";
    output << std::fixed << std::setprecision(15);
    output << "  Calculated Pi: " << calculatedPi << "\n";
    output << "  Reference Pi:  " << PI_REF << "\n";
    output << "  Abs Error:     " << std::scientific << absError << "\n";
    output << "  Std Deviation: " << stdDev << "\n";
    output << "  3-Sigma Limit: " << confidenceLimit << "\n";
    output << "  Time Elapsed:  " << std::fixed << std::setprecision(4) << elapsed.count() << " s\n";

    if (absError <= confidenceLimit)
        output << "  [SUCCESS] Error IS in 3 SD. Generator is OK.\n\n";
    else
        output << "  [WARNING] Error IS NOT in 3 SD! Generator might be broken.\n\n";
}

void tests::benchmarkSingleThread(std::ofstream& output)
{
    const size_t N = 100'000'000;
    std::vector<uint32_t> vecData(N);
    std::vector<uint32_t> stdData(N);

    std::minstd_rand std(42);
    auto begin = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < N; ++i)
    {
        stdData[i] = std();
    }
    auto end                                  = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationStd = end - begin;

    rng::VectorMinstd vec;
    vec.seed(42);
    begin = std::chrono::high_resolution_clock::now();
    vec.generateInt(std::span{vecData.data(), N});
    end                                       = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationVec = end - begin;

    output << "std::minstd_rand Time: " << durationStd.count() << " s\n";
    output << "VectorMinstd Time:     " << durationVec.count() << " s\n";
    output << "Speedup vs std::     " << durationStd.count() / durationVec.count() << "x\n";
}

void tests::measureScaling(std::ofstream& output)
{
    const size_t N  = 100'000'000;
    int maxThreads  = omp_get_max_threads();
    double baseTime = 0.0;

    output << "\nThreads | Time (s) | Speedup\n";
    output << "----------------------------------\n";

    for (int t = 1; t <= maxThreads; ++t)
    {
        omp_set_num_threads(t);
        auto begin = std::chrono::high_resolution_clock::now();

        tests::piBenchmark(output);

        auto end        = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(end - begin).count();

        if (t == 1) baseTime = duration;
        double speedup = baseTime / duration;

        output << std::setw(7) << t << " | " << std::setw(8) << std::fixed << std::setprecision(4) << duration << " | " << std::setw(6)
               << speedup << "x\n";
    }
}