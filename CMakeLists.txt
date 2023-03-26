cmake_minimum_required(VERSION 3.18)

project(pool-day)

option(BUILD_TEST "build the test application" OFF)
option(BUILD_DOCUMENTATION "build the documentation of the library" OFF)

set(PROJECT_NAME "pool-day")
set(SOURCES src/queue.c src/pool_day.c)

set(CMAKE_C_CFLAGS "${CMAKE_C_CFLAGS} -Wall -Werror -Wextra")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(BUILD_TEST)
  add_subdirectory(test)
endif(BUILD_TEST)

if(BUILD_DOCUMENTATION)
  add_subdirectory(doc)
endif(BUILD_DOCUMENTATION)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} pthread)
