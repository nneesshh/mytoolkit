#ifndef NEW_DELETE_ALLOCATOR_H
#define NEW_DELETE_ALLOCATOR_H

#include <new>

namespace mp {

	template <typename T>
	struct new_delete_allocator
	{
		typedef T * pointer;
		typedef const T * const_pointer;
		typedef T & reference;
		typedef const T & const_reference;
		typedef T value_type;
		typedef std::size_t size_type;
		typedef std::ptrdiff_t difference_type;

		template <typename U>
		struct rebind
		{
			typedef new_delete_allocator<U> other;
		};

		static pointer address(reference r) { return &r; }
		static const_pointer address(const_reference r) { return &r; }

		static pointer allocate(const size_type n, const void* = 0)
		{
			return (pointer) new char[n * sizeof(T)];
		}

		static void deallocate(const pointer p, const size_type)
		{
			// Note: "delete [] p" may cause destructor called twice, so must delete a char pointer.
			delete[] (char *)p;
		}

		static size_type max_size() { return (std::numeric_limits<size_type>::max)(); }

		bool operator==(const new_delete_allocator &) const { return true; }
		bool operator!=(const new_delete_allocator &) const { return false; }

		new_delete_allocator() {

		}

		template <typename U>
		new_delete_allocator(const new_delete_allocator<U> &) {

		}

		static void construct(const pointer p, const_reference t)
		{
			new ((void *)p) T(t);
		}

		static void destroy(const pointer p)
		{
			p->~T();
		}
	};

}

#endif