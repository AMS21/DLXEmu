cmake_minimum_required(VERSION 3.10.2)

project(DLXLib LANGUAGES CXX)

file(GLOB_RECURSE DLXLIB_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE DLXLIB_HEADERS CONFIGURE_DEPENDS "include/DLX/*.hpp")

add_library(${PROJECT_NAME} STATIC ${DLXLIB_SOURCES} ${DLXLIB_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC "include")
target_link_libraries(${PROJECT_NAME} PUBLIC Phi::Core fmt::fmt)
target_compile_definitions(${PROJECT_NAME} PUBLIC "$<$<CONFIG:RELWITHDBGINFO>:PHI_DEBUG>")
# We don't want a default logger
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)

if(${DLXEMU_COVERAGE_BUILD})
  target_compile_definitions(${PROJECT_NAME} PUBLIC "DLXEMU_COVERAGE_BUILD")
endif()
