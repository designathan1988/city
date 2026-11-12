// The frame of each way a walker walks, and its stripe and pace among the bodies around it (SUMO
// MSPModel_Striping PState::walk), see ped_stripes.h. The candidate stripes are the footway's own (ped_stripe_m apart) and the places just clear of each
// piece of furniture, so a gap between two pieces is found even off the stripes. A stripe a footprint stands in beside
// the walker is shut; ahead, each stripe's room is the gap to the nearest body across it. The walker takes the stripe
// with the most room (aside costs, off its own side costs, the stripes left to the oncoming are not its own), moves
// toward it at a share of its pace without ever stepping sideways into furniture (a piece its step ahead reaches included;
// standing in one, it steps out), and keeps its gap behind the body ahead both where it stands and where it steps to. Held
// up past its jamtime (by the way: sidewalk, zebra, a single stripe) it closes in on the bodies ahead with no gap kept, to touching,
// at ped_jam_share of its pace, and takes any stripe beside them that opens: SUMO lets it through the walkers, but here no body ever
// stands in another (a queue at a kerb is waited behind or passed beside, as people do).
#include "sim/traffic/ped_stripes.h"
#include "sim/traffic/traffic_system.h"

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
constexpr float WALKERS_SHARE = 0.5f; // of the room between two walkers side by side, what each may step into in a tick
// 1 / (stripes - 1): where the k-th of a room's stripes lies, without a division per walker
constexpr float STRIPE_STEP[MAX_STRIPES + 1] = { 0.f, 0.f, 1.f, 1.f / 2, 1.f / 3, 1.f / 4, 1.f / 5, 1.f / 6, 1.f / 7 };

bool across(const Body &o, float c) { return c > o.lat_lo - o.reach && c < o.lat_hi + o.reach; }

