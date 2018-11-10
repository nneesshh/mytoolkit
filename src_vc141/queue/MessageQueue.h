#pragma once

#include <mutex>
#include <condition_variable>
#include <chrono>

#ifdef __cplusplus 
extern "C" { 
#endif 
#include "circle_queue.h"

#ifdef __cplusplus 
}
#endif 

#include "../platform/utilities.h"

/************************************************************************/
/* thread safe message queue                                            */
/************************************************************************/
class MessageQueue {
public:
	MessageQueue() : _mutex(), _cond(), _cq(nullptr) { }

	int onGotMessage(void *msg, void *arg) { }

	int alloc(unsigned int size) {
		_cq = circqueue_new(size);
	}

	void destroy(struct messenger_t *msgr) {
		for (; len() > 0;) {
			util_sleep(5);
		}

		circqueue_destroy(_cq);
	}

	int push(void *msg) {
		int r = 0;
		{
			std::lock_guard<std::mutex> lck(_mutex);
			r = circqueue_push(_cq, msg);
		}
		return(r);
	}

	void * pop() {
		void *msg = nullptr;
		{
			std::lock_guard<std::mutex> lck(_mutex);
			msg = circqueue_pop(_cq);
		}
		return msg;
	}

	unsigned int len() {
		return CQ_LENGTH(_cq);
	}

	void cond_wait() {
		std::unique_lock<std::mutex> lck(_mutex);
		_cond.wait(lck);
	}

	void cond_timewait(uint64_t timeout) {
		std::unique_lock<std::mutex> lck(_mutex);
		_cond.wait_for(lck, std::chrono::seconds(1));
	}

	void cond_signal() {
		_cond.notify_all();
	}

private:
	unsigned int nextpow2(unsigned int num) {
		--num;
		num |= num >> 1;
		num |= num >> 2;
		num |= num >> 4;
		num |= num >> 8;
		num |= num >> 16;
		return ++num;
	}

	mutable std::mutex			_mutex;
	std::condition_variable		_cond;

	struct circqueue			*_cq;
};
extern struct messenger_t *		msgr_new(unsigned int); 
extern void						msgr_destroy(struct messenger_t *);
extern int						msgr_push(struct messenger_t *, void *);
extern void *					msgr_pop(struct messenger_t *);
extern unsigned int				msgr_len(struct messenger_t *);
extern void						msgr_cond_wait(struct messenger_t *);
extern void						msgr_cond_timewait(struct messenger_t *, uint64_t);
extern void						msgr_cond_signal(struct messenger_t *);

/*EOF*/