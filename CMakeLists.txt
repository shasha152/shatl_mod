cmake_minimum_required(VERSION 3.10)

project(shatl VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(third-party/imgui)
add_subdirectory(third-party/xdl)

file(GLOB SOURCES "src/*.cpp")
add_library(shatl SHARED main/lib.cpp ${SOURCES})

target_link_libraries(shatl PRIVATE
  log c++_static android
  ${CMAKE_SOURCE_DIR}/third-party/dobby/arm64-v8a/libdobby.a
  imgui EGL GLESv3
  xdl
)

target_include_directories(shatl PRIVATE third-party/Dobby include third-party/json/single_include)
add_compile_options(
  -Wall
  -Wextra)