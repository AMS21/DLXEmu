find_package(Threads REQUIRED)

# Phi
add_subdirectory(Phi)

include(Phi/scripts/cmake/Environment.cmake)

# Use latest standard available but at least C++-11
if(DEFINED CMAKE_CXX23_STANDARD_COMPILE_OPTION OR DEFINED CMAKE_CXX23_EXTENSION_COMPILE_OPTION)
  set(LatestCXXStandard 23)
elseif(DEFINED CMAKE_CXX20_STANDARD_COMPILE_OPTION OR DEFINED CMAKE_CXX20_EXTENSION_COMPILE_OPTION)
  set(LatestCXXStandard 20)
elseif(DEFINED CMAKE_CXX17_STANDARD_COMPILE_OPTION OR DEFINED CMAKE_CXX17_EXTENSION_COMPILE_OPTION)
  set(LatestCXXStandard 17)
elseif(DEFINED CMAKE_CXX14_STANDARD_COMPILE_OPTION OR DEFINED CMAKE_CXX14_EXTENSION_COMPILE_OPTION)
  set(LatestCXXStandard 14)
else()
  set(LatestCXXStandard 11)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD ${LatestCXXStandard})

# Catch2
add_subdirectory(Catch2)

# https://cmake.org/cmake/help/latest/policy/CMP0079.html
cmake_policy(SET CMP0079 NEW)
# Need to link threads with catch2 for emscripten to build
target_link_libraries(Catch2 PUBLIC Threads::Threads)

# glad
add_subdirectory(glad)

# glfw
if(NOT PHI_PLATFORM_EMSCRIPTEN)
  add_subdirectory(glfw)
endif()

# ImGui
add_library(
  "ImGui" STATIC
  "imgui/imgui.cpp"
  "imgui/imgui_demo.cpp"
  "imgui/imgui_draw.cpp"
  "imgui/imgui_tables.cpp"
  "imgui/imgui_widgets.cpp"
  "imgui/backends/imgui_impl_glfw.cpp"
  "imgui/backends/imgui_impl_opengl3.cpp")

target_compile_definitions(
  "ImGui"
  PUBLIC "IMGUI_IMPL_OPENGL_LOADER_GLAD"
         IMGUI_USER_CONFIG="${CMAKE_CURRENT_SOURCE_DIR}/../DLXEmu/include/DLXEmu/ImGuiConfig.hpp")

target_include_directories("ImGui" PUBLIC "imgui")

target_link_libraries("ImGui" PUBLIC glad glfw Phi::Core)

# ImGuiFilebrowser
add_library("ImGuiFilebrowser" INTERFACE)

target_include_directories("ImGuiFilebrowser" INTERFACE "imgui-filebrowser")

# OpenGL
set(OpenGL_GL_PREFERENCE "GLVND")

find_package(OpenGL REQUIRED)

# magic_enum
add_subdirectory(magic_enum)

# spdlog
set(SPDLOG_DISABLE_DEFAULT_LOGGER
    ON
    CACHE INTERNAL "")
set(SPDLOG_NO_EXCEPTIONS
    ON
    CACHE INTERNAL "")
add_subdirectory(spdlog)

# benchmark
if(DLXEMU_BUILD_BENCHMARKS)
  set(BENCHMARK_ENABLE_TESTING
      OFF
      CACHE INTERNAL "")
  set(BENCHMARK_ENABLE_INSTALL
      OFF
      CACHE INTERNAL "")
  set(BENCHMARK_ENABLE_EXCEPTIONS
      OFF
      CACHE INTERNAL "")
  add_subdirectory(benchmark EXCLUDE_FROM_ALL)
endif()
