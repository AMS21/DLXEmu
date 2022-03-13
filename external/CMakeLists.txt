# Phi
add_subdirectory(Phi)

include(Phi/scripts/cmake/Environment.cmake)

# Catch2
add_subdirectory(Catch2)

# glad
add_subdirectory(glad)

# glfw
if (NOT PHI_PLATFORM_EMSCRIPTEN)
    add_subdirectory(glfw)
endif()

# ImGui
add_library(
  "ImGui" STATIC
  "imgui/imgui.cpp" "imgui/imgui_demo.cpp" "imgui/imgui_draw.cpp" "imgui/imgui_tables.cpp"
  "imgui/imgui_widgets.cpp" "imgui/backends/imgui_impl_glfw.cpp" "imgui/backends/imgui_impl_opengl3.cpp")

target_compile_definitions("ImGui" PUBLIC "IMGUI_IMPL_OPENGL_LOADER_GLAD" IMGUI_USER_CONFIG="${CMAKE_CURRENT_SOURCE_DIR}/../DLXEmu/include/DLXEmu/ImGuiConfig.hpp")

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
