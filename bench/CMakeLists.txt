# Project
project("DLXLibBenchmark" CXX)

# Files
file(GLOB DLXLIB_BENCH_SOURCES "src/Execution.bench.cpp" "src/Parser.bench.cpp" "src/Tokenize.bench.cpp")
file(GLOB DLXLIB_BENCH_HEADERS)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${DLXLIB_BENCH_SOURCES} ${DLXLIB_BENCH_HEADERS})

phi_add_executable(NAME ${PROJECT_NAME} SOURCES ${DLXLIB_BENCH_SOURCES} HEADERS ${DLXLIB_BENCH_HEADERS} FOLDER "Benchmarks" PRIVATE_LINK_LIBRARIES DLXLib benchmark::benchmark_main STANDARD "latest")

phi_add_test(TARGET ${PROJECT_NAME})
