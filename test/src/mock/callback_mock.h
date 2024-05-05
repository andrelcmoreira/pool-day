#ifndef CALLBACK_MOCK_H_
#define CALLBACK_MOCK_H_

#include <gmock/gmock.h>

class TaskMock {
 public:
  MOCK_METHOD(void *, TaskCb, (void *));
};

class CbWrapper {
 public:
  CbWrapper();
  ~CbWrapper();

  static void *TaskCb(void *param);
  static TaskMock &mock();

 private:
  static TaskMock *mock_;
};

#endif  // CALLBACK_MOCK_H_
