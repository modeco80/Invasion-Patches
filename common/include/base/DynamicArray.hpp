#pragma once

#include <base/Allocator.hpp>
#include <base/Utility.hpp>
#include <base/Debug.hpp>

namespace base {

	/**
	 * A simple dynamic array.
	 */
	template <class Elem, class Alloc = StdAllocator<Elem>>
	struct DynamicArray {
		using ValueType = typename RemoveCVReference<Elem>::Type;
		using SizeType = size_t;
		using Reference = ValueType&;
		using ConstReference = const ValueType&;
		using Pointer = ValueType*;
		using ConstPointer = const ValueType*;

		constexpr DynamicArray() {
			size = 0;
			// Reserve at least 4 elements so we don't have to allocate right away
			Reserve(4);
		};

		inline DynamicArray(const DynamicArray& other) {
			Resize(other.capacity);
			TypedTransfer<Elem>::Copy(rawArray, other.rawArray, other.capacity);
		}

		inline DynamicArray(DynamicArray&& move) noexcept {
			rawArray = move.rawArray;
            capacity = move.capacity;

			// invalidate what we're moving from,
			// since this instance now owns the memory.
			move.rawArray = nullptr;
			move.capacity = 0;
		}

		inline DynamicArray& operator=(const DynamicArray& copy) {
			if(this == &copy)
				return *this;

			// call copy constructor (for ease of implementation)
			new(this) DynamicArray(copy);
			return *this;
		}

		inline DynamicArray& operator=(DynamicArray&& move) {
			if (this == &move)
				return *this;

			return *new(this) DynamicArray(move);
		}

		constexpr ~DynamicArray() {
			Reserve(0);
		}

		void Reserve(SizeType newCapacity) {
            auto destroy = [&](Elem* ptr, SizeType size) {
                for(SizeType i = 0; i < size; ++i)
                    ptr[i].~Elem();
                alloc.Deallocate(ptr);
            };

			if(newCapacity == 0) {
				// Sentinel value indicating we need to delete array.
				if(capacity != 0) {
                    destroy(rawArray, capacity);
                    capacity = 0;
                    size = 0;
					return;
				}
			}

			if(newCapacity < capacity) {
				return;
			}

			//BASE_DEBUG("Reserve() to new capacity %d\n", newCapacity);

			// Keep old data so we can copy it before nuking it from orbit
			auto oldArray = rawArray;
			auto oldCapacity = capacity;

			rawArray = alloc.Allocate(newCapacity);

			if(oldArray) {
                // Copy the old data to the new buffer
				TypedTransfer<Elem>::Copy(rawArray, oldArray, oldCapacity);

				// Initalize newly reserved elements by default construction
				for(SizeType i = oldCapacity; i < newCapacity; ++i)
					alloc.Construct(&rawArray[i]);

				// don't need the old array anymore
                destroy(oldArray, oldCapacity);
			} else {
				// Activate storage lifetime by default-construction
				for(SizeType i = 0; i < newCapacity; ++i)
					alloc.Construct(&rawArray[i]);
			}

            capacity = newCapacity;
		}

        void Resize(SizeType newSize) {
            if(newSize >= capacity) {
				//BASE_DEBUG("new size %d needs to reserve %d capacity (prev %d)\n", newSize, newSize + 8, size);
                Reserve(newSize + 8);
			} else {
				// Destroy
				for(SizeType i = newSize; i < size; ++i)
                 rawArray[i].~Elem();
			}

            size = newSize;
        }

        void PushBack(const Elem& elem) {
			//BASE_DEBUG("PushBack() current size %d\n", size);
            Resize(size + 1);
            rawArray[size-1] = elem;
        }

        [[nodiscard]] constexpr SizeType Capacity() const {
            return capacity;
        }

		[[nodiscard]] constexpr SizeType Size() const {
			return size;
		}

		[[nodiscard]] constexpr Pointer Data() {
			return &rawArray[0];
		}

		[[nodiscard]] constexpr ConstPointer Data() const {
			return &rawArray[0];
		}

		[[nodiscard]] constexpr Reference At(size_t index) {
			BASE_ASSERT(index <= size);
			return rawArray[index];
		}

		[[nodiscard]] constexpr ConstReference At(size_t index) const {
			BASE_ASSERT(index <= size);
			return rawArray[index];
		}

		constexpr Reference operator[](size_t index) {
			// If this is a debug build enforce
			// size checking using At() for verification,
			// otherwise just index into the array lmao
#ifdef DEBUG
			return At(index);
#else
			return rawArray[index];
#endif
		}

		constexpr ConstReference operator[](size_t index) const {
#ifdef DEBUG
			return At(index);
#else
			return rawArray[index];
#endif
		}

	   private:
		Alloc alloc;
		Pointer rawArray { nullptr };
		SizeType capacity { 0 }; // note that this is in Elem, not bytes
		SizeType size{0};
	};

} // namespace base
