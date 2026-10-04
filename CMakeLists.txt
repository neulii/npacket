cmake_minimum_required(VERSION 3.20)

project (npacket C)

find_package(Curses REQUIRED)

set (SOURCES
    src/npacket.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -pedantic
    -Werror=implicit-function-declaration
    -g
)
add_custom_command(TARGET npacket POST_BUILD
    COMMAND sudo setcap cap_net_raw,cap_net_admin=eip $<TARGET_FILE:npacket>
    COMMENT "Setting capabilities for packet capture"
)

target_link_libraries(${PROJECT_NAME} PRIVATE  ${CURSES_LIBRARIES})
