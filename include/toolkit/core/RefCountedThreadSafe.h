#pragma once

#include <functional>
#include <algorithm>
#include <atomic>

namespace core {

	class DefaultDeleter {
	public:
		template <typename T>
		void operator()(T *ptr) {
			delete ptr;
		}
	};

	class DestructorDeleter {
	public:
		template <typename T>
		void operator()(T *ptr) {
			ptr->~T();
		}
	};

	/* ref counted */
	template <typename T>
	class RefCounted {
	public:
		template <typename D = DefaultDeleter>
		RefCounted(T *ptr = nullptr, D deleter = D()) : _obj_ptr(ptr), _deleter(deleter), _ref_count(1) {
#ifndef NDEBUG
			_in_dtor = false;
#endif
		}

		template <typename D = DestructorDeleter, typename... Args>
		RefCounted(T *ptr, D deleter, Args&&... args) : _obj_ptr(ptr), _deleter(deleter), _ref_count(1) {
			new (_obj_ptr) T(std::forward<Args>(args)...);
		}

		~RefCounted() {
#ifndef NDEBUG
			if (!_in_dtor) {
				fprintf(stderr, "object deleted without calling Release()");
			}
#endif
			if (_deleter) {
				_deleter(_obj_ptr);
			}
			else {
				delete _obj_ptr;
			}
		}

		T * operator->() const {
			return _obj_ptr;
		}

		T & operator*() const {
			return *_obj_ptr;
		}

		T * get() const {
			return _obj_ptr;
		}

		/**
		* Add a reference to the internal reference count.
		*/
		MYDLL_METHOD(void) hold() {
#ifndef NDEBUG
			if (_in_dtor) {
				fprintf(stderr, "object hold after destruct");
			};
#endif
			++_ref_count;
		}

		/**
		* Remove a reference from the internal reference count.  When this
		* reaches 0, the object is destroyed.
		*/
		MYDLL_METHOD(bool) release() {
#ifndef NDEBUG
			if (_in_dtor
				|| 0 == _ref_count) {
				fprintf(stderr, "object release after destruct");
			};
#endif
			if (--_ref_count == 0) {
				_in_dtor = true;
			}
			return _in_dtor;
		}

		/**
		* Return reference count.
		*/
		MYDLL_METHOD(int) refcount() const {
			return _ref_count;
		}

	private:
		T *_obj_ptr = nullptr;

		std::function<void(T *)> _deleter;
		std::atomic<unsigned int> _ref_count;
		bool _in_dtor;
	};

	/* ref pointer */
	template <typename T>
	class RefPtr {
	public:
		explicit RefPtr(T *impl) : _impl(impl) { }
		~RefPtr() {
			if (_impl->release()) {
				delete _impl;
			}
		}

		RefPtr(const RefPtr<T> &other) {
			_impl = other._impl;
			_impl->hold();
		}

		template <typename U>
		RefPtr(const RefPtr<U> &other) {
			_impl = other._impl;
			_impl->hold();
		}

		RefPtr<T> & operator=(const RefPtr<T> &other) {
			RefPtr<T> tmp(other);
			swap(tmp);
			return *this;
		}

		template <typename U>
		RefPtr<T> & operator=(const RefPtr<U> &other) {
			RefPtr<T> tmp(other);
			swap(tmp);
			return *this;
		}

		T * operator->() const {
			return _impl->operator->();
		}

		T & operator*() const {
			return **_impl;
		}

		template <typename U>
		bool operator<(const RefPtr<U> &other) {
			return _impl->get() < other.get();
		}

		T * get() const {
			return _impl->get();
		}

		void swap(RefPtr<T> &other) {
			std::swap(_impl, other._impl);
		}

	private:
		T *_impl;
	};

	/* shared pointer */
	template <typename T>
	class SharedPtr {
	public:
		explicit SharedPtr(RefCounted<T> * impl) : _impl(impl) { }

		template <typename D = DefaultDeleter>
		explicit SharedPtr(T *ptr = nullptr, D deleter = D()) : _impl(new RefCounted<T>(ptr, deleter)) { }

		template <typename U, typename D = DefaultDeleter>
		explicit SharedPtr(U *ptr = nullptr, D deleter = D()) : _impl(new RefCounted<T>(ptr, deleter)) { }

		~SharedPtr() {
			if (_impl->release()) {
				delete _impl;
			}
		}

		SharedPtr(const SharedPtr<T> &other) {
			_impl = other._impl;
			_impl->hold();
		}

		template <typename U>
		SharedPtr(const SharedPtr<U> &other) {
			_impl = other._impl;
			_impl->hold();
		}

		SharedPtr<T> & operator=(const SharedPtr<T> &other) {
			SharedPtr<T> tmp(other);
			swap(tmp);
			return *this;
		}

		template <typename U>
		SharedPtr<T> & operator=(const SharedPtr<U> &other) {
			SharedPtr<T> tmp(other);
			swap(tmp);
			return *this;
		}

		T * operator->() const {
			return _impl->operator->();
		}

		T & operator*() const {
			return **_impl;
		}

		template <typename U>
		bool operator<(const SharedPtr<U> &other) {
			return _impl->get() < other.get();
		}

		T * get() const {
			return _impl->get();
		}

		void swap(SharedPtr<T> &other) {
			std::swap(_impl, other._impl);
		}

	private:
		RefCounted<T> *_impl;
	};

	/* operator */
	template <typename T>
	void swap(SharedPtr<T> &left, SharedPtr<T> &right) {
		left.swap(right);
	}

	template<typename T, typename U>
	bool operator==(const SharedPtr<T>& a, const SharedPtr<U>& b) {
		return (a.get() == b.get());
	}

	template<typename T>
	bool operator==(const SharedPtr<T>& a, const T* b) {
		return (a.get() == b);
	}

	template<typename T>
	bool operator==(const T* a, const SharedPtr<T>& b) {
		return (a == b.get());
	}

	template<typename T, typename U>
	bool operator!=(const SharedPtr<T>& a, const SharedPtr<U>& b) {
		return (a.get() != b.get());
	}

	template<typename T>
	bool operator!=(const SharedPtr<T>& a, const T* b) {
		return (a.get() != b);
	}

	template<typename T>
	bool operator!=(const T* a, const SharedPtr<T>& b) {
		return (a != b.get());
	}

	/* make shared */
	template <typename T, typename... Args>
	SharedPtr<T> make_shared(Args&&... args) {
		char * ptr = static_cast<char *>(operator new(sizeof(T) * sizeof(RefCounted<T>)));
		if (ptr == nullptr) {
			throw std::bad_alloc();
		}

		auto ptr2 = reinterpret_cast<T *>(ptr + sizeof(RefCounted<T>));
		try {
			auto impl = new(ptr) RefCounted<T>(ptr2, DestructorDeleter(), std::forward<T>(args)...);
			return SharedPtr<T>(impl);
		}
		catch (...) {
			operator delete(ptr);
			throw;
		}
	}
}