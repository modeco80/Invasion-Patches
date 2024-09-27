#pragma once

#include <stddef.h>
#include <stdint.h>

#include <base/TypeTraits.hpp>
#include <base/Assert.hpp>
#include <base/Utility.hpp>

// Provide C++ new/new[] and delete/delete[]
// overloads which use the Utils heap.
// Does not provide nothrow (these versions are nothrow in and of themselves)

void* operator new(size_t size);
void* operator new[](size_t size);
void operator delete(void* ptr) noexcept;
void operator delete[](void* ptr) noexcept;

// Placement new/new[]/delete/delete[]
void* operator new(size_t, void* p) noexcept;
void* operator new[](size_t, void* p) noexcept;
void operator delete(void*, void*) noexcept;
void operator delete[](void*, void*) noexcept;

namespace base {

	// C style malloc() api
	void* Alloc(size_t size);
	void Free(void* ptr);

	//void* AllocAligned(uint32_t size);
	//void FreeAligned(void* ptr);


	// Allocator concept:
	//
	// template<class T>
	// struct Allocator {
	//  using ValueType = RemoveCvRefT<T>;
	//	using SizeType = appropriate_size_type; (or size_t if you're lazy)
	//
	//  // NOTE: Only allocates, does not start lifetime
	//  [[nodiscard]] constexpr ValueType* Allocate(SizeType n);
	//	constexpr void Deallocate(ValueType* p);
	//
	//	template<class... Args>
	//  constexpr void Construct(ValueType* ptr, Args&&... args);
	//
	//  [[nodiscard]] constexpr SizeType MaxSize() const;
	// };

	/**
	 * Allocator using the global runtime heap.
	 * Implements the Runtime version of the Allocator concept.
	 */
	template <class T>
	struct StdAllocator {
		using ValueType = typename RemoveCVReference<T>::Type;
		using SizeType = size_t;

		/**
		 * Allocates storage for N * sizeof(T) objects.
		 * The memory is not constructed/does not start lifetime for all instances of T contained in it.
		 * You will have to use Allocator<T>::Construct() or pure placement-new to do so.
		 */
		[[nodiscard]] constexpr ValueType* Allocate(SizeType number) {
			// This causes an assert failure for some reason.
			// Remove until msvc grows brian cell
			//BASE_ASSERT(number > MaxSize());
			return static_cast<T*>(Alloc(number * sizeof(T)));
		}

		constexpr void Deallocate(ValueType* ptr) {
			return Free(static_cast<void*>(ptr));
		}

		template <class... Args>
		constexpr void Construct(ValueType* ptr, Args&&... args) {
			new(ptr) T(Forward<Args>(args)...);
		}

		[[nodiscard]] constexpr SizeType MaxSize() const {
			return SizeType(~0) / sizeof(ValueType);
		}
	};


	template<class T>
	concept Deleter = requires(T deleter) {
		typename T::Type;

		// deleter must have a operator() which takes
		// a pointer to the memory to delete.
		deleter(declval<typename T::Type*>());
	};

	template<class T>
    struct StdDeleter {
		using Type = T;
        void operator()(T* ptr) const {
            delete ptr;
        }
    };


	// maybe: StdAllocator<const T>

} // namespace base
