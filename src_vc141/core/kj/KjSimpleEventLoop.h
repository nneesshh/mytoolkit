#pragma once
//------------------------------------------------------------------------------
/**
@class CKjSimpleEventLoop

(C) 2016 n.lee
*/
#include <algorithm>

#include "../../UsingCapnp.h"

class KjSimpleEventLoop : public kj::Refcounted, public kj::TaskSet::ErrorHandler {
public:
	KjSimpleEventLoop();
	~KjSimpleEventLoop() noexcept(false) = default;

	kj::WaitScope&				GetWaitScope() {
		return _waitScope;
	}

	kj::Timer&					GetTimer();

	kj::Promise<void>			AfterDelay(kj::Duration delay) {
		return GetTimer().afterDelay(delay);
	}

	template <typename T>
	kj::Promise<T>				TimeoutAfter(kj::Duration delay, kj::Promise<T>&& p) {
		return GetTimer().timeoutAfter(delay, kj::mv(p));
	}

	template <typename Func>
	kj::PromiseForResult<Func, void> EvalForResult(Func&& func) {
		return kj::evalLater(kj::mv(func));
	}

	template <typename T>
	kj::ForkedPromise<T>		ForkPromise(kj::Promise<T>&& promise) {
		return promise.fork();
	}

	kj::Own<kj::TaskSet>		CreateTaskSet() {
		return kj::heap<kj::TaskSet>(*this);
	}

	kj::Own<kj::TaskSet>		CreateTaskSet(kj::TaskSet::ErrorHandler& errorHandler) {
		return kj::heap<kj::TaskSet>(errorHandler);
	}

private:
	void taskFailed(kj::Exception&& exception) override {
		fprintf(stderr, "[KjSimpleEventLoop::taskFailed()] desc(%s) -- pause!!!\n", exception.getDescription().cStr());
		system("pause");
		kj::throwFatalException(kj::mv(exception));
	}

	kj::EventLoop _eventLoop;
	kj::WaitScope _waitScope;
};

/*EOF*/