//------------------------------------------------------------------------------
//  FifoTaskQueue.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "FifoTaskQueue.h"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__,__LINE__)
#endif
#endif

//------------------------------------------------------------------------------
/**

*/
CFifoTaskQueue::CFifoTaskQueue(unsigned int size, unsigned int firedmax)
	: _tasks(circqueue_new(size))
	, _pFiredTasks(nullptr)
	, _uFiredMax(firedmax)
	, _chokeClearCD(150.0f) {

	_state_flag.SetFlag(state_flag_t::RUNNING);
}

//------------------------------------------------------------------------------
/**

*/
CFifoTaskQueue::~CFifoTaskQueue() {
	circqueue_destroy(_tasks);
}

//------------------------------------------------------------------------------
/**

*/
void
CFifoTaskQueue::OnUpdate(float elapsed) {
	if (IsIdle())
		return;

	if (_chokeClearCD.TickAndTimeout(elapsed)) {
		__clearAll();

		//
		_chokeClearCD.StartCD();
	}
}

//------------------------------------------------------------------------------
/**

*/
bool
CFifoTaskQueue::Enqueue(IFifoTask *task) {
	if (IsTerminated())
		return false;

	int r = 0;
	r = circqueue_push(_tasks, task);
	return (1 == r);
}

//------------------------------------------------------------------------------
/**

*/
IFifoTask *
CFifoTaskQueue::Dequeue() {
	if (IsTerminated())
		return nullptr;

	return (IFifoTask *)circqueue_pop(_tasks);
}

//------------------------------------------------------------------------------
/**

*/
void
CFifoTaskQueue::AddTask(IFifoTask *task) {
	if (!Enqueue(task)) {
		delete task;
	}

	// try fire next task, consume nothing
	FireNextTask(nullptr);
}

//------------------------------------------------------------------------------
/**

*/
void
CFifoTaskQueue::FireNextTask(IFifoTask *pConsume) {
	if (IsTerminated())
		return;

	_chokeClearCD.StartCD();	// restart after task response

	if (pConsume) {
		hash_fired_entry_t *entry = __getEntry(pConsume);
		if (entry)
			__removeEntry(entry);
	}

	//
	if (!IsCorked() && !CQ_IS_EMPTY(_tasks) && __getEntryCount()<_uFiredMax) {
		IFifoTask *pNext = Dequeue();
		if (pNext->Fire()) {
			__addEntry(pNext);
		}
		else {
			__clearAll();
		}
	}
}

//------------------------------------------------------------------------------
/**

*/
void
CFifoTaskQueue::__clearAll() {
	IFifoTask *task;

	// clear queueing tasks
	while (!CQ_IS_EMPTY(_tasks)) {
		task = Dequeue();
		__clearTask(task);
	}

	// clear fired tasks
	hash_fired_entry_t *entry, *_tmp;
	HASH_ITER(hh, _pFiredTasks, entry, _tmp) {
		task = entry->_fired_task;
		__clearTask(task);
		__removeEntry(entry);
	}
}

//------------------------------------------------------------------------------
/**

*/
void
CFifoTaskQueue::__clearTask(IFifoTask *task) {
	if (task) {
		char *err = "1112";
		size_t err_len = strlen(err);
		task->OnResponse(err, 1, err_len);
		task->OnReset();
		delete task;
	}
}

/* -- EOF -- */