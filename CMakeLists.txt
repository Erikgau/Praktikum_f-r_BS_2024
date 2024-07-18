cmake_minimum_required(VERSION 3.10)
project(Meilenstein3_1 C)

set(CMAKE_C_STANDARD 99)

add_executable(Meilenstein3_1 main.c main.h keyValStore.c keyValStore.h sub.c sub.h begandsub.c begandsub.h)
