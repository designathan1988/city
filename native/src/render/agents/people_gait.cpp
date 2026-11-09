// What a walker's body plays: the lab's citizenGait.ts stepGait, moving and standing, pace and gaitPlays, on the game's records (people_gait.h).
#include "render/agents/people_gait.h"

#include "core/math.h"
#include "core/random.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float LEAST = 1e-6f; // a stride, a fade or a pace under this is none
constexpr float LEAST_STRIDE = 1e-3f, LEAST_WEIGHT = 1e-3f; // citizenGait.ts: a stride is at least a millimetre; a play weighing less is left out
constexpr float PHASE_SPAN = 997.f, START_SPAN = 613.f; // createGait: a person's walk and its first play from its hash, two primes apart
constexpr uint32_t RUN_SHIFT = 9, RUN_BITS = 255; // runAt: a byte of the hash from bit 9

float smooth(float lo, float hi, float x) {
	const float t = clampf((x - lo) / (hi - lo), 0.f, 1.f);
	return t * t * (3 - 2 * t);
}
float approach(float dt, float time) { return 1.f - std::exp(-dt / std::max(time, LEAST)); }

// The angle from forward `f` to `v`, positive to the left (a model's +X is its left, agent_row).
float left_of(float fx, float fz, float vx, float vz) { return std::atan2(vx * fz - vz * fx, vx * fx + vz * fz); }
} // namespace

bool PeopleGait::init(const DataValue &gait, const PeopleAsset &asset, const StandingClips &standing) {
	clips_.init(asset, gait["idle_spread"].f());
	standing_ = standing;
	gaits_.clear();
	Tuning &k = k_;
	const struct {
		const char *key;
		float *to;
	} fields[] = { { "turn_steps", &k.turn_steps }, { "turn_steps_standing", &k.turn_steps_standing }, { "turn_in_place_ms", &k.turn_in_place },
		{ "turn_settle_s", &k.turn_settle_s }, { "turn_windup", &k.turn_windup }, { "turn_foot_m", &k.turn_foot_m }, { "feet_lead_rate", &k.feet_lead_rate },
		{ "turn_restep", &k.turn_restep }, { "turn_restep_fade_s", &k.turn_restep_fade_s }, { "stop_below_ms", &k.stop_below }, { "stop_decel", &k.stop_decel },
		{ "stop_entry_most", &k.stop_entry_most }, { "stop_walk_on_ms", &k.stop_walk_on_ms }, { "stop_done", &k.stop_done }, { "start_below_ms", &k.start_below },
		{ "start_done", &k.start_done }, { "clip_floor", &k.clip_floor }, { "stop_settle", &k.stop_settle }, { "stride_exponent", &k.stride_exponent },
		{ "gait_fade_s", &k.gait_fade_s }, { "turn_fade_s", &k.turn_fade_s }, { "fade_s", &k.fade_s }, { "run_time_s", &k.run_time_s },
		{ "run_from_ms", &k.run_from_ms }, { "run_span_ms", &k.run_span_ms }, { "run_ramp_ms", &k.run_ramp_ms }, { "velocity_time_s", &k.velocity_time_s },
		{ "accel_time_s", &k.accel_time_s }, { "jump_ms", &k.jump_ms }, { "still_ms", &k.still_ms }, { "armed_over_ms", &k.armed_over_ms },
		{ "idle_spread", &k.idle_spread }, { "dt_most_s", &k.dt_most_s }, { "stop_run_most", &k.stop_run_most } };
	for (const auto &f : fields) *f.to = gait[f.key].f();
	const auto clip = [&](const char *name) {
		const int c = asset.clip(name);
		if (c < 0) gait.fail(String("people.bin has no clip ") + name);
		return std::max(c, 0);
	};
	walk_shuffle_ = clip("walkShuffle");
	walk_slow_ = clip("walkSlow");
	walk_fast_ = clip("walkFast");
	run_ = clip("run");
	start_ = clip("start");
	stop_ = clip("stop");
	turn_left_ = clip("turnLeft");
	turn_right_ = clip("turnRight");
	walk_rest_ = clip("walkRest");
	walk_back_ = clip("walkBack");
	walk_left_ = clip("walkLeft");
	walk_right_ = clip("walkRight");
	const DataValue styles = gait["walk_styles"], elder = gait["elder_walks"];
	styles_.clear();
	for (int i = 0; i < styles.size(); i++) styles_.push_back(clip(styles[i].utf8().c_str()));
	elder_walks_.clear();
	for (int i = 0; i < elder.size(); i++) elder_walks_.push_back(clip(elder[i].utf8().c_str()));
	if (styles_.empty() || elder_walks_.empty()) gait.fail("walk styles and an elder's walks");
	if (!gait.reader.errors.is_empty()) return false;
	// citizenGait.ts walksOf: the shuffle, the slow walk, the person's own and the brisk walk, slowest first, on each body
	style_walks_.assign(asset.variations.size() * styles_.size(), {});
	for (size_t v = 0; v < asset.variations.size(); v++)
		for (size_t s = 0; s < styles_.size(); s++) {
			std::vector<int> &w = style_walks_[v * styles_.size() + s];
			for (int c : { walk_shuffle_, walk_slow_, styles_[s], walk_fast_ })
				if (std::find(w.begin(), w.end(), c) == w.end()) w.push_back(c);
			std::stable_sort(w.begin(), w.end(), [&](int a, int b) { return clips_.natural_pace(a, int(v), 1.f) < clips_.natural_pace(b, int(v), 1.f); });
		}
	return true;
}

