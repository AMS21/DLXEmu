#include "StructureParser.hpp"
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "No input file given!\n";
        return 1;
    }

    std::string file_name = argv[1];

    // Open file
    std::ifstream stream{file_name, std::ios::binary};
    if (!stream.is_open())
    {
        std::cerr << "Failed to open file " << file_name << '\n';
        return 1;
    }

    // Read the file
    std::string content;

    stream.seekg(0, std::ios::end);
    content.reserve(stream.tellg());
    stream.seekg(0, std::ios::beg);

    content.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

    // Convert it
    std::string converted = fuzz::ParseAsStrucutedDLXCode(
            reinterpret_cast<std::uint8_t*>(content.data()), content.size());

    std::cout << converted;

    return 0;
}
