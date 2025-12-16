#include "callback_mock.h"

TaskMock *CbWrapper::mock_;

CbWrapper::CbWrapper() {
  CbWrapper::mock_ = new TaskMock;
}

CbWrapper::~CbWrapper() {
  // FIXME: for some reason, the destructor of TaskMock is not being called when
  // we run the tests within the CI container
  delete CbWrapper::mock_;
}

void *CbWrapper::TaskCb(void *param) {
  return mock_->TaskCb(param);
}

void CbWrapper::OnTaskStartCb(uint32_t tid, const void *param) {
  mock_->OnTaskStartCb(tid, param);
}

void CbWrapper::OnTaskEndCb(uint32_t tid, const void *param, void *ret_val) {
  mock_->OnTaskEndCb(tid, param, ret_val);
}

TaskMock &CbWrapper::mock() {
  return *CbWrapper::mock_;
}
