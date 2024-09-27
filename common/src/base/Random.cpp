#include <base/Random.hpp>
#include <base/Bit.hpp>
#include <base/SaneWin.hpp>

namespace base {

	// preinitalized state/seed for xorshift
	// random number generation, which we build upon
	// in SeedInit()

	// XorShift32 state "Lily"
	static u32 xorshift32_state = 0x796C694C;

	u32 Random32() {
		u32 x = xorshift32_state;
		x ^= x << 13;
		x ^= x << 17;
		x ^= x >> 5;
		return xorshift32_state = x;
	}

	void SeedInit() {
		u32 seed_populate;

		// do some initial excercise of the state
		u32 max = (GetCurrentProcessId() ^ GetTickCount());
		for (u32 i = 0; i < max; ++i)
			seed_populate = Random32();

		// then reinitalize, fudging with the last calculated constant
		xorshift32_state = seed_populate * max;
	}

	bool RandomBytesIntoBuffer(u8* ptr, size_t count) {
#ifdef DEBUG
		if (!ptr)
			return false;

		// assert here.
		if(!(count & 3))
			return false;
#endif
		auto* IntView = BitCast<u32*>(ptr);
		for (size_t i = 0; i < count; ++i)
			*IntView++ = Random32();
		return true;
	}

	constexpr static char WORD_TABLE[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

	void RandomString(char* ptr, size_t len) {
		size_t i = 0;
		for(i = 0; i < len; ++i)
			ptr[i] = WORD_TABLE[Random32() % (sizeof(WORD_TABLE) - 1)];

		ptr[len] = '\0';
	}

}
