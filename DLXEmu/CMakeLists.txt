cmake_minimum_required(VERSION 3.10.2)

file(GLOB DLXEMULIB_SOURCES "src/main.cpp")
file(GLOB DLXEMULIB_HEADERS "")

# DLXEmuLib
add_library("DLXEmuLib" STATIC ${DLXEMULIB_SOURCES} ${DLXEMULIB_HEADERS})

target_include_directories("DLXEmuLib" PUBLIC "include")

target_link_libraries("DLXEmuLib" PUBLIC DLXLib)

# DLXEmu
add_executable("DLXEmu" "src/main.cpp")

target_link_libraries("DLXEmu" PRIVATE DLXEmuLib Phi::ProjectOptions)
