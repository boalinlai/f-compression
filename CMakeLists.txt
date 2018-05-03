cmake_minimum_required(VERSION 3.9)
project(f_compression)

set(CMAKE_CXX_STANDARD 11)

find_package (Threads)

add_executable("f-compression" main.cpp Huffman.cpp )
target_link_libraries ("f-compression" ${CMAKE_THREAD_LIBS_INIT})
