cmake_minimum_required(VERSION 2.6)

project(9ninecc)

add_executable(9ninecc codegen.c main.c map.c misc.c parse.c test.c token.c type.c vector.c)
