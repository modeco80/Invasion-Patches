#pragma once

#include <base/Assert.hpp>
#include <base/Utility.hpp>
#include <base/Allocator.hpp>

namespace base {

	// MAYBE: Move this to a shared spot, so we can do
	// Maybe<T> (or Optional<T>)
	namespace detail {

		/**
		 * A safe deferred container for class types.
		 * Performs no heap allocations.
		 */
		template <class T>
		struct DeferredHolderFor {
			constexpr ~DeferredHolderFor() {
				if(constructed)
					Get()->~T();
			}

			constexpr DeferredHolderFor() = default; // do not initalize anything

			explicit DeferredHolderFor(const T& t) {
				// call the copy constructor
				Construct(t);
			}

			constexpr DeferredHolderFor& operator=(const DeferredHolderFor& df) {
				if(this == &df)
					return *this;

				(*this->Get()) = (*df->Get());
			}

			constexpr DeferredHolderFor& operator=(const T& t) {
				Construct(t);
			}

			template <class... Args>
			constexpr void Construct(Args&&... args) {
				if(constructed)
					Destruct();

				// construct new T
				constructed = true;
				new(Get()) T(Forward<Args>(args)...);
			}

			constexpr void Destruct() {
				if(constructed) {
					constructed = false;
					Get()->~T();
				}
			}

			[[nodiscard]] constexpr bool IsConstructed() const {
				return constructed;
			}

			constexpr T& GetConstructed() {
				BASE_ASSERT(constructed);
				return *Get();
			}

			constexpr const T& GetConstructed() const {
				BASE_ASSERT(constructed);
				return *Get();
			}

		   private:
			constexpr T* Get() {
				return UBCast<T*>(&storage[0]);
			}

			constexpr const T* Get() const {
				return UBCast<const T*>(&storage[0]);
			}

			bool constructed { false };
			alignas(T) uint8_t storage[sizeof(T)] {};
		};

	} // namespace detail

	/**
	 * A class template for working with deterministic errors.
	 * \tparam T Return type. Can be void
	 * \tparam E Error type.
	 */
	template <class T, class E>
	struct Expected {
		constexpr Expected() = default;

		constexpr Expected(const T& t) // NOLINT
			: t(t) {
		}

		constexpr Expected(const E& e) // NOLINT
			: e(e) {
		}

		constexpr Expected& operator=(const T& t) {
			if(e.IsConstructed())
				e.Destruct();

			this->t = t;
			return *this;
		}

		constexpr Expected& operator=(const E& e) {
			if(t.IsConstructed())
				t.Destruct();

			this->e = e;
			return *this;
		}

		inline Expected& operator=(const Expected& e) {
			if(this == &e)
				return *this;

			if(e.HasValue())
				this->t = e.Value();

			if(e.HasError())
				this->e = e.Error();

			return *this;
		}

		[[nodiscard]] constexpr bool HasError() const {
			return e.IsConstructed();
		}

		[[nodiscard]] constexpr bool HasValue() const {
			return t.IsConstructed();
		}

		constexpr T& Value() {
			BASE_ASSERT(HasValue() && !HasError());
			return t.GetConstructed();
		}

		constexpr E& Error() {
			BASE_ASSERT(!HasValue() && HasError());
			return e.GetConstructed();
		}

		constexpr const T& Value() const {
			BASE_ASSERT(HasValue() && !HasError());
			return t.GetConstructed();
		}

		constexpr const E& Error() const {
			BASE_ASSERT(!HasValue() && HasError());
			return e.GetConstructed();
		}

	   private:
		detail::DeferredHolderFor<T> t;
		detail::DeferredHolderFor<E> e;
	};

	template <class E>
	struct Expected<void, E> {
		constexpr Expected() = default;

		constexpr Expected(const E& e) // NOLINT
			: e(e) {
		}

		constexpr Expected& operator=(const E& e) {
			this->e = e;
			return *this;
		}

		inline Expected& operator=(const Expected& e) {
			if(this == &e)
				return *this;

			if(e.HasError())
				this->e = e.Error();

			return *this;
		}

		[[nodiscard]] constexpr bool HasError() const {
			return e.IsConstructed();
		}

		constexpr E& Error() {
			BASE_ASSERT(HasError());
			return e.GetConstructed();
		}

		constexpr const E& Error() const {
			BASE_ASSERT(HasError());
			return e.GetConstructed();
		}

	   private:
		detail::DeferredHolderFor<E> e;
	};

	template <class E>
	using VoidExpected = Expected<void, E>;

#ifdef _WIN32
	template<class T>
	using Win32Expected = base::Expected<T, HRESULT>;
#endif


	// These macros allow a sort of "fake" try/catch pattern:
	// 	  
	// BASE_EXPECTED_TRY(expression_which_returns_Expected) {
	//    // "Though" block - expression returned a value
	// } BASE_EXPECTED_CATCH() {
	//    // Catch block - expression returned an error.
	// }
	// BASE_EXPECTED_ENDBLOCK

	// TODO: It *MIGHT* be worth synthesizing a scope guard that uses RAII
	// to execute the finally block.

	//
	// For correctness, the else branch should have (res.IsError()),
	// but we can assume !Expected::HasValue() == stores error.
	// Omits a branch, which is good.
	//
#define BASE_EXPECTED_TRY(x) { auto res = (x); if(res.HasValue()) goto __though; else goto __catch; __though:
#define BASE_EXPECTED_CATCH() __catch:
//#define BASE_EXPECTED_THOUGH() __though:

	// Use to exit the TRY() block
#define BASE_EXPECTED_END(...) __VA_ARGS__; goto __catch_finally;

	// Refer to the todo here
#define BASE_EXPECTED_FINALLY() __catch_finally:

#define BASE_EXPECTED_ENDBLOCK }
#define BASE_EXPECTED_ENDBLOCK_NOFINALLY __catch_finally: {}}

	// Allows for nicer syntax for returning no error from
	// VoidExpected<E> functions.

	//	template <class E>
	//constexpr static VoidExpected<E> NO_ERROR {};

} // namespace base
