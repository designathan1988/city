// The frame of each way a walker walks, and its stripe and pace among the bodies around it (SUMO
// MSPModel_Striping PState::walk), see ped_stripes.h. The candidate stripes are the footway's own (ped_stripe_m apart) and the places just clear of each
// piece of furniture, so a gap between two pieces is found even off the stripes. A stripe a footprint stands in beside
// the walker is shut; ahead, each stripe's room is the gap to the nearest body across it. The walker takes the stripe
// with the most room (aside costs, off its own side costs, the stripes left to the oncoming are not its own), moves
// toward it at a share of its pace without ever stepping sideways into furniture, and keeps its gap behind the body
// ahead. Held up past ped_jam_s it squeezes past walkers (never furniture) at ped_jam_share of its pace (SUMO's jamtime).
#include "sim/traffic/ped_stripes.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr int MAX_STRIPES = 8;
constexpr int MAX_CANDIDATES = MAX_STRIPES + 2 + 2 * MAX_BODIES;
constexpr int AIMED = 0, HERE = 1; // candidates[0]: the stripe aimed at; [1] where it is, when that is another place
constexpr float ONE_WAY_CLOSING = 0.5f; // a body coming the other way closes the gap at both paces: half the room
constexpr float INAPPROPRIATE_M = 1000.f; // SUMO's INAPPROPRIATE_PENALTY as room: a stripe left to the oncoming
constexpr float CLEAR_EPS_M = 0.01f; // a candidate this far beyond a body's reach is clear of it
// 1 / (stripes - 1): where the k-th of a room's stripes lies, without a division per walker
constexpr float STRIPE_STEP[MAX_STRIPES + 1] = { 0.f, 0.f, 1.f, 1.f / 2, 1.f / 3, 1.f / 4, 1.f / 5, 1.f / 6, 1.f / 7 };

bool across(const Body &o, float c) { return c > o.lat_lo - o.reach && c < o.lat_hi + o.reach; }

// The gap a body leaves ahead of the places across it: at or under zero beside a piece of furniture (shut), BIG when it
// is behind (a walker: theirs to keep the gap) or passed (furniture).
float gap_of(const Body &o) {
	if (o.furniture) return o.along_hi < -o.reach ? BIG : o.along_lo - o.reach;
	if (o.along_lo <= 0) return BIG;
	return (o.oncoming ? o.along_lo * ONE_WAY_CLOSING : o.along_lo) - o.reach;
}

// Each candidate's room ahead, as SUMO's bodies fill only the stripes their width covers: the candidates in order
// across, each body lowers the room of those across it alone.
void rooms_of(const Body *b, int n, const float *cand, int m, float look, float *room) {
	int order[MAX_CANDIDATES];
	for (int k = 0; k < m; k++) {
		room[k] = look;
		int i = k;
		for (; i > 0 && cand[order[i - 1]] > cand[k]; i--) order[i] = order[i - 1];
		order[i] = k;
	}
	for (int j = 0; j < n; j++) {
		const float gap = gap_of(b[j]);
		if (gap >= look) continue;
		const float lo = b[j].lat_lo - b[j].reach, hi = b[j].lat_hi + b[j].reach;
		int i = int(std::upper_bound(order, order + m, lo, [&](float v, int k) { return v < cand[k]; }) - order);
		for (; i < m && cand[order[i]] < hi; i++) room[order[i]] = std::min(room[order[i]], gap);
	}
}

// The room ahead of a centre at `c` across: the gap to the nearest body across it (only furniture when squeezing). With
// nobody about it is the whole look ahead.
float room_at(const Body *b, int n, float c, float look, bool squeeze) {
	float room = look;
	for (int k = 0; k < n; k++) {
		const Body &o = b[k];
		if ((squeeze && !o.furniture) || !across(o, c)) continue;
		room = std::min(room, gap_of(o));
	}
	return room;
}

