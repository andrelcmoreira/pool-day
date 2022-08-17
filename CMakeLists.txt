cmake_minimum_required(VERSION 3.18)

project(pool_day)

option(BUILD_TEST "build the test application" OFF)

set(PROJECT_NAME "pool_day")
set(SOURCES src/queue.c src/pool_day)

set(CMAKE_C_CFLAGS "${CMAKE_C_CFLAGS} -Wall -Werror -Wextra")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(BUILD_TEST)
  add_subdirectory(test)
endif(BUILD_TEST)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} pthread)
