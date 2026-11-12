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
	double tick_load() const { return speed() == SimSpeed::PAUSED || speed() == SimSpeed::STOPPED ? 0.0 : tick_load_.load(); }
	int steps_per_tick(SimSpeed sp) const;
	// One step without the thread (tests). Takes its own locks: the caller must not hold map_mutex.
	void step(CityState &s);
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
	std::atomic<double> tick_load_{ 0 }; // active tick wall cost / requested period; sleep is excluded, lock contention included
	// Paused, the thread sleeps on wake_ until set_speed() or stop(): they change the state under wake_mutex_ first, so no
	// change is missed and a stopped clock wakes nothing.
	std::mutex wake_mutex_;
	std::condition_variable wake_;
	double owed_ = 0; // seconds of the damage clock the map lock was not free for
};

} // namespace city
