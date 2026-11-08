// Segment surface: the profile's strips split into pieces (the ramp band of a footway at a dropped kerb), extruded between the two mouths; vertical faces where
// neighbouring pieces step in height (kerb faces), skirts down the outer edges and end faces of raised strips.
#include "render/roads/road_mesh.h"
#include "render/roads/road_section.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {

constexpr double UTURN_NOSES = 3; // the entry side of a U-turn's opening, in median noses

constexpr float BIG_D = 1e9f; // farther than any ramp: none found
constexpr float ROAD_MIN_SPAN = 0.05f, ISLAND_MIN_M = 1.f; // shorter than this no island is drawn

// Share of the full kerb height at d metres from a crossing mouth: none over the dropped kerb, rising over its flare.
float ramp_share(const StreetLook &L, float d) {
	if (d < L.ramp_full) return (L.ramp_full - d) / L.ramp_full;
	if (d <= L.ramp_flat) return 0.f;
	return std::min(1.f, (d - L.ramp_flat) / L.ramp_flare);
}

void heights(const StreetLook &L, const Piece &pc, float d, float &h0, float &h1) {
	h0 = h1 = pc.h;
	if (pc.ramp == Ramp::NONE || d >= BIG_D) return;
	float kerb = L.ramp_low + (L.h_curb - L.ramp_low) * ramp_share(L, d);
	if (pc.ramp == Ramp::KERB) h0 = h1 = kerb;
	else if (pc.ramp == Ramp::LOW_U0) h0 = std::min(pc.h, kerb);
	else h1 = std::min(pc.h, kerb);
}

struct Sample {
	SegFrame f;
	float at = 0, d = BIG_D;
	float tex = 0; // how far along its street, what the maps read
};

std::vector<Sample> samples(const RoadEmit &e, int sid, double v0, double v1, bool ca, bool cb, double lo, double hi) {
	std::vector<double> at;
	const int n = surface_steps(e, sid, v0, v1);
	for (int k = 0; k <= n; k++) at.push_back(v0 + (v1 - v0) * k / n);
	const float marks[3] = { e.L.ramp_full, e.L.ramp_flat, e.L.ramp_flat + e.L.ramp_flare };
	for (float m : marks) {
		if (ca && v0 + m < v1) at.push_back(v0 + m);
		if (cb && v1 - m > v0) at.push_back(v1 - m);
	}
	if (lo > v0) at.push_back(lo);
	if (hi < v1) at.push_back(hi);
	std::sort(at.begin(), at.end());
	at.erase(std::unique(at.begin(), at.end(), [](double a, double b) { return std::fabs(a - b) < ROAD_MIN_SPAN; }), at.end());
	std::vector<Sample> out;
	const double start = e.s.roads.street_run(sid).start;
	for (double a : at) {
		Sample sm;
		sm.f = seg_frame(e.s, sid, a);
		sm.at = float(a);
		sm.tex = float(start + a);
		if (ca) sm.d = std::min(sm.d, float(a - v0));
		if (cb) sm.d = std::min(sm.d, float(v1 - a));
		out.push_back(sm);
	}
	return out;
}

struct Ctx {
	const RoadEmit &e;
	const std::vector<Sample> &sm;
	LampRow lamp;
	Vector2 pool(float u, float at) const {
		if (!lamp.any) return Vector2(ROAD_POOL_NONE, 0);
		float du, dv;
		lamp.pool(u, at, du, dv);
		return Vector2(du, dv);
	}
	Vector3 at_u(size_t k, float u, float h) const { return sm[k].f.c + sm[k].f.left * u + Vector3(0, h, 0); }
};

// The wear coordinates of a piece at `u`. On a footway edge0 is the kerb's side and edge1 the lot's.
Color wear_at(const Piece &pc, float u) {
	if (pc.edge0 == pc.edge1) return wear_color(0, 0, 0, pc.mat, pc.tone); // paving nobody walks: a median's
	return wear_color(std::fabs(u - pc.edge0), std::fabs(u - pc.edge1), pc.lane, pc.mat, pc.tone);
}

