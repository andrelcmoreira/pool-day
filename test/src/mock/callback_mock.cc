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

void CbWrapper::OnTaskStartCb(uint32_t tid) {
  mock_->OnTaskStartCb(tid);
}

void CbWrapper::OnTaskEndCb(uint32_t tid, void *param) {
  mock_->OnTaskEndCb(tid, param);
}

TaskMock &CbWrapper::mock() {
  return *CbWrapper::mock_;
}