// Where the walker may step sideways now: its room (or where it stands, when the room moved off it: it steps back in at
// its own lateral pace, never a jump), less the furniture beside it on either side.
void side_limits(const WayFrame &f, const Body *b, int n, float &lo, float &hi) {
	lo = std::min(f.lo, f.u);
	hi = std::max(f.hi, f.u);
	for (int k = 0; k < n; k++) {
		const Body &o = b[k];
		if (!o.furniture || o.along_lo - o.reach >= 0 || o.along_hi + o.reach <= 0) continue;
		const float shut_lo = o.lat_lo - o.reach, shut_hi = o.lat_hi + o.reach;
		if (f.u <= shut_lo) hi = std::min(hi, shut_lo);
		else if (f.u >= shut_hi) lo = std::max(lo, shut_hi);
	}
	if (hi < lo) hi = lo;
}

// The places worth a score, each once (the same place twice scores the same and never wins the second time): the
// stripe aimed at first, where it is (if inside its room and another place), the room's stripes and the places just
// clear of each piece of furniture.
int candidates(const StripeRules &r, const CrowdEntry &w, const WayFrame &f, const Body *b, int n, float *out, bool &here) {
	int m = 0;
	out[m++] = clampf(w.u_goal, f.lo, f.hi);
	here = f.u >= f.lo && f.u <= f.hi && f.u != out[AIMED];
	if (here) out[m++] = f.u;
	else if (clampf(f.u, f.lo, f.hi) != out[AIMED]) out[m++] = clampf(f.u, f.lo, f.hi); // outside its room: its nearest edge
	auto fresh = [&](float c) { return c != out[AIMED] && !(here && c == out[HERE]); };
	const float span = f.hi - f.lo;
	const int stripes = std::clamp(1 + int(span * r.inv_stripe), 1, MAX_STRIPES);
	for (int k = 0; k < stripes; k++) {
		const float c = stripes == 1 ? (f.lo + f.hi) * 0.5f : f.lo + span * float(k) * STRIPE_STEP[stripes];
		if (fresh(c)) out[m++] = c;
	}
	for (int k = 0; k < n; k++) {
		if (!b[k].furniture) continue; // walkers: the stripes are SUMO's answer
		const float below = b[k].lat_lo - b[k].reach - CLEAR_EPS_M, above = b[k].lat_hi + b[k].reach + CLEAR_EPS_M;
		if (below >= f.lo) out[m++] = below;
		if (above <= f.hi) out[m++] = above;
	}
	return m;
}

} // namespace

V2 left_of(const V2 &d) { return V2(d.y, -d.x); }

// On a sidewalk: across is the sidewalk's own u (+ on the left of a -> b) whichever way it walks; in the world (where
// the last pose put it, facing its way) only when it is near a junction.
void sidewalk_frame(const CrowdEntry &w, float half, bool world, WayFrame &f) {
	f.u = w.u;
	f.lo = -half;
	f.hi = half;
	f.keep = clampf(w.keep, -half, half);
	if (!world) return;
	f.at = V2(w.x, w.z);
	f.fwd = V2(w.fx, w.fz);
	f.left = left_of(w.dir == 0 ? f.fwd : -f.fwd);
}

void line_frame(const CityState &s, const Pedestrian &w, WayFrame &f) {
	const TrafficParams &p = s.catalog.traffic;
	const Waypoint &to = w.ways[w.wi];
	const V2 from(w.lx, w.lz), dir = (V2(to.x, to.z) - from).norm();
	f.fwd = dir;
	f.left = left_of(dir);
	f.at = from + dir * w.ls + f.left * w.u;
	f.u = w.u;
	if (to.cross >= 0) { // over a zebra: its own right half, measured from its line
		const float zh = s.catalog.furniture.crossing.half();
		f.lo = -zh + p.ped_body_m - w.line_off;
		f.hi = std::max(f.lo, -p.ped_body_m - w.line_off);
		f.keep = clampf(0.f, f.lo, f.hi);
		return;
	}
	// round a corner: room opens off the line's start and, toward a kerb place it must reach, closes again no faster than
	// the walker steps aside (a share of its pace per metre walked), so it always meets the place
	const bool to_kerb = w.wi + 1 < w.wn && w.ways[w.wi + 1].cross >= 0;
	float half = std::min(walk_half(s, w.seg, w.side), p.ped_lateral_share * w.ls); // w.seg: the sidewalk it heads for
	if (to_kerb) half = std::min(half, p.ped_lateral_share * std::max(0.f, w.llen - w.ls));
	f.lo = -half;
	f.hi = half;
	f.keep = 0;
	f.reserve_from = f.hi - std::min(p.ped_reserve_share * (f.hi - f.lo), p.ped_reserve_max_m);
}

