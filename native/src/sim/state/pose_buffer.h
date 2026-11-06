// Lock-free triple buffer of agent poses: the simulation publishes after each tick, the render takes the newest without
// waiting. Each record carries the agent's poses at the last three publications and the frame their scheduled times,
// so the render draws a moment render.pose_delay_ticks behind the clock between two of them (snapshot interpolation,
// G. Fiedler, "Snapshot Interpolation", Gaffer On Games, 2014: a tick that lands late or early never stops nor jumps
// an agent).
// The frame also carries what the signalled approaches near the camera show (their heads' lit lenses).
#pragma once

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <memory>
#include <vector>

namespace city {

constexpr int POSE_TRAIL = 3; // publications kept per agent: k-2, k-1, k

struct Pose {
	float x = 0, y = 0, z = 0, fx = 0, fz = 1; // position and unit heading in the XZ plane
	float rise = 0; // metres up per metre along the heading: what a body on a grade tilts by
};

// An agent's poses at the last publications (oldest first). An agent missing from a publication (far, or new) starts
// over: all of them at its pose, so it never slides across from where it was long ago.
struct PoseTrail {
	Pose at[POSE_TRAIL];
	int64_t stamp_ns = 0; // publication of at[POSE_TRAIL - 1]
	void push(const Pose &p, int64_t stamp, int64_t previous_stamp) {
		if (stamp_ns == 0 || stamp_ns != previous_stamp)
			for (Pose &q : at) q = p;
		for (int i = 0; i + 1 < POSE_TRAIL; i++) at[i] = at[i + 1];
		at[POSE_TRAIL - 1] = p;
		stamp_ns = stamp;
	}
};

// AgentRecord::look of a vehicle: type in the low byte, then its paint, the brake light and whether anybody is in it.
constexpr uint32_t LOOK_TYPE_MASK = 0xFFu, LOOK_PAINT_SHIFT = 8, LOOK_BRAKE_SHIFT = 16, LOOK_EMPTY_SHIFT = 17, LOOK_ABOARD_SHIFT = 18, LOOK_ABOARD_MASK = 3u, LOOK_DOOR_SHIFT = 20, LOOK_DOOR_MASK = 31u; // how far its kerb door stands open
// How old a walker is: its pace and its body.
enum class PedAge : uint8_t { ADULT = 0, CHILD, ELDER, COUNT };
constexpr int PED_AGES = int(PedAge::COUNT);

// What somebody standing does with their hands and face, past the idle.
enum class PedAct : uint8_t { NONE = 0, TALK, LISTEN, LOOK, PHONE, HEADPHONES, BAG, CROUCH, PHOTO, COUNT };

struct AgentRecord {
	Pose at[POSE_TRAIL]; // the agent's PoseTrail
	uint32_t id = 0; // vehicle or pedestrian id (wheel odometer, look)
	uint32_t look = 0; // pedestrians: look hash; vehicles: type | paint << 8 | brake << 16
	float walked = 0; // pedestrians: metres walked (walk phase); vehicles: 0
	float rest = 0; // pedestrians: 0 walking .. 1 standing (the idle's share of the pose)
	float act_share = 0; // pedestrians: how far into `act` they are (0 standing idle .. 1)
	float act_since = 0, act_left = 0; // pedestrians: seconds since `act` began, and until a fright lets go of it
	uint8_t age = 0; // pedestrians: PedAge
	uint8_t act = 0; // pedestrians: PedAct
};

// A car standing in a bay (no trail: it does not move). look as AgentRecord's; ids are their own, counted from 0 as the fleet's: the same number names
// a car on the road and another in a bay.
struct ParkedRecord {
	Pose at;
	uint32_t id = 0, look = 0;
};

// A signalled approach (directed segment `ds` into its junction): the light its cars see (traffic Light) and whether
// the walkers over its crosswalk may cross.
struct SignalRecord {
	uint32_t ds = 0;
	uint8_t light = 0;
	bool walk = false;
};

// The record of approach `ds` in `signals` (sorted by ds), or none.
inline const SignalRecord *find_signal(const std::vector<SignalRecord> &signals, uint32_t ds) {
	auto it = std::lower_bound(signals.begin(), signals.end(), ds, [](const SignalRecord &r, uint32_t d) { return r.ds < d; });
	return it != signals.end() && it->ds == ds ? &*it : nullptr;
}

// The moment the render draws: between at[first] and at[first + 1] of every record, `t` of the way (PoseFrame::sample).
struct PoseSample {
	int first = POSE_TRAIL - 2;
	float t = 1;
	bool beyond = false, before = false; // the moment is past the newest publication / older than the oldest (held)
};

struct PoseFrame {
	std::vector<AgentRecord> vehicles, people;
	std::vector<AgentRecord> trains; // each car near the camera: look its RGB, id 1 when it is the metro's
	std::shared_ptr<const std::vector<ParkedRecord>> parked; // the cars standing in bays near the camera, made again only when one changes
	std::vector<float> seg_speed_share; // per segment: mean speed / limit (congestion layer), 1 when empty
	std::vector<SignalRecord> signals; // the signalled approaches near the camera, by ds
	const SignalRecord *signal(uint32_t ds) const { return find_signal(signals, ds); }
	int64_t stamp_ns[POSE_TRAIL] = {}; // scheduled times of the last publications (oldest first; 0: none yet)
	int64_t period_ns = 0; // tick period
	// The pair that holds `when` (ns, the render's clock less its delay), clamped to the trail's ends.
	PoseSample sample(int64_t when) const {
		PoseSample out;
		if (stamp_ns[0] == 0) return out; // fewer than a trail of publications: the newest pose
		int first = POSE_TRAIL - 2;
		while (first > 0 && when < stamp_ns[first]) first--;
		const int64_t a = stamp_ns[first], b = stamp_ns[first + 1];
		out.first = first;
		out.t = b > a ? float(double(when - a) / double(b - a)) : 1.f;
		out.beyond = out.t > 1.f;
		out.before = out.t < 0.f;
		out.t = out.t < 0.f ? 0.f : out.t > 1.f ? 1.f : out.t;
		return out;
	}
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
