#pragma once

namespace base {

	namespace detail {

		template <class T, T value>
		struct IntegralConstant {
			using ValueType = T;
			using Type = IntegralConstant;
			constexpr static ValueType Value = value;

			constexpr operator ValueType() const {
				return value;
			}

			constexpr ValueType operator()() const {
				return value;
			}
		};

		using TrueType = IntegralConstant<bool, true>;
		using FalseType = IntegralConstant<bool, false>;

#include <base/bits/traits/Remove.hpp>

		template <class T>
		using RemoveConst = __RemoveConst<T>;

		template <class T>
		using RemoveVolatile = __RemoveConst<T>;

		template <class T>
		using RemoveCV = RemoveConst<typename RemoveVolatile<T>::Type>;

		template <class T>
		using RemoveReference = __RemoveReference<T>;

		template <class T>
		using RemoveCVReference = RemoveCV<typename RemoveReference<T>::Type>;

#include <base/bits/traits/MakeSigned.hpp>
#include <base/bits/traits/MakeUnsigned.hpp>

		template <class T>
		using MakeUnsigned = typename __MakeUnsigned<T>::Type;

		template <class T>
		using MakeSigned = typename __MakeSigned<T>::Type;

		template <bool Cond, class TrueT, class FalseT>
		struct __ConditionalImpl {
			using Type = TrueT;
		};

		template <class TrueT, class FalseT>
		struct __ConditionalImpl<false, TrueT, FalseT> {
			using Type = FalseT;
		};

		template <bool Cond, class TrueT, class FalseT>
		using Conditional = __ConditionalImpl<Cond, TrueT, FalseT>;

		template <class T>
		consteval auto declval() -> T;

#include <base/bits/traits/CompilerIntrinTraits.hpp>

		// clang-format off

		template <class T>
		constexpr UnderlyingType<T> UnderlyingValue(T value) requires(IsEnum<T>) {
			return static_cast<UnderlyingType<T>>(value);
		}

		// clang-format on

	} // namespace detail

	using detail::FalseType;
	using detail::IntegralConstant;
	using detail::TrueType;

	using detail::RemoveConst;
	using detail::RemoveCV;
	using detail::RemoveCVReference;
	using detail::RemoveReference;
	using detail::RemoveVolatile;

	using detail::MakeSigned;
	using detail::MakeUnsigned;

	using detail::declval;

	using detail::IsClass;
	using detail::IsEnum;
	using detail::IsTrivial;
	using detail::IsUnion;
	using detail::IsTriviallyCopyable;

	using detail::UnderlyingType;
	using detail::UnderlyingValue;
} // namespace base
