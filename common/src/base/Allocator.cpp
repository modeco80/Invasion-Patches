#include <base/Allocator.hpp>
#include <base/Assert.hpp>

#define _WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace base {

	// Right now, we only need dword alignment, so this is fine
	// if we want to do more specific alignment we can

	/*void* AllocAligned(uint32_t count) {
		auto raw_pointer = Alloc(count + sizeof(uint32_t));
		auto value = reinterpret_cast<uintptr_t>(raw_pointer);
		value += (-value) & sizeof(uint32_t);

		// prepare the returned pointer by putting in the original malloc address
		auto* ret_pointer = reinterpret_cast<void*>(value);
		reinterpret_cast<uintptr_t*>(ret_pointer)[-1] = reinterpret_cast<uintptr_t>(raw_pointer);

		// util::DebugOut("debug(malloc): real pointer is %p", raw_pointer);
		return ret_pointer;
	}

	void FreeAligned(void* p) {
		auto real_pointer = reinterpret_cast<void*>(reinterpret_cast<uintptr_t*>(p)[-1]);
		// util::DebugOut("debug(free): real pointer is %p", real_pointer);
		if(real_pointer != nullptr)
			Free(real_pointer);
	}
	*/

	void* Alloc(size_t size) {
		return HeapAlloc(GetProcessHeap(), 0, size);
	}

	void Free(void* ptr) {
		HeapFree(GetProcessHeap(), 0, ptr);
	}

} // namespace base

void* operator new(size_t size) {
	auto* p = base::Alloc(size);
	BASE_ASSERT(p != nullptr && "Alloc() returned nullptr!!!");
	return p;
}

void* operator new[](size_t size) {
	auto* p = base::Alloc(size);
	BASE_ASSERT(p != nullptr && "Alloc() returned nullptr!!!");
	return p;
}

void operator delete(void* ptr) noexcept {
	if(ptr) // satisifies that delete/delete[] shouldn't break if nullptr is deleted
		base::Free(ptr);
}

void operator delete(void* ptr, size_t) noexcept {
	if(ptr) // satisifies that delete/delete[] shouldn't break if nullptr is deleted
		base::Free(ptr);
}

void operator delete[](void* ptr) noexcept {
	if(ptr) // satisifies that delete/delete[] shouldn't break if nullptr is deleted
		base::Free(ptr);
}

void operator delete[](void* ptr, size_t) noexcept {
	if(ptr) // satisifies that delete/delete[] shouldn't break if nullptr is deleted
		base::Free(ptr);
}

// placement new/delete support

void* operator new(size_t, void* p) noexcept {
	return p;
}

void* operator new[](size_t, void* p) noexcept {
	return p;
}

void operator delete(void*, void*) noexcept {
}

void operator delete[](void*, void*) noexcept {
}
