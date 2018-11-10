#pragma once
//------------------------------------------------------------------------------
/**
@class CFifoTaskQueue

(C) 2016 n.lee
*/
#include <string>

#include "../c_container/uthash.h"
#include "../queue/circle_queue.h"
#include "../core/BitFlag.h"
#include "Cooldown.h"

///
class IFifoTask;
struct fftq_item_t {
	char			chRequest[1024];

	IFifoTask		*_task;
};

///
struct hash_fired_entry_t {
	IFifoTask		*_fired_task;

	UT_hash_handle	hh;
};

//------------------------------------------------------------------------------
/**
@brief IFifoTask
*/
class IFifoTask {
public:
	virtual ~IFifoTask() { }

	virtual size_t			OnResponse(char *buffer, size_t size, size_t nitems) = 0;
	virtual void			OnReset() = 0;

	virtual bool			Fire() = 0;
};

//------------------------------------------------------------------------------
/**
@brief CFifoTaskQueue
*/
class CFifoTaskQueue {
public:
	CFifoTaskQueue(unsigned int size, unsigned int firedmax = 1);
	~CFifoTaskQueue();

	struct state_flag_t : public CBitFlag {
		enum STATE {
			IDLE = 0x00
			, RUNNING = 0x01
			, CORKED = 0x02
			, TERMINATED = 0x04
		};
	};

	void					OnUpdate(float elapsed);

public:
	bool					Enqueue(IFifoTask *task);
	IFifoTask *				Dequeue();

	bool					IsIdle() {
		return CQ_IS_EMPTY(_tasks) && 0 == __getEntryCount();
	}

	bool					IsFull() {
		return CQ_IS_FULL(_tasks);
	}

	//
	void					AddTask(IFifoTask *pTask);
	void					FireNextTask(IFifoTask *pConsume);

	void					Cork() {
		_state_flag.SetFlag(state_flag_t::CORKED);
	}

	void					Uncork() {
		_state_flag.ClearFlag(state_flag_t::CORKED);
	}

	bool					IsCorked() {
		return _state_flag.TestFlag(state_flag_t::CORKED);
	}

	void					Terminate() {
		_state_flag.SetFlag(state_flag_t::TERMINATED);
	}

	bool					IsTerminated() {
		return _state_flag.TestFlag(state_flag_t::TERMINATED);
	}

private:
	/** add entry **/
	hash_fired_entry_t *	__addEntry(IFifoTask *fired_task) {
		hash_fired_entry_t *entry = nullptr;
		HASH_FIND_PTR(_pFiredTasks, &fired_task, entry);
		if (nullptr == entry) {
			entry = (hash_fired_entry_t *)malloc(sizeof(hash_fired_entry_t));
			entry->_fired_task = fired_task;
			HASH_ADD_PTR(_pFiredTasks, _fired_task, entry);
		}
		return entry;
	}

	/** remove entry **/
	void					__removeEntry(hash_fired_entry_t *entry) {
		if (entry) {
			HASH_DEL(_pFiredTasks, entry);
			free(entry);
		}
	}

	/** get entry **/
	hash_fired_entry_t *	__getEntry(IFifoTask *task) {
		hash_fired_entry_t *entry = NULL;
		HASH_FIND_PTR(_pFiredTasks, &task, entry);
		return entry;
	}

	/** get entry count **/
	unsigned int			__getEntryCount() {
		return HASH_COUNT(_pFiredTasks);
	}

	/** **/
	void					__clearAll();
	void					__clearTask(IFifoTask *task);

private:
	struct circqueue		*_tasks;			// queueing
	hash_fired_entry_t		*_pFiredTasks;		// fired tasks
	unsigned int			_uFiredMax;

	CCooldown				_chokeClearCD;

	struct state_flag_t		_state_flag;
};

/*EOF*/