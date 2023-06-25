cmake_minimum_required(VERSION 3.18)
project(dumper C)

set(CMAKE_C_STANDARD 17)

add_executable(dumper main.c serial_linux.c)
