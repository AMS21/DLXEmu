cmake_minimum_required(VERSION 3.10.2)

file(GLOB DLXEMULIB_SOURCES "src/CodeEditor.cpp" "src/CommandLineArgs.cpp" "src/Emulator.cpp" "src/Window.cpp")
file(GLOB DLXEMULIB_HEADERS "include/dlxemu/CodeEditor.hpp" "include/dlxemu/CommandLineArgs.hpp" "include/dlxemu/Emulator.hpp" "include/dlxemu/Window.hpp")

# DLXEmuLib
add_library("DLXEmuLib" STATIC ${DLXEMULIB_SOURCES} ${DLXEMULIB_HEADERS})

target_include_directories("DLXEmuLib" PUBLIC "include")

target_link_libraries("DLXEmuLib" PUBLIC DLXLib Phi::ProjectOptions ImGui
                                         ImGuiFilebrowser)

phi_enable_static_analyzers("DLXEmuLib")

# DLXEmu
add_executable("DLXEmu" "src/main.cpp")

target_link_libraries("DLXEmu" PRIVATE DLXEmuLib Phi::ProjectOptions)
phi_enable_static_analyzers("DLXEmu")