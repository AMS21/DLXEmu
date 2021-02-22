cmake_minimum_required(VERSION 3.10.2)

file(GLOB DLXEMULIB_SOURCES "src/CommandLineArgs.cpp" "src/Emulator.cpp" "src/Window.cpp")
file(GLOB DLXEMULIB_HEADERS "include/CommandLineArgs.hpp" "include/Emulator.hpp" "include/Window.hpp")

# DLXEmuLib
add_library("DLXEmuLib" STATIC ${DLXEMULIB_SOURCES} ${DLXEMULIB_HEADERS})

target_include_directories("DLXEmuLib" PUBLIC "include")

target_link_libraries("DLXEmuLib" PUBLIC DLXLib Phi::ProjectOptions ImGui ImGuiColorTextEdit ImGuiFilebrowser)

# DLXEmu
add_executable("DLXEmu" "src/main.cpp")

target_link_libraries("DLXEmu" PRIVATE DLXEmuLib Phi::ProjectOptions)