void PeopleGait::play(Gait &g, int key, float phase, float fade) const {
	if (g.cur.key == key) return;
	g.prev = g.cur;
	g.has_prev = true;
	g.cur = Playing{ key, phase, 0.f };
	g.fade = 0;
	g.fade_time = fade;
}

// citizenGait.ts pace: the two walks blended at this pace, the second one's weight, the ground one blended cycle covers.
void PeopleGait::pace(Gait &g, const std::vector<int> &walks, int body, float size, float speed) const {
	const int first = walks.front(), last = walks.back();
	const auto stride_of = [&](int c) { return clips_.stride(c, body, size); };
	g.walk_a = g.walk_b = first;
	g.walk_w = 0;
	g.step_scale = 1;
	const float own = clips_.natural_pace(first, body, size);
	if (speed <= own) {
		g.step_scale = std::pow(std::max(speed, LEAST) / std::max(own, LEAST), k_.stride_exponent);
		g.stride = stride_of(first) * g.step_scale;
		return;
	}
	for (size_t i = 0; i + 1 < walks.size(); i++) {
		const float va = clips_.natural_pace(walks[i], body, size), vb = clips_.natural_pace(walks[i + 1], body, size);
		if (speed > vb || vb <= va) continue;
		const float w = (speed - va) / (vb - va);
		g.walk_a = walks[i];
		g.walk_b = walks[i + 1];
		g.walk_w = w;
		g.stride = stride_of(walks[i]) + (stride_of(walks[i + 1]) - stride_of(walks[i])) * w;
		return;
	}
	g.walk_a = g.walk_b = last;
	g.stride = stride_of(last);
}

// citizenGait.ts standing: turning on the spot with the turn clips' steps, a stop settling, or standing (the simulation's act).
void PeopleGait::standing(Gait &g, float dt, float turned, bool spin, float turn_sign, int body) const {
	const bool is_turn = g.cur.key == turn_left_ || g.cur.key == turn_right_;
	if (spin || (is_turn && g.settling < k_.turn_settle_s)) {
		const int key = spin ? (turn_sign > 0 ? turn_left_ : turn_right_) : g.cur.key;
		if (spin) g.settling = 0;
		else g.settling += dt;
		if (g.cur.key != key) {
			play(g, key, 0, k_.turn_fade_s);
			g.cur.acc = k_.turn_windup;
		}
		// The feet lead a turn: they step from its first moment, and the body follows once the steps carry the pose
		g.cur.acc += spin ? std::max(turned, k_.feet_lead_rate * dt) : turned;
		g.cur.phase = clips_.fraction_at(key, body, g.cur.acc);
		if (g.cur.phase >= k_.turn_restep) { // still turning past the end of the clip: step round again
			g.prev = g.cur;
			g.has_prev = true;
			g.cur = Playing{ key, 0.f, k_.turn_windup };
			g.fade = 0;
			g.fade_time = k_.turn_restep_fade_s;
		}
	} else if (g.cur.key == stop_ && g.cur.phase < 1) {
		g.cur.phase = std::min(1.f, g.cur.phase + k_.stop_settle * dt / clips_.seconds(stop_, body));
	} else {
		play(g, STANDING, 0, k_.fade_s);
	}
	g.armed = true;
}

