#include <benchmark/benchmark.h>

#include <DLX/Tokenize.hpp>
#include <phi/compiler_support/warning.hpp>
#include <cstdlib>
#include <limits>
#include <random>

PHI_CLANG_SUPPRESS_WARNING("-Wglobal-constructors")
PHI_CLANG_AND_GCC_SUPPRESS_WARNING("-Wdeprecated-declarations")

static void BM_TokzenizeRandom(benchmark::State& state)
{
    phi::int64_t length = state.range(0);

    // Prepare randomness
    std::random_device                  random_device;
    std::uniform_int_distribution<char> dist(1, 127);

    // Prepare string
    std::string string;
    string.resize(static_cast<phi::size_t>(length));

    for (phi::size_t i{0u}; i < static_cast<phi::size_t>(length); ++i)
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
    phi::int64_t                        count         = state.range(0);
    static constexpr const phi::int64_t string_length = 4;

    // Prepare string
    std::string string;
    string.reserve(static_cast<phi::size_t>(count * string_length));

    for (phi::int64_t i{0u}; i < count; ++i)
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
