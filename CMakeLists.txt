cmake_minimum_required(VERSION 3.29)
project(vole_machine)

set(CMAKE_CXX_STANDARD 20)

add_executable(vole_machine main.cpp instructions.txt cpu_simulator.cpp cpu_simulator.h
)
