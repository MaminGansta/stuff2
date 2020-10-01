cmake_minimum_required(VERSION 3.12)

project(RungeKutta VERSION 0.0.0)

set(SOURCES
    main.cpp
    runge_kutta/runge_kutta.h
    runge_kutta/runge_kutta.cpp
 )

 add_executable(RungeKutta ${SOURCES})

set_target_properties(
    RungeKutta PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
