#pragma once

#include <stdint.h>
#include <stddef.h>

namespace base::detail {

	/**
	 * FNV1a hash. Pretty self explanatory.
	 *
	 * \returns u32 hash value.
	 * \param[in] input The input data to hash
	 * \param[in] length Size of the input data.
	 * \param[in] lastval Last hashed value, or 0 for a fresh hash.
	 */
	uint32_t fnv1a_hash(const void* input, size_t length, uint32_t lastval);

} // namespace base::detail
