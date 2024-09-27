// This header provides BASE_ASSERT() and BASE_VERIFY(),
// alongside definitions of the internal functions
// which the two macros dispatch to.
//
// BASE_ASSERT() behaves similarly to assert(),
// in that when in release, the statement boils away,
// and the runtime code for it also is gone.
//
// BASE_VERIFY(), however, does not go away,
// and is always active regardless of mode.
//
// Rule is to put BASE_ASSERT() where something should
// ideally never happen, and put BASE_VERIFY() where it
// absolutely, totally, should not, ever in the history of the
// universe happen, and if it does, it would wreak havoc on all
// of Earth.
//
// I'm only kidding of course, but, for the most part,
// BASE_VERIFY() is used for sanity checking I'd always like in.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NDEBUG
void __Base__AssertFailure(const char* exp, const char* function, const char* file, unsigned line);
#endif

#ifdef __cplusplus
}
#endif

#ifndef NDEBUG
	#define BASE_ASSERT(x)                                     \
		do {                                                     \
			if(!(x)) {                                           \
				__Base__AssertFailure(#x, __func__, __FILE__, __LINE__); \
				/*__builtin_unreachable();*/                         \
			}                                                    \
		} while(0)
#else
	#define BASE_ASSERT(x)
#endif
