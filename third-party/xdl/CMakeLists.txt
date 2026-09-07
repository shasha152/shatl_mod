cmake_minimum_required(VERSION 3.22.1)
project(xdl)

file(GLOB XDL_SRC *.c)
add_library(xdl STATIC ${XDL_SRC})
#target_compile_options(xdl PUBLIC -Weverything -Werror)
target_include_directories(xdl PUBLIC include .)
#target_link_libraries(xdl log)

if((${ANDROID_ABI} STREQUAL "arm64-v8a") OR (${ANDROID_ABI} STREQUAL "x86_64"))
    target_link_options(xdl PUBLIC "-Wl,-z,max-page-size=16384")
endif()
