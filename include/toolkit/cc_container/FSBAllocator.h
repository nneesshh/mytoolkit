#ifndef FIXED_SIZE_BLOCK_ALLOCATOR_H
#define FIXED_SIZE_BLOCK_ALLOCATOR_H

#include <new>
#include <cassert>
#include <vector>

namespace mp {

#ifdef FSBALLOCATOR_USE_THREAD_SAFE_LOCKING_PTHREAD
#undef FSBALLOCATOR_USE_THREAD_SAFE_LOCKING_GCC
#define FSBALLOCATOR_USE_THREAD_SAFE_LOCKING_OBJECT
#include <pthread.h>

	class FSBAllocator_Mutex
	{
		pthread_mutex_t mutex;

	public:
		FSBAllocator_Mutex() { pthread_mutex_init(&mutex, NULL); }
		~FSBAllocator_Mutex() { pthread_mutex_destroy(&mutex); }
		void lock() { pthread_mutex_lock(&mutex); }
		void unlock() { pthread_mutex_unlock(&mutex); }
	};
#endif

#if defined(FSBALLOCATOR_USE_THREAD_SAFE_LOCKING_GCC) || defined(FSBALLOCATOR_USE_THREAD_SAFE_LOCKING_GCC_WITH_SCHED)
#undef FSBALLOCATOR_USE_THREAD_SAFE_LOCKING_PTHREAD
#define FSBALLOCATOR_USE_THREAD_SAFE_LOCKING_OBJECT
#ifdef FSBALLOCATOR_USE_THREAD_SAFE_LOCKING_GCC_WITH_SCHED
#include <sched.h>
#endif
	class FSBAllocator_Mutex
	{
		volatile int lockFlag;

	public:
		FSBAllocator_Mutex() : lockFlag(0) {}
		void lock()
		{
			while (!__sync_bool_compare_and_swap(&lockFlag, 0, 1))
			{
#ifdef FSBALLOCATOR_USE_THREAD_SAFE_LOCKING_GCC_WITH_SCHED
				sched_yield();
#endif
			}
		}
		void unlock() { lockFlag = 0; }
	};
#endif

	/************************************************************************/
	/* FSBAllocator_ElemAllocator                                           */
	/************************************************************************/
	template<unsigned ElemSize>
	class FSBAllocator_ElemAllocator
	{
		typedef std::size_t Data_t;

		static const Data_t s_BlockElements = 512;
		static const Data_t s_DSize = sizeof(Data_t);
		static const Data_t s_ElemSizeInDSize = (ElemSize + (s_DSize - 1)) / s_DSize;
		static const Data_t s_UnitSizeInDSize = s_ElemSizeInDSize + 1;
		static const Data_t s_BlockSize = s_BlockElements*s_UnitSizeInDSize;

		class MemBlock
		{
			Data_t* block;
			Data_t firstFreeUnitIndex, allocatedElementsAmount, endIndex;

		public:
			MemBlock() :
				block(0),
				firstFreeUnitIndex(Data_t(-1)),
				allocatedElementsAmount(0)
			{}

			bool isFull() const
			{
				return allocatedElementsAmount == s_BlockElements;
			}

			void clear()
			{
				delete[] block;
				block = 0;
				firstFreeUnitIndex = Data_t(-1);
			}

			void* allocate(Data_t vectorIndex)
			{
				if (firstFreeUnitIndex == Data_t(-1))
				{
					if (!block)
					{
						block = new Data_t[s_BlockSize];
						if (!block) return 0;
						endIndex = 0;
					}

					Data_t* retval = block + endIndex;
					endIndex += s_UnitSizeInDSize;
					retval[s_ElemSizeInDSize] = vectorIndex;
					++allocatedElementsAmount;
					return retval;
				}
				else
				{
					Data_t* retval = block + firstFreeUnitIndex;
					firstFreeUnitIndex = *retval;
					++allocatedElementsAmount;
					return retval;
				}
			}

