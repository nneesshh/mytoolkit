#pragma once

#include <vector>
#include <algorithm>

template<class T, class _A = std::allocator<T> >
class SortedVector : public std::vector<T, _A> {
	using _SUPER = std::vector<T, _A>;

public:
	void SetSorted(bool bSorted = true) {
		_bSorted = bSorted;
	}

	void DoSort() {
		if (!_bSorted) {
			std::sort(_SUPER::begin(), _SUPER::end());
			SetSorted();
		}
	}

	bool Contains(const T& t) {
		if (!_bSorted) {
			DoSort();
		}
		return std::binary_search(_SUPER::begin(), _SUPER::end(), t);
	}

	typename _SUPER::iterator LowerBoundIt(const T& key) {
		if (!_bSorted)
			DoSort();

		typename _SUPER::iterator it = std::lower_bound(_SUPER::begin(), _SUPER::end(), key);
		return it;
	}

	T * LowerBoundPtr(const T& key) {
		typename _SUPER::iterator it = LowerBoundIt(key);

		if (it == _SUPER::end())
			return 0;

		T *t = &(*it);
		return t;
	}

	typename _SUPER::iterator FindIt(const T& key) {
		typename _SUPER::iterator it = LowerBoundIt(key);

		if (it != _SUPER::end()) {
			// check exactly match
			if (!((*it) < key) && !(key < (*it)))
				return it;
		}
		return _SUPER::end();
	}

	T * FindPtr(const T& key) {
		typename _SUPER::iterator it = FindIt(key);
		if (it != _SUPER::end())
			return &(*it);
		return 0;
	}

	void PushSorted(const T& t) {
		if (!_bSorted) {
			DoSort();
		}

		// insert at "lower_bound" (the proper sorted location).
		std::insert(std::lower_bound(_SUPER::begin(), _SUPER::end(), t), t);
	}

	void PushUnsorted(const T& t) {
		SetSorted(false);
		push_back(t);
	}

	SortedVector<T>& operator=(_SUPER& v) {
		for (auto& it : v) 
			PushUnsorted(it);
		return this;
	}

	template<class _Pr> inline
		void DoSort(_Pr pr) {
		if (!_bSorted) {
			std::sort(_SUPER::begin(), _SUPER::end(), pr);
			SetSorted();
		}
	}

	template<class _Pr> inline
		typename _SUPER::iterator LowerBoundIt(const T& key, _Pr pr) {
		if (!_bSorted) {
			std::sort(_SUPER::begin(), _SUPER::end(), pr);
			SetSorted();
		}
		typename _SUPER::iterator it = std::lower_bound(_SUPER::begin(), _SUPER::end(), key, pr);
		return it;
	}

	template<class _Pr> inline
		T * LowerBoundPtr(const T& key, _Pr pr) {
		typename _SUPER::iterator it = LowerBoundIt(key, pr);

		if (it == _SUPER::end())
			return 0;

		T *t = &(*it);
		return t;
	}

	template<class _Pr> inline
		void PushSorted(const T& t, _Pr pr) {
		if (!_bSorted) {
			std::sort(_SUPER::begin(), _SUPER::end(), pr);
			SetSorted();
		}

		// Insert at "lower_bound" (the proper sorted location).
		std::insert(std::lower_bound(_SUPER::begin(), _SUPER::end(), t, pr), t);
	}

	template<class _Pr> inline
		typename _SUPER::iterator FindIt(const T& key, _Pr pr) {
		typename _SUPER::iterator it = LowerBoundIt(key, pr);

		if (it != _SUPER::end())

			// NOTE: We have to apply this using the predicate function, be careful...
			if (!(pr((*it), key)) && !(pr(key, (*it))))

				return it;

		return _SUPER::end();
	}

	template<class _Pr> inline
		T * FindPtr(const T& key, _Pr pr) {
		typename _SUPER::iterator it = FindIt(key, pr);
		if (it != _SUPER::end())
			return &(*it);

		return 0;
	}

protected:
	bool _bSorted;

};

/*EOF*/