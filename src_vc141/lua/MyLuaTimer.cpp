//------------------------------------------------------------------------------
//  MyLuaTimer.cpp
//  (C) 2011 n.lee
//------------------------------------------------------------------------------
#include "MyLuaTimer.h"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__,__LINE__)
#endif
#endif

static luacpp::CMyLuaTimer *s_mylua_timer = nullptr;

static void
mylua_set_expire_timer(lvar_array_t *input, lvar_array_t *output) {

	uint32_t timerid = 0;
	int delay_ms = 0;
	int func_refid = 0;

	if (s_mylua_timer
		&& input) {

		int n = input->t;
		if (n >= 3) {

			struct lvar_t *v0 = &input->v[0];
			struct lvar_t *v1 = &input->v[1];
			struct lvar_t *v2 = &input->v[2];

			if (LT_INTEGER == v0->type) {
				timerid = (int)lbind_as_integer(input, 0);
			}
			else if (LT_REAL == v0->type) {
				timerid = (int)lbind_as_real(input, 0);
			}
			else {
				printf("\n\n\n!!! [mylua_set_expire_timer()] failed!!! timerid is invalid -- timerid(%d)delay_ms(%d)func_refid(%d) !!!\n\n\n",
					timerid, delay_ms, func_refid);
				goto FAILED;
			}

			if (LT_INTEGER == v1->type) {
				delay_ms = (int)lbind_as_integer(input, 1);
			}
			else if (LT_REAL == v1->type) {
				delay_ms = (int)lbind_as_real(input, 1);
			}
			else {
				printf("\n\n\n!!! [mylua_set_expire_timer()] failed!!! delay ms is invalid -- timerid(%d)delay_ms(%d)func_refid(%d) !!!\n\n\n",
					timerid, delay_ms, func_refid);
				goto FAILED;
			}

			if (LT_FUNCREF == v2->type) {
				func_refid = lbind_as_func_refid(input, 2);
			}
			else {
				printf("\n\n\n!!! [mylua_set_expire_timer()] failed!!! callback function is invalid -- timerid(%d)delay_ms(%d)func_refid(%d) !!!\n\n\n",
					timerid, delay_ms, func_refid);
				goto FAILED;
			}

			if (!s_mylua_timer->IsTimeoutEventTicking(timerid)) {

				s_mylua_timer->SetExpireEventInMs(timerid, delay_ms, func_refid);

				// success
				if (output) {
					lbind_add_boolean(output, true);
				}
				return;
			}
		}
	}

	printf("\n\n\n!!! [mylua_set_expire_timer()] failed!!! maybe timer is already in ticking -- timerid(%d)delay_ms(%d)func_refid(%d) !!!\n\n\n",
		timerid, delay_ms, func_refid);

FAILED:
	// failed
	if (output) {
		lbind_add_boolean(output, false);
	}
}

static void
mylua_set_interval_timer(lvar_array_t *input, lvar_array_t *output) {

	uint32_t timerid = 0;
	int delay_ms = 0;
	int func_refid = 0;

	if (s_mylua_timer
		&& input) {

		int n = input->t;
		if (n >= 3) {

			struct lvar_t *v0 = &input->v[0];
			struct lvar_t *v1 = &input->v[1];
			struct lvar_t *v2 = &input->v[2];

			if (LT_INTEGER == v0->type) {
				timerid = (int)lbind_as_integer(input, 0);
			}
			else if (LT_REAL == v0->type) {
				timerid = (int)lbind_as_real(input, 0);
			}
			else {
				printf("\n\n\n!!! [mylua_set_interval_timer()] failed!!! timerid is invalid -- timerid(%d)delay_ms(%d)func_refid(%d) !!!\n\n\n",
					timerid, delay_ms, func_refid);
				goto FAILED;
			}

			if (LT_INTEGER == v1->type) {
				delay_ms = (int)lbind_as_integer(input, 1);
			}
			else if (LT_REAL == v1->type) {
				delay_ms = (int)lbind_as_real(input, 1);
			}
			else {
				printf("\n\n\n!!! [mylua_set_interval_timer()] failed!!! delay_ms is invalid -- timerid(%d)delay_ms(%d)func_refid(%d) !!!\n\n\n",
					timerid, delay_ms, func_refid);
				goto FAILED;
			}

			if (LT_FUNCREF == v2->type) {
				func_refid = lbind_as_func_refid(input, 2);
			}
			else {
				printf("\n\n\n!!! [mylua_set_interval_timer()] failed!!! func_refid is invalid -- timerid(%d)delay_ms(%d)func_refid(%d) !!!\n\n\n",
					timerid, delay_ms, func_refid);
				goto FAILED;
			}

			if (timerid > 0
				&& func_refid > 0) {

				if (!s_mylua_timer->IsTimeoutEventTicking(timerid)) {

					s_mylua_timer->SetIntervalEventInMs(timerid, delay_ms, func_refid);

					// success
					if (output) {
						lbind_add_boolean(output, true);
					}
					return;
				}
			}
		}
	}

	printf("\n\n\n!!! [mylua_set_interval_timer()] failed!!! maybe timer is already in ticking -- timerid(%d)delay_ms(%d)func_refid(%d) !!!\n\n\n",
		timerid, delay_ms, func_refid);

FAILED:
	// failed
	if (output) {
		lbind_add_boolean(output, false);
	}
}