			void deallocate(Data_t* ptr)
			{
				*ptr = firstFreeUnitIndex;
				firstFreeUnitIndex = ptr - block;

				if (--allocatedElementsAmount == 0)
					clear();
			}
		};

		struct BlocksVector
		{
			std::vector<MemBlock> data;

			BlocksVector() { data.reserve(1024); }

			~BlocksVector()
			{
				for (std::size_t i = 0; i < data.size(); ++i)
					data[i].clear();
			}
		};

		static BlocksVector s_blocksVector;
		static std::vector<Data_t> s_blocksWithFree;

#ifdef FSBALLOCATOR_USE_THREAD_SAFE_LOCKING_OBJECT
		static FSBAllocator_Mutex s_mutex;

		struct Lock
		{
			Lock() { s_mutex.lock(); }
			~Lock() { s_mutex.unlock(); }
		};

#endif

	public:
		static void* allocate()
		{
#ifdef FSBALLOCATOR_USE_THREAD_SAFE_LOCKING_OBJECT
			Lock lock;
#endif

			if (s_blocksWithFree.empty())
			{
				s_blocksWithFree.push_back(s_blocksVector.data.size());
				s_blocksVector.data.push_back(MemBlock());
			}

			const Data_t index = s_blocksWithFree.back();
			MemBlock& block = s_blocksVector.data[index];
			void* retval = block.allocate(index);

			if (block.isFull())
				s_blocksWithFree.pop_back();

			return retval;
		}

		static void deallocate(void* ptr)
		{
			if (!ptr) return;

#ifdef FSBALLOCATOR_USE_THREAD_SAFE_LOCKING_OBJECT
			Lock lock;
#endif

			Data_t* unitPtr = (Data_t*)ptr;
			const Data_t blockIndex = unitPtr[s_ElemSizeInDSize];
			MemBlock& block = s_blocksVector.data[blockIndex];

			if (block.isFull())
				s_blocksWithFree.push_back(blockIndex);
			block.deallocate(unitPtr);
		}
	};

	template<unsigned ElemSize>
	typename FSBAllocator_ElemAllocator<ElemSize>::BlocksVector
		FSBAllocator_ElemAllocator<ElemSize>::s_blocksVector;

	template<unsigned ElemSize>
	std::vector<typename FSBAllocator_ElemAllocator<ElemSize>::Data_t>
		FSBAllocator_ElemAllocator<ElemSize>::s_blocksWithFree;

#ifdef FSBALLOCATOR_USE_THREAD_SAFE_LOCKING_OBJECT
	template<unsigned ElemSize>
	FSBAllocator_Mutex FSBAllocator_ElemAllocator<ElemSize>::mutex;
#endif

	/************************************************************************/
	/* FSBAllocator2_ElemAllocator                                          */
	/************************************************************************/
	template<unsigned ElemSize>
	class FSBAllocator2_ElemAllocator
	{
		static const std::size_t s_BlockElements = 1024;
		static const std::size_t s_DSize = sizeof(std::size_t);
		static const std::size_t s_ElemSizeInDSize = (ElemSize + (s_DSize - 1)) / s_DSize;
		static const std::size_t s_BlockSize = s_BlockElements*s_ElemSizeInDSize;

		struct Blocks
		{
			std::vector<std::size_t*> ptrs;

			Blocks()
			{
				ptrs.reserve(256);
				ptrs.push_back(new std::size_t[s_BlockSize]);
			}

			~Blocks()
			{
				for (std::size_t i = 0; i < ptrs.size(); ++i)
					delete[] ptrs[i];
			}
		};

		static Blocks s_blocks;
		static std::size_t s_headIndex;
		static std::size_t* s_freeList;
		static std::size_t s_allocatedElementsAmount;

#ifdef FSBALLOCATOR_USE_THREAD_SAFE_LOCKING_OBJECT
		static FSBAllocator_Mutex s_mutex;

		struct Lock
		{
			Lock() { s_mutex.lock(); }
			~Lock() { s_mutex.unlock(); }
		};

#endif

