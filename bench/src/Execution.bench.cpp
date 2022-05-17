#include <benchmark/benchmark.h>

#include <DLX/Parser.hpp>
#include <DLX/Processor.hpp>
#include <phi/algorithm/string_length.hpp>

static void BM_ProcessorCountWithADDI(benchmark::State& state)
{
    static constexpr const char program_source[] = "ADDI R1 R1 #1\n";

    std::int64_t count         = state.range(0);
    std::int64_t string_length = phi::string_length(program_source).unsafe();

    // Prepare source code
    std::string source;
    source.reserve(count * string_length);

    for (std::size_t i{0u}; i < count; ++i)
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

    std::int64_t count = state.range(0);

    // Parse it
    auto prog = dlx::Parser::Parse(program_source);

    dlx::Processor proc;
    proc.SetMaxNumberOfSteps(0u); // Allow unlimited number of steps
    proc.LoadProgram(prog);

    // Set end value
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, static_cast<std::int32_t>(count));

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

    std::int64_t count         = state.range(0);
    std::int64_t string_length = phi::string_length(program_source).unsafe();
    ;

    // Parse it
    auto prog = dlx::Parser::Parse(program_source);

    dlx::Processor proc;
    proc.SetMaxNumberOfSteps(static_cast<std::uint64_t>(count)); // Limit number of executions
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