// The gap a body leaves ahead of the places across it: at or under zero beside a piece of furniture (shut), BIG when it
// is behind (a walker: theirs to keep the gap) or passed (furniture).
float gap_of(const Body &o) {
	if (o.under) return BIG; // ground a vehicle holds, under its feet: it walks on out of it
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

// The room ahead of a centre at `c` across: the gap to the nearest body across it, a walker's to touching it. With nobody about
// it is the whole look ahead. `own`: the walker's own way on from where it stands (not a stripe's): a piece it is already alongside
// (its near end behind its centre) is no wall ahead, as walking along it takes it no deeper into it; it steps out of it sideways
// meanwhile (side_limits).
float room_at(const Body *b, int n, float c, float look, bool own) {
	float room = look;
	for (int k = 0; k < n; k++) {
		const Body &o = b[k];
		if (!across(o, c) || (own && o.furniture && o.along_lo < 0)) continue;
		room = std::min(room, gap_of(o));
	}
	return room;
}

// Out of the band of a piece it stands in, the walker steps to the side its room holds, the nearer one when both do.
bool out_low(const WayFrame &f, float shut_lo, float shut_hi) {
	const bool low_fits = shut_lo >= std::min(f.lo, f.u), high_fits = shut_hi <= std::max(f.hi, f.u);
	if (low_fits != high_fits) return low_fits;
	return f.u - shut_lo <= shut_hi - f.u;
}

// Where the walker may step sideways now: its room (or where it stands, when the room moved off it: it steps back in at
// its own lateral pace, never a jump), less the bodies beside it on either side: a piece of furniture up to its edge, a
// walker up to half the room between the two (the other steps this tick too, and may take the other half). A piece is
// beside it as soon as this tick's step ahead (`ahead`) reaches it, so the walker never steps into the band of a piece it is
// about to reach (a walker ahead is kept off by the pace's gap). Standing in a piece's band (put there, or brought there by
// its way), it steps out of it at `aside`, to the side its room holds. A walker beside it that its way has brought nearer than
// their two bodies (two ways that converge) makes it step away, `aside` at most: both do, so they part; so does ground a
// vehicle holds that it stands on (ped_held.cpp), past its room's edge if need be but never into a piece. The room and the
// furniture bound it hard, the walkers only within that: a walker never pushes it into a piece, nor keeps it in one.
void side_limits(const WayFrame &f, const Body *b, int n, float aside, float walk, float ahead, float &lo, float &hi) {
	float piece_lo = -BIG, piece_hi = BIG; // the furniture
	float soft_lo = -BIG, soft_hi = BIG; // the walkers beside it
	for (int k = 0; k < n; k++) {
		const Body &o = b[k];
		const float reached = o.furniture ? ahead : 0.f;
		if (o.under || o.along_lo - o.reach >= reached || o.along_hi + o.reach <= 0) continue; // ahead of its step or behind, not beside
		const float shut_lo = o.lat_lo - o.reach, shut_hi = o.lat_hi + o.reach;
		float &bound_lo = o.furniture ? piece_lo : soft_lo, &bound_hi = o.furniture ? piece_hi : soft_hi;
		const float mine = o.furniture ? 1.f : WALKERS_SHARE;
		if (f.u <= shut_lo) bound_hi = std::min(bound_hi, f.u + (shut_lo - f.u) * mine);
		else if (f.u >= shut_hi) bound_lo = std::max(bound_lo, f.u - (f.u - shut_hi) * mine);
		else if (o.furniture ? out_low(f, shut_lo, shut_hi) : f.u < (o.lat_lo + o.lat_hi) * 0.5f) bound_hi = std::min(bound_hi, std::max(shut_lo, f.u - aside)); // lower
		else bound_lo = std::max(bound_lo, std::min(shut_hi, f.u + aside));
	}
	// ground a vehicle holds, under its feet: it walks off it (`walk`: a tick at its own pace), past its room's edge if need be
	// (it steps back in after), as far as the furniture lets it (held between the two, it walks on along its way)
	for (int k = 0; k < n; k++) {
		if (!b[k].under) continue;
		const float off = f.u < b[k].lat_lo ? f.u - walk : f.u + walk;
		lo = hi = piece_lo <= piece_hi ? clampf(off, piece_lo, piece_hi) : f.u;
		return;
	}
	const float hard_lo = std::max(std::min(f.lo, f.u), piece_lo), hard_hi = std::min(std::max(f.hi, f.u), piece_hi); // the room and the furniture
	lo = std::max(hard_lo, std::min(std::max(soft_lo, hard_lo), hard_hi));
	hi = std::min(hard_hi, std::max(std::min(soft_hi, hard_hi), hard_lo));
	if (hi < lo) lo = hi = clampf(f.u, hi, lo); // made to step past its room's edge: to the edge; from both sides: it stays
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

// The pace that keeps the walker's gap and headway behind `room` ahead: its own speed at most; held up past its jamtime (`squeeze`) a
// share of it, closing to touching with no gap kept.
float pace_for(const StripeRules &r, const CrowdEntry &w, float room, bool squeeze) {
	return std::clamp((room - (squeeze ? 0.f : r.gap)) * r.inv_headway, 0.f, squeeze ? w.speed * r.jam_share : w.speed);
}

} // namespace

V2 left_of(const V2 &d) { return V2(d.y, -d.x); }

// On a sidewalk: across is the sidewalk's own u (+ on the left of a -> b) whichever way it walks; in the world (where
// the last pose put it, facing its way) only when it is near a junction.
void sidewalk_frame(const CrowdEntry &w, float half, float stripe, bool world, WayFrame &f) {
	f.u = w.u;
	f.lo = -half;
	f.hi = half;
	f.narrow = 2 * half < stripe;
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
		f.zebra = true;
		f.lo = -zh + p.ped_body_m - w.line_off;
		f.hi = std::max(f.lo, -p.ped_body_m - w.line_off);
		f.keep = clampf(0.f, f.lo, f.hi);
		f.narrow = f.hi - f.lo < p.ped_stripe_m;
		return;
	}
	if (toward_kerb(w)) {
		// the queue before a kerb runs along the way across (ped_kerb.cpp), so across it is along the kerb: its room is the
		// waiting span, measured from its own line (line_off: how far left of the zebra's middle that line lies)
		const KerbSpan span = kerb_span(s);
		const float aside = -w.line_off - span.middle; // its line right of the span's middle
		f.lo = aside - span.room;
		f.hi = aside + span.room;
		f.keep = 0;
		f.queue = true;
		return;
	}
	// round a corner: room opens off the line's start no faster than the walker steps aside (a share of its pace per metre
	// walked), as wide either side as the sidewalk it heads for (w.seg)
	const float wide = walk_half(s, w.seg, w.side), half = std::min(wide, p.ped_lateral_share * w.ls);
	f.narrow = 2 * wide < p.ped_stripe_m;
	f.lo = -half;
	f.hi = half;
	if (w.wi + 1 == w.wn) {
		// the last line ends on the next sidewalk, somewhere across it (ped_routes.cpp aim_onto_sidewalk): its room closes onto that
		// sidewalk's room toward the end, no faster than the walker steps aside, so it comes onto the sidewalk inside it
		Pose mid;
		sidewalk_pose(s, w.seg, w.side, w.s, 0.f, mid);
		const float ends = (to.x - mid.x) * mid.fz - (to.z - mid.z) * mid.fx; // where across the sidewalk (+ left of a -> b)
		const float left = w.dir == 0 ? ends : -ends; // and left of the way it walks
		const float slack = p.ped_lateral_share * std::max(0.f, w.llen - w.ls);
		f.lo = std::max(f.lo, -wide - left - slack);
		f.hi = std::min(f.hi, wide - left + slack);
	}
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
	stripe = p.ped_stripe_m;
	inv_stripe = 1.f / p.ped_stripe_m;
	gain = p.ped_stripe_gain_m;
	hold = p.ped_stripe_hold_s;
	jam_s = p.ped_jam_s;
	jam_crossing_s = p.ped_jam_crossing_s;
	jam_narrow_s = p.ped_jam_narrow_s;
	jam_share = p.ped_jam_share;
	moving = p.ped_moving_ms;
	settled_keeps = p.ped_lateral_cost >= p.ped_keep_right_cost;
}

