cmake_minimum_required(VERSION 3.24)
project(TMchat C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(TMchat
        client.c
        server.c)
