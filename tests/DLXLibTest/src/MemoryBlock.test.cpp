#include <phi/core/types.hpp>
#include <phi/test/test_macros.hpp>

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

TEST_CASE("IsAddressValid")
{
    dlx::MemoryBlock mem{1000u, 10u};

    // Address is before our starting address
    CHECK_FALSE(mem.IsAddressValid(0u, 0u));
    CHECK_FALSE(mem.IsAddressValid(0u, 1u));
    CHECK_FALSE(mem.IsAddressValid(999u, 1u));
    CHECK_FALSE(mem.IsAddressValid(999u, 2u));
    CHECK_FALSE(mem.IsAddressValid(999u, 4u));
    CHECK_FALSE(mem.IsAddressValid(999u, 8u));
    CHECK_FALSE(mem.IsAddressValid(998u, 2u));

    // Address is after our memory block
    CHECK_FALSE(mem.IsAddressValid(1010u, 1u));
    CHECK_FALSE(mem.IsAddressValid(1011u, 1u));
    CHECK_FALSE(mem.IsAddressValid(1012u, 1u));
    CHECK_FALSE(mem.IsAddressValid(1013u, 1u));

    // Address + size land after our memory block
    CHECK_FALSE(mem.IsAddressValid(1009u, 4u));
    CHECK_FALSE(mem.IsAddressValid(1008u, 4u));
    CHECK_FALSE(mem.IsAddressValid(1007u, 4u));

    // Address + size will overflow
    CHECK_FALSE(mem.IsAddressValid(phi::usize::max(), 2u));
    CHECK_FALSE(mem.IsAddressValid(2u, phi::usize::max()));

    // Starting address + Size will overflow
    mem.SetStartingAddress(phi::usize::max());

    CHECK_FALSE(mem.IsAddressValid(1u, 1u));

    // Valid addresses
    mem.SetStartingAddress(0u);

    CHECK(mem.IsAddressValid(0u, 1u));
    CHECK(mem.IsAddressValid(0u, 2u));
    CHECK(mem.IsAddressValid(0u, 4u));
    CHECK(mem.IsAddressValid(0u, 8u));

    CHECK(mem.IsAddressValid(1u, 1u));
    CHECK(mem.IsAddressValid(2u, 1u));
    CHECK(mem.IsAddressValid(3u, 1u));
    CHECK(mem.IsAddressValid(4u, 1u));
    CHECK(mem.IsAddressValid(5u, 1u));
    CHECK(mem.IsAddressValid(7u, 1u));
    CHECK(mem.IsAddressValid(8u, 1u));
    CHECK(mem.IsAddressValid(9u, 1u));

    CHECK(mem.IsAddressValid(4u, 4u));
}

TEST_CASE("Resize")
{
    dlx::MemoryBlock mem{1000u, 10u};
    CHECK(mem.GetSize() == 10u);

    mem.Resize(25u);
    CHECK(mem.GetSize() == 25u);

    mem.Resize(3u);
    CHECK(mem.GetSize() == 3u);

    mem.Resize(7u);
    CHECK(mem.GetSize() == 7u);
}
