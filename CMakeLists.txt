cmake_minimum_required(VERSION 3.23)
project (Progetto C)

set(CMAKE_C_STANDARD 99)

add_executable(Progetto src/main.c src/include/config_parser.h src/lib/config_parser.c src/include/test.h src/lib/test.c)
