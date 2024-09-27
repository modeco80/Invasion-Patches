template <class T>
struct __MakeSigned {
	using Type = void;
};

template <>
struct __MakeSigned<signed char> {
	using Type = char;
};

template <>
struct __MakeSigned<signed short> {
	using Type = signed short;
};

template <>
struct __MakeSigned<signed int> {
	using Type = signed int;
};

template <>
struct __MakeSigned<signed long> {
	using Type = signed long;
};

template <>
struct __MakeSigned<signed long long> {
	using Type = signed long long;
};

template <>
struct __MakeSigned<unsigned char> {
	using Type = char;
};

template <>
struct __MakeSigned<unsigned short> {
	using Type = signed short;
};

template <>
struct __MakeSigned<unsigned int> {
	using Type = signed int;
};

template <>
struct __MakeSigned<unsigned long> {
	using Type = signed long;
};

template <>
struct __MakeSigned<unsigned long long> {
	using Type = signed long long;
};

template <>
struct __MakeSigned<char> {
	using Type = char;
};

template <>
struct __MakeSigned<char8_t> {
	using Type = char8_t;
};

template <>
struct __MakeSigned<char16_t> {
	using Type = char16_t;
};

template <>
struct __MakeSigned<char32_t> {
	using Type = char32_t;
};

template <>
struct __MakeSigned<bool> {
	using Type = bool;
};