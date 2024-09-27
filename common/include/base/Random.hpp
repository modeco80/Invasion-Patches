#pragma once

#include <base/Stdint.hpp>

namespace base {

	/**
	 * Xorshift32 random number generator.
	 */
	u32 Random32();

	void SeedInit();

	/**
	 * Places 4 random bytes at a time into a buffer.
	 * The buffer must have a u32-aligned size.
	 */
	bool RandomBytesIntoBuffer(u8* ptr, usize count);

	void RandomString(char* ptr, size_t len);
}
