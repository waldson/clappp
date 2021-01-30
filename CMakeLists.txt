cmake_minimum_required(VERSION 3.6)
project(clap++)

include_directories(include/)
set(CMAKE_CXX_STANDARD 11)

add_executable(example src/example.cpp)
