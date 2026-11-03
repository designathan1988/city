// The simulation clock (Roadcraft app/clock.ts + sim/worker.ts): a fixed 50 ms step on its own thread; at speed N
// each tick runs N steps. One city minute passes per real second at 1x.
#pragma once

#include "sim/state/city_state.h"

#include <atomic>
#include <thread>

namespace city {

// Explicit states of the clock. Transitions: any -> any through set_speed(); stop() -> STOPPED.
enum class SimSpeed : int { PAUSED = 0, NORMAL = 1, FAST = 2, FASTEST = 3, STOPPED = 4 };

class Simulation {
public:
	void start(CityState &s); // starts the tick thread (speed NORMAL)
	void stop(); // joins the thread; safe to call twice
	void set_speed(SimSpeed sp);
	SimSpeed speed() const { return speed_.load(); }
	int steps_per_tick(SimSpeed sp) const;
	// Share of the current tick period elapsed since the last tick (0..1), for render interpolation.
	float interpolation() const;
	// One step without the thread (tests). Caller holds world_mutex.
	void step(CityState &s);
	~Simulation() { stop(); }

private:
	void run();
	void tick(CityState &s, int steps); // steps under the two locks, poses published once
	CityState *s_ = nullptr;
	double owed_ = 0; // growth time deferred while map_mutex was busy
	std::thread thread_;
	std::atomic<SimSpeed> speed_{ SimSpeed::STOPPED };
	std::atomic<bool> running_{ false };
	std::atomic<int64_t> last_tick_ns_{ 0 };
	std::atomic<int64_t> period_ns_{ 0 };
};

} // namespace city