static void
mylua_stop_timer(lvar_array_t *input, lvar_array_t *output) {
	
	uint32_t timerid = 0;

	if (s_mylua_timer
		&& input) {

		int n = input->t;
		if (n >= 1) {

			struct lvar_t *v0 = &input->v[0];
			if (LT_INTEGER == v0->type) {
				timerid = (int)lbind_as_integer(input, 0);
			}
			else if (LT_REAL == v0->type) {
				timerid = (int)lbind_as_real(input, 0);
			}

			if (timerid > 0) {
				s_mylua_timer->KillTimeoutEvent(timerid);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
namespace luacpp {

static void
on_expire(void *arg) {
	mylua_timeout_event_t *evt_ = (mylua_timeout_event_t *)arg;
	lua_State *L = evt_->_host->MyLua().GlueState();

	lbind_call_function_by_refid_with_arg_int_return_void(L, evt_->_lua_func_refid, evt_->_id);
	lbind_remove_function_by_refid(L, evt_->_lua_func_refid);

	// mark idle
	evt_->_state = MYLUA_TIMEOUT_EVENT_STATE_IDLE;
}

#ifndef TIMEOUT_DISABLE_INTERVALS
static void
on_interval(void *arg) {
	mylua_timeout_event_t *evt_ = (mylua_timeout_event_t *)arg;
	lua_State *L = evt_->_host->MyLua().GlueState();

	lbind_call_function_by_refid_with_arg_int_return_void(L, evt_->_lua_func_refid, evt_->_id);

	// mark idle
	evt_->_state = MYLUA_TIMEOUT_EVENT_STATE_IDLE;
}
#endif

//------------------------------------------------------------------------------
/**

*/
CMyLuaTimer::CMyLuaTimer(CMyLua& mylua, CTimeoutEventHub& eventHub)
	: _refMyLua(mylua)
	, _refEventHub(eventHub) {
	//
	CMyLuaTimer::ExportApi2Lua(this);
}

//------------------------------------------------------------------------------
/**

*/
CMyLuaTimer::~CMyLuaTimer() {

	for (auto& iter : _mapTimeoutEvent) {
		mylua_timeout_event_t& evt_ = iter.second;
		_refEventHub.DelTimeout(&evt_._to);
	}
	_mapTimeoutEvent.clear();

	// clear static
	if ((uintptr_t)this == (uintptr_t)s_mylua_timer) {
		s_mylua_timer = nullptr;
	}
}

//------------------------------------------------------------------------------
/**

*/
void
CMyLuaTimer::SetExpireEventInMs(uint32_t id, unsigned int expire_time_in_ms, const int func_refid) {

	if (IsTimeoutEventTicking(id)) {
		char chDesc[256];
#if defined(__CYGWIN__) || defined( _WIN32)
		_snprintf_s(chDesc, sizeof(chDesc), "!!! [CMyLuaTimer::SetExpireEventInMs()] set timer(%d) failed because timer is already ticking !!!", id);
#else
		snprintf(chDesc, sizeof(chDesc), "!!! [CMyLuaTimer::SetExpireEventInMs()] set timer(%d) failed because timer is already ticking !!!", id);
#endif
		fprintf(stderr, chDesc);
		system("pause");
		throw std::exception(chDesc);
	}

	lua_State *L = MyLua().GlueState();

	mylua_timeout_event_t *evt_ = &_mapTimeoutEvent[id];
	evt_->_id = id;
	evt_->_lua_func_refid = func_refid;
	evt_->_host = this;
	evt_->_state = MYLUA_TIMEOUT_EVENT_STATE_TICKING;

	_refEventHub.InitExpireTimeout(&evt_->_to);
	_refEventHub.AddTimeout(&evt_->_to, expire_time_in_ms);
	timeout_setcb(&evt_->_to, on_expire, evt_);
}

#ifndef TIMEOUT_DISABLE_INTERVALS
//------------------------------------------------------------------------------
/**

*/
void
CMyLuaTimer::SetIntervalEventInMs(uint32_t id, unsigned int interval_time_in_ms, const int func_refid) {

	if (IsTimeoutEventTicking(id)) {
		char chDesc[256];
#if defined(__CYGWIN__) || defined( _WIN32)
		_snprintf_s(chDesc, sizeof(chDesc), "!!! [CMyLuaTimer::SetIntervalEventInMs()] set timer(%d) failed because timer is already ticking !!!", id);
#else
		snprintf(chDesc, sizeof(chDesc), "!!! [CMyLuaTimer::SetIntervalEventInMs()] set timer(%d) failed because timer is already ticking !!!", id);
#endif
		fprintf(stderr, chDesc);
		system("pause");
		throw std::exception(chDesc);
	}

	lua_State *L = MyLua().GlueState();

	mylua_timeout_event_t *evt_ = &_mapTimeoutEvent[id];
	evt_->_id = id;
	evt_->_lua_func_refid = func_refid;
	evt_->_host = this;
	evt_->_state = MYLUA_TIMEOUT_EVENT_STATE_TICKING;

	_refEventHub.InitIntervalTimeout(&evt_->_to);
	_refEventHub.AddTimeout(&evt_->_to, interval_time_in_ms);
	timeout_setcb(&evt_->_to, on_interval, evt_);
}
#endif

//------------------------------------------------------------------------------
/**

*/
void
CMyLuaTimer::ExportApi2Lua(CMyLuaTimer *timer) {

	s_mylua_timer = timer;

	s_mylua_timer->MyLua().RegisterCFunc2Lua("mylua_set_expire_timer", mylua_set_expire_timer);
	s_mylua_timer->MyLua().RegisterCFunc2Lua("mylua_set_interval_timer", mylua_set_interval_timer);
	s_mylua_timer->MyLua().RegisterCFunc2Lua("mylua_stop_timer", mylua_stop_timer);
}

}
/* -- EOF -- */