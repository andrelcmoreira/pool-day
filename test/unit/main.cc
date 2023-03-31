#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::InitGoogleMock;

int main(int argc, char **argv) {
  InitGoogleMock(&argc, argv);

  return RUN_ALL_TESTS();
}
