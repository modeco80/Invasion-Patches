#pragma once

#include <base/Fnv1a.hpp>

namespace base {

	/**
	 * Hash trait template. Provides a function
	 * to hash a given value of type T.
	 */
	template <class T>
	struct Hash {
		// Hash:
		// static uint32_t hash(const T&);
	};

#define HASH_TRIVIAL_SPECIALIZATION(T)                                                  \
	template <>                                                                         \
	struct Hash<T> {                                                                    \
		inline static uint32_t hash(const T& t) {                                       \
			return detail::fnv1a_hash(reinterpret_cast<const void*>(&t), sizeof(T), 0); \
		}                                                                               \
	};

	// trivial specializations for ... trivial types!
	HASH_TRIVIAL_SPECIALIZATION(uint8_t)
	HASH_TRIVIAL_SPECIALIZATION(uint16_t)
	HASH_TRIVIAL_SPECIALIZATION(uint32_t)
	HASH_TRIVIAL_SPECIALIZATION(uint64_t)
	HASH_TRIVIAL_SPECIALIZATION(int8_t)
	HASH_TRIVIAL_SPECIALIZATION(int16_t)
	HASH_TRIVIAL_SPECIALIZATION(int32_t)
	HASH_TRIVIAL_SPECIALIZATION(int64_t)

	// MSVC doesn't consider (u)intptr
	// as seperate types.
//#ifndef _MSC_VER
//	HASH_TRIVIAL_SPECIALIZATION(uintptr_t)
//	HASH_TRIVIAL_SPECIALIZATION(intptr_t)
//#endif

#undef HASH_TRIVIAL_SPECIALIZATION

} // namespace base
