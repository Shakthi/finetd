cmake_minimum_required(VERSION 2.8.9...3.23.2)
project(finetd)



file(GLOB SOURCES "src/*.c")

add_executable(finetd ${SOURCES})

install(TARGETS finetd DESTINATION /usr/local/bin)
