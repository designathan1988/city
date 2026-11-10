// The simulation clock: a fixed step (sim_tick_hz, data/catalog/tuning.json) on its own thread, decoupled from the frame
// rate (G. Fiedler, "Fix Your Timestep!", Gaffer On Games, 2004); at speed N each tick runs N steps.
#pragma once

#include "sim/state/city_state.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace city {

// Explicit states of the clock. Transitions: any -> any through set_speed(); stop() -> STOPPED.
enum class SimSpeed : int { PAUSED = 0, NORMAL = 1, FAST = 2, FASTEST = 3, STOPPED = 4 };

class Simulation {
public:
	void start(CityState &s, bool paused = false); // starts the tick thread, at NORMAL or, so that no tick runs before the caller says, PAUSED
	void stop(); // joins the thread; safe to call twice
	void set_speed(SimSpeed sp);
	SimSpeed speed() const { return speed_.load(); }
	uint64_t wakeups() const { return wakeups_.load(); } // turns of the tick thread's loop (probes: a pause turns none)
	int steps_per_tick(SimSpeed sp) const;
	// True once no tick of the thread runs or can run until the speed changes: the clock is stopped, or paused with the thread
	// parked (a tick in flight when the pause came is waited for). False while the clock runs. Whatever steps the city on the
	// caller's thread (tests, probes) asks this first, so its steps never interleave with the clock's.
	bool hold_still();
	// `ticks` ticks on the caller's thread (tests), only when hold_still(); refused (false) while the clock runs. Takes its
	// own locks: the caller must not hold map_mutex.
	bool step(CityState &s, int ticks);
	~Simulation() { stop(); }

private:
	void run();
	void tick(CityState &s, int steps, int64_t stamp_ns); // steps under the two locks, poses published once (at stamp_ns)
	CityState *s_ = nullptr;
	std::thread thread_;
	std::atomic<SimSpeed> speed_{ SimSpeed::STOPPED };
	std::atomic<bool> running_{ false };
	std::atomic<int64_t> period_ns_{ 0 };
	std::atomic<uint64_t> wakeups_{ 0 };
	// Paused, the thread sleeps on wake_ until set_speed() or stop(): they change the state under wake_mutex_ first, so no
	// change is missed and a stopped clock wakes nothing. parked_ (under wake_mutex_) is true while it sleeps there, and
	// parked_cv_ tells hold_still() when it gets there.
	std::mutex wake_mutex_;
	std::condition_variable wake_;
	std::condition_variable parked_cv_;
	bool parked_ = false;
	double owed_ = 0; // seconds of the damage clock the map lock was not free for
};

} // namespace city
