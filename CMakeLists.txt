cmake_minimum_required(VERSION 3.0)
project(mfgd)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")

# GLFW - Use FetchContent para baixar o GLFW
include(FetchContent)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.4  # Defina a versão específica que deseja baixar (verifique a compatibilidade com o macOS M1)
)
FetchContent_MakeAvailable(glfw)

# Adicione o diretório de include do GLFW e GL3W ao include_directories
include_directories(
    ${glfw_SOURCE_DIR}/include
    ${glfw_SOURCE_DIR}/deps
    src
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Encontre as bibliotecas necessárias para o GLFW
find_package(OpenGL REQUIRED)
if(APPLE)
  find_package(Freetype REQUIRED)
endif(APPLE)

add_subdirectory(src)
