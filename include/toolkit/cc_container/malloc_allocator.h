#ifndef MALLOC_ALLOCATOR_H
#define MALLOC_ALLOCATOR_H

#include <cstdlib>

namespace mp {

	template <typename T>
	struct malloc_allocator
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
			typedef malloc_allocator<U> other;
		};

		pointer address(reference r) { return &r; }
		const_pointer address(const_reference r) { return &r; }

		pointer allocate(const size_type n, const void* = 0)
		{
			const pointer ret = (pointer)std::malloc(n * sizeof(T));
			if (ret == 0)
				throw std::bad_alloc();
			return ret;
		}

		void deallocate(const pointer p, const size_type)
		{
			std::free(p);
		}

		size_type max_size() { return (std::numeric_limits<size_type>::max)(); }

		bool operator==(const malloc_allocator &) const { return true; }
		bool operator!=(const malloc_allocator &) const { return false; }

		malloc_allocator() {

		}

		template <typename U>
		malloc_allocator(const malloc_allocator<U> &) {

		}

		~malloc_allocator() {

		}

		void construct(const pointer p, const_reference t)
		{
			new ((void *)p) T(t);
		}

		void destroy(const pointer p)
		{
			p->~T();
		}
	};

}

#endif