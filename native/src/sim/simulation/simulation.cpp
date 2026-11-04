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
} // namespace

double CityClock::hour() const {
	double m = std::fmod(minutes.load() + day_shift.load(), double(MINUTES_PER_DAY));
	if (m < 0) m += MINUTES_PER_DAY;
	return m / MINUTES_PER_HOUR;
}

void CityClock::set_hour(double h) { day_shift.store(h * MINUTES_PER_HOUR - minutes.load()); }

void Simulation::start(CityState &s) {
	stop();
	s_ = &s;
	period_ns_ = int64_t(NS_PER_S / double(s.catalog.tune.sim_tick_hz));
	speed_ = SimSpeed::NORMAL;
	running_ = true;
	thread_ = std::thread([this]() { run(); });
}

void Simulation::stop() {
	running_ = false;
	if (thread_.joinable()) thread_.join();
	speed_ = SimSpeed::STOPPED;
}

void Simulation::set_speed(SimSpeed sp) {
	if (speed_.load() == SimSpeed::STOPPED || sp == SimSpeed::STOPPED) return; // only start()/stop() enter or leave STOPPED
	speed_ = sp;
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
		{
			std::shared_lock<std::shared_mutex> map(s.map_mutex);
			std::lock_guard<std::mutex> tr(s.traffic_mutex);
			traffic::step(s, float(dt));
		}
		{
			std::unique_lock<std::shared_mutex> map(s.map_mutex, std::try_to_lock); // mesh jobs may hold it shared
			s.clock.minutes.store(s.clock.minutes.load() + dt * s.catalog.tune.sim_minutes_per_second);
			if (map.owns_lock()) {
				grow_step(s, owed_ + dt);
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

void Simulation::step(CityState &s) {
	tick(s, 1, now_ns());
}

void Simulation::run() {
	constexpr int MAX_CATCHUP_TICKS = 2;
	int64_t deadline = now_ns();
	while (running_) {
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