void tops(const Ctx &c, const Piece &pc) {
	for (size_t k = 0; k + 1 < c.sm.size(); k++) {
		float a0, a1, b0, b1;
		heights(c.e.L, pc, c.sm[k].d, a0, a1);
		heights(c.e.L, pc, c.sm[k + 1].d, b0, b1);
		const Vector3 p[4] = { c.at_u(k, pc.u0, a0), c.at_u(k, pc.u1, a1), c.at_u(k + 1, pc.u1, b1), c.at_u(k + 1, pc.u0, b0) };
		float va = c.sm[k].at, vb = c.sm[k + 1].at, ta = c.sm[k].tex, tb = c.sm[k + 1].tex;
		const Vector2 uv[4] = { Vector2(pc.u0, ta), Vector2(pc.u1, ta), Vector2(pc.u1, tb), Vector2(pc.u0, tb) };
		const Vector2 uv2[4] = { c.pool(pc.u0, va), c.pool(pc.u1, va), c.pool(pc.u1, vb), c.pool(pc.u0, vb) };
		const Color plain = road_color(pc.col, pc.mat);
		const bool wears = pc.mat == RoadMat::LANE || pc.mat == RoadMat::FOOTWAY;
		const Color at_u0 = wears ? wear_at(pc, pc.u0) : plain, at_u1 = wears ? wear_at(pc, pc.u1) : plain;
		const Color cols[4] = { at_u0, at_u1, at_u1, at_u0 };
		emit_quad(c.e.mb, p, uv, uv2, cols, Vector3(0, 1, 0));
	}
}

// Vertical quad at u between heights lo/hi at samples k and k+1, facing `side` (+1 = +u).
void wall(const Ctx &c, size_t k, float u, float lo0, float hi0, float lo1, float hi1, float side, const Color &col) {
	const Vector3 p[4] = { c.at_u(k, u, lo0), c.at_u(k + 1, u, lo1), c.at_u(k + 1, u, hi1), c.at_u(k, u, hi0) };
	float va = c.sm[k].at, vb = c.sm[k + 1].at;
	const float ta = c.sm[k].tex, tb = c.sm[k + 1].tex;
	const Vector2 uv[4] = { Vector2(ta, lo0), Vector2(tb, lo1), Vector2(tb, hi1), Vector2(ta, hi0) }; // a face: the run along it first
	const Vector2 uv2[4] = { c.pool(u, va), c.pool(u, vb), c.pool(u, vb), c.pool(u, va) };
	emit_quad(c.e.mb, p, uv, uv2, road_color(col, RoadMat::SKIRT), c.sm[k].f.left * side);
}

void steps(const Ctx &c, const Piece &a, const Piece &b) {
	const float eps = c.e.L.face_eps;
	for (size_t k = 0; k + 1 < c.sm.size(); k++) {
		float x0, ha0, hb0, x1, ha1, hb1;
		heights(c.e.L, a, c.sm[k].d, x0, ha0);
		heights(c.e.L, b, c.sm[k].d, hb0, x0);
		heights(c.e.L, a, c.sm[k + 1].d, x1, ha1);
		heights(c.e.L, b, c.sm[k + 1].d, hb1, x1);
		if (std::fabs(ha0 - hb0) < eps && std::fabs(ha1 - hb1) < eps) continue;
		bool a_high = ha0 + ha1 > hb0 + hb1;
		const Piece &hi = a_high ? a : b;
		wall(c, k, a.u1, std::min(ha0, hb0), std::max(ha0, hb0), std::min(ha1, hb1), std::max(ha1, hb1), a_high ? 1.f : -1.f, hi.face);
	}
}

void skirts(const Ctx &c, const std::vector<Piece> &ps) {
	const StreetLook &L = c.e.L;
	for (size_t k = 0; k + 1 < c.sm.size(); k++) {
		float h0, h1, g0, g1;
		heights(L, ps.front(), c.sm[k].d, h0, h1);
		heights(L, ps.front(), c.sm[k + 1].d, g0, g1);
		wall(c, k, ps.front().u0, -L.skirt, h0, -L.skirt, g0, -1.f, ps.front().face);
		heights(L, ps.back(), c.sm[k].d, h0, h1);
		heights(L, ps.back(), c.sm[k + 1].d, g0, g1);
		wall(c, k, ps.back().u1, -L.skirt, h1, -L.skirt, g1, 1.f, ps.back().face);
	}
}

// The raised strips' end faces at sample `end`, looking `along` the road (-1 back, +1 ahead). `island_only` for a nose.
void end_faces(const Ctx &c, const std::vector<Piece> &ps, size_t end, float along, bool island_only) {
	const StreetLook &L = c.e.L;
	const Sample &s = c.sm[end];
	for (const Piece &pc : ps) {
		float h0, h1;
		heights(L, pc, s.d, h0, h1);
		if ((h0 < L.face_eps && h1 < L.face_eps) || (island_only && !pc.island)) continue;
		const Vector3 p[4] = { c.at_u(end, pc.u0, 0), c.at_u(end, pc.u1, 0), c.at_u(end, pc.u1, h1), c.at_u(end, pc.u0, h0) };
		const Vector2 uv[4] = { Vector2(pc.u0, 0), Vector2(pc.u1, 0), Vector2(pc.u1, h1), Vector2(pc.u0, h0) };
		const Vector2 uv2[4] = { c.pool(pc.u0, s.at), c.pool(pc.u1, s.at), c.pool(pc.u1, s.at), c.pool(pc.u0, s.at) };
		emit_quad(c.e.mb, p, uv, uv2, road_color(pc.face, RoadMat::SKIRT), s.f.fwd * along);
	}
}

