#pragma once

#include <base/Allocator.hpp>
#include <base/CharTraits.hpp>

namespace base {

	/**
	 * A "view" of a string. Does not own the memory,
	 * and when copied, simply copies its pointer/length.
	 * Essentially a special-cased span for strings,
	 * to avoid creating tons of String objects which manage their memory on the heap.
	 */
	template <class T, class Traits = CharTraits<T>>
	struct BasicStringView {
		constexpr BasicStringView() : data_ptr(nullptr), len(0) {}

		// Permit the compiler to force generate a trivial
		// copy constructor.
		constexpr BasicStringView(const BasicStringView&) = default;

		constexpr BasicStringView(const T* ptr, size_t len) : data_ptr(ptr), len(len) {}

		constexpr BasicStringView(const T* ptr) : data_ptr(ptr), len(strlen(ptr)) {}

		[[nodiscard]] constexpr size_t Length() const { return len; }

		constexpr const T* Data() const { return data_ptr; }

		constexpr const T* CStr() const { return data_ptr; }

		constexpr const T& operator[](size_t index) const { return data_ptr[index]; }

		int Find(T needle) const { return Traits::Find(CStr(), needle); }

		int Find(BasicStringView needle) const { return Traits::Find(CStr(), needle.CStr()); }

		friend constexpr bool operator==(const BasicStringView& lhs, const BasicStringView& rhs) {
			return !Traits::Compare(lhs.data_ptr, rhs.data_ptr);
		}

		friend constexpr bool operator!=(const BasicStringView& lhs, const BasicStringView& rhs) { return !(lhs == rhs); }

	   private:
		const T* data_ptr;
		size_t len;
	};

	template <class T, class Traits = CharTraits<T>, class Alloc = StdAllocator<T>>
	struct BasicString {
		using CharType = T;
		using SizeType = size_t;

		inline BasicString() = default;

		inline BasicString(SizeType length) { Resize(length); }

		inline BasicString(const T* cstr) {
			BASE_ASSERT(cstr != nullptr);
			CopyFromCString(cstr);
		}

		inline BasicString(const T* mem, SizeType length) {
			BASE_ASSERT(mem != nullptr);

			// TODO: maybe some interning.
			// 32 chars max intern, before it becomes an allocation.
			Resize(length);
			// memcpy(&memory[0], &mem[0], length * sizeof(T));
			Traits::Copy(&mem[0], &memory[0], length);
		}

		inline BasicString(BasicString&& move) noexcept {
			memory = move.memory;
			len = move.len;

			// invalidate the string we're moving from,
			// since this instance now owns the memory.

			move.memory = nullptr;
			move.len = 0;
		}

		inline BasicString(const BasicString& source) {
			// new buffer.
			Resize(source.len);
			Traits::Copy(&source.memory[0], &memory[0], source.len);
		}

		inline BasicString& operator=(const BasicString& copy) {
			if(this == &copy)
				return *this;

			// call copy constructor (for ease of implementation)
			new(this) BasicString(copy);
			return *this;
		}

		inline ~BasicString() { Resize(0); }

		inline const T* c_str() const { return memory; }

		[[nodiscard]] inline SizeType length() const { return len; }

		inline T& operator[](SizeType index) { return memory[index]; }

		inline const T& operator[](SizeType index) const { return memory[index]; }

		inline T* data() { return memory; }

		inline const T* data() const { return memory; }

		inline void Resize(SizeType newLength) {
			if(newLength == 0) {
				// Destroy the buffer, if we have one to destroy
				if(memory) {
					len = 0;
					alloc.Deallocate(memory);
				}
				return;
			}

			// save old buffer
			auto* old = memory;

			// allocate new buffer
			memory = alloc.Allocate(newLength + 1);
			memory[newLength] = '\0';

			if(old) {
				// copy the old buffer in
				// TODO: could probably maybe truncate for any length?
				if(len <= newLength)
					Traits::Copy(&old[0], &memory[0], len);

				// don't need it
				alloc.Deallocate(old);
			}
			len = newLength;
		}

		int Find(CharType needle) const { return Traits::Find(c_str(), needle); }

		int Find(const BasicString& needle) const { return Traits::Find(c_str(), needle.c_str()); }

		int Find(BasicStringView<CharType> needle) const { return Traits::Find(c_str(), needle.c_str()); }

		int FindLast(CharType needle) const { return Traits::FindLast(c_str(), needle); }

		inline BasicString substr(SizeType pos, SizeType len = -1) {
			if(pos > this->len)
				return "";

			if(len != -1) {
				if((pos + len) > this->len)
					return "";
			} else {
				len = 0;

				auto d = &data()[pos];

				// let's try to find a null terminator
				while(*d != '\0') {
					// If we weren't able to find the terminator, then give up,
					// and just say it's the end of the string.
					if(d == &data()[this->len]) {
						len = pos - this->len;
						break;
					}

					++len;
					++d;
				}
			}

			return BasicString(&data()[pos], len);
		}

		friend BasicString operator+(const BasicString& lhs, const BasicString& rhs) {
			BasicString string(lhs.length() + rhs.length());

			// Concat the strings together
			Traits::Copy(lhs.data(), string.data(), lhs.length());
			Traits::Copy(rhs.data(), string.data() + lhs.length(), rhs.length());
			return string;
		}

		BasicString& operator+=(const BasicString& rhs) {
			auto l = this->length();
			Resize(l + rhs.length());
			Traits::Copy(rhs.data(), data() + l, rhs.length());
			return *this;
		}

		BasicString& operator+=(CharType rhs) {
			auto l = this->length();
			Resize(l + 1);
			data()[l] = rhs;
			return *this;
		}

		// equality operators
		// operator== might need some work done to it.

		friend inline bool operator==(const BasicString& lhs, const BasicString& rhs) {
			// would probably need some work for introducing U8String
			return !Traits::Compare(lhs.data(), rhs.data());
		}

		friend inline bool operator!=(const BasicString& lhs, const BasicString& rhs) { return !(lhs == rhs); }

		operator BasicStringView<T>() { return BasicStringView<T>(data(), length()); }
		operator BasicStringView<T>() const { return BasicStringView<T>(c_str(), length()); }

	   private:
		void CopyFromCString(const T* cstr) {
			if(!cstr)
				return;

			const auto clen = Traits::Length(cstr);

			Resize(clen);
			Traits::Copy(cstr, data(), clen);
		}

		T* memory { nullptr };
		SizeType len {};
		Alloc alloc;
	};

	using String = BasicString<char>;
	using StringView = BasicStringView<char>;

} // namespace base
