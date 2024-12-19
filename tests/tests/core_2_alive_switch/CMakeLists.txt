cmake_minimum_required(VERSION 3.16)
project(ControlUnitLogicOperator VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find the libgpiod package
find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED libgpiod)

# Gather source files recursively
file(GLOB_RECURSE PROJECT_SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.c"
    "${CMAKE_SOURCE_DIR}/linux_board/*.c"
    "${CMAKE_SOURCE_DIR}/score_lib/*.c"
    "${CMAKE_SOURCE_DIR}/lib/board_dbc/can[0-9].c"
    "${CMAKE_SOURCE_DIR}/lib/DPS/dps_*.c"
    "${CMAKE_SOURCE_DIR}/lib/DPS/lib/c_vector/c_vector.c"
)

# Add executable
add_executable(main
    ./main.c
    ${PROJECT_SOURCES}
)

# Include libgpiod headers
target_include_directories(main PRIVATE ${GPIOD_INCLUDE_DIRS})

# Link libgpiod library
target_link_libraries(main PRIVATE ${GPIOD_LIBRARIES})

# Set custom build profiles
set(DEBUG_FLAGS "-O0 -fsanitize=address -g")
set(RELEASE_FLAGS "-O2")

# Add custom build configurations
add_custom_target(build_debug
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan "Building Debug Version"
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug -Bdebug -H${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} --build debug -- -j$(nproc)
)

add_custom_target(build_release
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan "Building Release Version"
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release -Brelease -H${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} --build release -- -j$(nproc)
)
