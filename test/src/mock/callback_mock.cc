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

void CbWrapper::OnTaskStartCb() {
  mock_->OnTaskStartCb();
}

void CbWrapper::OnTaskEndCb(void *param) {
  mock_->OnTaskEndCb(param);
}

TaskMock &CbWrapper::mock() {
  return *CbWrapper::mock_;
}
