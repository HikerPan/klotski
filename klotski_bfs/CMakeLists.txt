cmake_minimum_required(VERSION 3.5)

project(klotski_bfs LANGUAGES C)

add_executable(klotski_bfs main.c klotski_position.c)

install(TARGETS klotski_bfs
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
