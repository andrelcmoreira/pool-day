#include "callback_mock.h"

TaskMock CbWrapper::mock_;

void CbWrapper::TaskCb(void *param) {
  mock_.TaskCb(param);
}

TaskMock &CbWrapper::mock() {
  return CbWrapper::mock_;
}
