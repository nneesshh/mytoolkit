#pragma once
//------------------------------------------------------------------------------
/**
	@class CEventDispatcher

	(C) 2016 n.lee
*/
#include <vector>
#include <map>
#include <functional>

namespace e_d {

	template<typename... ParamTypes>
	class EventSlot {
	public:
		void operator()(ParamTypes... args) {
			for (std::function<void(ParamTypes...)>& d : _delegates)
				d(args...);
		}

		void Attach(std::function<void(ParamTypes...)> d) {
			_delegates.push_back(d);
		}

	private:
		std::vector<std::function<void(ParamTypes...)>> _delegates;
	};

	template<typename Functor, typename... ParamTypes>
	void ConnectSlotHandler(EventSlot<ParamTypes...>& e, Functor f) {
		e.Attach(f);
	}

	template<class T, typename... ParamTypes>
	void ConnectSlotHandler(EventSlot<ParamTypes...>& e, T& o, void(T::*m)(ParamTypes...)) {
		e.Attach([&o, m](ParamTypes... args) { (o.*m)(args...); });
	}

	//------------------------------------------------------------------------------
	/**
	@brief CEventDispatcher
	*/
	template<typename... ParamTypes>
	class CEventDispatcher {
	public:
		CEventDispatcher() = default;
		~CEventDispatcher() = default;

		template<typename Functor>
		void Connect(uint64_t uSlotId, Functor f) {
			EventSlot<ParamTypes...>& e = _mapSlot[uSlotId];
			ConnectSlotHandler<Functor, ParamTypes...>(e, f);
		}

		template<class T>
		void Connect(uint64_t uSlotId, T& o, void(T::*m)(ParamTypes...)) {
			EventSlot<ParamTypes...>& e = _mapSlot[uSlotId];
			ConnectSlotHandler<T, ParamTypes...>(e, o, m);
		}

		void Emit(uint64_t uSlotId, ParamTypes... args) {
			EventSlot<ParamTypes...>& e = _mapSlot[uSlotId];
			e(args...);
		}

	private:
		std::map<uint64_t, EventSlot<ParamTypes...>> _mapSlot; // slot id 2 slot 
	};
}

/*EOF*/