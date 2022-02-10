cmake_minimum_required(VERSION 3.14)
project(rubixCube)

set(CMAKE_CXX_STANDARD 14)

add_executable(rubixCube main.cpp shader.h stb_image.h data.h util.h record.h)

# glfw
add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)
target_link_libraries(rubixCube PRIVATE glfw)

# glew
add_definitions(-DGLEW_STATIC)
add_subdirectory(lib/glew EXCLUDE_FROM_ALL)
target_link_libraries(rubixCube PRIVATE libglew_static)

# glm
add_subdirectory(lib/glm EXCLUDE_FROM_ALL)
target_link_libraries(rubixCube PRIVATE glm)

# glad
# https://github.com/Dav1dde/glad/issues/186
add_subdirectory(lib/glad EXCLUDE_FROM_ALL)
target_link_libraries(rubixCube PRIVATE glad)
