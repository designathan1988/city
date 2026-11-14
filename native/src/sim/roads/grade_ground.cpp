// What a road's heights are solved from (grade_parts.h): a segment's frame (its stations along the curve, the plates it is
// held level over, how it is built), the ground read under its stations across its casing, and the line it is designed
// on: the mean of that ground over a reach along the road, within the ground grade, one line along every piece of a road
// laid in pieces. The lines live in the solve's cache (GroundCache), within its budget: let go, they are read again.
#include "sim/roads/grade_parts.h"

#include "core/math.h"
#include "core/units.h"
#include "sim/catalog/catalog.h"
#include "sim/roads/grade_line.h"
#include "sim/roads/junction.h"
#include "sim/roads/road_network.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace city {

namespace {
constexpr int ACROSS = 7; // the ground is read at this many places across the casing, evenly from edge to edge
constexpr int TOP_SPREAD = 2; // a station's top is the highest within this many stations: a chord clears both its ends
constexpr double LEAST_LEN_M = 1e-3;
constexpr float NO_LIFT = 1e-6f; // a lift under this is none: the road follows its design line
constexpr double STRAIGHT_ON_RAD = 55.0 * PI_D / DEG_PER_HALF_TURN; // two legs bent less carry one road on, not a corner
constexpr float BYTES_PER_MB = 1024.f * 1024.f;

// Half of what a road is read across: its widest side, footway included, and the casing band past it.
float half_read(const Profile &p) { return p.half() + CASING_BAND_M; }

// How far the two legs of a node turn from going straight on: 0 for a straight joint.
double turn_at(const RoadNetwork &net, const RoadNode &nd, int n) {
	const Arm first = arm_of(net.segs[size_t(nd.segs[0])].q, net.profile_of(nd.segs[0]), net.segs[size_t(nd.segs[0])].a == n ? 0 : 1);
	const Arm second = arm_of(net.segs[size_t(nd.segs[1])].q, net.profile_of(nd.segs[1]), net.segs[size_t(nd.segs[1])].a == n ? 0 : 1);
	const double along = first.dx * second.dx + first.dz * second.dz;
	return PI_D - std::acos(std::max(-1.0, std::min(1.0, along)));
}

// A node that only carries one road on: a straight joint of one profile and one structure (the cut of a long road).
bool carries_on(const RoadNetwork &net, int n) {
	const RoadNode &nd = net.nodes[size_t(n)];
	if (nd.segs.size() != 2 || !nd.joint) return false;
	const RoadSegment &one = net.segs[size_t(nd.segs[0])], &other = net.segs[size_t(nd.segs[1])];
	return one.profile == other.profile && structure_of(one.flags) == structure_of(other.flags);
}

// How far from node n a segment is held level. A joint that carries one road on holds nothing; two legs that run on
// through a change (of width, of structure, a bend) hold the margin alone; a plain dead end its cut and the margin; a
// junction its mouth, the widest footway and casing band of its roads, and the margin.
float plate_of(const RoadNetwork &net, int n, double mouth) {
	const RoadNode &nd = net.nodes[size_t(n)];
	if (nd.segs.size() == 2 && (nd.joint || turn_at(net, nd, n) < STRAIGHT_ON_RAD)) return carries_on(net, n) ? 0.f : PLATE_MARGIN_M;
	if (nd.segs.size() == 1 && !nd.bulb) return float(mouth) + PLATE_MARGIN_M;
	float beside = 0;
	for (int s : nd.segs) {
		const Profile &p = net.profile_of(s);
		beside = std::max(beside, std::max(p.left - p.asph_left, p.right - p.asph_right) + CASING_BAND_M);
	}
	return float(mouth) + beside + PLATE_MARGIN_M;
}

// One piece of a road laid in pieces, and whether it runs the way the road is walked.
struct Piece {
	int seg;
	bool forward;
};

// The pieces of the road segment s is a piece of, in order from one end to the other (s alone when no joint carries it on).
std::vector<Piece> road_of(const RoadGrade &grade, const RoadNetwork &net, int s) {
	std::vector<Piece> ahead{ { s, true } }, behind;
	const auto walk = [&](int from_node, std::vector<Piece> &out, bool towards_b) {
		for (int at = s, n = from_node; carries_on(net, n);) {
			const RoadNode &nd = net.nodes[size_t(n)];
			const int next = nd.segs[0] == at ? nd.segs[1] : nd.segs[0];
			if (next == s || !grade.has(next)) break; // a ring comes back to its start
			const RoadSegment &g = net.segs[size_t(next)];
			const bool forward = towards_b ? g.a == n : g.b == n;
			out.push_back({ next, forward });
			n = (g.a == n) ? g.b : g.a;
			at = next;
		}
	};
	walk(net.segs[size_t(s)].b, ahead, true);
	walk(net.segs[size_t(s)].a, behind, false);
	std::reverse(behind.begin(), behind.end());
	behind.insert(behind.end(), ahead.begin(), ahead.end());
	return behind;
}

// The ground of segment s read when its lines do not stand for its frame; counted in the cache.
void ground_of(RoadGrade &grade, const RoadNetwork &net, int s, const GroundAt &ground_at, const GradeTune &t) {
	if (grade.ground.by_seg[size_t(s)].read_for(grade.prof[size_t(s)])) return;
	grade.ground.open(s);
	read_ground(grade, net, s, ground_at, t);
	grade.ground.close(s);
}

void write_line(RoadGrade &grade, int s, const std::vector<float> &line) {
	grade.ground.open(s).line = line;
	grade.ground.close(s);
}
} // namespace

