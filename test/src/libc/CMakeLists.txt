cmake_minimum_required(VERSION 3.18)

project(preload)

set(PROJECT_NAME "preload")
set(SOURCES libc_preload.c)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME})
