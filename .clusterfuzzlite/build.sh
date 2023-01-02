#!/bin/bash -eu

# Create build directory
mkdir -p build
cd build

# Configure cmake
cmake .. -G Ninja \
	-DCMAKE_C_COMPILER="${CC}" -DCMAKE_CXX_COMPILER="${CXX}" \
	-DCMAKE_C_FLAGS="${CFLAGS} -DPHI_CONFIG_ENABLE_ASSERTIONS" -DCMAKE_CXX_FLAGS="${CXXFLAGS} -DPHI_CONFIG_ENABLE_ASSERTIONS" \
	-DPHI_FUZZING_LIBRARY:STRING="${LIB_FUZZING_ENGINE}"

# Build fuzzers
ninja tests/Fuzzing/all

# Remove helper targets
rm -f bin/generate_dictionary_app bin/convert_structured_text bin/fuzz_code_editor_verbose

# Copy fuzzers to out
cp bin/* "${OUT}/"