		static void freeAll()
		{
			for (std::size_t i = 1; i < s_blocks.ptrs.size(); ++i)
				delete[] s_blocks.ptrs[i];
			s_blocks.ptrs.resize(1);
			s_headIndex = 0;
			s_freeList = 0;
		}

	public:
		static void* allocate()
		{
#ifdef FSBALLOCATOR_USE_THREAD_SAFE_LOCKING_OBJECT
			Lock lock;
#endif

			++s_allocatedElementsAmount;

			if (s_freeList)
			{
				std::size_t* retval = s_freeList;
				s_freeList = reinterpret_cast<std::size_t*>(*s_freeList);
				return retval;
			}

			if (s_headIndex == s_BlockSize)
			{
				s_blocks.ptrs.push_back(new std::size_t[s_BlockSize]);
				s_headIndex = 0;
			}

			std::size_t* retval = &(s_blocks.ptrs.back()[s_headIndex]);
			s_headIndex += s_ElemSizeInDSize;
			return retval;
		}

		static void deallocate(void* ptr)
		{
			if (ptr)
			{
#ifdef FSBALLOCATOR_USE_THREAD_SAFE_LOCKING_OBJECT
				Lock lock;
#endif

				std::size_t* sPtr = (std::size_t*)ptr;
				*sPtr = reinterpret_cast<std::size_t>(s_freeList);
				s_freeList = sPtr;

				if (--s_allocatedElementsAmount == 0)
					freeAll();
			}
		}

		static void cleanSweep(std::size_t unusedValue = std::size_t(-1))
		{
#ifdef FSBALLOCATOR_USE_THREAD_SAFE_LOCKING_OBJECT
			Lock lock;
#endif

			while (s_freeList)
			{
				std::size_t* current = s_freeList;
				s_freeList = reinterpret_cast<std::size_t*>(*s_freeList);
				*current = unusedValue;
			}

			for (std::size_t i = s_headIndex; i < s_BlockSize; i += s_ElemSizeInDSize)
				s_blocks.ptrs.back()[i] = unusedValue;

			for (std::size_t blockInd = 1; blockInd < s_blocks.ptrs.size();)
			{
				std::size_t* block = s_blocks.ptrs[blockInd];
				std::size_t freeAmount = 0;
				for (std::size_t i = 0; i < s_BlockSize; i += s_ElemSizeInDSize)
					if (block[i] == unusedValue)
						++freeAmount;

				if (freeAmount == s_BlockElements)
				{
					delete[] block;
					s_blocks.ptrs[blockInd] = s_blocks.ptrs.back();
					s_blocks.ptrs.pop_back();
				}
				else ++blockInd;
			}

			const std::size_t* lastBlock = s_blocks.ptrs.back();
			for (s_headIndex = s_BlockSize; s_headIndex > 0; s_headIndex -= s_ElemSizeInDSize)
				if (lastBlock[s_headIndex - s_ElemSizeInDSize] != unusedValue)
					break;

			const std::size_t lastBlockIndex = s_blocks.ptrs.size() - 1;
			for (std::size_t blockInd = 0; blockInd <= lastBlockIndex; ++blockInd)
			{
				std::size_t* block = s_blocks.ptrs[blockInd];
				for (std::size_t i = 0; i < s_BlockSize; i += s_ElemSizeInDSize)
				{
					if (blockInd == lastBlockIndex && i == s_headIndex)
						break;

					if (block[i] == unusedValue)
						deallocate(block + i);
				}
			}
		}
	};

	template<unsigned ElemSize>
	typename FSBAllocator2_ElemAllocator<ElemSize>::Blocks
		FSBAllocator2_ElemAllocator<ElemSize>::s_blocks;

	template<unsigned ElemSize>
	std::size_t FSBAllocator2_ElemAllocator<ElemSize>::s_headIndex = 0;

	template<unsigned ElemSize>
	std::size_t* FSBAllocator2_ElemAllocator<ElemSize>::s_freeList = 0;