void span(const Ctx &c, const std::vector<Piece> &ps) {
	for (size_t i = 0; i < ps.size(); i++) {
		tops(c, ps[i]);
		if (i + 1 < ps.size()) steps(c, ps[i], ps[i + 1]);
	}
	skirts(c, ps);
}

// From sample `from` to sample `to`, both included.
std::vector<Sample> stretch(const std::vector<Sample> &sm, size_t from, size_t to) {
	return std::vector<Sample>(sm.begin() + from, sm.begin() + to + 1);
}

// The first sample at `v` or past it.
size_t sample_at(const std::vector<Sample> &sm, double v) {
	size_t k = 0;
	while (k + 1 < sm.size() && sm[k].at < v - ROAD_MIN_SPAN) k++;
	return k;
}

// A section whose island stands back of the crossings: the carriageway bare up to its noses at `lo` and `hi`.
void with_noses(const RoadEmit &e, const std::vector<Sample> &sm, const LampRow &lamp, const std::vector<Piece> &ps, double lo, double hi) {
	const std::vector<Piece> bare = without_island(ps);
	const size_t a = sample_at(sm, lo), b = sample_at(sm, hi);
	const bool nose_a = a > 0, nose_b = b + 1 < sm.size();
	const std::vector<Sample> before = stretch(sm, 0, a), island = stretch(sm, a, b), after = stretch(sm, b, sm.size() - 1);
	Ctx head{ e, before, lamp }, body{ e, island, lamp }, tail{ e, after, lamp };
	span(body, ps);
	if (nose_a) {
		span(head, bare);
		end_faces(body, ps, 0, -1.f, true);
	}
	if (nose_b) {
		span(tail, bare);
		end_faces(body, ps, island.size() - 1, 1.f, true);
	}
	end_faces(head, nose_a ? bare : ps, 0, -1.f, false);
	end_faces(tail, nose_b ? bare : ps, after.size() - 1, 1.f, false);
}
} // namespace

void emit_segment(const RoadEmit &e, int sid) {
	const RoadSegment &g = e.s.roads.segs[sid];
	const Profile &p = e.s.catalog.profile(g.profile);
	double v0 = g.retreat_a, v1 = g.len - g.retreat_b;
	if (v1 - v0 < ROAD_MIN_SPAN || p.edges.empty()) return;
	if (e.far) { // a kerb's face or a dropped kerb is under a pixel from there: the strips run whole from mouth to mouth
		const std::vector<Sample> whole = samples(e, sid, v0, v1, false, false, v0, v1);
		const Ctx c{ e, whole, lamp_row(e.s.roads, sid) };
		for (const Piece &pc : pieces_of(e.L, p)) tops(c, pc);
		return;
	}
	bool ring_or_rail = p.ring || p.rail;
	bool ca = !ring_or_rail && e.s.roads.crosswalk_on(g.a, sid), cb = !ring_or_rail && e.s.roads.crosswalk_on(g.b, sid); // a crossing between junctions has a stop line on both
	float island0, island1;
	const bool island = island_span(p, island0, island1);
	const double nose = e.s.catalog.furniture.crossing.zebra_to_m + e.L.median_nose;
	// A U-turn's opening: a nose on the side it is entered from, a car's half circle on the other (landscape.ts medianNose).
	const auto opening = [&](int node) {
		const int arm = e.s.roads.uturn_arm(node);
		return arm < 0 ? 0.0 : arm == sid ? double(e.L.median_nose) * UTURN_NOSES : double(e.s.catalog.tune.road_uturn_opening_m);
	};
	const double open_a = opening(g.a), open_b = opening(g.b);
	const double lo = island && (ca || open_a > 0) ? v0 + std::max(ca ? nose : 0.0, open_a) : v0, hi = island && (cb || open_b > 0) ? v1 - std::max(cb ? nose : 0.0, open_b) : v1;
	std::vector<Sample> sm = samples(e, sid, v0, v1, ca, cb, lo, hi);
	std::vector<Piece> ps = pieces_of(e.L, p);
	const LampRow lamp = lamp_row(e.s.roads, sid);
	if (lo > v0 || hi < v1) {
		if (hi - lo >= ISLAND_MIN_M) {
			with_noses(e, sm, lamp, ps, lo, hi);
			return;
		}
		ps = without_island(ps);
	}
	Ctx c{ e, sm, lamp };
	span(c, ps);
	end_faces(c, ps, 0, -1.f, false);
	end_faces(c, ps, sm.size() - 1, 1.f, false);
}

} // namespace city
