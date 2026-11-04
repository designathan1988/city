// Segment surface: the profile's strips split into pieces (gutters inside
// the outer kerbs, the ramp band of a footway at a dropped kerb), extruded between the two mouths; vertical faces where
// neighbouring pieces step in height (kerb faces), skirts down the outer edges and end faces of raised strips.
#include "render/roads/road_mesh.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float BIG_D = 1e9f, WALK_KEEP = 0.2f, STRAIGHT_TOL = 0.5f;
constexpr float ROAD_MIN_SPAN = 0.05f;

enum class Ramp : uint8_t { NONE, KERB, LOW_U0, LOW_U1 };

struct Piece {
	float u0 = 0, u1 = 0, h = 0;
	RoadMat mat = RoadMat::ASPHALT;
	Color col;
	Ramp ramp = Ramp::NONE;
};

RoadMat paving_mat(Paving p) {
	switch (p) {
		case Paving::PAVERS: return RoadMat::PAVERS;
		case Paving::STONE: return RoadMat::STONE;
		case Paving::GRASS: return RoadMat::GRASS;
		default: return RoadMat::FOOTWAY;
	}
}

RoadMat surface_mat(Surface s) {
	switch (s) {
		case Surface::CONCRETE: return RoadMat::CONCRETE;
		case Surface::COBBLE: return RoadMat::COBBLE;
		case Surface::BALLAST: return RoadMat::BALLAST;
		default: return RoadMat::ASPHALT;
	}
}

bool kind_at(const Profile &p, int i, StripKind k) { return i >= 0 && i < int(p.edges.size()) && p.edges[i].strip.kind == k; }
bool raised_walk(const Profile &p, int i) { return kind_at(p, i, StripKind::SIDEWALK) && !p.edges[i].strip.flush; }
// A kerb between a raised footway and the carriageway (the ones that get gutters and dropped kerbs).
bool outer_kerb(const Profile &p, int i) { return kind_at(p, i, StripKind::CURB) && (raised_walk(p, i - 1) || raised_walk(p, i + 1)); }

void carriageway(const StreetLook &L, const Profile &p, int i, const Piece &base, std::vector<Piece> &out) {
	Piece pc = base;
	bool g0 = outer_kerb(p, i - 1) && !p.rail, g1 = outer_kerb(p, i + 1) && !p.rail;
	float w = pc.u1 - pc.u0, gw = std::min(L.gutter_w, w * 0.5f);
	Piece gut = pc;
	gut.mat = RoadMat::GUTTER;
	gut.col = L.gutter;
	if (g0) {
		gut.u0 = pc.u0;
		gut.u1 = pc.u0 += gw;
		out.push_back(gut);
	}
	float end = pc.u1;
	if (g1) pc.u1 -= gw;
	out.push_back(pc);
	if (g1) {
		gut.u0 = pc.u1;
		gut.u1 = end;
		out.push_back(gut);
	}
}

void walk(const StreetLook &L, const Profile &p, int i, const Piece &base, std::vector<Piece> &out) {
	Piece pc = base;
	float w = pc.u1 - pc.u0, band = std::min(L.ramp_run, w - WALK_KEEP);
	if (pc.h <= 0 || w < L.ramp_min_walk) {
		out.push_back(pc);
		return;
	}
	Piece r = pc;
	if (outer_kerb(p, i + 1)) { // kerb on the +u side
		r.u0 = pc.u1 = pc.u1 - band;
		r.ramp = Ramp::LOW_U1;
		out.push_back(pc);
		out.push_back(r);
	} else if (outer_kerb(p, i - 1)) {
		r.u1 = pc.u0 = pc.u0 + band;
		r.ramp = Ramp::LOW_U0;
		out.push_back(r);
		out.push_back(pc);
	} else out.push_back(pc);
}

