#include <benchmark/benchmark.h>

#include <DLX/Parser.hpp>
#include <DLX/Processor.hpp>
#include <phi/algorithm/string_length.hpp>
#include <phi/core/types.hpp>

PHI_CLANG_SUPPRESS_WARNING("-Wglobal-constructors")

static void BM_ProcessorCountWithADDI(benchmark::State& state)
{
    static constexpr const char* program_source = "ADDI R1 R1 #1\n";

    phi::int64_t                      count         = state.range(0);
    static constexpr const phi::usize string_length = phi::string_length(program_source);

    // Prepare source code
    std::string source;
    source.reserve(static_cast<phi::size_t>(count) * string_length.unsafe());

    for (phi::int64_t i{0}; i < count; ++i)
    {
        source += program_source;
    }

    // Parse it
    auto prog = dlx::Parser::Parse(source);

    dlx::Processor proc;
    proc.SetMaxNumberOfSteps(0u); // Allow unlimited number of steps
    proc.LoadProgram(prog);

    for (auto _ : state)
    {
        // Actual execution
        proc.ExecuteCurrentProgram();

        auto res = proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1);
        benchmark::DoNotOptimize(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 0);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(count);
    state.SetComplexityN(count);
}
BENCHMARK(BM_ProcessorCountWithADDI)->RangeMultiplier(2)->Range(8, 8 << 17)->Complexity();

static void BM_ProcessorCountWithLoop(benchmark::State& state)
{
    static constexpr const char program_source[] = R"dlx(
loop:
    SLT R2 R1 R3
    BEQZ R2 end
    ADDI R1 R1 #1
    J loop
end:
    HALT
)dlx";

    phi::int64_t count = state.range(0);

    // Parse it
    auto prog = dlx::Parser::Parse(program_source);

    dlx::Processor proc;
    proc.SetMaxNumberOfSteps(0u); // Allow unlimited number of steps
    proc.LoadProgram(prog);

    // Set end value
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, static_cast<phi::int32_t>(count));

    for (auto _ : state)
    {
        // Actual execution
        proc.ExecuteCurrentProgram();

        auto res = proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1);
        benchmark::DoNotOptimize(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 0);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(count);
    state.SetComplexityN(count);
}
BENCHMARK(BM_ProcessorCountWithLoop)->RangeMultiplier(2)->Range(8, 8 << 17)->Complexity();

static void BM_ProcessorInfiniteLoop(benchmark::State& state)
{
    static constexpr const char program_source[] = R"dlx(
loop:
    J loop
)dlx";

    phi::int64_t count = state.range(0);

    // Parse it
    auto prog = dlx::Parser::Parse(program_source);

    dlx::Processor proc;
    proc.SetMaxNumberOfSteps(static_cast<phi::uint64_t>(count)); // Limit number of executions
    proc.LoadProgram(prog);

    for (auto _ : state)
    {
        // Actual execution
        proc.ExecuteCurrentProgram();
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(count);
    state.SetComplexityN(count);
}
BENCHMARK(BM_ProcessorInfiniteLoop)->RangeMultiplier(2)->Range(8, 8 << 17)->Complexity();
