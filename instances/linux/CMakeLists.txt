cmake_minimum_required(VERSION 3.16)
project(ControlUnitLogicOperator VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_SYSTEM_PROCESSOR x86)

# Force 32-bit compilation
set(CMAKE_C_FLAGS "-m32 ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "-m32 ${CMAKE_CXX_FLAGS}")
set(CMAKE_LINKER_FLAGS "-m32 ${CMAKE_LINKER_FLAGS}")

# Include the compiled libgpiod headers
include_directories(${CMAKE_SOURCE_DIR}/libgpiod/include)

# Link to the 32-bit libgpiod
link_directories(${CMAKE_SOURCE_DIR}/libgpiod/)

# Gather source files recursively
file(GLOB_RECURSE PROJECT_SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.c"
    "${CMAKE_SOURCE_DIR}/linux_board/*.c"
    "${CMAKE_SOURCE_DIR}/main.c"
)

file(GLOB_RECURSE PROJECT_LIBS
    "${CMAKE_SOURCE_DIR}/lib/board_dbc/dbc/out_lib/*c"
    "${CMAKE_SOURCE_DIR}/lib/DPS/dps_*.c"
    "${CMAKE_SOURCE_DIR}/lib/DPS/lib/c_vector/c_vector.c"
    "${CMAKE_SOURCE_DIR}/lib/rigen_fun_simulink22_ert_rtw/*.c"
    "${CMAKE_SOURCE_DIR}/lib/TV/*.c"
)

# Add executable
add_executable(main
    ${PROJECT_SOURCES}
)

# Create a library for the `lib` sources
add_library(lib_sources STATIC
    ${PROJECT_LIBS}
)

# Include directories for the library
target_include_directories(lib_sources PRIVATE
    "${CMAKE_SOURCE_DIR}/lib"
)

target_compile_options(lib_sources PRIVATE
    $<$<CONFIG:Debug>:-O0 -m32 -fsanitize=address,undefined -g>
    $<$<CONFIG:Release>:-O2 -m32>
)

# Include libgpiod headers
target_include_directories(main PRIVATE ${GPIOD_INCLUDE_DIRS})

# Link libgpiod library
target_link_libraries(main PRIVATE gpiod)

# Link the math library
target_link_libraries(main PRIVATE m)

# Link the `lib_sources` library to the main executable
target_link_libraries(main PRIVATE lib_sources)

# Set compile options based on build type
target_compile_options(main PRIVATE
    $<$<CONFIG:Debug>:-DDEBUG -Wall -Wextra -Werror -O0 -m32 -pedantic -fsanitize=address,undefined -g>
    $<$<CONFIG:Release>:-DDEBUG -Wall -Wextra -Werror -O2 -m32 -pedantic>
)

target_link_options(main PRIVATE
    $<$<CONFIG:Debug>:-m32 -Wall -Wextra -Werror -O0 -m32 -pedantic -fsanitize=address,undefined -g>
    $<$<CONFIG:Release>:-m32 -Wall -Wextra -Werror -O2 -m32>
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
