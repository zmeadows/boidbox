cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(boidbox LANGUAGES C)

set(CMAKE_C_FLAGS "-Wall -Wextra -O3 -g -Wno-unused-parameter")

include_directories("/home/zmeadows/rpi-rgb-led-matrix/include")
link_directories("/home/zmeadows/rpi-rgb-led-matrix/lib")

add_executable(boidbox
    main.c
    boid.h
    boid.c
    )

target_link_libraries(boidbox librgbmatrix.a stdc++ rt m pthread)
