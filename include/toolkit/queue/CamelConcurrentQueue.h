#pragma once
//------------------------------------------------------------------------------
/**
@class CCamelWorkQueue

(C) 2016 n.lee
*/
#include <vector>

#include "concurrent/concurrentqueue.h"

//------------------------------------------------------------------------------
/**
@brief CCamelWorkQueue
*/
class CCamelConcurrentQueue {
public:
	CCamelConcurrentQueue(int nProducerNum, int nConsumerNum);
	~CCamelConcurrentQueue();

	using CallbackEntry = std::function<void()>;
	struct MyTraits : public moodycamel::ConcurrentQueueDefaultTraits {
		static const size_t BLOCK_SIZE = 256;
	};

	void RunOnce();
	void Close();

	bool Add(std::function<void()>&& workCb);

private:
	int _producerNum;
	int _consumerNum;
	bool _close = false;

	moodycamel::ConcurrentQueue<CallbackEntry, MyTraits> _callbacks;
};
using CCamelWorkQueuePtr = std::shared_ptr<CCamelConcurrentQueue>;

/*EOF*/