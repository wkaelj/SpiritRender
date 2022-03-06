cmake_minimum_required(VERSION 3.10)

project(linkedlist VERSION 1.0)

add_library(linkedlist linked_list.c)

target_include_directories(linkedlist PUBLIC linke_list.h)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

