#pragma once
//------------------------------------------------------------------------------
/**
@class CCamelBlockingConcurrentQueue

(C) 2016 n.lee
*/
#include <vector>
#include <thread>

#include "concurrent/blockingconcurrentqueue.h"

//------------------------------------------------------------------------------
/**
@brief CCamelBlockingConcurrentQueue
*/
class CCamelBlockingConcurrentQueue {
public:
	CCamelBlockingConcurrentQueue(int nProducerNum, int nConsumerNum);
	~CCamelBlockingConcurrentQueue();

	using CallbackEntry = std::function<void()>;
	struct MyTraits : public moodycamel::ConcurrentQueueDefaultTraits {
		static const size_t BLOCK_SIZE = 256;
	};

	void Start(std::function<void()> initCb, std::function<void()> disposeCb);
	void Run();
	void Finish();

	bool Add(std::function<void()>&& workCb);

private:
	int _producerNum;
	int _consumerNum;
	bool _done = false;

	std::function<void()> _initCb;
	std::function<void()> _disposeCb;

	moodycamel::BlockingConcurrentQueue<CallbackEntry, MyTraits> _callbacks;

	// threads
	std::vector<std::thread> _threads;
};
using CCamelBlockingConcurrentQueuePtr = std::shared_ptr<CCamelBlockingConcurrentQueue>;

/*EOF*/