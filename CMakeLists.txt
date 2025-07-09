cmake_minimum_required(VERSION 3.20)

#project(matrix_library VERSION 0.0.1.0 LANGUAGES CXX PROJECT_DESCRIPTION "A library that handles matrices")
project(matrix_library VERSION 0.0.1 LANGUAGES CXX)


add_library(matrix_library src/MathVector.cpp)

target_include_directories(matrix_library PRIVATE include)
target_include_directories(matrix_library PUBLIC include/matrix_library)