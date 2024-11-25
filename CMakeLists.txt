cmake_minimum_required(VERSION 3.27)
project(src C)

set(CMAKE_C_STANDARD 11)

add_executable(src
        game_snake.c
        snake.h
        game_snake.c
)
