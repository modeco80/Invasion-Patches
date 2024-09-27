template <class T>
struct __MakeUnsigned {
	using Type = void;
};

template <>
struct __MakeUnsigned<signed char> {
	using Type = unsigned char;
};

template <>
struct __MakeUnsigned<signed short> {
	using Type = unsigned short;
};

template <>
struct __MakeUnsigned<signed int> {
	using Type = unsigned int;
};

template <>
struct __MakeUnsigned<signed long> {
	using Type = unsigned long;
};

template <>
struct __MakeUnsigned<signed long long> {
	using Type = unsigned long long;
};

template <>
struct __MakeUnsigned<unsigned char> {
	using Type = unsigned char;
};

template <>
struct __MakeUnsigned<unsigned short> {
	using Type = unsigned short;
};

template <>
struct __MakeUnsigned<unsigned int> {
	using Type = unsigned int;
};

template <>
struct __MakeUnsigned<unsigned long> {
	using Type = unsigned long;
};

template <>
struct __MakeUnsigned<unsigned long long> {
	using Type = unsigned long long;
};

template <>
struct __MakeUnsigned<char> {
	using Type = unsigned char;
};

template <>
struct __MakeUnsigned<char8_t> {
	using Type = char8_t;
};

template <>
struct __MakeUnsigned<char16_t> {
	using Type = char16_t;
};

template <>
struct __MakeUnsigned<char32_t> {
	using Type = char32_t;
};

template <>
struct __MakeUnsigned<bool> {
	using Type = bool;
};