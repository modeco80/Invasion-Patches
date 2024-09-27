template <class T>
constexpr inline bool IsEnum = __is_enum(T);

template <class T>
constexpr inline bool IsUnion = __is_union(T);

template <class T>
constexpr inline bool IsClass = __is_class(T);

template <class TBase, class TMaybeDerived>
constexpr inline bool IsBaseOf = __is_base_of(TBase, TMaybeDerived);

template <class T>
constexpr inline bool IsTriviallyCopyable = __is_trivially_copyable(T);

template <class T>
	requires(IsEnum<T>)
using UnderlyingType = __underlying_type(T);

template <class T>
constexpr inline bool IsTrivial = __is_trivial(T);