	template<unsigned ElemSize>
	std::size_t FSBAllocator2_ElemAllocator<ElemSize>::s_allocatedElementsAmount = 0;

#ifdef FSBALLOCATOR_USE_THREAD_SAFE_LOCKING_OBJECT
	template<unsigned ElemSize>
	FSBAllocator_Mutex FSBAllocator2_ElemAllocator<ElemSize>::mutex;
#endif

	/************************************************************************/
	/* FSBAllocator                                                         */
	/************************************************************************/
	template<typename Ty>
	class FSBAllocator
	{
	public:
		typedef std::size_t size_type;
		typedef std::ptrdiff_t difference_type;
		typedef Ty *pointer;
		typedef const Ty *const_pointer;
		typedef Ty& reference;
		typedef const Ty& const_reference;
		typedef Ty value_type;

		pointer address(reference val) const { return &val; }
		const_pointer address(const_reference val) const { return &val; }

		template<class Other>
		struct rebind
		{
			typedef FSBAllocator<Other> other;
		};

		FSBAllocator() throw()
		{

		}

		template<class Other>
		FSBAllocator(const FSBAllocator<Other>&) throw()
		{
			
		}

		~FSBAllocator()
		{
		
		}

		template<class Other>
		FSBAllocator& operator=(const FSBAllocator<Other>&) { return *this; }

		pointer allocate(size_type count, const void* = 0)
		{
			assert(count == 1);
			return static_cast<pointer>
				(FSBAllocator_ElemAllocator<sizeof(Ty)>::allocate());
		}

		void deallocate(pointer ptr, size_type)
		{
			FSBAllocator_ElemAllocator<sizeof(Ty)>::deallocate(ptr);
		}

		void construct(pointer ptr, const Ty& val)
		{
			new ((void *)ptr) Ty(val);
		}

		void destroy(pointer ptr)
		{
			ptr->Ty::~Ty();
		}

		size_type max_size() const throw()
		{
			return (std::numeric_limits<size_type>::max)();
		}
	};

	/************************************************************************/
	/* FSBAllocator2                                                        */
	/************************************************************************/
	template<typename Ty>
	class FSBAllocator2
	{
	public:
		typedef std::size_t size_type;
		typedef std::ptrdiff_t difference_type;
		typedef Ty *pointer;
		typedef const Ty *const_pointer;
		typedef Ty& reference;
		typedef const Ty& const_reference;
		typedef Ty value_type;

		pointer address(reference val) const { return &val; }
		const_pointer address(const_reference val) const { return &val; }

		template<class Other>
		struct rebind
		{
			typedef FSBAllocator2<Other> other;
		};

		FSBAllocator2() throw()
		{

		}

		template<class Other>
		FSBAllocator2(const FSBAllocator2<Other>&) throw()
		{

		}

		~FSBAllocator2() throw()
		{

		}

		template<class Other>
		FSBAllocator2& operator=(const FSBAllocator2<Other>&) { return *this; }

		pointer allocate(size_type count, const void* = 0)
		{
			assert(count == 1);
			return static_cast<pointer>
				(FSBAllocator2_ElemAllocator<sizeof(Ty)>::allocate());
		}

		void deallocate(pointer ptr, size_type)
		{
			FSBAllocator2_ElemAllocator<sizeof(Ty)>::deallocate(ptr);
		}

		void construct(pointer ptr, const Ty& val)
		{
			new ((void *)ptr) Ty(val);
		}

		void destroy(pointer ptr)
		{
			ptr->Ty::~Ty();
		}

		size_type max_size() const throw()
		{
			return (std::numeric_limits<size_type>::max)();
		}

		void cleanSweep(std::size_t unusedValue = std::size_t(-1))
		{
			FSBAllocator2_ElemAllocator<sizeof(Ty)>::cleanSweep(unusedValue);
		}
	};

	/************************************************************************/
	/* FSBRefCountAllocator                                                 */
	/************************************************************************/
	typedef FSBAllocator2<std::size_t> FSBRefCountAllocator;

}

#endif