std::vector<Piece> pieces_of(const StreetLook &L, const Profile &p) {
	std::vector<Piece> out;
	for (int i = 0; i < int(p.edges.size()); i++) {
		const Strip &s = p.edges[i].strip;
		Piece pc;
		pc.u0 = p.edges[i].u0;
		pc.u1 = p.edges[i].u1;
		pc.mat = surface_mat(p.surface);
		pc.col = L.surface[int(p.surface)];
		bool asph = p.surface == Surface::ASPHALT;
		switch (s.kind) {
			case StripKind::PARKING: if (asph) pc.col = L.parking; break;
			case StripKind::BIKE: pc.col = L.bike; pc.mat = RoadMat::ASPHALT; break;
			case StripKind::SHOULDER: if (p.surface != Surface::BALLAST) pc.col = L.shoulder; break;
			case StripKind::CURB:
				pc.mat = RoadMat::KERB;
				pc.col = L.curb;
				pc.h = outer_kerb(p, i) ? L.h_curb : L.h_curb_median;
				pc.ramp = outer_kerb(p, i) ? Ramp::KERB : Ramp::NONE;
				break;
			case StripKind::SIDEWALK:
				pc.mat = paving_mat(s.pave);
				pc.col = L.paving[int(s.pave) & 3];
				pc.h = s.flush ? 0 : L.h_sidewalk;
				walk(L, p, i, pc, out);
				continue;
			case StripKind::MEDIAN:
				pc.mat = paving_mat(s.pave);
				pc.col = L.median[int(s.pave) & 3];
				pc.h = s.flush ? 0 : L.h_median;
				break;
			case StripKind::BARRIER: pc.mat = RoadMat::CONCRETE; pc.col = L.barrier; pc.h = L.h_barrier; break;
			default: break;
		}
		if (pc.h == 0 && s.kind != StripKind::MEDIAN) carriageway(L, p, i, pc, out);
		else out.push_back(pc);
	}
	return out;
}

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
};

std::vector<Sample> samples(const RoadEmit &e, int sid, double v0, double v1, bool ca, bool cb) {
	const RoadSegment &g = e.s.roads.segs[sid];
	// a straight segment is linear in position, height, texture and pool coordinates: its two mouths are enough
	bool straight = g.q.is_straight(STRAIGHT_TOL);
	std::vector<double> at;
	int n = straight ? 1 : std::max(1, int(std::ceil((v1 - v0) / e.L.step)));
	for (int k = 0; k <= n; k++) at.push_back(v0 + (v1 - v0) * k / n);
	const float marks[3] = { e.L.ramp_full, e.L.ramp_flat, e.L.ramp_flat + e.L.ramp_flare };
	for (float m : marks) {
		if (ca && v0 + m < v1) at.push_back(v0 + m);
		if (cb && v1 - m > v0) at.push_back(v1 - m);
	}
	std::sort(at.begin(), at.end());
	at.erase(std::unique(at.begin(), at.end(), [](double a, double b) { return std::fabs(a - b) < ROAD_MIN_SPAN; }), at.end());
	std::vector<Sample> out;
	for (double a : at) {
		Sample sm;
		sm.f = seg_frame(e.s, sid, a);
		sm.at = float(a);
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
		if (!lamp.any) return Vector2(POOL_NONE, 0);
		float du, dv;
		lamp.pool(u, at, du, dv);
		return Vector2(du, dv);
	}
	Vector3 at_u(size_t k, float u, float h) const { return sm[k].f.c + sm[k].f.left * u + Vector3(0, h, 0); }
};

void tops(const Ctx &c, const Piece &pc) {
	for (size_t k = 0; k + 1 < c.sm.size(); k++) {
		float a0, a1, b0, b1;
		heights(c.e.L, pc, c.sm[k].d, a0, a1);
		heights(c.e.L, pc, c.sm[k + 1].d, b0, b1);
		const Vector3 p[4] = { c.at_u(k, pc.u0, a0), c.at_u(k, pc.u1, a1), c.at_u(k + 1, pc.u1, b1), c.at_u(k + 1, pc.u0, b0) };
		float va = c.sm[k].at, vb = c.sm[k + 1].at;
		const Vector2 uv[4] = { Vector2(pc.u0, va), Vector2(pc.u1, va), Vector2(pc.u1, vb), Vector2(pc.u0, vb) };
		const Vector2 uv2[4] = { c.pool(pc.u0, va), c.pool(pc.u1, va), c.pool(pc.u1, vb), c.pool(pc.u0, vb) };
		emit_quad(c.e.mb, p, uv, uv2, road_color(pc.col, pc.mat), Vector3(0, 1, 0));
	}
}

