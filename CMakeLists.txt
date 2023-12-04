cmake_minimum_required(VERSION 3.18)

project(pool-day)

option(BUILD_SAMPLE "build the sample application" OFF)
option(BUILD_UNIT_TESTS "build the unit tests of the library" OFF)
option(BUILD_DOCUMENTATION "build the documentation of the library" OFF)
option(BUILD_COVERAGE "build the coverage data" OFF)
option(ENABLE_LOGGING "enable library logs" OFF)

set(PROJECT_NAME "pool-day")
set(SOURCES src/queue.c src/pool_day.c)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wextra -Wpedantic")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake")

include(doc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

if(BUILD_SAMPLE)
  add_subdirectory(sample)
endif(BUILD_SAMPLE)

if(BUILD_COVERAGE)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
endif(BUILD_COVERAGE)

if(BUILD_UNIT_TESTS)
  add_subdirectory(test)
endif(BUILD_UNIT_TESTS)

if(BUILD_DOCUMENTATION)
  enable_doc()
endif(BUILD_DOCUMENTATION)

if(ENABLE_LOGGING)
  add_definitions(-DLIB_LOGGING)
endif(ENABLE_LOGGING)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} pthread)
