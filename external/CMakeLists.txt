find_package(Threads REQUIRED)

# Phi
add_subdirectory(Phi)

include(Phi/scripts/cmake/Environment.cmake)

# fmt
add_subdirectory(fmt)

# Ensure exceptions are disable for fmtlib
target_compile_definitions(fmt PUBLIC FMT_EXCEPTIONS=0)

# glad
add_subdirectory(glad/cmake)

# Add target
glad_add_library(glad STATIC LANGUAGE C++ API gl:core=3.3)
add_library(glad::glad ALIAS glad)

set_target_properties(glad PROPERTIES LINKER_LANGUAGE "C")

# glfw
if(NOT PHI_PLATFORM_EMSCRIPTEN)
  set(GLFW_BUILD_EXAMPLES
      OFF
      CACHE INTERNAL "")
  set(GLFW_BUILD_TESTS
      OFF
      CACHE INTERNAL "")
  set(GLFW_BUILD_DOCS
      OFF
      CACHE INTERNAL "")
  set(GLFW_INSTALL
      OFF
      CACHE INTERNAL "")

  add_subdirectory(glfw)

  set_target_properties(glfw PROPERTIES LINKER_LANGUAGE "C")
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

target_link_libraries("ImGui" PUBLIC glad::glad glfw Phi::Core)

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

# magic_enum
add_subdirectory(magic_enum EXCLUDE_FROM_ALL)
