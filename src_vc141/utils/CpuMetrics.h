#pragma once
//------------------------------------------------------------------------------
/**
    @class CCpuMetrics
    
    (C) 2016 n.lee
*/
#include <string>
#include <unordered_map>

#include "../base/time/time.h"

#define CPU_METRICS_CONCAT_(x, y) x##y
#define CPU_METRICS_UNIQUE_NAME(prefix)	CPU_METRICS_CONCAT_(prefix, __LINE__)

#define DECLARE_CPU_METRICS_VAR(name)	CCpuMetrics CPU_METRICS_UNIQUE_NAME(_cm)(name)
#define CPU_METRICS_RESET()				CCpuMetrics::Reset()
#define CPU_METRICS_DUMP()				CCpuMetrics::Dump()

#define CPU_METRICS_BEGIN(name)	{	\
	DECLARE_CPU_METRICS_VAR(name);

#define CPU_METRICS_END()			\
	}

#define CPU_METRICS_END_AND_DUMP()	\
	}								\
	CPU_METRICS_DUMP();

// stack
struct cpu_metrics_stack_t {

	cpu_metrics_stack_t() {
		_payload_in_ms = 0;
		_payload_delta_in_ms = 0;
		_reentrant_num = 0;

		_next = nullptr;
	}

	uint64_t _payload_in_ms; // total time
	int _payload_delta_in_ms; // delta time
	int _reentrant_num;

	struct cpu_metrics_stack_t *_next;
};
using MAP_CPU_METRICS_STACK = std::unordered_map<std::string, cpu_metrics_stack_t>;

// cpu metrics
struct cpu_metrics_t {

	cpu_metrics_t() {
		_elapsed_in_ms = 0;
		_top = nullptr;
		_depth = 0;
	}

	void restart(base::TimeTicks& nowTicks) {
		_start_time_point = nowTicks;
		_elapsed_in_ms = 0;
		_top = nullptr;
		_depth = 0;

		_map_cpu_metrics_stack.clear();
	}

	base::TimeTicks _start_time_point;
	int _elapsed_in_ms;

	cpu_metrics_stack_t *_top; // stack top frame
	int _depth; // stack depth

	MAP_CPU_METRICS_STACK _map_cpu_metrics_stack;
};

//------------------------------------------------------------------------------
/** 
	@brief CCpuMetrics
*/
class CCpuMetrics {
public:
	CCpuMetrics(const char *sName) : _name(sName) {

		_self = &s_entry._map_cpu_metrics_stack[_name];;
		_startTimePoint = base::TimeTicks::Now();

		if (s_entry._start_time_point.is_null())
			s_entry._start_time_point = _startTimePoint;

		//
		Push();
	}

	~CCpuMetrics() {
		//
		Pop();

		// generate delta time -- ignore others except first entrance
		if (0 == _self->_reentrant_num) {
			base::TimeTicks nowTicks = base::TimeTicks::Now();
			base::TimeDelta elapsed = nowTicks - _startTimePoint;
			base::TimeDelta totalElapsed = nowTicks - s_entry._start_time_point;

			// delta
			_self->_payload_delta_in_ms += (int)elapsed.InMilliseconds();
			_self->_payload_in_ms += _self->_payload_delta_in_ms;

			s_entry._elapsed_in_ms = (int)totalElapsed.InMilliseconds();

			// check top (=parent)
			if (s_entry._top) {
				cpu_metrics_stack_t *parent = s_entry._top;

				// exclude self delta from parent delta
				parent->_payload_delta_in_ms -= _self->_payload_delta_in_ms;
			}
		}
	}

	std::string _name;
	cpu_metrics_stack_t *_self = nullptr;
	base::TimeTicks _startTimePoint;

private:
	void						Push() {
		++(_self->_reentrant_num);

		_self->_next = s_entry._top;
		s_entry._top = _self;
		++(s_entry._depth);
	}

	void						Pop() {
		--(_self->_reentrant_num);

		s_entry._top = _self->_next;
		_self->_next = nullptr;
		--(s_entry._depth);
	}

public:
	// 
	static cpu_metrics_t s_entry;

	//
	static bool					Reset() {
		if (s_entry._depth > 0
			|| s_entry._top)
			return false;

		base::TimeTicks nowTicks = base::TimeTicks::Now();
		s_entry.restart(nowTicks);
		return true;
	}

	//
	static void					Dump() {
		int total_elapsed_in_ms = (s_entry._elapsed_in_ms > 0) ? s_entry._elapsed_in_ms : 1;
		int n = s_entry._map_cpu_metrics_stack.size();
		int i = 0;
		double dTotal = 0.0;

		//
		fprintf(stderr, "\n[CCpuMetrics::Dump()] start --------------------------------\n");
		for (auto& iter : s_entry._map_cpu_metrics_stack) {
			const char *sName = iter.first.c_str();
			cpu_metrics_stack_t& stack = iter.second;

			uint64_t uPayload = stack._payload_in_ms;
			int nDelta = stack._payload_delta_in_ms;
			double dCost = (double)uPayload * 100 / total_elapsed_in_ms;
			fprintf(stderr, " [%d/%d] %s <--> %.2f%% (%d)+\n",
				i + 1, n, sName, dCost, nDelta);

			//
			++i;
			dTotal += dCost;
		}
		fprintf(stderr, "[CCpuMetrics::Dump()] over, total <--> %.2f%%  --------------------------------\n",
			dTotal);
	}
};

/*EOF*/