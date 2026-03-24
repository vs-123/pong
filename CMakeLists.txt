cmake_minimum_required(VERSION 3.10)

project(pong)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE you_werent_supposed_to_be_here)

find_package(raylib)

include(FetchContent)
FetchContent_Declare(
  ystar
  GIT_REPOSITORY https://github.com/vs-123/ystar.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(ystar)

set(SRC
   src/main.c
)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE raylib ystar)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
