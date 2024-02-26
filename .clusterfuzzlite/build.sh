#!/bin/bash -eu

# Create build directory
mkdir -p build
cd build

# Configure cmake
cmake .. -G Ninja \
    -DCMAKE_C_COMPILER="${CC}" -DCMAKE_CXX_COMPILER="${CXX}" \
    -DCMAKE_C_FLAGS="${CFLAGS} -DPHI_CONFIG_ENABLE_ASSERTIONS" -DCMAKE_CXX_FLAGS="${CXXFLAGS} -DPHI_CONFIG_ENABLE_ASSERTIONS" \
    -DPHI_FUZZING_LIBRARY:STRING="${LIB_FUZZING_ENGINE}" \
    -DDLXEMU_USE_GLAD:BOOL=OFF

# Build fuzzers
ninja tests/Fuzzing/all

# Remove helper targets
rm -f bin/generate_dictionary_app bin/fuzz_code_editor_verbose bin/fuzz_memory_block_verbose

# Copy fuzzers to out
cp bin/* "${OUT}/"

# Build code editor fuzzer with DLXEMU_VERIFY_UNDO_REDO
cmake . -DCMAKE_C_FLAGS="${CFLAGS} -DPHI_CONFIG_ENABLE_ASSERTIONS -DDLXEMU_VERIFY_UNDO_REDO" -DCMAKE_CXX_FLAGS="${CXXFLAGS} -DPHI_CONFIG_ENABLE_ASSERTIONS -DDLXEMU_VERIFY_UNDO_REDO"

ninja fuzz_code_editor

cp bin/fuzz_code_editor "${OUT}/fuzz_code_editor_undo_redo"

# Build code editor fuzzer with DLXEMU_VERIFY_COLUMN
cmake . -DCMAKE_C_FLAGS="${CFLAGS} -DPHI_CONFIG_ENABLE_ASSERTIONS -DDLXEMU_VERIFY_COLUMN" -DCMAKE_CXX_FLAGS="${CXXFLAGS} -DPHI_CONFIG_ENABLE_ASSERTIONS -DDLXEMU_VERIFY_COLUMN"

ninja fuzz_code_editor

cp bin/fuzz_code_editor "${OUT}/fuzz_code_editor_column"
