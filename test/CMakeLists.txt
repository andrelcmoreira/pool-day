cmake_minimum_required(VERSION 3.18)

project(pool-day-tests)

#find_package(gtest)

set(PROJECT_NAME "pool-day-tests")

set(TEST_MAIN_SOURCE ${CMAKE_CURRENT_LIST_DIR}/src/main.cc)
set(SOURCES_DIR "${CMAKE_CURRENT_LIST_DIR}")

include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/../../include
  ${CMAKE_CURRENT_LIST_DIR}
)

link_directories(${CMAKE_SOURCE_DIR}/../../build)

set(
  SOURCES
  ${SOURCES_DIR}/src/queue_test.cc
  ${SOURCES_DIR}/src/pool_day_test.cc
  ${TEST_MAIN_SOURCE}
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(
  ${PROJECT_NAME}
  gtest
  gmock
  gtest_main
  pool-day
)
