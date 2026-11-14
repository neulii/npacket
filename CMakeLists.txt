cmake_minimum_required(VERSION 3.20)

project(npacket LANGUAGES C)

find_package(Curses REQUIRED)

set(SOURCES
    src/npacket.c
    src/parsingPrinting.c
)

find_library(PCAP_LIBRARY
        NAMES pcap
)
add_executable(${PROJECT_NAME} ${SOURCES})




add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND /usr/bin/sudo -n /usr/sbin/setcap
            cap_net_raw,cap_net_admin=eip
            "$<TARGET_FILE:${PROJECT_NAME}>"
    COMMENT "Setting capabilities for packet capture"
    VERBATIM
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -pedantic
    -Werror=implicit-function-declaration
)


target_link_libraries(${PROJECT_NAME} PRIVATE ${CURSES_LIBRARIES} ${PCAP_LIBRARY})
