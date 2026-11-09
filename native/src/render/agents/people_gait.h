// What a walker's body plays, decided from how the simulation moves it and what it says the walker is doing (the lab's render/citizenGait.ts):
// set off with the walk start, walk in its own style blended with the slow walk, the shuffle and the brisk walk by pace (an elder in the
// elder's walk), break into a run, step aside or back, brake into the walk stop, turn on the spot with the turn clips' steps; standing, the
// idle or what the simulation's act is (people's own clock), a crouch by its own course. Each walker's gait is kept by its id while the same
// person walks (the record's look hash). Out: up to PEOPLE_PLAYS weighted clips at their phases, the heaviest first (people_pose.gdshaderinc).
// Data: agents.json people.gait.
#pragma once

#include "core/shader_contracts/people.h"
#include "render/agents/people_asset.h"
#include "render/agents/people_gait_clips.h"
#include "render/data_reader.h"
#include "sim/state/pose_buffer.h"

#include <cstdint>
#include <vector>

namespace city {

// A look hash's low ten bits: a person's seed for the shaders and the clock-played clips' places (people_face.gdshaderinc).
constexpr uint32_t PEOPLE_SEED_MASK = 1023;

// One clip of a figure's pose: which, where in it (a loop's fraction, a one-shot's 0..1), how much of the pose.
struct Play {
	int clip = 0;
	float phase = 0, weight = 0;
};

// What the gait reads of the simulation's standing acts: each act's clip, and the crouch's three.
struct StandingClips {
	int idle = 0;
	int act[int(PedAct::COUNT)] = {};
	int crouch_down = 0, crouch_idle = 0, crouch_up = 0;
};

class PeopleGait {
public:
	bool init(const DataValue &gait, const PeopleAsset &asset, const StandingClips &standing);
	void set_tick(float seconds) { tick_s_ = seconds; } // the simulation's step: the record's last two poses are this far apart
	// The plays of a walker this frame (`at` its pose now, `body` and `size` what it is drawn as, `clock` the simulation's seconds); how many.
	int step(const AgentRecord &rec, const Pose &at, int body, float size, double clock, Play *out);
	// What each clip says on a body: its seconds, its pace, a clip played by the clock from each person's own place (people_gait_clips.h).
	const GaitClipFacts &clips() const { return clips_; }

private:
	enum Key : int { LOCO = -1, STANDING = -2 }; // past these, a clip's index played on its own
	struct Playing {
		int key = STANDING;
		float phase = 0, acc = 0; // fraction through the clip; distance (start, stop) or angle (turns) it has covered
	};
	struct Gait {
		uint32_t look = 0;
		bool born = false;
		double time = 0;
		float vx = 0, vz = 0, speed = 0, accel = 0; // the drawn body's velocity, smoothed, and its rate of change
		float cycle = 0, run = 0; // the walks' shared phase; the run's share of the forward walk
		int walk_a = 0, walk_b = 0;
		float walk_w = 0, stride = 1, direction = 0;
		float step_scale = 1, back_scale = 1, left_scale = 1, right_scale = 1;
		float forward_w = 1, back_w = 0, left_w = 0, right_w = 0;
		Playing cur, prev;
		bool has_prev = false;
		float fade = 1, fade_time = 1, settling = 0;
		bool armed = true;
	};
	struct Tuning {
		float turn_steps = 0, turn_steps_standing = 0, turn_in_place = 0, turn_settle_s = 0, turn_windup = 0, turn_foot_m = 0, feet_lead_rate = 0;
		float turn_restep = 0, turn_restep_fade_s = 0, stop_below = 0, stop_decel = 0, stop_entry_most = 0, stop_walk_on_ms = 0, stop_done = 0;
		float start_below = 0, start_done = 0, clip_floor = 0, stop_settle = 0, stride_exponent = 0, gait_fade_s = 0, turn_fade_s = 0, fade_s = 0;
		float run_time_s = 0, run_from_ms = 0, run_span_ms = 0, run_ramp_ms = 0, velocity_time_s = 0, accel_time_s = 0, jump_ms = 0, still_ms = 0;
		float armed_over_ms = 0, idle_spread = 0, dt_most_s = 0, stop_run_most = 0;
	};
	Tuning k_;
	float tick_s_ = 1;
	GaitClipFacts clips_;
	StandingClips standing_;
	std::vector<Gait> gaits_; // by walker id
	std::vector<int> styles_; // the walks people walk in, one each for life
	std::vector<std::vector<int>> style_walks_; // per body and style: that body's walks slowest first (citizenGait.ts walksOf)
	std::vector<int> elder_walks_;
	int walk_shuffle_ = 0, walk_slow_ = 0, walk_fast_ = 0, run_ = 0, start_ = 0, stop_ = 0, turn_left_ = 0, turn_right_ = 0;
	int walk_rest_ = 0, walk_back_ = 0, walk_left_ = 0, walk_right_ = 0;
	void play(Gait &g, int key, float phase, float fade) const;
	void pace(Gait &g, const std::vector<int> &walks, int body, float size, float speed) const;
	void standing(Gait &g, float dt, float turned, bool spin, float turn_sign, int body) const;
	void moving(Gait &g, float dt, float speed, float distance_speed, float turn_v, int body, float size, uint32_t hash, bool elder) const;
	int add(const Gait &g, const Playing &p, float weight, const AgentRecord &rec, int body, double clock, Play *out, int n) const;
};

} // namespace city
