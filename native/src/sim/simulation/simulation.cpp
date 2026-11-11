// Tick thread, speed state machine and the city clock.
#include "sim/simulation/simulation.h"

#include "core/units.h"
#include "sim/traffic/traffic_system.h"

#include <chrono>
#include <cmath>

namespace city {

namespace {
int64_t now_ns() {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

constexpr int SPEEDS = int(SimSpeed::STOPPED) + 1;
constexpr int MOVES = int(SpeedMove::STOP) + 1;
constexpr uint8_t speed_bit(SimSpeed s) { return uint8_t(1u << int(s)); }
constexpr uint8_t RUNNING = speed_bit(SimSpeed::PAUSED) | speed_bit(SimSpeed::NORMAL) | speed_bit(SimSpeed::FAST) | speed_bit(SimSpeed::FASTEST);
constexpr uint8_t STOP_ONLY = speed_bit(SimSpeed::STOPPED);
// The speeds each move may lead to, by the speed it leaves (rows: SpeedMove; columns: SimSpeed, STOPPED last): a start leaves
// STOPPED for PAUSED or NORMAL and nothing else, a change goes between the running speeds, a stop ends any (twice is no harm).
constexpr uint8_t SPEED_MOVES[MOVES][SPEEDS] = {
	{ 0, 0, 0, 0, uint8_t(speed_bit(SimSpeed::PAUSED) | speed_bit(SimSpeed::NORMAL)) },
	{ RUNNING, RUNNING, RUNNING, RUNNING, 0 },
	{ STOP_ONLY, STOP_ONLY, STOP_ONLY, STOP_ONLY, STOP_ONLY },
};
} // namespace

double CityClock::hour() const {
	double m = std::fmod(minutes.load() + day_shift.load(), double(MINUTES_PER_DAY));
	if (m < 0) m += MINUTES_PER_DAY;
	return m / MINUTES_PER_HOUR;
}

void CityClock::set_hour(double h) { day_shift.store(h * MINUTES_PER_HOUR - minutes.load()); }

void Simulation::start(CityState &s, bool paused) {
	stop();
	s_ = &s;
	period_ns_ = int64_t(NS_PER_S / double(s.catalog.tune.sim_tick_hz));
	enter(SpeedMove::START, paused ? SimSpeed::PAUSED : SimSpeed::NORMAL); // stop() above left it STOPPED
	running_ = true;
	thread_ = std::thread([this]() { run(); });
}

void Simulation::stop() {
	{
		std::lock_guard<std::mutex> lk(wake_mutex_);
		running_ = false;
	}
	wake_.notify_all();
	if (thread_.joinable()) thread_.join();
	enter(SpeedMove::STOP, SimSpeed::STOPPED);
}

void Simulation::set_speed(SimSpeed sp) {
	if (enter(SpeedMove::CHANGE, sp)) wake_.notify_all(); // a stopped clock, or a stop asked here, is refused by the table
}

bool Simulation::enter(SpeedMove by, SimSpeed next) {
	std::lock_guard<std::mutex> lk(wake_mutex_);
	if (!(SPEED_MOVES[int(by)][int(speed_.load())] & speed_bit(next))) return false;
	speed_.store(next);
	return true;
}

int Simulation::steps_per_tick(SimSpeed sp) const {
	if (!s_) return 0;
	switch (sp) {
		case SimSpeed::NORMAL: return 1;
		case SimSpeed::FAST: return int(s_->catalog.tune.sim_speed_fast);
		case SimSpeed::FASTEST: return int(s_->catalog.tune.sim_speed_fastest);
		default: return 0;
	}
}

void Simulation::tick(CityState &s, int steps, int64_t stamp_ns) {
	const double dt = 1.0 / s.catalog.tune.sim_tick_hz;
	for (int k = 0; k < steps; k++) {
		bool growing = false, chosen = false;
		{
			std::shared_lock<std::shared_mutex> map(s.map_mutex);
			std::lock_guard<std::mutex> tr(s.traffic_mutex);
			traffic::step(s, float(dt));
			growing = s.traffic_levels.growth_on && !s.growth.cells.empty();
			chosen = !s.traffic.flow_control.changed.empty();
		}
		if (chosen) { // rare: the mesh jobs read the control off the network, so it changes under the exclusive lock
			map_hold::WriterHold map(s.map_mutex, map_hold::Writer::TICK);
			std::lock_guard<std::mutex> tr(s.traffic_mutex);
			traffic::apply_flow_control(s);
		}
		s.clock.minutes.store(s.clock.minutes.load() + dt * s.catalog.tune.sim_minutes_per_second);
		// It takes the map itself: shared to decide, exclusive only to add (city_growth.h). The add waits its turn: a mesh job's
		// short hold must not decide on which tick a building grows.
		if (growing) grow_step(s, dt);
		{
			map_hold::WriterHold map(s.map_mutex, map_hold::Writer::TICK, std::try_to_lock); // mesh jobs may hold it shared: the damage clock is owed what it missed
			if (map.owns_lock()) {
				damage_step(s, owed_ + dt);
				owed_ = 0;
			} else owed_ += dt;
		}
	}
	{
		std::shared_lock<std::shared_mutex> map(s.map_mutex);
		std::lock_guard<std::mutex> tr(s.traffic_mutex);
		traffic::publish_poses(s, stamp_ns);
	}
}

bool Simulation::hold_still() {
	std::unique_lock<std::mutex> lk(wake_mutex_);
	const SimSpeed sp = speed_.load();
	if (sp == SimSpeed::STOPPED) return true; // no thread
	if (sp != SimSpeed::PAUSED) return false;
	// set_speed(PAUSED) returns while the thread may still be inside a tick: its steps would land among the caller's.
	parked_cv_.wait(lk, [this]() { return parked_ || !running_; });
	return true;
}

bool Simulation::step(CityState &s, int ticks) {
	if (!hold_still()) return false;
	for (int k = 0; k < ticks; k++) tick(s, 1, now_ns());
	return true;
}

void Simulation::run() {
	constexpr int MAX_CATCHUP_TICKS = 2;
	int64_t deadline = now_ns();
	while (running_) {
		wakeups_++;
		if (speed_.load() == SimSpeed::PAUSED) { // nothing to step: wait for the speed or stop(), not for the clock
			std::unique_lock<std::mutex> lk(wake_mutex_);
			parked_ = true;
			parked_cv_.notify_all();
			wake_.wait(lk, [this]() { return !running_ || speed_.load() != SimSpeed::PAUSED; });
			parked_ = false;
			deadline = now_ns(); // the clock goes on from now, not from where it stopped
			continue;
		}
		deadline += period_ns_.load();
		int64_t wait = deadline - now_ns();
		if (wait > 0) {
			std::this_thread::sleep_for(std::chrono::nanoseconds(wait));
		} else if (-wait > MAX_CATCHUP_TICKS * period_ns_.load()) {
			deadline = now_ns(); // too far behind: drop, never spiral
		}
		const int steps = steps_per_tick(speed_.load());
		if (steps > 0 && s_->ready) tick(*s_, steps, deadline); // stamped when due, not when done: an even timeline
	}
}

} // namespace city
