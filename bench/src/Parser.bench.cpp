#include <benchmark/benchmark.h>

#include <DLX/Parser.hpp>
#include <DLX/Tokenize.hpp>
#include <phi/algorithm/string_length.hpp>
#include <phi/compiler_support/warning.hpp>

PHI_CLANG_SUPPRESS_WARNING("-Wglobal-constructors")

static void BM_TokenizeAndParseADD(benchmark::State& state)
{
    std::int64_t count         = state.range(0);
    std::int64_t string_length = 4;

    // Prepare string
    std::string string;
    string.reserve(count * string_length);

    for (std::int64_t i{0}; i < count; ++i)
    {
        string += "ADD\n";
    }

    for (auto _ : state)
    {
        auto res = dlx::Parser::Parse(string);
        benchmark::DoNotOptimize(res);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * count);
    state.SetBytesProcessed(state.iterations() * count * string_length);
    state.SetComplexityN(count * string_length);
}
BENCHMARK(BM_TokenizeAndParseADD)->RangeMultiplier(2)->Range(8, 8 << 17)->Complexity();

static void BM_ParseADD(benchmark::State& state)
{
    std::int64_t count         = state.range(0);
    std::int64_t string_length = 4;

    // Prepare string
    std::string string;
    string.reserve(count * string_length);

    for (std::int64_t i{0}; i < count; ++i)
    {
        string += "ADD\n";
    }

    // Tokenize
    auto tokens = dlx::Tokenize(string);

    for (auto _ : state)
    {
        auto res = dlx::Parser::Parse(tokens);
        benchmark::DoNotOptimize(res);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * count);
    state.SetBytesProcessed(state.iterations() * count * string_length);
    state.SetComplexityN(count * string_length);
}
BENCHMARK(BM_ParseADD)->RangeMultiplier(2)->Range(8, 8 << 17)->Complexity();

static void BM_ParseADDR1R1R1(benchmark::State& state)
{
    std::int64_t                count         = state.range(0);
    constexpr static const char test_string[] = "ADD, R1, R1, R1\n";
    std::int64_t                string_length = phi::string_length(test_string).unsafe();

    // Prepare string
    std::string string;
    string.reserve(count * string_length);

    for (std::int64_t i{0}; i < count; ++i)
    {
        string += test_string;
    }

    // Tokenize
    auto tokens = dlx::Tokenize(string);

    for (auto _ : state)
    {
        auto res = dlx::Parser::Parse(tokens);
        benchmark::DoNotOptimize(res);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * count);
    state.SetBytesProcessed(state.iterations() * count * string_length);
    state.SetComplexityN(count * string_length);
}
BENCHMARK(BM_ParseADDR1R1R1)->RangeMultiplier(2)->Range(8, 8 << 17)->Complexity();
