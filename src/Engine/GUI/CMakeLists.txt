cmake_minimum_required(VERSION 3.10)
project(GUI CXX)
###################################################
add_library(GUI)

target_link_libraries(${PROJECT_NAME} PRIVATE Math)

target_include_directories(
${PROJECT_NAME}
	PRIVATE	"${CMAKE_BINARY_DIR}"
	PRIVATE	"${CMAKE_SOURCE_DIR}/"
)

target_sources(${PROJECT_NAME}
  PRIVATE ${ENGINE_DIR}/GUI/GUI.cpp
  PRIVATE ${ENGINE_DIR}/GUI/ImGuiManager.cpp
  PRIVATE ${ENGINE_DIR}/GUI/imgui_demo.cpp
  PRIVATE ${ENGINE_DIR}/GUI/imgui_impl_opengl3.cpp
)

add_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui Renderer)

#target_compile_definitions(${PROJECT_NAME} PUBLIC -DIMGUI_IMPL_OPENGL_LOADER_CUSTOM <BlackBox/Renderer/OpenGL/Core.hpp>)
add_private_headers()
