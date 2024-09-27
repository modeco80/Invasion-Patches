#pragma once

#include <base/SaneWin.hpp>
//#include <base/CString.hpp>

#ifdef DEBUG
#include <string.h>
#endif

namespace base {

#ifdef DEBUG
#define BASE_DEBUG(fmt, ...) base::Debug(fmt "\n", ##__VA_ARGS__)
#else
#define BASE_DEBUG(fmt, ...)
#endif

	template<class...Args>
	inline void Debug(const char* fmt, Args&&... args) {
#ifdef DEBUG
		char egg[2048]{};
		wsprintfA(&egg[0], fmt, args...);

		WriteConsole(
			GetStdHandle(STD_OUTPUT_HANDLE),
			egg,
			strlen(egg),
			nullptr,
			nullptr
		);
#endif
	}
}
