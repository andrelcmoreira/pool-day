#ifndef CALLBACK_MOCK_H_
#define CALLBACK_MOCK_H_

#include <gmock/gmock.h>

class TaskMock {
 public:
  MOCK_METHOD(void *, TaskCb, (void *));
  MOCK_METHOD(void, OnTaskStartCb, ());
  MOCK_METHOD(void, OnTaskEndCb, (void *));
};

class CbWrapper {
 public:
  CbWrapper();
  ~CbWrapper();

  static void *TaskCb(void *param);
  static void OnTaskStartCb();
  static void OnTaskEndCb(void *param);
  static TaskMock &mock();

 private:
  static TaskMock *mock_;
};

#endif  // CALLBACK_MOCK_H_
