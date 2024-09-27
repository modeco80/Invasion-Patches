template <class T>
struct __RemoveReference {
	using Type = T;
};

template <class T>
struct __RemoveReference<T&> {
	using Type = T;
};

template <class T>
struct __RemoveReference<T&&> {
	using Type = T;
};

template <class T>
struct __RemoveConst {
	using Type = T;
};

template <class T>
struct __RemoveConst<const T> {
	using Type = T;
};

template <class T>
struct __RemoveVolatile {
	using Type = T;
};

template <class T>
struct __RemoveVolatile<volatile T> {
	using Type = T;
};