bool GroundLines::read_for(const GradeProfile &p) const {
	return top.size() == p.h.size() && mean.size() == p.h.size() && half == p.half && std::equal(q, q + CURVE_NUMBERS, p.q);
}

GroundLines &GroundCache::open(int s) {
	GroundLines &l = by_seg[size_t(s)];
	if (!l.held) {
		l.held = true;
		held.push_back(s);
	}
	bytes -= std::min(bytes, l.bytes());
	return l;
}

void GroundCache::close(int s) {
	GroundLines &l = by_seg[size_t(s)];
	l.used = solve;
	bytes += l.bytes();
}

void GroundCache::drop(int s) {
	if (size_t(s) >= by_seg.size() || !by_seg[size_t(s)].held) return;
	bytes -= std::min(bytes, by_seg[size_t(s)].bytes());
	by_seg[size_t(s)] = GroundLines();
	held.erase(std::remove(held.begin(), held.end(), s), held.end());
}

void GroundCache::fit(size_t budget) {
	if (bytes <= budget) return;
	// the oldest read first; of one solve, by id: the same map lets the same lines go
	std::sort(held.begin(), held.end(), [&](int a, int b) {
		const uint32_t ua = by_seg[size_t(a)].used, ub = by_seg[size_t(b)].used;
		return ua != ub ? ua < ub : a < b;
	});
	size_t gone = 0;
	while (gone < held.size() && bytes > budget) {
		GroundLines &l = by_seg[size_t(held[gone++])];
		bytes -= std::min(bytes, l.bytes());
		l = GroundLines();
	}
	held.erase(held.begin(), held.begin() + std::ptrdiff_t(gone));
}

void frame_profile(RoadGrade &grade, const RoadNetwork &net, int s) {
	const RoadSegment &g = net.segs[size_t(s)];
	GradeProfile &p = grade.prof[size_t(s)];
	const float len = float(std::max(LEAST_LEN_M, g.len));
	const int count = std::max(2, std::min(STATIONS_MOST, int(std::ceil(len / STATION_M)) + 1));
	p.len = len;
	p.step = len / float(count - 1);
	p.half = half_read(net.profile_of(s));
	p.a = g.a;
	p.b = g.b;
	p.plate_a = std::min(len * float(MOUTH_SHARE), plate_of(net, g.a, g.retreat_a));
	p.plate_b = std::min(len * float(MOUTH_SHARE), plate_of(net, g.b, g.retreat_b));
	p.structure = structure_of(g.flags);
	p.walls = (g.flags & SEG_CUT_WALLS) != 0;
	p.manual = std::fabs(net.nodes[size_t(g.a)].lift) > NO_LIFT || std::fabs(net.nodes[size_t(g.b)].lift) > NO_LIFT;
	const double curve[CURVE_NUMBERS] = { g.q.x0, g.q.z0, g.q.cx, g.q.cz, g.q.x1, g.q.z1 };
	std::copy(curve, curve + CURVE_NUMBERS, p.q);
	if (p.h.size() != size_t(count)) p.h.assign(size_t(count), 0.f);
	p.valid = true;
}

void read_ground(RoadGrade &grade, const RoadNetwork &net, int s, const GroundAt &ground_at, const GradeTune &t) {
	const RoadSegment &g = net.segs[size_t(s)];
	const GradeProfile &p = grade.prof[size_t(s)];
	GroundLines &l = grade.ground.by_seg[size_t(s)];
	const size_t count = p.h.size();
	thread_local std::vector<float> highest;
	highest.assign(count, 0.f);
	l.mean.assign(count, 0.f);
	l.line.clear();
	for (size_t i = 0; i < count; i++) {
		const double at = t_at_length(g.table, double(p.step) * double(i));
		double x, z, dx, dz;
		g.q.at(at, x, z);
		g.q.deriv(at, dx, dz);
		const double run = std::hypot(dx, dz);
		const double nx = run > 0 ? dz / run : 0, nz = run > 0 ? -dx / run : 0; // across the way
		float high = -std::numeric_limits<float>::max(), sum = 0;
		for (int k = 0; k < ACROSS; k++) {
			const double off = double(p.half) * (2.0 * k / (ACROSS - 1) - 1.0);
			const float y = ground_at(float(x + nx * off), float(z + nz * off));
			high = std::max(high, y);
			sum += y;
		}
		highest[i] = high + t.clearance;
		l.mean[i] = sum / float(ACROSS) + t.clearance;
	}
	l.top.assign(count, 0.f);
	const int last = int(count) - 1;
	for (int i = 0; i <= last; i++) {
		float high = highest[size_t(i)];
		for (int k = std::max(0, i - TOP_SPREAD); k <= std::min(last, i + TOP_SPREAD); k++) high = std::max(high, highest[size_t(k)]);
		l.top[size_t(i)] = high;
	}
	std::copy(p.q, p.q + CURVE_NUMBERS, l.q);
	l.half = p.half;
}

