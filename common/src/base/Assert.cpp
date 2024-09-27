// This file implements callbacks called into by BASE_ASSERT().

#include <base/Debug.hpp>

extern "C" void __lilycrt_exit_fast(int code);

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NDEBUG
	void __Base__AssertFailure(const char* exp, const char* function, const char* file, unsigned line) {
		BASE_DEBUG("BASE_ASSERT(%s) failed in %s:%d(%s)", exp, file, line, function);
		Sleep(15000);
		__lilycrt_exit_fast(0xA55341FA); // ASSERTFAilure
	}
#endif

#ifdef __cplusplus
}
#endif
