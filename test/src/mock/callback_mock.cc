#include "callback_mock.h"

TaskMock CbWrapper::cb_mock_;

void CbWrapper::TaskCb(void *param) {
  cb_mock_.TaskCb(param);
}

TaskMock &CbWrapper::cb_mock() {
  return CbWrapper::cb_mock_;
}
