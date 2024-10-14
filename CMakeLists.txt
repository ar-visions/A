cmake_minimum_required    (VERSION 3.8)
enable_language           (ASM)
project                   (A)
set                       (is_exe FALSE)
include                   (A.cmake)

target_link_libraries     (A ffi)

add_executable            (a-test src/a-test.c)
target_link_libraries     (a-test /home/kalen/src/A/silver-debug/libA.so)
target_include_directories(a-test PRIVATE . ./src)