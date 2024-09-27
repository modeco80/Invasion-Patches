#pragma once

#include <stddef.h>

namespace base {

	/**
	 * A POD array.
	 */
	template <class T, size_t N>
	struct Array {
		constexpr size_t Size() {
			return N;
		}

		constexpr T* Data() {
			return &_arr_notouchy[0];
		}

		constexpr const T* Data() const {
			return &_arr_notouchy[0];
		}

		constexpr T& operator[](size_t index) {
			return _arr_notouchy[index];
		}

		constexpr const T& operator[](size_t index) const {
			return _arr_notouchy[index];
		}

		// only public to keep POD contract true
		T _arr_notouchy[N];
	};

} // namespace base
