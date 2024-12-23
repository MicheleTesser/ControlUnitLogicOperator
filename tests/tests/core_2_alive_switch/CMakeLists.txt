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
    "${CMAKE_SOURCE_DIR}/lib/rigen_fun_simulink22_ert_rtw/*.c"
    "${CMAKE_SOURCE_DIR}/main.c"
)

# Add executable
add_executable(main
    ${PROJECT_SOURCES}
)

# Include libgpiod headers
target_include_directories(main PRIVATE ${GPIOD_INCLUDE_DIRS})

# Link libgpiod library
target_link_libraries(main PRIVATE ${GPIOD_LIBRARIES})

# Link the math library
target_link_libraries(main PRIVATE m)

# Set compile options based on build type
target_compile_options(main PRIVATE
    $<$<CONFIG:Debug>:-Wall -Wextra -O0 -fsanitize=address -g>
    $<$<CONFIG:Release>:-Wall -Wextra -O2>
)

target_link_options(main PRIVATE
    $<$<CONFIG:Debug>:-Wall -Wextra -O0 -fsanitize=address -g>
    $<$<CONFIG:Release>:-Wall -Wextra -O2>
)

# Optional: Create custom targets for convenience
add_custom_target(build_debug
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug -Bdebug -S${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} --build debug
)

add_custom_target(build_release
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release -Brelease -S${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} --build release
)

