//------------------------------------------------------------------------------
//  KjSimpleEventLoop.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "KjSimpleEventLoop.h"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__,__LINE__)
#endif
#endif

#ifdef _WIN32
static kj::Win32IocpEventPort s_eventPort;
#else
static kj::UnixEventPort s_eventPort;
#endif

KjSimpleEventLoop::KjSimpleEventLoop() 
	: _eventLoop(s_eventPort), _waitScope(_eventLoop) {

}

kj::Timer&
KjSimpleEventLoop::GetTimer() {
	return s_eventPort.getTimer();
}

// test
class KjDummyEventPort : public kj::EventPort {
public:
	bool runnable = false;
	int callCount = 0;

	bool wait() override {
		KJ_FAIL_ASSERT("Nothing to wait for.");
	}

	bool poll() override { return false; }
	void setRunnable(bool runnable) override {
		this->runnable = runnable;
		++callCount;
	}
};

/* -- EOF -- */