StripeChoice choose_stripe(const StripeRules &r, const CrowdEntry &w, const WayFrame &f, const Body *b, int n) {
	// how long it stands held up before it squeezes past: by the way (one stripe only: narrow; a zebra; else), never in a
	// kerb's queue
	const float jam_after = f.queue ? BIG : f.narrow ? r.jam_narrow_s : (f.zebra ? r.jam_crossing_s : r.jam_s);
	// nobody about, on the stripe it aims at (inside its room, not one left to the oncoming), not held up: it walks on
	if (n == 0 && r.settled_keeps && w.u_goal == f.u && f.u >= f.lo && f.u <= f.hi && f.u <= f.reserve_from && w.jam < jam_after) {
		StripeChoice out;
		out.u = out.u_goal = f.u;
		out.lo = std::max(f.lo, f.u - r.aside_share * w.speed * r.dt);
		out.hi = std::min(f.hi, f.u + r.aside_share * w.speed * r.dt);
		out.held = w.held + r.dt;
		out.pace = std::min(std::clamp((r.look - r.gap) * r.inv_headway, 0.f, w.speed), w.pace + r.accel * r.dt);
		return out; // jam 0: free ahead
	}
	float cand[MAX_CANDIDATES];
	bool here = false;
	const int m = candidates(r, w, f, b, n, cand, here);
	float rooms[MAX_CANDIDATES];
	rooms_of(b, n, cand, m, r.look, rooms);
	float aimed_room = r.look, aimed = 0, best_score = -BIG;
	int best = AIMED;
	for (int k = 0; k < m; k++) {
		const float c = cand[k], room = rooms[k];
		const float sc = room - r.lateral_cost * std::fabs(c - f.u) - r.keep_cost * std::fabs(c - f.keep) -
				(c > f.reserve_from ? INAPPROPRIATE_M : 0.f);
		if (k == AIMED) {
			aimed_room = room;
			aimed = sc;
		}
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
	// the gap behind the body ahead on its own way on from where it stands; held up past its jamtime it closes in to touching
	const float room = room_at(b, n, f.u, r.look, true);
	float want = pace_for(r, w, room, false);
	const bool squeezing = w.jam >= jam_after;
	if (squeezing) {
		out.jam = want > r.moving ? 0.f : w.jam;
		want = pace_for(r, w, room, true);
	} else {
		out.jam = want < r.moving ? w.jam + r.dt : 0.f;
	}
	out.pace = std::min(want, w.pace + r.accel * r.dt);
	// aside at a share of its own pace (it can step aside standing), never into the furniture beside it or reached this tick
	const float aside = r.aside_share * w.speed * r.dt;
	float lo, hi;
	side_limits(f, b, n, aside, w.speed * r.dt, out.pace * r.dt, lo, hi);
	out.u = clampf(f.u + std::clamp(out.u_goal - f.u, -aside, aside), lo, hi);
	out.lo = std::min(out.u, std::max(lo, f.u - aside)); // where its step aside may end this tick: what ped_apart.h may move it within
	out.hi = std::max(out.u, std::min(hi, f.u + aside));
	// the step ahead is taken where it steps to: its gap is kept to the bodies across that place as well
	if (out.u != f.u) out.pace = std::min(out.pace, pace_for(r, w, room_at(b, n, out.u, r.look, true), squeezing));
	return out;
}

void on_sidewalk(Pedestrian &w) {
	w.u = w.u_goal = w.dir == 0 ? -w.track : w.track; // its own side of the way
	w.stripe_held = 0;
	w.pace = w.speed;
	w.jam = 0;
}

void onto_line(Pedestrian &w) {
	w.u = w.u_goal = 0; // a new line starts where the walker stands
	w.stripe_held = 0;
}

} // namespace traffic
} // namespace city