// citizenGait.ts moving: set off, walk or run, or stop.
void PeopleGait::moving(Gait &g, float dt, float speed, float distance_speed, float turn_v, int body, float size, uint32_t hash, bool elder) const {
	g.settling = 0;
	const bool forward = std::fabs(g.direction) < LEAST; // start and stop captures have no lateral travel
	const int cur = g.cur.key;
	if (!forward && (cur == start_ || cur == stop_)) play(g, LOCO, 0, k_.gait_fade_s);
	if (g.cur.key != LOCO && g.cur.key != start_ && g.cur.key != stop_) {
		if (speed >= clips_.natural_pace(walk_shuffle_, body, size) && speed < k_.start_below && !elder && forward) {
			play(g, start_, clips_.start_entry(start_, body, speed, size), k_.gait_fade_s);
			g.cur.acc = clips_.curve_at(start_, body, g.cur.phase) * size;
		} else play(g, LOCO, 0, k_.gait_fade_s);
	} else if (g.cur.key == LOCO && g.armed && speed < k_.stop_below && g.accel < -k_.stop_decel && !elder && g.run < k_.stop_run_most && forward) {
		const float entry = clips_.stop_entry(stop_, body, speed, size);
		if (entry < k_.stop_entry_most) {
			play(g, stop_, entry, k_.gait_fade_s);
			g.cur.acc = clips_.curve_at(stop_, body, entry) * size;
			g.armed = false;
		}
	} else if (g.cur.key == stop_ && (speed > k_.stop_below + k_.stop_walk_on_ms || g.accel > k_.stop_decel || g.cur.phase >= k_.stop_done)) {
		play(g, LOCO, 0, k_.gait_fade_s);
	}
	if (speed > k_.stop_below + k_.armed_over_ms) g.armed = true;
	Playing &now = g.cur;
	const float travel = speed * dt;
	if (now.key == start_) {
		now.acc += travel;
		now.phase = std::max(clips_.fraction_at(start_, body, now.acc / size), now.phase + k_.clip_floor * dt / clips_.seconds(start_, body));
		if (now.phase >= k_.start_done) {
			g.cycle = 0;
			play(g, LOCO, 0, k_.gait_fade_s);
		}
	} else if (now.key == stop_) {
		now.acc += travel;
		now.phase = std::min(1.f, std::max(clips_.fraction_at(stop_, body, now.acc / size), now.phase + k_.clip_floor * dt / clips_.seconds(stop_, body)));
	}
	const std::vector<int> &walks = elder ? elder_walks_ : style_walks_[size_t(body) * styles_.size() + hash % styles_.size()];
	pace(g, walks, body, size, speed);
	const float x = std::cos(g.direction), y = std::sin(g.direction);
	const auto share = [&](int c) { return std::min(1.f, std::pow(std::max(speed, LEAST) / std::max(clips_.natural_pace(c, body, size), LEAST), k_.stride_exponent)); };
	g.back_scale = share(walk_back_);
	g.left_scale = share(walk_left_);
	g.right_scale = share(walk_right_);
	const float run_at = k_.run_from_ms + k_.run_span_ms * float((hash >> RUN_SHIFT) & RUN_BITS) / float(RUN_BITS);
	const float runs = elder ? 0.f : smooth(run_at, run_at + k_.run_ramp_ms, speed * std::max(0.f, x));
	g.run += (runs - g.run) * approach(dt, k_.run_time_s);
	const auto stride_of = [&](int c) { return clips_.stride(c, body, size); };
	const float forward_stride = g.stride * (1 - g.run) + stride_of(run_) * g.run;
	const float back_stride = stride_of(walk_back_) * g.back_scale, left_stride = stride_of(walk_left_) * g.left_scale;
	const float right_stride = stride_of(walk_right_) * g.right_scale;
	const float fw = std::max(0.f, x) / std::max(LEAST, forward_stride), bw = std::max(0.f, -x) / std::max(LEAST, back_stride);
	const float lw = std::max(0.f, y) / std::max(LEAST, left_stride), rw = std::max(0.f, -y) / std::max(LEAST, right_stride);
	const float total = std::max(LEAST, fw + bw + lw + rw);
	g.forward_w = fw / total;
	g.back_w = bw / total;
	g.left_w = lw / total;
	g.right_w = rw / total;
	g.stride = std::hypot(g.forward_w * forward_stride - g.back_w * back_stride, g.left_w * left_stride - g.right_w * right_stride);
	const float stepping = std::hypot(distance_speed, k_.turn_foot_m * turn_v); // a turn made while barely moving is footwork too
	g.cycle = std::fmod(g.cycle + stepping * dt / std::max(LEAST_STRIDE, g.stride), 1.f);
}

