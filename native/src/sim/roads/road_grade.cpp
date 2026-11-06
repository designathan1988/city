// A road's profile made ready for the solve (Roadcraft 4.0 world/elevation.ts, the stations step and readStations): its
// stations along the curve, the ground read under them across its casing, and its junction plates; and the reads of a
// solved profile. The solve itself is road_grade_solve.cpp, one profile's solvers road_grade_profile.cpp.
#include "sim/roads/road_grade_internal.h"

#include "core/math.h"
#include "core/units.h"
#include "sim/roads/junction.h"
#include "sim/roads/road_network.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
// readStations LATERAL: where the ground is read across the casing, as shares of its half width.
constexpr float LATERAL[] = { -1.f, -0.62f, -0.28f, 0.f, 0.28f, 0.62f, 1.f };
constexpr int LATERAL_COUNT = int(sizeof(LATERAL) / sizeof(LATERAL[0]));
constexpr int DILATE_STATIONS = 2; // DILATE: a chord clears the ground at both its ends
constexpr float SMOOTH_REACH_M = 36.f; // SMOOTH_REACH: how far along a road its grade line is averaged
constexpr double TINY_LEN = 1e-3;
constexpr float AUTHORED_EPS = 1e-6f; // a lift under this is none: the road follows its grade line

// Half the casing (roadTypes.ts casingHalf): the road's widest side, footway included, and the casing band past it.
float casing_half(const Profile &p) { return p.half() + CASING_BAND_M; }

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

// How far from its node a segment is held flat (elevation.ts reachOf): over the whole junction, its footways and casing
// band included, and a margin. A node that only carries one road on (a straight joint of one profile and one structure,
// the cut of a long road) holds none; a transition (junction/build.ts: two legs bent under MAX_CONTINUOUS_BEND where the
// width, the structure or only the direction changes: a taper, a ramp landing, a bend) holds the margin alone.
float plate_reach(const RoadNetwork &net, int n, double retreat) {
	const RoadNode &nd = net.nodes[size_t(n)];
	if (nd.segs.size() == 2 && (nd.joint || bend_at(net, nd, n) < MAX_CONTINUOUS_BEND)) {
		const RoadSegment &p = net.segs[size_t(nd.segs[0])], &q = net.segs[size_t(nd.segs[1])];
		const bool chain = nd.joint && p.profile == q.profile && structure_of(p.flags) == structure_of(q.flags);
		return chain ? 0.f : PLATE_MARGIN_M;
	}
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
// base, the mean across it smoothed along the road and held to the ground grade: the line it is designed on.
void read_stations(const RoadSegment &g, GradeProfile &p, int count, const GroundAt &ground, const GradeTune &t) {
	p.ceil.assign(size_t(count), 0.f);
	p.base.assign(size_t(count), 0.f);
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
		p.base[size_t(i)] = sum / float(LATERAL_COUNT) + t.clearance; // the mean, not the highest: cut balances fill
	}
	dilate(p.ceil, DILATE_STATIONS);
	smooth_profile(p.base, p.step, SMOOTH_REACH_M);
	slope_limit(p.base, p.step, t.ground);
}
} // namespace

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
	p.a = g.a;
	p.b = g.b;
	p.plate_a = std::min(len * float(MOUTH_SHARE), plate_reach(net, g.a, g.retreat_a));
	p.plate_b = std::min(len * float(MOUTH_SHARE), plate_reach(net, g.b, g.retreat_b));
	p.structure = structure_of(g.flags);
	p.walls = (g.flags & SEG_CUT_WALLS) != 0;
	p.manual = std::fabs(net.nodes[size_t(g.a)].lift) > AUTHORED_EPS || std::fabs(net.nodes[size_t(g.b)].lift) > AUTHORED_EPS;
	if (p.h.size() != size_t(count)) p.h = p.base;
	p.valid = true;
}

double RoadGrade::at(int s, double arc) const {
	const GradeProfile &p = prof[size_t(s)];
	const double last = double(p.h.size() - 1);
	const double f = std::min(last, std::max(0.0, arc / double(p.step)));
	const size_t lo = size_t(std::floor(f)), hi = std::min(p.h.size() - 1, lo + 1);
	return double(p.h[lo]) + double(p.h[hi] - p.h[lo]) * (f - double(lo));
}

} // namespace city
