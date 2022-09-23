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

# fmt
add_subdirectory(fmt)

# Ensure exceptions are disable for fmtlib
target_compile_definitions(fmt PUBLIC FMT_EXCEPTIONS=0)

# glad
add_subdirectory(glad)

# glfw
if(NOT PHI_PLATFORM_EMSCRIPTEN)
  add_subdirectory(glfw)
else()
  message(STATUS "Using system glfw")
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