int PeopleGait::step(const AgentRecord &rec, const Pose &at, int body, float size, double clock, Play *out) {
	if (rec.id >= gaits_.size()) gaits_.resize(size_t(rec.id) + 1);
	Gait &g = gaits_[rec.id];
	const uint32_t hash = hash_u32(rec.look);
	// The drawn body's velocity over the last simulation tick, and how fast its heading turned.
	const Pose &a = rec.at[POSE_TRAIL - 2], &b = rec.at[POSE_TRAIL - 1];
	const float tick = std::max(tick_s_, LEAST);
	float rx = (b.x - a.x) / tick, rz = (b.z - a.z) / tick;
	if (std::hypot(rx, rz) > k_.jump_ms) { // a re-seat, not a step
		rx = g.vx;
		rz = g.vz;
	}
	const float turn_v = left_of(a.fx, a.fz, b.fx, b.fz) / tick;
	if (!g.born || g.look != rec.look) {
		g = Gait();
		g.born = true;
		g.look = rec.look;
		g.time = clock;
		g.vx = rx;
		g.vz = rz;
		g.speed = std::hypot(rx, rz);
		g.cycle = float(hash % uint32_t(PHASE_SPAN)) / PHASE_SPAN;
		g.cur = Playing{ g.speed > 0 ? LOCO : STANDING, float(hash % uint32_t(START_SPAN)) / START_SPAN, 0.f };
		g.fade = g.fade_time = 1;
	}
	const float dt = clampf(float(clock - g.time), 0.f, k_.dt_most_s);
	g.time = clock;
	const float distance_speed = std::hypot(rx, rz), kv = approach(dt, k_.velocity_time_s);
	g.vx += (rx - g.vx) * kv;
	g.vz += (rz - g.vz) * kv;
	const float speed = std::hypot(g.vx, g.vz);
	if (dt > 0) g.accel += ((speed - g.speed) / dt - g.accel) * approach(dt, k_.accel_time_s);
	g.speed = speed;
	if (speed > 0) g.direction = left_of(at.fx, at.fz, g.vx, g.vz);
	const float turned = std::fabs(turn_v) * dt;
	const bool spin = std::fabs(turn_v) > (speed < k_.turn_in_place ? k_.turn_steps_standing : k_.turn_steps);
	const float turn_sign = turn_v > 0 ? 1.f : -1.f;
	const bool elder = rec.age == uint8_t(PedAge::ELDER);
	const bool crouched = rec.act == uint8_t(PedAct::CROUCH) && rec.act_share > 0;
	if (!crouched && distance_speed > k_.still_ms && speed > k_.still_ms && (!spin || speed >= k_.turn_in_place)) {
		moving(g, dt, speed, distance_speed, turn_v, body, size, hash, elder);
	} else {
		standing(g, dt, turned, spin && !crouched, turn_sign, body);
		g.cycle = std::fmod(g.cycle + std::hypot(distance_speed, k_.turn_foot_m * turn_v) * dt / std::max(LEAST_STRIDE, g.stride), 1.f);
	}
	// Whatever is fading out finishes its own steps meanwhile.
	if (g.has_prev) {
		Playing &p = g.prev;
		if (p.key == stop_) p.phase = std::min(1.f, p.phase + k_.stop_settle * dt / clips_.seconds(stop_, body));
		else if (p.key == start_ || p.key == turn_left_ || p.key == turn_right_) p.phase = std::min(1.f, p.phase + dt / clips_.seconds(p.key, body));
	}
	if (g.fade < 1) g.fade = std::min(1.f, g.fade + dt / std::max(g.fade_time, LEAST));
	if (g.fade >= 1) g.has_prev = false;
	// gaitPlays: the play now eased in over the one fading out; the same clip at the same phase told once.
	const float eased = g.fade * g.fade * (3 - 2 * g.fade);
	Play plays[2 * PEOPLE_PLAYS + 2];
	int n = add(g, g.cur, eased, rec, body, clock, plays, 0);
	if (g.has_prev) n = add(g, g.prev, 1 - eased, rec, body, clock, plays, n);
	std::sort(plays, plays + n, [](const Play &l, const Play &r) { return l.weight > r.weight; });
	// More than the shader blends: the lightest go, the rest made whole again (the lab blends those on the CPU, citizenPlays.ts cpuPose).
	n = std::min(n, int(PEOPLE_PLAYS));
	float total = 0;
	for (int i = 0; i < n; i++) total += plays[i].weight;
	for (int i = 0; i < n; i++) out[i] = Play{ plays[i].clip, plays[i].phase, plays[i].weight / std::max(total, LEAST) };
	return n;
}

