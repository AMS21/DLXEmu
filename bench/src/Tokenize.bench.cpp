#include <benchmark/benchmark.h>

#include <DLX/Tokenize.hpp>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <random>

static void BM_TokzenizeRandom(benchmark::State& state)
{
    std::int64_t length = state.range(0);

    // Prepare randomness
    std::random_device                  random_device;
    std::uniform_int_distribution<char> dist(1, 127);

    // Prepare string
    std::string string;
    string.resize(length);

    for (std::size_t i{0u}; i < length; ++i)
    {
        string[i] = dist(random_device);
    }

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(dlx::Tokenize(string));
        benchmark::ClobberMemory();
    }

    state.SetBytesProcessed(state.iterations() * length);
    state.SetComplexityN(length);
}
BENCHMARK(BM_TokzenizeRandom)->RangeMultiplier(2)->Range(8, 8 << 17)->Complexity();

static void BM_TokzenizeADD(benchmark::State& state)
{
    std::int64_t count         = state.range(0);
    std::int64_t string_length = 4;

    // Prepare string
    std::string string;
    string.reserve(count * string_length);

    for (std::size_t i{0u}; i < count; ++i)
    {
        string += "ADD\n";
    }

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(dlx::Tokenize(string));
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * count);
    state.SetBytesProcessed(state.iterations() * count * string_length);
    state.SetComplexityN(count * string_length);
}
BENCHMARK(BM_TokzenizeADD)->RangeMultiplier(2)->Range(8, 8 << 17)->Complexity();
