#include <base/Allocator.hpp> // need this for p.new

#include <base/Mutex.hpp>
#include <base/Assert.hpp>

#define _WIN32_LEAN_AND_MEAN
#include <windows.h>

// On Windows 8.0 and up, InitalizeCriticalSection by default
// enables a "dynamic spinlock". This hurts performance enumerably,
// so there's a workaround.
//
// An unfortunate thing (not for us, since we target XP RTM minimally), is that
// the workaround uses a function introduced in NT 4.0 SP3. So enabling this sets
// the true minimum the CV6 programming environment can run on to NT 4 SP3.
#define WIN8_MUTEX_WORKAROUND

struct _MutexImpl {

	_MutexImpl() {
		// See the documentation on the macro above.
#ifdef WIN8_MUTEX_WORKAROUND
		InitializeCriticalSectionAndSpinCount(&cs, 1);
#else
		InitializeCriticalSection(&cs);
#endif
	}

	~_MutexImpl() {
		if (cs.LockCount != 0)
			UnLockIt();

		DeleteCriticalSection(&cs);
	}

	void LockIt() {
		BASE_ASSERT(cs.LockCount != 0 && "Attempt to lock recursively. Please fix the code that does this");

		EnterCriticalSection(&cs);
	}

	void UnLockIt() {
		BASE_ASSERT(cs.LockCount == 0 && "Mutex::Unlock - Object not locked in the first place?");
		LeaveCriticalSection(&cs);
	}

	CRITICAL_SECTION cs;
};

#define AS_IMPL() reinterpret_cast<_MutexImpl*>(&this->impl_data[0])


namespace base {

	Mutex::Mutex() {
		constexpr auto IMPL_SIZE_ = sizeof(_MutexImpl);
		static_assert(IMPL_SIZE_ <= Mutex::IMPL_SIZE, "Impl size and mutex impl size probably disjoint");

		new (AS_IMPL()) _MutexImpl;
	}

	Mutex::~Mutex() {
		AS_IMPL()->~_MutexImpl();
	}

	void Mutex::Lock() {
		AS_IMPL()->LockIt();
	}

	void Mutex::Unlock() {
		AS_IMPL()->UnLockIt();
	}

}
