#ifndef CALLBACK_MOCK_H_
#define CALLBACK_MOCK_H_

#include <gmock/gmock.h>

class TaskMock {
 public:
  MOCK_METHOD(void *, TaskCb, (void *));
  MOCK_METHOD(void, OnTaskStartCb, (uint32_t, void *));
  MOCK_METHOD(void, OnTaskEndCb, (uint32_t, void *, void *));
};

class CbWrapper {
 public:
  CbWrapper();
  ~CbWrapper();

  static void *TaskCb(void *param);
  static void OnTaskStartCb(uint32_t tid, void *param);
  static void OnTaskEndCb(uint32_t tid, void *param, void *ret_val);
  static TaskMock &mock();

 private:
  static TaskMock *mock_;
};

#endif  // CALLBACK_MOCK_H_
