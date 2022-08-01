#include <catch2/catch_test_macros.hpp>

#include <DLX/MemoryBlock.hpp>
#include <vector>

TEST_CASE("MemoryBlock")
{
    dlx::MemoryBlock mem{1000u, 20u};

    CHECK((mem.GetStartingAddress() == 1000u).unsafe());
    CHECK((mem.GetSize() == 20u).unsafe());

    mem.SetStartingAddress(100u);

    CHECK((mem.GetStartingAddress() == 100u).unsafe());
    CHECK((mem.GetSize() == 20u).unsafe());
}

TEST_CASE("GetRawMemory")
{
    dlx::MemoryBlock mem{1000u, 10u};

    std::vector<dlx::MemoryBlock::MemoryByte>& vec = mem.GetRawMemory();

    CHECK_FALSE(vec.empty());
    CHECK(vec.size() == 10u);

    const dlx::MemoryBlock const_mem{1000u, 10u};

    const std::vector<dlx::MemoryBlock::MemoryByte>& const_vec = const_mem.GetRawMemory();

    CHECK_FALSE(const_vec.empty());
    CHECK(const_vec.size() == 10u);
}
