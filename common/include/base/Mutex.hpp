#pragma once

namespace base {

	/**
	 * A mutex.
	 */
	struct Mutex {
		Mutex();
		~Mutex();

		void Lock();
		void Unlock();

		// impl data.
	private:
		constexpr static auto IMPL_SIZE = 24;
		unsigned char impl_data[IMPL_SIZE];
	};

	template <typename T>
	concept Lockable = requires(T t) {
		{ t.Lock() };
		{ t.Unlock() };
	};

	/**
 	 * Scoped lock guard.
	 */
	template<typename Mut> requires(Lockable<Mut>)
	struct LockGuard {
		LockGuard(Mut& mtx)
			: mutex(mtx) {
			mutex.Lock();
		}

		~LockGuard() {
			mutex.Unlock();
		}
	private:
		Mut& mutex;
	};

}
