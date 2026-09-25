cmake_minimum_required(VERSION 3.10)

project(shatl VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(protobuf_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(protobuf_BUILD_CONFORMANCE OFF CACHE BOOL "" FORCE)
set(protobuf_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(protobuf_BUILD_PROTOC_BINARIES ON CACHE BOOL "" FORCE)
set(protobuf_BUILD_LIBPROTOC OFF CACHE BOOL "" FORCE)
set(protobuf_BUILD_LIBUPB OFF CACHE BOOL "" FORCE)

add_subdirectory(third-party/imgui)
add_subdirectory(third-party/xdl)
add_subdirectory(third-party/protobuf EXCLUDE_FROM_ALL)

set(PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/proto)
set(PROTO_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)

file(MAKE_DIRECTORY ${PROTO_OUT_DIR})

set(PROTO_FILES
  ${PROTO_DIR}/player.proto
  ${PROTO_DIR}/packet.proto
  ${PROTO_DIR}/setting.proto
  ${PROTO_DIR}/item.proto
  ${PROTO_DIR}/multi_player.proto
)

set(PROTO_SRCS)
set(PROTO_HDRS)

foreach(PROTO_FILE ${PROTO_FILES})
  get_filename_component(PROTO_NAME
    ${PROTO_FILE}
    NAME_WE
  )

  list(APPEND PROTO_SRCS
    ${PROTO_OUT_DIR}/${PROTO_NAME}.pb.cc
  )

  list(APPEND PROTO_HDRS
    ${PROTO_OUT_DIR}/${PROTO_NAME}.pb.h
  )
endforeach()

add_custom_command(
  OUTPUT
  ${PROTO_SRCS}
  ${PROTO_HDRS}

  COMMAND
  ${CMAKE_CURRENT_SOURCE_DIR}/tools/protoc
  --proto_path=${PROTO_DIR}
  --cpp_out=${PROTO_OUT_DIR}
  ${PROTO_FILES}

  DEPENDS
  ${PROTO_FILES}
  protobuf::protoc

  VERBATIM
)

add_custom_target(generate_proto
  DEPENDS
  ${PROTO_SRCS}
  ${PROTO_HDRS}
)

file(GLOB SOURCES src/*.cpp src/register/*.cpp src/server/*.cpp)
add_library(shatl SHARED main/lib.cpp ${SOURCES} ${PROTO_SRCS})

target_link_libraries(shatl PRIVATE
  log c++_static android
  ${CMAKE_SOURCE_DIR}/third-party/dobby/arm64-v8a/libdobby.a
  imgui EGL GLESv3
  xdl protobuf::libprotobuf-lite
)

target_include_directories(shatl PRIVATE
  third-party/Dobby
  third-party/json/single_include
  ${PROTO_OUT_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/third-party/protobuf/third_party/utf8_range
)
target_include_directories(shatl PUBLIC ${CMAKE_SOURCE_DIR}/include)
add_compile_options(
  -Wall
  -Wextra)