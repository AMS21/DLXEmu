cmake_minimum_required(VERSION 3.10.2)

# Project
project("DLXEmuTest" CXX)

# Files
file(GLOB DLXEMU_TEST_SOURCES "src/CodeEditor.test.cpp")
file(GLOB DLXEMU_TEST_HEADERS )

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${DLXEMU_TEST_SOURCES} ${DLXEMU_TEST_HEADERS})

add_executable("DLXEmuUnittest" ${DLXEMU_TEST_SOURCES} ${DLXEMU_TEST_HEADERS})

target_link_libraries("DLXEmuUnittest" PRIVATE DLXEmuLib Catch2::Catch2WithMain Phi::ProjectOptions)

target_include_directories("DLXEmuUnittest" PRIVATE "include")

#target_compile_definitions("DLXEmuUnittest" PRIVATE "_GLIBCXX_DEBUG;_LIBCPP_DEBUG=1;_ITERATOR_DEBUG_LEVEL=2")

set_target_properties("DLXEmuUnittest" PROPERTIES FOLDER "Tests")
set_target_properties("DLXEmuUnittest" PROPERTIES CXX_STANDARD 20)

phi_add_test(TARGET DLXEmuUnittest)
