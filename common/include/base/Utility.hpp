#pragma once

#include <base/TypeTraits.hpp>
#include <stddef.h>
#include <string.h>

namespace base {

	template <class T>
	constexpr T&& Move(T&& t) {
		return static_cast<T&&>(t);
	}

	template <class T>
	constexpr T&& Forward(T&& t) {
		return static_cast<T&&>(t);
	}

	/**
	 * Pair. Simple enough.
	 * Data layout is POD, and C-compatible if used with C-compatible types.
	 */
	template <class T1, class T2>
	struct Pair {
		using FirstType = T1;
		using SecondType = T2;

		constexpr Pair() = default;

		/**
		 * Initialize the pair with values.
		 */
		constexpr Pair(T1&& t1, T2&& t2)
			: first(Move(t1)),
			  second(Move(t2)) {
		}

		T1 first;
		T2 second;
	};

	// deduction guide so Pair{1, 0.24} is well-formed.
	template <class T1, class T2>
	Pair(T1, T2) -> Pair<T1, T2>;

	/**
	 * Typed transfer utilities.
	 *
	 * Automatically switches to more/less performant code
	 * per each class.
	 */
	template <class T>
	struct TypedTransfer {
		inline static void Copy(T* dest, const T* source, size_t length) requires(IsTriviallyCopyable<T>) {
			// For performance reasons, if we can copy via memcpy(),
			// prefer that.
			memcpy(dest, source, length * sizeof(T));
		}

		inline static void Copy(T* dest, const T* source, size_t length) {
			// If we can't, oh well, that's OK too.
			for(size_t i = 0; i < length; ++i)
				new(&dest[i]) T(source[i]);
		}

		inline static void Move(T* dest, const T* source, size_t length) requires(IsTriviallyCopyable<T>) {
			memmove(dest, source, length * sizeof(T));
		}

		inline static void Move(T* dest, const T* source, size_t length) {
			// If we can't, oh well, that's OK too.
			for(size_t i = 0; i < length; ++i)
				new(&dest[i]) T(Move<T>(source[i]));
		}
	};

} // namespace base