void design_lines(RoadGrade &grade, const RoadNetwork &net, int s, const GroundAt &ground_at, const GradeTune &t, std::vector<int> &pieces) {
	const std::vector<Piece> road = road_of(grade, net, s);
	for (const Piece &c : road) ground_of(grade, net, c.seg, ground_at, t);
	thread_local std::vector<float> line;
	if (road.size() == 1) {
		line = grade.ground.by_seg[size_t(s)].mean;
		mean_over(line, grade.prof[size_t(s)].step, t.line_reach);
		within_grade(line, grade.prof[size_t(s)].step, t.ground);
		write_line(grade, s, line);
		pieces.push_back(s);
		return;
	}
	// the ground along the whole road on one even step, and the design line made of it as of one profile
	float total = 0, step = std::numeric_limits<float>::max();
	for (const Piece &c : road) {
		total += grade.prof[size_t(c.seg)].len;
		step = std::min(step, grade.prof[size_t(c.seg)].step);
	}
	const int count = int(std::ceil(total / step)) + 1;
	const float even = total / float(count - 1);
	line.assign(size_t(count), 0.f);
	float from = 0;
	size_t piece = 0;
	for (int i = 0; i < count; i++) {
		const float arc = float(i) * even;
		while (piece + 1 < road.size() && arc > from + grade.prof[size_t(road[piece].seg)].len) from += grade.prof[size_t(road[piece++].seg)].len;
		const GradeProfile &p = grade.prof[size_t(road[piece].seg)];
		const float along = std::min(p.len, arc - from);
		line[size_t(i)] = line_at(grade.ground.by_seg[size_t(road[piece].seg)].mean, (road[piece].forward ? along : p.len - along) / p.step);
	}
	mean_over(line, even, t.line_reach);
	within_grade(line, even, t.ground);
	thread_local std::vector<float> own;
	from = 0;
	for (const Piece &c : road) {
		const GradeProfile &p = grade.prof[size_t(c.seg)];
		own.resize(p.h.size());
		for (size_t i = 0; i < own.size(); i++) {
			const float arc = p.step * float(i);
			own[i] = line_at(line, (from + (c.forward ? arc : p.len - arc)) / even);
		}
		write_line(grade, c.seg, own);
		pieces.push_back(c.seg);
		from += p.len;
	}
}

GradeTune grade_tune(const Tuning &tn) {
	GradeTune t;
	t.ground = tn.grade_ground;
	t.ramp = tn.grade_ramp;
	t.deck = tn.grade_deck;
	t.deck_tie = tn.grade_deck_tie;
	t.tunnel = tn.grade_tunnel;
	t.clearance = tn.grade_clearance_m;
	t.line_reach = tn.grade_line_reach_m;
	t.plate_curve = tn.grade_plate_curve_m;
	t.tie = tn.grade_tie_m;
	t.tie_share = tn.grade_tie_share;
	t.curve = tn.grade_curve_m;
	t.crest = tn.grade_crest_m;
	t.ramp_sag = tn.grade_ramp_sag_m;
	t.ends_margin = tn.grade_ends_margin;
	t.under_deck = tn.grade_under_deck_m;
	t.floor_fade = tn.grade_floor_fade_m;
	t.floor_ramp_share = tn.grade_floor_ramp_share;
	t.rounds_most = int(tn.grade_rounds_most);
	t.ground_cache = size_t(tn.grade_ground_cache_mb * BYTES_PER_MB);
	for (size_t s = 0; s < STRUCTURE_COUNT; s++) t.structure[s] = structure_spec(Structure(s), tn);
	return t;
}

StructureSpec structure_spec(Structure s, const Tuning &tn) {
	switch (s) {
		case Structure::ELEVATED: return { tn.grade_clear_elevated_m, tn.grade_deck_elevated_m, true };
		case Structure::BRIDGE: return { tn.grade_clear_bridge_m, tn.grade_deck_bridge_m, true };
		case Structure::TUNNEL:
			// a tunnel's depth: its headroom, its arch and the ground past them, under the lowest ground
			return { -(tn.grade_tunnel_headroom_m + tn.grade_tunnel_arch_m + tn.grade_tunnel_floor_m), tn.grade_deck_tunnel_m, false };
		case Structure::GROUND: break;
	}
	return { 0.f, tn.grade_deck_ground_m, false };
}

} // namespace city
