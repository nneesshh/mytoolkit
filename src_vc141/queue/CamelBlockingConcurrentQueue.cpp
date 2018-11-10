//------------------------------------------------------------------------------
//  CamelBlockingConcurrentQueue.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "CamelBlockingConcurrentQueue.h"

//------------------------------------------------------------------------------
/**

*/
CCamelBlockingConcurrentQueue::CCamelBlockingConcurrentQueue(int nProducerNum, int nConsumerNum)
	: _producerNum(nProducerNum)
	, _consumerNum(nConsumerNum)
	, _callbacks(nProducerNum * MyTraits::BLOCK_SIZE) {

}

//------------------------------------------------------------------------------
/**

*/
CCamelBlockingConcurrentQueue::~CCamelBlockingConcurrentQueue() {

}

//------------------------------------------------------------------------------
/**

*/
void
CCamelBlockingConcurrentQueue::Start(std::function<void()> initCb, std::function<void()> disposeCb) {
	_initCb = std::move(initCb);
	_disposeCb = std::move(disposeCb);

	// start thread
	{
		int i;
		for (i = 0; i < _consumerNum; ++i) {
			std::thread t([this]() {
				this->Run();
			});
			_threads.emplace_back(std::move(t));
		}
	}
}

//------------------------------------------------------------------------------
/**

*/
void
CCamelBlockingConcurrentQueue::Run() {
	// thread init
	{
		_initCb();
	}
	
	// thread work queue
	int nCount = 0;
	CallbackEntry workCb;
	while (!_done) {
		//
		// Get next work item.
		//
		bool found = _callbacks.wait_dequeue_timed(workCb, std::chrono::milliseconds(1500));
		if (found) {
			workCb();
		}

		//
		++nCount;
	}

	// thread dispose
	{
		_disposeCb();
	}
}

//------------------------------------------------------------------------------
/**

*/
void
CCamelBlockingConcurrentQueue::Finish() {
	//
	// Set done flag and notify.
	//
	_done = true;

	// join
	for (auto& t : _threads) {
		if (t.joinable()) {
			t.join();
		}
	}
}

//------------------------------------------------------------------------------
/**

*/
bool
CCamelBlockingConcurrentQueue::Add(std::function<void()>&& workCb) {
	if (_done) {
		// error
		fprintf(stderr, "[CCamelBlockingConcurrentQueue::Add()] can't enqueue, callback is dropped!!!");
		return false;
	}

	//
	// Add work item.
	//
	if (!_callbacks.enqueue(std::move(workCb))) {
		// error
		fprintf(stderr, "[CCamelBlockingConcurrentQueue::Add()] enqueue failed, callback is dropped!!!");
		return false;
	}
	return true;
}

/** -- EOF -- **/