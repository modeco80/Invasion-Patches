#pragma once

#include <stddef.h>
#include <string.h>

#include <base/Assert.hpp>

namespace base {

	/**
	 * Basic character traits.
	 */
	template <class CharT>
	struct CharTraits {
		// inline static std::size_t Length(const CharT* __restrict);
		// inline static void Copy(const CharT* __restrict, CharT* __restrict, size_t);
		// inline static int Compare(const CharT* __restrict, const CharT* __restrict);
	};

	/**
	 * Implementation of CharTraits<> for char.
	 */
	template <>
	struct CharTraits<char> {
		inline static uintptr_t Length(const char* __restrict str) {
			if(str == nullptr)
				return 0;
			return strlen(str);
		}

		inline static void Copy(const char* __restrict src, char* __restrict dest, size_t length) { memcpy(dest, src, length * sizeof(char)); }

		inline static int Compare(const char* __restrict str1, const char* __restrict str2) {
			// NB: It is impossible for str1 == str2,
			// due to the fact that we use __restrict here.

			// Nevertheless, we do need to assert for some idiot-cases.
			BASE_ASSERT(str1 != nullptr);
			BASE_ASSERT(str2 != nullptr);
			return strcmp(str1, str2);
		}

		inline static int Find(const char* __restrict src, char needle) {
			auto p = strchr(src, needle);
			if(p == nullptr)
				return -1;

			return (p - src);
		}

		inline static int Find(const char* __restrict src, const char* __restrict needle) {
			auto p = strstr(src, needle);
			if(p == nullptr)
				return -1;
			return (p - src);
		}

		inline static int FindLast(const char* __restrict src, char needle) {
			auto p = strrchr(src, needle);
			if(p == nullptr)
				return -1;

			return (p - src);
		}
	};

} // namespace base
