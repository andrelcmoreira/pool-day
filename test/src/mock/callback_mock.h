#ifndef TASK_MOCK_H_
#define TASK_MOCK_H_

#include <gmock/gmock.h>

class TaskMock {
 public:
   MOCK_METHOD(void, TaskCb, (void *));
};

class CbWrapper {
  public:
   static void TaskCb(void *param);
   static TaskMock &mock();

   static TaskMock mock_;
};

#endif  // TASK_MOCK_H_
