cmake_minimum_required(VERSION 3.21)

project(fractal-julia)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -fopenmp")

find_package(fmt CONFIG REQUIRED)
find_package(SFML COMPONENTS 
System 
Network
Graphics 
Window 
CONFIG REQUIRED)

add_executable(
    fractal-julia 
    palette.cpp
    fractal_simd.cpp
    main.cpp
    fractal_serial.cpp)


target_link_libraries(fractal-julia PRIVATE 
fmt::fmt
SFML::Network
SFML::System
SFML::Graphics
SFML::Window)
