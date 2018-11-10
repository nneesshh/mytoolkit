#ifndef __CORE_REF_COUNTED_H__
#define __CORE_REF_COUNTED_H__

#include "../platform/types.h"
#include <atomic>

#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#ifndef __cplusplus
#error RefCounted requires C++
#endif

class IRefCounted {
protected:
	/**
	 * Protected so users of refcounted classes don't use std::auto_ptr
	 * or the delete operator.
	 *
	 * Interfaces that derive from RefCounted should define an inline,
	 * empty, protected destructor as well.
	 */
	virtual ~IRefCounted() { }

public:
	/**
	 * Add a reference to the internal reference count.
	 */
	MYDLL_METHOD(void) ref() = 0;

	/**
	 * Remove a reference from the internal reference count.  When this
	 * reaches 0, the object is destroyed.
	 */
	MYDLL_METHOD(void) unref() = 0;

	/**
	 * Return reference count.
	 */
	MYDLL_METHOD(int) refcount() const = 0;
};


/**
* A basic implementation of the RefCounted interface.  Derive
* your implementations from RefImplementation<YourInterface>.
*/
template<class Interface>
class RefImplementation : public Interface {
protected:
	RefImplementation() {
		_ref_count = 0;
	}

	/**
	 * So the implementation can put its destruction logic in the destructor,
	 * as natural C++ code does.
	 */
	virtual ~RefImplementation() { }

public:
	void MYAPI_CALL ref() {
		++_ref_count;
	}

	void MYAPI_CALL unref() {
		if (--_ref_count == 0) {
			delete this;
		}
	}

	int MYAPI_CALL refcount() const {
		return _ref_count;
	}

private:
	std::atomic_int _ref_count;
};


/* auto ptr for IRefCounted */
template<typename T>
class RefPtr {
public:
	RefPtr(T* rsh = nullptr) {
		_ptr = nullptr;
		*this = rsh;
	}

	RefPtr(const RefPtr<T>& rsh) {
		_ptr = nullptr;
		*this = rsh;
	}

	virtual ~RefPtr() {
		if (_ptr) {
			_ptr->unref();
			_ptr = 0;
		}
	}

	RefPtr<T>& operator=(T* rsh) {
		if (rsh != _ptr) {
			if (_ptr) {
				_ptr->unref();
			}
			_ptr = rsh;
			if (_ptr) {
				_ptr->ref();
			}
		}
		return *this;
	}

	RefPtr<T>& operator=(const RefPtr<T>& rsh) {
		*this = rsh._ptr;
		return *this;
	}

	T* operator->() const {
		return _ptr;
	}

	T& operator*() const {
		return *_ptr;
	}

	operator bool() const {
		return (_ptr != 0);
	}

	T* get() const {
		return _ptr;
	}

private:
	T* _ptr;
};


template<typename T, typename U>
bool operator==(const RefPtr<T>& a, const RefPtr<U>& b) {
	return (a.get() == b.get());
}

template<typename T>
bool operator==(const RefPtr<T>& a, const T* b) {
	return (a.get() == b);
}

template<typename T>
bool operator==(const T* a, const RefPtr<T>& b) {
	return (a == b.get());
}

template<typename T, typename U>
bool operator!=(const RefPtr<T>& a, const RefPtr<U>& b) {
	return (a.get() != b.get());
}

template<typename T>
bool operator!=(const RefPtr<T>& a, const T* b) {
	return (a.get() != b);
}

template<typename T>
bool operator!=(const T* a, const RefPtr<T>& b) {
	return (a != b.get());
}

#endif