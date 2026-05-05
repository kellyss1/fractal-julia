cmake_minimum_required(VERSION 3.21)

project(fractal-julia)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")

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

if(MINGW)
    target_link_options(fractal-julia PRIVATE -static-libgcc -static-libstdc++)
endif()

target_link_libraries(fractal-julia PRIVATE 
fmt::fmt
SFML::Network
SFML::System
SFML::Graphics
SFML::Window)

if(WIN32)
    set(PROJECT_RUNTIME_DLLS "")

    if(DEFINED VCPKG_INSTALLED_DIR AND DEFINED VCPKG_TARGET_TRIPLET)
        file(GLOB VCPKG_DEBUG_DLLS
            "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/bin/*.dll")
        list(APPEND PROJECT_RUNTIME_DLLS ${VCPKG_DEBUG_DLLS})
    endif()

    if(MINGW)
        get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
        list(APPEND PROJECT_RUNTIME_DLLS
            "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
            "${MINGW_BIN_DIR}/libstdc++-6.dll"
            "${MINGW_BIN_DIR}/libwinpthread-1.dll")
    endif()

    add_custom_command(TARGET fractal-julia POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:fractal-julia>
            ${PROJECT_RUNTIME_DLLS}
            $<TARGET_FILE_DIR:fractal-julia>
        COMMAND_EXPAND_LISTS
    )
endif()