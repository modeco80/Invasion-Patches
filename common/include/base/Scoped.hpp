#pragma once
#include <base/TypeTraits.hpp>
#include <base/Memory.hpp>

namespace base {

    /** A scoped pointer, which will be deleted when this object loses scope.
        Essentially unique_ptr<T>. **/
    template<class T, Deleter _Deleter = StdDeleter<T>>
    struct Scoped {
        using Pointer = T*;

        constexpr Scoped() = default;

        constexpr Scoped(Pointer ptr)
            : ptr(ptr) {

        }

        Scoped(const Scoped&) = delete;
        Scoped& operator=(const Scoped&) = delete;

        Scoped(Scoped&& move) {
            ptr = move.ptr;
            move.ptr = nullptr;
        }

        Scoped& operator=(Scoped&& move) {
            if(&move == &this)
                return *this;

            ptr = move.ptr;
            move.ptr = nullptr;
        }

        ~Scoped() {
            if(ptr)
                _Deleter{}(ptr); // static operator()!!!
        }

        Pointer Get() const {
            return ptr;
        }

        operator T*() const {
            return ptr;
        }

        Pointer operator->() const {
            return ptr;
        }

        // addrof operator (laterz;)

    private:
        Pointer ptr{nullptr};
    };

    template<class T, class ...Args>
    auto MakeScoped(Args&&... args) {
        return Scoped<T, StdDeleter<T>>{ new T(static_cast<Args&&>(args)...) };
    }
}
