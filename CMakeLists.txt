cmake_minimum_required(VERSION 3.21)
project(KG3)

set(CMAKE_CXX_STANDARD 23)

add_executable(KG3 main.cpp program/shader.h program/shader.cpp imglib/stb_image.h imglib/stb_image.cpp camera/camera.cpp camera/camera.h coneobject/cone_approx.h pointlight/pointlight.cpp pointlight/pointlight.h skybox/skybox.cpp skybox/skybox.h skyboxes.h)

target_link_libraries(KG3 opengl32.lib)
target_link_libraries(KG3 glew32.lib)
target_link_libraries(KG3 glfw3.lib)