StripeRules::StripeRules(const TrafficParams &p, float step) {
	dt = step;
	look = p.ped_look_m;
	gap = p.ped_gap_m;
	inv_headway = 1.f / p.ped_headway_s;
	accel = p.ped_accel;
	aside_share = p.ped_lateral_share;
	lateral_cost = p.ped_lateral_cost;
	keep_cost = p.ped_keep_right_cost;
	inv_stripe = 1.f / p.ped_stripe_m;
	gain = p.ped_stripe_gain_m;
	hold = p.ped_stripe_hold_s;
	jam_s = p.ped_jam_s;
	jam_share = p.ped_jam_share;
	moving = p.ped_moving_ms;
	settled_keeps = p.ped_lateral_cost >= p.ped_keep_right_cost;
}

StripeChoice choose_stripe(const StripeRules &r, const CrowdEntry &w, const WayFrame &f, const Body *b, int n) {
	// nobody about, on the stripe it aims at (inside its room, not one left to the oncoming), not held up: it walks on
	if (n == 0 && r.settled_keeps && w.u_goal == f.u && f.u >= f.lo && f.u <= f.hi && f.u <= f.reserve_from && w.jam < r.jam_s) {
		StripeChoice out;
		out.u = out.u_goal = f.u;
		out.held = w.held + r.dt;
		out.pace = std::min(std::clamp((r.look - r.gap) * r.inv_headway, 0.f, w.speed), w.pace + r.accel * r.dt);
		return out; // jam 0: free ahead
	}
	float cand[MAX_CANDIDATES];
	bool here = false;
	const int m = candidates(r, w, f, b, n, cand, here);
	float rooms[MAX_CANDIDATES];
	rooms_of(b, n, cand, m, r.look, rooms);
	float aimed_room = r.look, here_room = r.look, aimed = 0, best_score = -BIG;
	int best = AIMED;
	for (int k = 0; k < m; k++) {
		const float c = cand[k], room = rooms[k];
		const float sc = room - r.lateral_cost * std::fabs(c - f.u) - r.keep_cost * std::fabs(c - f.keep) -
				(c > f.reserve_from ? INAPPROPRIATE_M : 0.f);
		if (k == AIMED) {
			aimed_room = room;
			aimed = sc;
		}
		if (k == HERE && here) here_room = room;
		if (sc > best_score) {
			best = k;
			best_score = sc;
		}
	}
	StripeChoice out;
	out.held = w.held + r.dt;
	out.u_goal = cand[AIMED];
	const bool shut = aimed_room < r.gap; // closed ahead: no waiting out the hold
	if (best != AIMED && best_score > aimed + r.gain && (out.held >= r.hold || shut)) {
		out.u_goal = cand[best];
		out.held = 0;
	}
	// the gap behind the body ahead in its own span; held up past ped_jam_s it squeezes past the walkers until clear
	// (where it is was scored already, as a candidate or as the stripe aimed at, unless it stands outside its room)
	const bool scored = here || f.u == cand[AIMED];
	const float lead = !scored && n > 0 ? room_at(b, n, f.u, r.look, false) : here ? here_room : f.u == cand[AIMED] ? aimed_room : r.look;
	float want = std::clamp((lead - r.gap) * r.inv_headway, 0.f, w.speed);
	if (w.jam >= r.jam_s) {
		const float squeeze = room_at(b, n, f.u, r.look, true);
		out.jam = want > r.moving ? 0.f : w.jam;
		want = std::clamp((squeeze - r.gap) * r.inv_headway, 0.f, w.speed * r.jam_share);
	} else {
		out.jam = want < r.moving ? w.jam + r.dt : 0.f;
	}
	out.pace = std::min(want, w.pace + r.accel * r.dt);
	// aside at a share of its own pace (it can step aside standing), never into the furniture beside it
	const float aside = r.aside_share * w.speed * r.dt;
	float lo, hi;
	side_limits(f, b, n, lo, hi);
	out.u = clampf(f.u + std::clamp(out.u_goal - f.u, -aside, aside), lo, hi);
	return out;
}

} // namespace traffic
} // namespace city
