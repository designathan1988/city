// A road's profile made ready for the solve (Roadcraft 4.0 world/elevation.ts, the stations step and readStations): its
// stations along the curve, the ground read under them across its casing, and its junction plates; and the reads of a
// solved profile. The solve itself is road_grade_solve.cpp with its rounds in road_grade_rounds.cpp, one profile's
// solvers road_grade_profile.cpp, the figures it is tuned by road_grade_tune.cpp.
#include "sim/roads/road_grade_internal.h"

#include "core/math.h"
#include "core/units.h"
#include "sim/roads/junction.h"
#include "sim/roads/road_network.h"

#include <algorithm>
#include <cmath>
#include <deque>

namespace city {

namespace {
// readStations LATERAL: where the ground is read across the casing, as shares of its half width.
constexpr float LATERAL[] = { -1.f, -0.62f, -0.28f, 0.f, 0.28f, 0.62f, 1.f };
constexpr int LATERAL_COUNT = int(sizeof(LATERAL) / sizeof(LATERAL[0]));
constexpr int DILATE_STATIONS = 2; // DILATE: a chord clears the ground at both its ends
constexpr float SMOOTH_REACH_M = 36.f; // SMOOTH_REACH: how far along a road its grade line is averaged
constexpr double TINY_LEN = 1e-3;
constexpr float AUTHORED_EPS = 1e-6f; // a lift under this is none: the road follows its grade line
constexpr float BIG_STEP = 1e9f;

// A line of values at `at` (in its steps), linear between them and held at its ends.
float read_line(const std::vector<float> &v, float at) {
	const float f = std::min(float(v.size() - 1), std::max(0.f, at));
	const size_t lo = size_t(f), hi = std::min(v.size() - 1, lo + 1);
	return v[lo] + (v[hi] - v[lo]) * (f - float(lo));
}

// junction/build.ts MAX_CONTINUOUS_BEND: two legs bent less than this carry one road on through a transition, not a corner.
constexpr double MAX_CONTINUOUS_BEND = 55.0 * PI_D / DEG_PER_HALF_TURN;

// How far two legs at a node turn from going straight on (junction/build.ts bendAngle): 0 for a straight joint.
double bend_at(const RoadNetwork &net, const RoadNode &nd, int n) {
	double d[2][2];
	for (int k = 0; k < 2; k++) {
		const int s = nd.segs[size_t(k)];
		const Arm a = arm_of(net.segs[size_t(s)].q, net.profile_of(s), net.segs[size_t(s)].a == n ? 0 : 1);
		d[k][0] = a.dx;
		d[k][1] = a.dz;
	}
	return PI_D - std::acos(std::max(-1.0, std::min(1.0, d[0][0] * d[1][0] + d[0][1] * d[1][1])));
}

// A node that only carries one road on: a straight joint of one profile and one structure.
bool carries_on(const RoadNetwork &net, int n) {
	const RoadNode &nd = net.nodes[size_t(n)];
	if (nd.segs.size() != 2 || !nd.joint) return false;
	const RoadSegment &p = net.segs[size_t(nd.segs[0])], &q = net.segs[size_t(nd.segs[1])];
	return p.profile == q.profile && structure_of(p.flags) == structure_of(q.flags);
}

// How far from its node a segment is held flat (elevation.ts reachOf): over the whole junction, its footways and casing
// band included, and a margin. A node that only carries one road on (a straight joint of one profile and one structure,
// the cut of a long road) holds none; a transition (junction/build.ts: two legs bent under MAX_CONTINUOUS_BEND where the
// width, the structure or only the direction changes: a taper, a ramp landing, a bend) holds the margin alone.
float plate_reach(const RoadNetwork &net, int n, double retreat) {
	const RoadNode &nd = net.nodes[size_t(n)];
	if (nd.segs.size() == 2 && (nd.joint || bend_at(net, nd, n) < MAX_CONTINUOUS_BEND)) return carries_on(net, n) ? 0.f : PLATE_MARGIN_M;
	// A plain dead end is no junction (junction/build.ts surfaceMode: one leg is one only as a turning bulb): no footway
	// casing to hold flat, the margin alone past its cut.
	if (nd.segs.size() == 1 && !nd.bulb) return float(retreat) + PLATE_MARGIN_M;
	float extra = 0;
	for (int s : nd.segs) {
		const Profile &p = net.profile_of(s);
		extra = std::max(extra, std::max(p.left - p.asph_left, p.right - p.asph_right) + CASING_BAND_M);
	}
	return float(retreat) + extra + PLATE_MARGIN_M;
}

bool same_curve(const GradeProfile &p, const Quad &q) {
	const double now[CURVE_NUMBERS] = { q.x0, q.z0, q.cx, q.cz, q.x1, q.z1 };
	return std::equal(now, now + CURVE_NUMBERS, p.q);
}

// The ground under the road at its stations: ceil, the highest across the casing (dilated) over the surface clearance;
// raw, the mean across it, of which grade_inputs and grade_chains make the line it is designed on.
void read_stations(const RoadSegment &g, GradeProfile &p, int count, const GroundAt &ground, const GradeTune &t) {
	p.ceil.assign(size_t(count), 0.f);
	p.raw.assign(size_t(count), 0.f);
	for (int i = 0; i < count; i++) {
		const double tt = t_at_length(g.table, double(p.step) * i);
		double x, z, dx, dz;
		g.q.at(tt, x, z);
		g.q.deriv(tt, dx, dz);
		const double l = std::hypot(dx, dz);
		const double nx = l > 0 ? dz / l : 0, nz = l > 0 ? -dx / l : 0; // the left of the way (geometry.h)
		float high = -BIG, sum = 0;
		for (float u : LATERAL) {
			const float v = ground(float(x + nx * p.half * u), float(z + nz * p.half * u));
			high = std::max(high, v);
			sum += v;
		}
		p.ceil[size_t(i)] = high + t.clearance;
		p.raw[size_t(i)] = sum / float(LATERAL_COUNT) + t.clearance; // the mean, not the highest: cut balances fill
	}
	dilate(p.ceil, DILATE_STATIONS);
}
} // namespace

float casing_half(const Profile &p) { return p.half() + CASING_BAND_M; }

// The inputs of segment s as they stand: stations, ground (read again only when its curve, casing or ground changed),
// plates, structure; its heights left as they were (the solve starts them from the grade line when it takes them).
void grade_inputs(RoadGrade &grade, const RoadNetwork &net, int s, const GroundAt &ground, bool reground, const GradeTune &t) {
	const RoadSegment &g = net.segs[size_t(s)];
	GradeProfile &p = grade.prof[size_t(s)];
	const float len = float(std::max(TINY_LEN, g.len));
	const int count = std::max(2, std::min(MAX_STATIONS, int(std::ceil(len / STATION_M)) + 1));
	const float half = casing_half(net.profile_of(s));
	const bool keep = p.valid && !reground && p.read_half == half && same_curve(p, g.q) && int(p.ceil.size()) == count;
	p.len = len;
	p.step = len / float(count - 1);
	p.half = half;
	if (!keep) {
		read_stations(g, p, count, ground, t);
		const double q[CURVE_NUMBERS] = { g.q.x0, g.q.z0, g.q.cx, g.q.cz, g.q.x1, g.q.z1 };
		std::copy(q, q + CURVE_NUMBERS, p.q);
		p.read_half = half;
	}
	p.base = p.raw;
	smooth_profile(p.base, p.step, SMOOTH_REACH_M);
	slope_limit(p.base, p.step, t.ground);
	p.a = g.a;
	p.b = g.b;
	p.plate_a = std::min(len * float(MOUTH_SHARE), plate_reach(net, g.a, g.retreat_a));
	p.plate_b = std::min(len * float(MOUTH_SHARE), plate_reach(net, g.b, g.retreat_b));
	p.structure = structure_of(g.flags);
	p.walls = (g.flags & SEG_CUT_WALLS) != 0;
	for (int k = 0; k < 2; k++) { // a node of this road alone is a dead end: a bulb, or cut straight across
		const RoadNode &nd = net.nodes[size_t(k == 0 ? g.a : g.b)];
		p.ends[k] = nd.segs.size() != 1 ? RoadEndKind::JOINT : nd.bulb ? RoadEndKind::BULB : RoadEndKind::CUT;
	}
	p.manual = std::fabs(net.nodes[size_t(g.a)].lift) > AUTHORED_EPS || std::fabs(net.nodes[size_t(g.b)].lift) > AUTHORED_EPS;
	if (p.h.size() != size_t(count)) p.h = p.base;
	p.valid = true;
}

void grade_chains(RoadGrade &grade, const RoadNetwork &net, const std::vector<int> &read, float ground_grade, std::vector<int> &ends,
		std::vector<char> &seen) {
	struct Piece {
		int seg;
		bool forward; // it runs the way the road is walked
	};
	std::vector<int> marked; // what `seen` holds, put back at the end
	const auto see = [&](int s) {
		seen[size_t(s)] = 1;
		marked.push_back(s);
	};
	const auto beyond = [&](int at, int n) {
		if (!carries_on(net, n)) return -1;
		const RoadNode &nd = net.nodes[size_t(n)];
		const int next = nd.segs[0] == at ? nd.segs[1] : nd.segs[0];
		return seen[size_t(next)] || !grade.has(next) ? -1 : next;
	};
	for (int s : read) {
		if (seen[size_t(s)]) continue;
		see(s);
		std::deque<Piece> run{ { s, true } };
		for (int at = s, n = net.segs[size_t(s)].b, next = beyond(at, n); next >= 0; next = beyond(at, n)) {
			see(next);
			const bool forward = net.segs[size_t(next)].a == n;
			run.push_back({ next, forward });
			n = forward ? net.segs[size_t(next)].b : net.segs[size_t(next)].a;
			at = next;
		}
		for (int at = s, n = net.segs[size_t(s)].a, next = beyond(at, n); next >= 0; next = beyond(at, n)) {
			see(next);
			const bool forward = net.segs[size_t(next)].b == n;
			run.push_front({ next, forward });
			n = forward ? net.segs[size_t(next)].a : net.segs[size_t(next)].b;
			at = next;
		}
		if (run.size() < 2) continue;
		float total = 0, step = BIG_STEP;
		for (const Piece &c : run) {
			total += grade.prof[size_t(c.seg)].len;
			step = std::min(step, grade.prof[size_t(c.seg)].step);
		}
		// the ground along the whole road, on one even step, and the design line made of it as of one profile
		const int count = int(std::ceil(total / step)) + 1;
		const float even = total / float(count - 1);
		std::vector<float> line(size_t(count), 0.f);
		float from = 0;
		size_t piece = 0;
		for (int i = 0; i < count; i++) {
			const float arc = float(i) * even;
			while (piece + 1 < run.size() && arc > from + grade.prof[size_t(run[piece].seg)].len) from += grade.prof[size_t(run[piece++].seg)].len;
			const GradeProfile &p = grade.prof[size_t(run[piece].seg)];
			const float along = std::min(p.len, arc - from);
			line[size_t(i)] = read_line(p.raw, (run[piece].forward ? along : p.len - along) / p.step);
		}
		smooth_profile(line, even, SMOOTH_REACH_M);
		slope_limit(line, even, ground_grade);
		from = 0;
		for (const Piece &c : run) {
			GradeProfile &p = grade.prof[size_t(c.seg)];
			for (size_t i = 0; i < p.base.size(); i++) {
				const float own = p.step * float(i);
				p.base[i] = read_line(line, (from + (c.forward ? own : p.len - own)) / even);
			}
			from += p.len;
			ends.push_back(p.a);
			ends.push_back(p.b);
		}
	}
	for (int s : marked) seen[size_t(s)] = 0;
}

double RoadGrade::at(int s, double arc) const {
	const GradeProfile &p = prof[size_t(s)];
	const double last = double(p.h.size() - 1);
	const double f = std::min(last, std::max(0.0, arc / double(p.step)));
	const size_t lo = size_t(std::floor(f)), hi = std::min(p.h.size() - 1, lo + 1);
	return double(p.h[lo]) + double(p.h[hi] - p.h[lo]) * (f - double(lo));
}

} // namespace city
