cmake_minimum_required(VERSION 3.23.0)

project(linkedlist VERSION 1.0)

add_library(${PROJECT_NAME} STATIC
    linked_list.c)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
