cmake_minimum_required(VERSION 3.18)

project(pool-day-sample-4)

set(PROJECT_NAME "pool-day-sample-4")
set(SOURCES main.c)

set(CMAKE_C_FLAGS "-Wall -Werror -Wextra")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
link_directories(${CMAKE_CURRENT_LIST_DIR}/../../../build)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} pool-day)
