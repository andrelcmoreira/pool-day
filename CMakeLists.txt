cmake_minimum_required(VERSION 3.18)

project(pool-day)

option(BUILD_SAMPLES "build the sample applications" OFF)
option(BUILD_UNIT_TESTS "build the unit tests of the library" OFF)
option(BUILD_DOCUMENTATION "build the documentation of the library" OFF)
option(BUILD_COVERAGE "build the coverage data" OFF)
option(ENABLE_LOGGING "enable library logs" OFF)

set(PROJECT_NAME "pool-day")
set(SOURCES src/task.c src/queue.c src/pool_day.c)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wextra -Wpedantic")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O3 -s")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake")

include(doc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

if(BUILD_SAMPLES)
  add_subdirectory(samples)
endif(BUILD_SAMPLES)

if(BUILD_COVERAGE)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
endif(BUILD_COVERAGE)

if(BUILD_UNIT_TESTS)
  add_definitions(-DUNIT_TESTS)
  add_subdirectory(test)
  add_subdirectory(test/src/libc)
endif(BUILD_UNIT_TESTS)

if(BUILD_DOCUMENTATION)
  enable_doc()
endif(BUILD_DOCUMENTATION)

if(ENABLE_LOGGING)
  add_definitions(-DLIB_LOGGING)
  list(APPEND SOURCES "src/logger.c")
endif(ENABLE_LOGGING)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} pthread)

install(TARGETS ${PROJECT_NAME} DESTINATION /usr/lib)
install(FILES include/pool_day.h DESTINATION /usr/include/pool_day)
install(FILES include/internal/errors.h DESTINATION /usr/include/pool_day/internal)
install(FILES include/internal/task.h DESTINATION /usr/include/pool_day/internal)
