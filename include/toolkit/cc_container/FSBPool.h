/*-
* Copyright (c) 2013 Cosku Acay, http://www.coskuacay.com
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*/

#ifndef FIXED_SIZE_BLOCK_POOL_H
#define FIXED_SIZE_BLOCK_POOL_H

#include <climits>
#include <cstddef>

namespace mp {

	template <typename T, size_t BlockSize = 4096>
	class FSBPool {
	public:
		/* Member types */
		typedef T               value_type;
		typedef T*              pointer;
		typedef T&              reference;
		typedef const T*        const_pointer;
		typedef const T&        const_reference;
		typedef size_t          size_type;
		typedef ptrdiff_t       difference_type;
		typedef std::false_type propagate_on_container_copy_assignment;
		typedef std::true_type  propagate_on_container_move_assignment;
		typedef std::true_type  propagate_on_container_swap;

		template <typename U> struct rebind {
			typedef FSBPool<U> other;
		};

		/* Member functions */
		FSBPool() noexcept;
		FSBPool(const FSBPool& memoryPool) noexcept;
		FSBPool(FSBPool&& memoryPool) noexcept;
		template <class U> FSBPool(const FSBPool<U>& memoryPool) noexcept;

		~FSBPool() noexcept;

		FSBPool& operator=(const FSBPool& memoryPool) = delete;
		FSBPool& operator=(FSBPool&& memoryPool) noexcept;

		pointer address(reference x) const noexcept;
		const_pointer address(const_reference x) const noexcept;

		// Can only allocate one object at a time. n and hint are ignored
		pointer allocate(size_type n = 1, const_pointer hint = 0);
		void deallocate(pointer p, size_type n = 1);

		size_type max_size() const noexcept;

		template <class U, class... Args> void construct(U* p, Args&&... args);
		template <class U> void destroy(U* p);

		template <class... Args> pointer newElement(Args&&... args);
		void deleteElement(pointer p);

	private:
		union slot_t {
			~slot_t() = delete;
			value_type _element;
			slot_t* _next;
		};

		typedef char* data_pointer;
		typedef slot_t slot_type;
		typedef slot_t* slot_pointer;

		slot_pointer _currentBlock;
		slot_pointer _currentSlot;
		slot_pointer _lastSlot;
		slot_pointer _freeSlots;

		size_type padPointer(data_pointer p, size_type align) const noexcept;
		void allocateBlock();

		static_assert(BlockSize >= 2 * sizeof(slot_type), "BlockSize too small.");
	};

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	template <typename T, size_t BlockSize>
	inline typename FSBPool<T, BlockSize>::size_type
		FSBPool<T, BlockSize>::padPointer(data_pointer p, size_type align)
		const noexcept
	{
		uintptr_t result = reinterpret_cast<uintptr_t>(p);
		return ((align - result) % align);
	}

	template <typename T, size_t BlockSize>
	FSBPool<T, BlockSize>::FSBPool()
		noexcept
	{
		_currentBlock = nullptr;
		_currentSlot = nullptr;
		_lastSlot = nullptr;
		_freeSlots = nullptr;
	}

	template <typename T, size_t BlockSize>
	FSBPool<T, BlockSize>::FSBPool(const FSBPool& memoryPool)
		noexcept :
	FSBPool()
	{}

	template <typename T, size_t BlockSize>
	FSBPool<T, BlockSize>::FSBPool(FSBPool&& memoryPool)
		noexcept
	{
		_currentBlock = memoryPool.currentBlock_;
		memoryPool.currentBlock_ = nullptr;
		_currentSlot = memoryPool.currentSlot_;
		_lastSlot = memoryPool.lastSlot_;
		_freeSlots = memoryPool.freeSlots;
	}

	template <typename T, size_t BlockSize>
	template<class U>
	FSBPool<T, BlockSize>::FSBPool(const FSBPool<U>& memoryPool)
		noexcept :
	FSBPool()
	{}

	template <typename T, size_t BlockSize>
	FSBPool<T, BlockSize>&
		FSBPool<T, BlockSize>::operator=(FSBPool&& memoryPool)
		noexcept
	{
		if (this != &memoryPool)
		{
			std::swap(_currentBlock, memoryPool.currentBlock_);
			_currentSlot = memoryPool.currentSlot_;
			_lastSlot = memoryPool.lastSlot_;
			_freeSlots = memoryPool.freeSlots;
		}
		return *this;
	}

