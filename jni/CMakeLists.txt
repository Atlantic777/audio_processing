cmake_minimum_required(VERSION 2.8)

add_library(readfile readFile.h readFile.c)
add_library(processing processing.h processing.c)
target_link_libraries(processing readfile -lm)
add_library(wavheader WAVheader.h WAVheader.c)

add_executable(shelving main.c const.h)
target_link_libraries(shelving processing wavheader)
