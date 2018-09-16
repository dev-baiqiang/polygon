# Copyright (c) 2018 BaiQiang. All rights reserved.

cmake_minimum_required(VERSION 3.7)
project(polygon)

set(CMAKE_CXX_STANDARD 11)
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party")

set(SOURCE_FILES
        main.cpp
        shader.c
        screenshot.c path.cpp)

add_executable(polygon ${SOURCE_FILES})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})


# glfw
set(GLFW_DIR "${LIB_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL ON CACHE INTERNAL "Generate installation target")
add_subdirectory("${GLFW_DIR}")
target_link_libraries(${PROJECT_NAME} "glfw")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# glad
set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")

# glm
set(GLM_DIR "${LIB_DIR}/glm")
add_subdirectory(third_party/glm)
target_include_directories(${PROJECT_NAME} PRIVATE "${GLM_DIR}")

# stb
set(STB_DIR "${LIB_DIR}/stb")
target_include_directories(${PROJECT_NAME} PRIVATE "${STB_DIR}")

target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} "legacy_stdio_definitions.lib")


add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
        )
