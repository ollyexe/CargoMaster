cmake_minimum_required(VERSION 3.22.1)
project(CargoMasters C)

set(CMAKE_C_STANDARD 90)

add_executable(CargoMasters src/main.c src/lib/*.c)