int PeopleGait::add(const Gait &g, const Playing &p, float weight, const AgentRecord &rec, int body, double clock, Play *out, int n) const {
	const auto put = [&](int clip, float phase, float w) {
		if (w < LEAST_WEIGHT) return;
		for (int i = 0; i < n; i++)
			if (out[i].clip == clip && std::fabs(out[i].phase - phase) < LEAST) {
				out[i].weight += w;
				return;
			}
		if (n < 2 * PEOPLE_PLAYS + 2) out[n++] = Play{ clip, phase, w };
	};
	if (weight < LEAST_WEIGHT) return n;
	if (p.key == LOCO) {
		const float forward_walk = weight * g.forward_w * (1 - g.run), stride_weight = forward_walk * g.step_scale;
		put(walk_rest_, 0.f, forward_walk * (1 - g.step_scale) + weight * (g.back_w * (1 - g.back_scale) + g.left_w * (1 - g.left_scale) + g.right_w * (1 - g.right_scale)));
		put(walk_back_, g.cycle, weight * g.back_scale * g.back_w);
		put(walk_left_, g.cycle, weight * g.left_scale * g.left_w);
		put(walk_right_, g.cycle, weight * g.right_scale * g.right_w);
		put(g.walk_a, g.cycle, stride_weight * (g.walk_a == g.walk_b ? 1.f : 1.f - g.walk_w));
		if (g.walk_a != g.walk_b) put(g.walk_b, g.cycle, stride_weight * g.walk_w);
		put(run_, g.cycle, weight * g.forward_w * g.run);
		return n;
	}
	if (p.key == STANDING) {
		// What the simulation says it is doing, eased in over the idle (ped_acts.cpp); a crouch by its own course, not the clock.
		const int act = rec.act < uint8_t(PedAct::COUNT) ? rec.act : 0;
		const float doing = act != 0 ? clampf(rec.act_share, 0.f, 1.f) : 0.f;
		const uint32_t seed = rec.look & PEOPLE_SEED_MASK;
		if (act == int(PedAct::CROUCH) && doing > 0) {
			const float down = clips_.seconds(standing_.crouch_down, body), up = clips_.seconds(standing_.crouch_up, body);
			if (rec.act_left < up) put(standing_.crouch_up, 1.f - rec.act_left / up, weight * doing);
			else if (rec.act_since < down) put(standing_.crouch_down, rec.act_since / down, weight * doing);
			else put(standing_.crouch_idle, clips_.clock_phase(standing_.crouch_idle, body, rec.act_since - down, 0), weight * doing);
		} else if (doing > 0) {
			put(standing_.act[act], clips_.clock_phase(standing_.act[act], body, clock, seed), weight * doing);
		}
		put(standing_.idle, clips_.clock_phase(standing_.idle, body, clock, seed), weight * (1 - doing));
		return n;
	}
	put(p.key, p.phase, weight);
	return n;
}

} // namespace city
