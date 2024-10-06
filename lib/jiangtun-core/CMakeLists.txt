cmake_minimum_required(VERSION 3.10)

set(CMAKE_VERBOSE_MAKEFILE ON)

project(jiangtun-core VERSION 0.1.0)
set(CMAKE_C_STANDARD 90)

file(GLOB_RECURSE SRC_FILES src/*.c)
add_library(jiangtun-core STATIC ${SRC_FILES})
target_include_directories(jiangtun-core PUBLIC include)
target_compile_options(jiangtun-core PRIVATE -Wall -Wextra -Wpedantic -Werror)

enable_testing()

add_executable(jiangtun-core_test_command test/command.c)
target_link_libraries(jiangtun-core_test_command jiangtun-core)
target_compile_options(jiangtun-core_test_command PRIVATE -Wall -Wextra -Wpedantic -Werror)
add_test(NAME jiangtun-core_test_command COMMAND jiangtun-core_test_command)

add_executable(jiangtun-core_test_arduino test/arduino.c)
target_link_libraries(jiangtun-core_test_arduino jiangtun-core)
target_compile_options(jiangtun-core_test_arduino PRIVATE -Wall -Wextra -Wpedantic -Werror)
add_test(NAME jiangtun-core_test_arduino COMMAND jiangtun-core_test_arduino)
