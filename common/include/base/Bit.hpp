#pragma once

namespace base {
	template<class To, class From> requires(sizeof(To) == sizeof(From))
	constexpr To BitCast(From from) {
		return __builtin_bit_cast(To, from);
	}
}
