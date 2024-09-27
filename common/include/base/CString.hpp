#pragma once
//#include <base/Stdint.hpp>

namespace base {

	constexpr auto Strlen(const char* str) {
		auto newptr = str;
		while(*(++newptr));
		return (newptr - str);
	}

}
