add_subdirectory(Phi)

# ImGui add_library("ImGui" STATIC "imgui/imgui.cpp" "imgui/imgui_demo.cpp"
# "imgui/imgui_draw.cpp" "imgui/imgui_widgets.cpp")

# target_include_directories("ImGui" PUBLIC "extlib/imgui")

# OpenGL set(OpenGL_GL_PREFERENCE "GLVND")

# find_package(OpenGL REQUIRED)

add_subdirectory(magic_enum)
