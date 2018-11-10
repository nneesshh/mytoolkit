#ifndef MEMORY_POOL_ALLOCATOR_H
#define MEMORY_POOL_ALLOCATOR_H

#include <memory>

#include "FSBPool.h"

namespace mp {

	template <class T, std::size_t BlockSize = 4096>
	class FSBPoolAllocator {

		static FSBPool<T, BlockSize> s_pool;

	public:
		typedef T * pointer;
		typedef const T * const_pointer;
		typedef T & reference;
		typedef const T & const_reference;
		typedef T value_type;
		typedef std::size_t size_type;
		typedef std::ptrdiff_t difference_type;

		template <class U>
		struct rebind {
			typedef FSBPoolAllocator<U, BlockSize> other;
		};

		FSBPoolAllocator() {

		}

		template <class U>
		FSBPoolAllocator(const FSBPoolAllocator<U, BlockSize> &other) {

		}

		~FSBPoolAllocator() {

		}

		template<class U>
		FSBPoolAllocator& operator=(const FSBPoolAllocator<U, BlockSize> &other) {
			return *this;
		}

		pointer allocate(size_type n = 1, std::allocator<void>::const_pointer hint = 0) {
			return s_pool.allocate();
		}

		void deallocate(pointer p, size_type n) {
			s_pool.deallocate(p);
		}

		template <class... Args>
		void construct(pointer p, Args&&... args) {
			s_pool.construct(p, args...);
		}

		void destroy(pointer p) {
			s_pool.destroy(p);
		}

		/// Return the largest possible storage available through a call to allocate.
		size_type max_size() const {
			return s_pool.max_size();
		};
	};

	template<class T, std::size_t BlockSize>
	typename FSBPool<T, BlockSize>
		FSBPoolAllocator<T, BlockSize>::s_pool;
}

#endif