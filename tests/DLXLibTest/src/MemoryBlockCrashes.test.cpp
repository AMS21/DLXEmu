#include <DLX/MemoryBlock.hpp>
#include <phi/test/test_macros.hpp>

// reference binding to misaligned address 0xXXXXXXXXXXXX for type 'const double', which requires 8 byte alignment
TEST_CASE("crash-96023913945c4f152223ebd84084b26adea8d610")
{
    dlx::MemoryBlock memory_block{1000u, 128u};

    (void)memory_block.LoadDouble(1031u);

    // Same for float
    (void)memory_block.LoadFloat(1031u);
}

// reference binding to misaligned address 0xXXXXXXXXXXXX for type 'const std::int32_t' (aka 'const int'), which requires 4 byte alignment
TEST_CASE("crash-ca64a959f0bcef92756815f5d1fc19109a8d2038")
{
    dlx::MemoryBlock memory_block{1000u, 128u};

    memory_block.SetStartingAddress(5u);
    (void)memory_block.LoadWord(8u);
}