	template <typename T, size_t BlockSize>
	FSBPool<T, BlockSize>::~FSBPool()
		noexcept
	{
		slot_pointer curr = _currentBlock;
		while (curr != nullptr) {
			slot_pointer prev = curr->_next;
			operator delete(reinterpret_cast<void*>(curr));
			curr = prev;
		}
	}

	template <typename T, size_t BlockSize>
	inline typename FSBPool<T, BlockSize>::pointer
		FSBPool<T, BlockSize>::address(reference x)
		const noexcept
	{
		return &x;
	}

	template <typename T, size_t BlockSize>
	inline typename FSBPool<T, BlockSize>::const_pointer
		FSBPool<T, BlockSize>::address(const_reference x)
		const noexcept
	{
		return &x;
	}

	template <typename T, size_t BlockSize>
	void
		FSBPool<T, BlockSize>::allocateBlock()
	{
		// Allocate space for the new block and store a pointer to the previous one
		data_pointer newBlock = reinterpret_cast<data_pointer>
			(operator new(BlockSize));
		reinterpret_cast<slot_pointer>(newBlock)->_next = _currentBlock;
		_currentBlock = reinterpret_cast<slot_pointer>(newBlock);
		// Pad block body to staisfy the alignment requirements for elements
		data_pointer body = newBlock + sizeof(slot_pointer);
		size_type bodyPadding = padPointer(body, alignof(slot_type));
		_currentSlot = reinterpret_cast<slot_pointer>(body + bodyPadding);
		_lastSlot = reinterpret_cast<slot_pointer>
			(newBlock + BlockSize - sizeof(slot_type) + 1);
	}

	template <typename T, size_t BlockSize>
	inline typename FSBPool<T, BlockSize>::pointer
		FSBPool<T, BlockSize>::allocate(size_type n, const_pointer hint)
	{
		if (_freeSlots != nullptr) {
			pointer result = reinterpret_cast<pointer>(_freeSlots);
			_freeSlots = _freeSlots->_next;
			return result;
		}
		else {
			if (_currentSlot >= _lastSlot)
				allocateBlock();
			return reinterpret_cast<pointer>(_currentSlot++);
		}
	}

	template <typename T, size_t BlockSize>
	inline void
		FSBPool<T, BlockSize>::deallocate(pointer p, size_type n)
	{
		if (p != nullptr) {
			reinterpret_cast<slot_pointer>(p)->_next = _freeSlots;
			_freeSlots = reinterpret_cast<slot_pointer>(p);
		}
	}

	template <typename T, size_t BlockSize>
	inline typename FSBPool<T, BlockSize>::size_type
		FSBPool<T, BlockSize>::max_size()
		const noexcept
	{
		/*size_type maxBlocks = -1 / BlockSize;
		return (BlockSize - sizeof(data_pointer)) / sizeof(slot_type) * maxBlocks;*/
		return (std::numeric_limits<FSBPool<T, BlockSize>::size_type>::max)();
	}

	template <typename T, size_t BlockSize>
	template <class U, class... Args>
	inline void
		FSBPool<T, BlockSize>::construct(U* p, Args&&... args)
	{
		new (p) U(std::forward<Args>(args)...);
	}

	template <typename T, size_t BlockSize>
	template <class U>
	inline void
		FSBPool<T, BlockSize>::destroy(U* p)
	{
		p->~U();
	}

	template <typename T, size_t BlockSize>
	template <class... Args>
	inline typename FSBPool<T, BlockSize>::pointer
		FSBPool<T, BlockSize>::newElement(Args&&... args)
	{
		pointer result = allocate();
		construct<value_type>(result, std::forward<Args>(args)...);
		return result;
	}

	template <typename T, size_t BlockSize>
	inline void
		FSBPool<T, BlockSize>::deleteElement(pointer p)
	{
		if (p != nullptr) {
			p->~value_type();
			deallocate(p);
		}
	}

}
#endif // MEMORY_POOL_H
