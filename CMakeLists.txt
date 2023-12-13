cmake_minimum_required(VERSION 3.10)
project(infix_to_postfix)

set(CMAKE_CXX_STANDARD 14)

add_executable(postfix
        main.cpp
        expression.cpp
        expression.h
        )
