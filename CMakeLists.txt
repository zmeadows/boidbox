cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(boidbox LANGUAGES C)

set(CMAKE_C_FLAGS "-Wall -Wextra -O3")

add_executable(boidbox main.c boid.h boid.c)
