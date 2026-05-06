#pragma once

#include <fstream>

namespace tests
{
void isRNGCorrect(std::ofstream& output);
void piBenchmark(std::ofstream& output);
void benchmarkSingleThread(std::ofstream&);
void measureScaling(std::ofstream&);
}  // namespace tests