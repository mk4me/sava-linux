#pragma once

#include <boost/atomic.hpp>

namespace utils {
	class Spinlock {
	private:
		typedef enum {Locked, Unlocked} LockState;
		boost::atomic<LockState> state;

	public:
		Spinlock() : state(Unlocked) {}

		void lock()
		{
			while (state.exchange(Locked, boost::memory_order_acquire) == Locked) {
				/* busy-wait */
			}
		}
		void unlock()
		{
			state.store(Unlocked, boost::memory_order_release);
		}
	};
}