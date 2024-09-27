
#pragma once
#include <base/DynamicArray.hpp>
#include <base/TypeTraits.hpp>

namespace ui {
	namespace impl {

		enum class Constness {
			Const,
			Mut,
		};

		/// A typed event slot.
		template <class T>
		struct EventSlot;

		template <class Retty, class... Args>
		struct EventSlot<Retty(Args...)> {
			constexpr EventSlot() = default;
			EventSlot(const EventSlot&) = delete;
			EventSlot(EventSlot&&) = delete;

			~EventSlot() {
				for(auto i = 0z; i < functors.Size(); ++i) {
					delete functors[i];
				}
				functors.Clear();
				functors.ShrinkToFit();
			}

			// Abstractions R cool.
			struct Functor {
				virtual ~Functor() = default;
				constexpr virtual Retty operator()(Args&&... args) = 0;
			};

			// plain functor
			struct PlainFunctor : Functor {
				using PtrType = Retty (*)(Args...);

				constexpr PlainFunctor(PtrType functor) : Functor() { pFunctor = functor; }

				Retty operator()(Args&&... args) override { return pFunctor(static_cast<Args&&>(args)...); }

				// functor
				PtrType pFunctor;
			};

			template <class EnclosingClass, Constness Const = Constness::Mut>
			struct MemberFunctor : Functor {
				// clang-format off
				using PtrType = typename base::Conditional<
				Const == Constness::Const, 
					Retty (EnclosingClass::*)(Args...) const,  	// true
					Retty (EnclosingClass::*)(Args...)			// false
				>::Type;
				// clang-format on

				constexpr MemberFunctor(EnclosingClass* klass, PtrType functor) : Functor() {
					pBoundClass = klass;
					pFunctor = functor;
				}

				constexpr Retty operator()(Args&&... args) override {
					// thank god we have abstractions.
					return (pBoundClass->*pFunctor)(static_cast<Args&&>(args)...);
				}

				EnclosingClass* pBoundClass;
				Retty (EnclosingClass::*pFunctor)(Args...);
			};

			void AttachListener(PlainFunctor::PtrType pFunctor) {
				auto f = new PlainFunctor(pFunctor);
				functors.PushBack(f);
			}

			template <class EnclosingClass>
			void AttachListener(EnclosingClass* klass, typename MemberFunctor<EnclosingClass>::PtrType pFunctor) {
				auto f = new MemberFunctor<EnclosingClass>(klass, pFunctor);
				functors.PushBack(f);
			}

			void operator()(Args&&... args) {
				for(auto i = 0z; i < functors.Size(); ++i)
					(*functors[i])(static_cast<Args&&>(args)...);
			}

		   private:
			base::DynamicArray<Functor*> functors;
		};

	} // namespace impl

	using impl::EventSlot;
} // namespace ui