// Vertical quad at u between heights lo/hi at samples k and k+1, facing `side` (+1 = +u).
void wall(const Ctx &c, size_t k, float u, float lo0, float hi0, float lo1, float hi1, float side, const Color &col) {
	const Vector3 p[4] = { c.at_u(k, u, lo0), c.at_u(k + 1, u, lo1), c.at_u(k + 1, u, hi1), c.at_u(k, u, hi0) };
	float va = c.sm[k].at, vb = c.sm[k + 1].at;
	const Vector2 uv[4] = { Vector2(lo0, va), Vector2(lo1, vb), Vector2(hi1, vb), Vector2(hi0, va) };
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
		wall(c, k, a.u1, std::min(ha0, hb0), std::max(ha0, hb0), std::min(ha1, hb1), std::max(ha1, hb1), a_high ? 1.f : -1.f, hi.col);
	}
}

void skirts_and_ends(const Ctx &c, const std::vector<Piece> &ps) {
	const StreetLook &L = c.e.L;
	for (size_t k = 0; k + 1 < c.sm.size(); k++) {
		float h0, h1, g0, g1;
		heights(L, ps.front(), c.sm[k].d, h0, h1);
		heights(L, ps.front(), c.sm[k + 1].d, g0, g1);
		wall(c, k, ps.front().u0, -L.skirt, h0, -L.skirt, g0, -1.f, ps.front().col);
		heights(L, ps.back(), c.sm[k].d, h0, h1);
		heights(L, ps.back(), c.sm[k + 1].d, g0, g1);
		wall(c, k, ps.back().u1, -L.skirt, h1, -L.skirt, g1, 1.f, ps.back().col);
	}
	for (size_t end : { size_t(0), c.sm.size() - 1 }) {
		const Sample &s = c.sm[end];
		Vector3 out = s.f.fwd * (end == 0 ? -1.f : 1.f);
		for (const Piece &pc : ps) {
			float h0, h1;
			heights(L, pc, s.d, h0, h1);
			if (h0 < L.face_eps && h1 < L.face_eps) continue;
			const Vector3 p[4] = { c.at_u(end, pc.u0, 0), c.at_u(end, pc.u1, 0), c.at_u(end, pc.u1, h1), c.at_u(end, pc.u0, h0) };
			const Vector2 uv[4] = { Vector2(pc.u0, 0), Vector2(pc.u1, 0), Vector2(pc.u1, h1), Vector2(pc.u0, h0) };
			const Vector2 uv2[4] = { c.pool(pc.u0, s.at), c.pool(pc.u1, s.at), c.pool(pc.u1, s.at), c.pool(pc.u0, s.at) };
			emit_quad(c.e.mb, p, uv, uv2, road_color(pc.col, RoadMat::SKIRT), out);
		}
	}
}
} // namespace

void emit_segment(const RoadEmit &e, int sid) {
	const RoadSegment &g = e.s.roads.segs[sid];
	const Profile &p = e.s.catalog.profile(g.profile);
	double v0 = g.retreat_a, v1 = g.len - g.retreat_b;
	if (v1 - v0 < ROAD_MIN_SPAN || p.edges.empty()) return;
	bool ring_or_rail = p.ring || p.rail;
	bool ca = !ring_or_rail && crossing_node(e.s, g.a), cb = !ring_or_rail && crossing_node(e.s, g.b);
	std::vector<Sample> sm = samples(e, sid, v0, v1, ca, cb);
	std::vector<Piece> ps = pieces_of(e.L, p);
	Ctx c{ e, sm, lamp_row(e.s.roads, sid) };
	for (size_t i = 0; i < ps.size(); i++) {
		tops(c, ps[i]);
		if (i + 1 < ps.size()) steps(c, ps[i], ps[i + 1]);
	}
	skirts_and_ends(c, ps);
}

} // namespace city
