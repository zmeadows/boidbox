cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(boidbox LANGUAGES C)

set(CMAKE_C_COMPILER "/usr/bin/clang")

set(CMAKE_C_FLAGS "-std=c2x -g -O3 -Wall -Wextra -Weverything -Werror \
                  -Wno-declaration-after-statement \
                  -Wno-padded \
                  ")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(_XOPEN_SOURCE)

include_directories("/home/zmeadows/rpi-rgb-led-matrix/include")
link_directories("/home/zmeadows/rpi-rgb-led-matrix/lib")

add_executable(boidbox
    main.c
    boid.h
    boid.c
    led_panel.h
    led_panel.c
    streak.h
    streak.c
    utils.h
    utils.c
    test.h
    test.c
    )

target_link_libraries(boidbox librgbmatrix.a stdc++ rt m pthread)
