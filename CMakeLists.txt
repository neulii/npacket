cmake_minimum_required(VERSION 3.20)

project (npacket C)

find_package(Curses REQUIRED)

set (SOURCES
    npacket.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -pedantic
    -Werror=implicit-function-declaration
    #-g
)

target_link_libraries(${PROJECT_NAME} PRIVATE  ${CURSES_LIBRARIES})
