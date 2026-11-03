// Lock-free triple buffer of agent poses (roadcraft-novo shared/contracts/poses.ts): the simulation publishes
// after each tick, the render takes the newest without waiting. Each record carries the previous and current pose.
#pragma once

#include <atomic>
#include <cstdint>
#include <vector>

namespace city {

struct AgentRecord {
	float px = 0, py = 0, pz = 0, pfx = 0, pfz = 1; // previous pose
	float x = 0, y = 0, z = 0, fx = 0, fz = 1; // current pose
	uint32_t id = 0; // vehicle or pedestrian id (wheel odometer, look)
	uint32_t look = 0; // pedestrians: look hash; vehicles: type | paint << 8 | brake << 16
	float walked = 0; // pedestrians: metres walked (walk phase); vehicles: 0
};

struct PoseFrame {
	std::vector<AgentRecord> vehicles, people;
	std::vector<float> seg_speed_share; // per segment: mean speed / limit (congestion layer), 1 when empty
	int64_t tick_ns = 0, period_ns = 0; // publication time and tick period (render interpolation)
};

class PoseBuffer {
public:
	PoseFrame &back() { return frames_[back_]; }
	void publish() { back_ = state_.exchange(back_ | NEW) & IDX; }
	// Returns the newest frame (and true when it changed since the last call).
	const PoseFrame &acquire(bool &fresh) {
		fresh = (state_.load() & NEW) != 0;
		if (fresh) front_ = state_.exchange(front_) & IDX;
		return frames_[front_];
	}

private:
	static constexpr int NEW = 4, IDX = 3;
	PoseFrame frames_[3];
	std::atomic<int> state_{ 1 }; // middle = 1
	int back_ = 0, front_ = 2;
};

} // namespace city
