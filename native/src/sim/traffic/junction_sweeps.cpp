// Where two movements' bodies meet at a junction, the conflicts the gate keeps apart (build_node): each connector is swept along
// its way (the end of the lane it leaves, its curve, the start of the lane it enters) by a body of each class (traffic.json
// body_*), a rectangle of its envelope sweep_margin_m larger at its middle every sweep_step_m, heading the way's chord. Two
// connectors conflict where any of their rectangles overlap, and each keeps, per pair of classes, how far along the other's way
// that other body has gone once it is clear. A connector is driven only by the classes whose sweep over its curve keeps off the
// corners' footways (max_class): a turn too tight for a body is not driven by it. The reference game's rule and envelopes.
// The work follows where bodies meet, never every sample against every sample: a way's samples are bound in blocks by boxes that
// hold their largest bodies, so two stretches whose boxes stay apart are never looked at sample by sample; the three envelopes
// share their middle and heading, each inside the next, so a pair of classes meets only where the pair one class larger does;
// and of each pair only the farthest meeting sample along each way is kept, so it is searched from the way's end.
#include "sim/traffic/lane_graph_internal.h"

#include "sim/street/footway.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace city {
namespace traffic {

namespace {
constexpr float LEAST_CHORD_M = 1e-3f; // the chord's ends closer than this: the way's own direction
constexpr int BLOCK = 8; // samples one box bounds
// A box is this much larger than the bodies it holds: its middle is kept in the map's coordinates, rounded by up to half a
// millimetre twelve kilometres out, and no body may fall outside it by that.
constexpr float BOX_SLACK_M = 0.01f;
constexpr int HEAVY = BODY_CLASSES - 1;
constexpr float HALF = 0.5f;

struct Sample {
	V2 at, f; // a body's middle and its heading
	float c = 0; // its arc along the way, 0 where the curve starts
};

// An oriented rectangle: a middle, a unit heading, half its length and half its width.
struct Box {
	Sample mid;
	float hl = 0, hw = 0;
};

// The half sizes of each class's rectangle at a sample: its envelope, half a step longer each way, a margin round it.
struct Sizes {
	float hl[BODY_CLASSES], hw[BODY_CLASSES];
};

// A connector's way swept: its samples in order of arc, the boxes of its blocks of BLOCK samples, its curve's length.
struct Way {
	std::vector<Sample> s;
	std::vector<Box> box;
	float len = 0;
};

// A body class's envelope: its length and width.
void envelope(const TrafficParams &p, int cls, float &length, float &width) {
	const float sizes[BODY_CLASSES][2] = { { p.body_small_length_m, p.body_small_width_m }, { p.body_car_length_m, p.body_car_width_m },
		{ p.body_heavy_length_m, p.body_heavy_width_m } };
	length = sizes[cls][0];
	width = sizes[cls][1];
}

// The way's point `at` metres along: on the lane it leaves before 0, on its curve to its length, on the lane it enters after.
void way_point(const CityState &s, const Conn &c, float at, Pose &o) {
	const LaneGraph &g = s.traffic.g;
	if (at < 0) lane_pose(s, c.from_ds, c.from_lane, g.ds[size_t(c.from_ds)].len + at, o);
	else if (at <= c.len) conn_pose(c, at, o);
	else lane_pose(s, c.to_ds, c.to_lane, at - c.len, o);
}

// Two rectangles (middles, unit headings, half lengths and widths) overlap: no separating axis among their four sides.
bool overlap(const Sample &a, float ahl, float ahw, const Sample &b, float bhl, float bhw) {
	const V2 d = b.at - a.at, al(-a.f.y, a.f.x), bl(-b.f.y, b.f.x);
	const float along = std::fabs(a.f.dot(b.f)), across = std::fabs(a.f.dot(bl));
	return std::fabs(d.dot(a.f)) < ahl + bhl * along + bhw * across && std::fabs(d.dot(al)) < ahw + bhl * across + bhw * along &&
			std::fabs(d.dot(b.f)) < ahl * along + ahw * across + bhl && std::fabs(d.dot(bl)) < ahl * across + ahw * along + bhw;
}

bool overlap(const Sample &a, float ahl, float ahw, const Box &b) { return overlap(a, ahl, ahw, b.mid, b.hl, b.hw); }

// The box of samples [from, to): every heavy rectangle among them, in the frame of the middle one's heading.
Box box_of(const std::vector<Sample> &s, int from, int to, const Sizes &z) {
	const Sample &m = s[size_t((from + to) / 2)];
	const V2 l(-m.f.y, m.f.x);
	float lo_f = std::numeric_limits<float>::max(), hi_f = -lo_f, lo_l = lo_f, hi_l = -lo_f;
	for (int k = from; k < to; k++) {
		const Sample &q = s[size_t(k)];
		const V2 ql(-q.f.y, q.f.x);
		for (const float a : { -z.hl[HEAVY], z.hl[HEAVY] })
			for (const float b : { -z.hw[HEAVY], z.hw[HEAVY] }) {
				const V2 corner = (q.at - m.at) + q.f * a + ql * b; // from the middle first: two near points subtract exactly
				const float along = corner.dot(m.f), across = corner.dot(l);
				lo_f = std::min(lo_f, along);
				hi_f = std::max(hi_f, along);
				lo_l = std::min(lo_l, across);
				hi_l = std::max(hi_l, across);
			}
	}
	Box out;
	out.mid.f = m.f;
	out.mid.at = m.at + m.f * ((lo_f + hi_f) * HALF) + l * ((lo_l + hi_l) * HALF);
	out.hl = (hi_f - lo_f) * HALF + BOX_SLACK_M;
	out.hw = (hi_l - lo_l) * HALF + BOX_SLACK_M;
	return out;
}

Way sweep(const CityState &s, const Conn &c, const Sizes &z) {
	const TrafficParams &p = s.catalog.traffic;
	Way w;
	w.len = c.len;
	const float from = -p.body_heavy_length_m * HALF - p.sweep_queue_back_m, to = c.len + p.body_heavy_length_m * HALF;
	for (float at = from; at <= to; at += p.sweep_step_m) {
		Pose m, a, b;
		way_point(s, c, at, m);
		way_point(s, c, at + p.heading_chord_m, a);
		way_point(s, c, at - p.heading_chord_m, b);
		const V2 f(a.x - b.x, a.z - b.z);
		const float l = f.len();
		w.s.push_back({ V2(m.x, m.z), l > LEAST_CHORD_M ? f * (1.f / l) : V2(m.fx, m.fz), at });
	}
	for (int k = 0; k < int(w.s.size()); k += BLOCK) w.box.push_back(box_of(w.s, k, std::min(int(w.s.size()), k + BLOCK), z));
	return w;
}

// The largest class whose body, at its middle along the curve, keeps every corner off the footways of the junction's corners.
int largest_fit(const CityState &s, const Way &way, const JunctionKerbs &kerbs) {
	for (int cls = HEAVY; cls >= 0; cls--) {
		float length, width;
		envelope(s.catalog.traffic, cls, length, width);
		bool fits = true;
		for (const Sample &q : way.s) {
			if (q.c < 0 || q.c > way.len) continue;
			const V2 l(-q.f.y, q.f.x);
			for (const float a : { -length * HALF, length * HALF })
				for (const float b : { -width * HALF, width * HALF }) {
					const V2 corner = q.at + q.f * a + l * b;
					fits = fits && !kerbs.on_footway(corner.x, corner.y);
				}
			if (!fits) break;
		}
		if (fits) return cls;
	}
	return -1;
}

// For each block of `a`, the blocks of `b` whose boxes meet it: no body of `a` there meets one of `b` outside them.
std::vector<std::vector<int>> meeting_blocks(const Way &a, const Way &b, uint64_t &tests) {
	std::vector<std::vector<int>> out(a.box.size());
	for (size_t p = 0; p < a.box.size(); p++)
		for (size_t q = 0; q < b.box.size(); q++) {
			tests++;
			if (overlap(a.box[p].mid, a.box[p].hl, a.box[p].hw, b.box[q])) out[p].push_back(int(q));
		}
	return out;
}

// The farthest sample of `a`, at or before `from`, whose body of class `ca` meets a body of class `cb` on `b`; -1 where none does.
// A body past its curve by its own half has left it (it meets nothing more there).
int furthest(const Way &a, const Way &b, const std::vector<std::vector<int>> &meets, int ca, int cb, int from, const Sizes &z,
		uint64_t &tests) {
	for (int i = from; i >= 0; i--) {
		const std::vector<int> &near = meets[size_t(i / BLOCK)];
		if (near.empty()) {
			i = i / BLOCK * BLOCK; // the whole block stays apart from `b`: on to the block before it
			continue;
		}
		const Sample &x = a.s[size_t(i)];
		if (x.c > a.len + z.hl[ca]) continue;
		for (const int q : near) {
			tests++;
			if (!overlap(x, z.hl[ca], z.hw[ca], b.box[size_t(q)])) continue;
			const int end = std::min(int(b.s.size()), (q + 1) * BLOCK);
			for (int k = q * BLOCK; k < end; k++) {
				const Sample &y = b.s[size_t(k)];
				if (y.c > b.len + z.hl[cb]) continue;
				tests++;
				if (overlap(x, z.hl[ca], z.hw[ca], y, z.hl[cb], z.hw[cb])) return i;
			}
		}
	}
	return -1;
}
} // namespace

int body_class(const TrafficParams &p, VehicleType type) {
	const VehicleClass &v = p.classes[int(type)];
	for (int cls = 0; cls < BODY_CLASSES; cls++) {
		float length, width;
		envelope(p, cls, length, width);
		if (v.length <= length && 2 * v.half_width <= width) return cls;
	}
	return HEAVY;
}

SweepWork find_conflicts(CityState &s, const std::vector<int> &ids) {
	LaneGraph &g = s.traffic.g;
	const TrafficParams &p = s.catalog.traffic;
	SweepWork work;
	if (ids.empty()) return work;
	Sizes z;
	for (int cls = 0; cls < BODY_CLASSES; cls++) {
		envelope(p, cls, z.hl[cls], z.hw[cls]);
		z.hl[cls] = z.hl[cls] * HALF + p.sweep_step_m * HALF + p.sweep_margin_m;
		z.hw[cls] = z.hw[cls] * HALF + p.sweep_margin_m;
	}
	const JunctionKerbs kerbs(s.roads, g.conns[size_t(ids.front())].node);
	std::vector<Way> ways(ids.size());
	for (size_t i = 0; i < ids.size(); i++) {
		Conn &c = g.conns[size_t(ids[i])];
		ways[i] = sweep(s, c, z);
		c.max_class = int8_t(largest_fit(s, ways[i], kerbs));
	}
	const float half_step = p.sweep_step_m * HALF;
	std::array<int, BODY_PAIRS> fa, fb; // per class pair [ca * BODY_CLASSES + cb]: the farthest meeting sample on a, on b
	std::array<float, BODY_PAIRS> clear_a, clear_b;
	for (size_t i = 0; i < ids.size(); i++)
		for (size_t j = i + 1; j < ids.size(); j++) {
			Conn &a = g.conns[size_t(ids[i])], &b = g.conns[size_t(ids[j])];
			if (a.from_ds == b.from_ds && a.from_lane == b.from_lane) continue; // out of one lane: they follow each other
			work.pairs++;
			const Way &wa = ways[i], &wb = ways[j];
			const std::vector<std::vector<int>> ab = meeting_blocks(wa, wb, work.tests);
			std::vector<std::vector<int>> ba(wb.box.size());
			for (size_t q = 0; q < ab.size(); q++)
				for (const int r : ab[q]) ba[size_t(r)].push_back(int(q));
			// from the largest bodies down: a pair meets only where both pairs one class larger meet, so its search on each way
			// starts at the nearer of their farthest samples there, and none is made where either never meets
			for (int ca = HEAVY; ca >= 0; ca--)
				for (int cb = HEAVY; cb >= 0; cb--) {
					int from_a = int(wa.s.size()) - 1, from_b = int(wb.s.size()) - 1;
					if (ca < HEAVY) {
						from_a = std::min(from_a, fa[size_t((ca + 1) * BODY_CLASSES + cb)]);
						from_b = std::min(from_b, fb[size_t((ca + 1) * BODY_CLASSES + cb)]);
					}
					if (cb < HEAVY) {
						from_a = std::min(from_a, fa[size_t(ca * BODY_CLASSES + cb + 1)]);
						from_b = std::min(from_b, fb[size_t(ca * BODY_CLASSES + cb + 1)]);
					}
					const size_t k = size_t(ca * BODY_CLASSES + cb);
					fa[k] = from_a < 0 || from_b < 0 ? -1 : furthest(wa, wb, ab, ca, cb, from_a, z, work.tests);
					fb[k] = fa[k] < 0 ? -1 : furthest(wb, wa, ba, cb, ca, from_b, z, work.tests);
				}
			if (fa[size_t(HEAVY * BODY_CLASSES + HEAVY)] < 0) continue; // their largest bodies never meet: nor do any
			for (int ca = 0; ca < BODY_CLASSES; ca++)
				for (int cb = 0; cb < BODY_CLASSES; cb++) {
					const size_t k = size_t(ca * BODY_CLASSES + cb);
					clear_a[k] = fa[k] < 0 ? -NO_LEADER : wa.s[size_t(fa[k])].c + half_step;
					clear_b[size_t(cb * BODY_CLASSES + ca)] = fb[k] < 0 ? -NO_LEADER : wb.s[size_t(fb[k])].c + half_step;
				}
			a.conflicts.push_back(ids[j]);
			a.conflict_clear.push_back(clear_b);
			b.conflicts.push_back(ids[i]);
			b.conflict_clear.push_back(clear_a);
		}
	return work;
}

} // namespace traffic
} // namespace city
