cmake_minimum_required(VERSION 3.20)

project(YSTL)

add_executable(main ./main.cpp)
target_include_directories(main PUBLIC include)
target_compile_features(main PUBLIC cxx_std_17)