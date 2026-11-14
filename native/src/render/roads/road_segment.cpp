// Segment surface: the profile's strips split into pieces (the bands of a footway that slope to a lowered kerb: kerb_drop.h), extruded
// along the segment: the carriageway and its kerbs between the carriageway's mouths, each side's footway between its own mouths (a
// junction's corner may run it on past the carriageway's, sim/roads/junction_corner.h), the island between its noses; vertical faces
// where neighbouring pieces step in height (kerb faces), skirts down the outer edges and end faces of raised strips.
#include "render/roads/kerb_drop.h"
#include "render/roads/road_mesh.h"
#include "render/roads/road_section.h"
#include "sim/roads/junction_corner.h"
#include "sim/roads/parking_layout.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {

constexpr double UTURN_NOSES = 3; // the entry side of a U-turn's opening, in median noses
constexpr float ROAD_MIN_SPAN = 0.05f, ISLAND_MIN_M = 1.f; // shorter than this no island is drawn
constexpr float EDGE_M = 1e-4f; // a piece's edge this near the kerb stone's back is at it
constexpr double HALF = 0.5;

// A piece's two edge heights at arc `at` (kerb_drop.h): a kerb where it drops on the piece's side, a footway band by each edge's
// distance from that kerb.
void heights(const StreetLook &L, const KerbDrops &k, const Piece &pc, double at, float &h0, float &h1) {
	h0 = h1 = pc.h;
	if (pc.ramp == Ramp::NONE) return;
	const int side = street_side(pc.u0 + pc.u1);
	if (pc.ramp == Ramp::KERB) {
		h0 = h1 = kerb_height(L, k, side, at);
		return;
	}
	h0 = band_height(L, k, side, at, pc.kerb_d0, pc.band, pc.h);
	h1 = band_height(L, k, side, at, pc.kerb_d1, pc.band, pc.h);
}

// The side a piece lies on past its kerb stone's back (the footway's pieces: STREET_RIGHT, STREET_LEFT), or -1: the carriageway,
// its kerbs, the island.
int footway_side(const Piece &pc, const SideSection sec[2]) {
	if (pc.u0 >= sec[STREET_LEFT].back - EDGE_M) return STREET_LEFT;
	if (pc.u1 <= -sec[STREET_RIGHT].back + EDGE_M) return STREET_RIGHT;
	return -1;
}

struct Sample {
	SegFrame f;
	float at = 0;
	float tex = 0; // how far along its street, what the maps read
	float off_mouth = 0; // how far along from the nearest mouth that opens on a node's paving
};

// The first of `arcs` at `v` or past it.
size_t arc_index(const std::vector<double> &arcs, double v) {
	size_t k = 0;
	while (k + 1 < arcs.size() && arcs[k] < v - ROAD_MIN_SPAN) k++;
	return k;
}

// Every arc the surface is drawn at between its mouths: its curve's steps, every arc where a lowered kerb starts or ends a slope,
// the island's noses and the footways' mouths; then each of those snapped to the arc it is drawn at.
void lay_arcs(const RoadEmit &e, int sid, SurfaceLayout &lay) {
	std::vector<double> &at = lay.arcs;
	const int n = surface_steps(e, sid, lay.v0, lay.v1);
	for (int k = 0; k <= n; k++) at.push_back(lay.v0 + (lay.v1 - lay.v0) * k / n);
	std::vector<double> marks{ lay.lo, lay.hi, lay.walk_from[0], lay.walk_from[1], lay.walk_to[0], lay.walk_to[1] };
	// between two paved mouths the nearer one changes at the middle: an arc there keeps that distance linear on every stretch
	if (lay.paved[0] && lay.paved[1]) marks.push_back((lay.v0 + lay.v1) * HALF);
	kerb_marks(e.L, lay.drops, marks);
	for (double m : marks)
		if (m > lay.v0 && m < lay.v1) at.push_back(m);
	std::sort(at.begin(), at.end());
	at.erase(std::unique(at.begin(), at.end(), [](double a, double b) { return std::fabs(a - b) < ROAD_MIN_SPAN; }), at.end());
	for (double *v : { &lay.lo, &lay.hi, &lay.walk_from[0], &lay.walk_from[1], &lay.walk_to[0], &lay.walk_to[1] }) *v = at[arc_index(at, *v)];
}

std::vector<Sample> samples_of(const RoadEmit &e, int sid, const SurfaceLayout &lay) {
	std::vector<Sample> out;
	const double start = e.s.roads.street_run(sid).start;
	for (double a : lay.arcs) {
		Sample sm;
		sm.f = seg_frame(e.s, sid, a);
		sm.at = float(a);
		sm.tex = float(start + a);
		const double from_a = a - lay.v0, to_b = lay.v1 - a;
		sm.off_mouth = float(lay.paved[0] && lay.paved[1] ? std::min(from_a, to_b) : lay.paved[0] ? from_a : lay.paved[1] ? to_b : double(WEAR_SPAN_M));
		out.push_back(sm);
	}
	return out;
}

struct Ctx {
	const RoadEmit &e;
	const std::vector<Sample> &sm;
	const KerbDrops &drops;
	size_t k0 = 0, k1 = 0; // the stretch drawn: samples k0 to k1
	float dashed = 0; // of a dashed line's length, the share that is paint, by the style's dashes at the road's speed
	bool yellows = true; // the style paints yellow lines (false: every line white)
	Vector3 at_u(size_t k, float u, float h) const { return sm[k].f.c + sm[k].f.left * u + Vector3(0, h, 0); }
	// What the far level paints along a lane's edge (road_body.gdshaderinc reads it from a lane's UV, which nothing else
	// reads there: the asphalt is laid in the world's frame): the share of the line's length that is paint (a continuous
	// line whole, a dashed one and an edge with no line stated the dashes' share), FAR_YELLOW more when it is yellow.
	float far_line(int line) const {
		const int kind = line % LINE_YELLOW;
		const bool whole = kind == int(LineKind::SOLID) || kind == int(LineKind::DOUBLE);
		return (whole ? 1.f : dashed) + (line >= LINE_YELLOW && yellows ? FAR_YELLOW : 0.f);
	}
};

// The wear coordinates of a piece at `u`. On a footway edge0 is the kerb's side and edge1 the lot's.
Color wear_at(const Piece &pc, float u) {
	if (pc.edge0 == pc.edge1) return wear_color(0, 0, 0, pc.mat, pc.tone); // paving nobody walks: a median's
	return wear_color(std::fabs(u - pc.edge0), std::fabs(u - pc.edge1), pc.lane, pc.mat, pc.tone);
}

// The asphalt's wear at `u` of a piece (road_look.h, UV2): a lane across itself and off a paved mouth; the carriageway's other
// asphalt off its nearer edge (a strip of no carriageway, a rail's bed, wears by none).
Vector2 wear_uv(const Piece &pc, float u, float off_mouth) {
	if (pc.mat == RoadMat::LANE) return Vector2(u - pc.lane_u0, off_mouth);
	if (pc.mat != RoadMat::ASPHALT) return Vector2();
	return Vector2(pc.edge0 == pc.edge1 ? WEAR_SPAN_M : std::min(std::fabs(u - pc.edge0), std::fabs(u - pc.edge1)), 0);
}

void tops(const Ctx &c, const Piece &pc) {
	for (size_t k = c.k0; k < c.k1; k++) {
		float a0, a1, b0, b1;
		heights(c.e.L, c.drops, pc, c.sm[k].at, a0, a1);
		heights(c.e.L, c.drops, pc, c.sm[k + 1].at, b0, b1);
		const Vector3 p[4] = { c.at_u(k, pc.u0, a0), c.at_u(k, pc.u1, a1), c.at_u(k + 1, pc.u1, b1), c.at_u(k + 1, pc.u0, b0) };
		const float ta = c.sm[k].tex, tb = c.sm[k + 1].tex;
		const Vector2 lines(c.far_line(pc.line0), c.far_line(pc.line1)); // a lane's UV: its two edges' lines, the same all over it
		const bool lane = pc.mat == RoadMat::LANE;
		const Vector2 uv[4] = { lane ? lines : Vector2(pc.u0, ta), lane ? lines : Vector2(pc.u1, ta), lane ? lines : Vector2(pc.u1, tb),
			lane ? lines : Vector2(pc.u0, tb) };
		const Color plain = road_color(pc.col, pc.mat);
		const bool wears = pc.mat == RoadMat::LANE || pc.mat == RoadMat::FOOTWAY;
		const Color at_u0 = wears ? wear_at(pc, pc.u0) : plain, at_u1 = wears ? wear_at(pc, pc.u1) : plain;
		const Color cols[4] = { at_u0, at_u1, at_u1, at_u0 };
		const float ma = c.sm[k].off_mouth, mb = c.sm[k + 1].off_mouth;
		const Vector2 wear[4] = { wear_uv(pc, pc.u0, ma), wear_uv(pc, pc.u1, ma), wear_uv(pc, pc.u1, mb), wear_uv(pc, pc.u0, mb) };
		emit_quad(c.e.mb, p, uv, cols, Vector3(0, 1, 0), wear);
	}
}

// Vertical quad at u between heights lo/hi at samples k and k+1, facing `side` (+1 = +u).
void wall(const Ctx &c, size_t k, float u, float lo0, float hi0, float lo1, float hi1, float side, const Color &col) {
	const Vector3 p[4] = { c.at_u(k, u, lo0), c.at_u(k + 1, u, lo1), c.at_u(k + 1, u, hi1), c.at_u(k, u, hi0) };
	const float ta = c.sm[k].tex, tb = c.sm[k + 1].tex;
	const Vector2 uv[4] = { Vector2(ta, lo0), Vector2(tb, lo1), Vector2(tb, hi1), Vector2(ta, hi0) }; // a face: the run along it first
	emit_quad(c.e.mb, p, uv, road_color(col, RoadMat::SKIRT), c.sm[k].f.left * side);
}

void steps(const Ctx &c, const Piece &a, const Piece &b) {
	const float eps = c.e.L.face_eps;
	for (size_t k = c.k0; k < c.k1; k++) {
		float x0, ha0, hb0, x1, ha1, hb1;
		heights(c.e.L, c.drops, a, c.sm[k].at, x0, ha0);
		heights(c.e.L, c.drops, b, c.sm[k].at, hb0, x0);
		heights(c.e.L, c.drops, a, c.sm[k + 1].at, x1, ha1);
		heights(c.e.L, c.drops, b, c.sm[k + 1].at, hb1, x1);
		if (std::fabs(ha0 - hb0) < eps && std::fabs(ha1 - hb1) < eps) continue;
		bool a_high = ha0 + ha1 > hb0 + hb1;
		const Piece &hi = a_high ? a : b;
		wall(c, k, a.u1, std::min(ha0, hb0), std::max(ha0, hb0), std::min(ha1, hb1), std::max(ha1, hb1), a_high ? 1.f : -1.f, hi.face);
	}
}

// The skirt down the outer edge of `pc` at u (its u0 when `side` is -1, its u1 when +1).
void skirt(const Ctx &c, const Piece &pc, float side) {
	const StreetLook &L = c.e.L;
	for (size_t k = c.k0; k < c.k1; k++) {
		float h0, h1, g0, g1;
		heights(L, c.drops, pc, c.sm[k].at, h0, h1);
		heights(L, c.drops, pc, c.sm[k + 1].at, g0, g1);
		wall(c, k, side < 0 ? pc.u0 : pc.u1, -L.skirt, side < 0 ? h0 : h1, -L.skirt, side < 0 ? g0 : g1, side, pc.face);
	}
}

// The raised strips' end faces at sample `end`, looking `along` the road (-1 back, +1 ahead). `island_only` for a nose.
void end_faces(const Ctx &c, const std::vector<Piece> &ps, size_t end, float along, bool island_only) {
	const StreetLook &L = c.e.L;
	const Sample &s = c.sm[end];
	for (const Piece &pc : ps) {
		float h0, h1;
		heights(L, c.drops, pc, s.at, h0, h1);
		if ((h0 < L.face_eps && h1 < L.face_eps) || (island_only && !pc.island)) continue;
		const Vector3 p[4] = { c.at_u(end, pc.u0, 0), c.at_u(end, pc.u1, 0), c.at_u(end, pc.u1, h1), c.at_u(end, pc.u0, h0) };
		const Vector2 uv[4] = { Vector2(pc.u0, 0), Vector2(pc.u1, 0), Vector2(pc.u1, h1), Vector2(pc.u0, h0) };
		emit_quad(c.e.mb, p, uv, road_color(pc.face, RoadMat::SKIRT), s.f.fwd * along);
	}
}

// What a stretch holds: the island, each side's footway.
struct Hold {
	bool island = true, walk[2] = { true, true };
};

std::vector<Piece> held(const std::vector<Piece> &ps, const std::vector<Piece> &bare, const SideSection sec[2], const Hold &h) {
	std::vector<Piece> out;
	for (const Piece &pc : h.island ? ps : bare) {
		const int side = footway_side(pc, sec);
		if (side < 0 || h.walk[side]) out.push_back(pc);
	}
	return out;
}

// One stretch: the pieces' tops, the faces between them and the skirts down its outer edges where they are the section's own
// (a side whose footway the corner draws has its skirt along the corner's back).
void span(const Ctx &c, const std::vector<Piece> &ps, const Hold &h, const bool walks[2]) {
	for (size_t i = 0; i < ps.size(); i++) {
		tops(c, ps[i]);
		if (i + 1 < ps.size()) steps(c, ps[i], ps[i + 1]);
	}
	if (ps.empty()) return;
	if (h.walk[STREET_RIGHT] || !walks[STREET_RIGHT]) skirt(c, ps.front(), -1.f);
	if (h.walk[STREET_LEFT] || !walks[STREET_LEFT]) skirt(c, ps.back(), 1.f);
}

// The surface stretch by stretch, cut where the island or a footway starts or ends: the near level with its faces and the island's
// noses, the far one its tops alone.
void stretches(const RoadEmit &e, int sid, const SurfaceLayout &lay, const std::vector<Piece> &ps, bool island) {
	const Profile &p = e.s.catalog.profile(e.s.roads.segs[size_t(sid)].profile);
	const SideSection sec[2] = { side_section(p, false), side_section(p, true) };
	const std::vector<Sample> sm = samples_of(e, sid, lay);
	const std::vector<Piece> bare = without_island(ps);
	bool walks[2] = { false, false };
	for (const Piece &pc : ps)
		if (footway_side(pc, sec) >= 0) walks[footway_side(pc, sec)] = true;
	std::vector<size_t> cuts{ 0, sm.size() - 1 };
	for (double v : { lay.lo, lay.hi, lay.walk_from[0], lay.walk_from[1], lay.walk_to[0], lay.walk_to[1] }) cuts.push_back(arc_index(lay.arcs, v));
	std::sort(cuts.begin(), cuts.end());
	cuts.erase(std::unique(cuts.begin(), cuts.end()), cuts.end());
	const size_t n = cuts.size() - 1; // stretches
	const auto hold = [&](size_t i) {
		const double mid = (lay.arcs[cuts[i]] + lay.arcs[cuts[i + 1]]) * HALF;
		Hold h;
		h.island = island && mid > lay.lo && mid < lay.hi;
		for (int s : { STREET_RIGHT, STREET_LEFT }) h.walk[s] = mid > lay.walk_from[s] && mid < lay.walk_to[s];
		return h;
	};
	Ctx c{ e, sm, lay.drops };
	const StreetLook::PaintStyle &style = e.L.styles[size_t(e.s.marking_style.load())];
	size_t band = 0; // the dashes of the road's speed, as its paint draws them (road_markings.cpp)
	while (band < style.dash_kmh.size() && p.speed >= style.dash_kmh[band]) band++;
	c.dashed = style.dash[band] / (style.dash[band] + style.gap[band]);
	c.yellows = !style.all_white;
	for (size_t i = 0; i < n; i++) {
		c.k0 = cuts[i];
		c.k1 = cuts[i + 1];
		const Hold h = hold(i);
		const std::vector<Piece> here = held(ps, bare, sec, h);
		if (e.far) {
			for (const Piece &pc : here) tops(c, pc);
			continue;
		}
		span(c, here, h, walks);
		if (i == 0) end_faces(c, here, c.k0, -1.f, false);
		else if (h.island && !hold(i - 1).island) end_faces(c, here, c.k0, -1.f, true); // the island's nose
		if (i + 1 == n) end_faces(c, here, c.k1, 1.f, false);
		else if (h.island && !hold(i + 1).island) end_faces(c, here, c.k1, 1.f, true);
	}
}
} // namespace

SurfaceLayout surface_layout(const RoadEmit &e, int sid) {
	const RoadSegment &g = e.s.roads.segs[size_t(sid)];
	const Profile &p = e.s.catalog.profile(g.profile);
	SurfaceLayout lay;
	lay.v0 = lay.lo = g.retreat_a;
	lay.v1 = lay.hi = g.len - g.retreat_b;
	footway_mouths(e, sid, lay.walk_from, lay.walk_to);
	lay.paved[0] = node_paved(e, g.a);
	lay.paved[1] = node_paved(e, g.b);
	lay.drops.v0 = lay.v0;
	lay.drops.v1 = lay.v1;
	float island0, island1;
	if (!e.far) {
		lay.drops = kerb_drops_of(e.s, sid);
		if (island_span(p, island0, island1)) {
			const double nose = e.s.catalog.furniture.crossing.zebra_to_m + e.L.median_nose;
			// A U-turn's opening: a nose on the side it is entered from, a car's half circle on the other.
			const auto opening = [&](int node) {
				const int arm = e.s.roads.uturn_arm(node);
				return arm < 0 ? 0.0 : arm == sid ? double(e.L.median_nose) * UTURN_NOSES : double(e.s.catalog.tune.road_uturn_opening_m);
			};
			const double open_a = opening(g.a), open_b = opening(g.b);
			if (lay.drops.ca || open_a > 0) lay.lo = lay.v0 + std::max(lay.drops.ca ? nose : 0.0, open_a);
			if (lay.drops.cb || open_b > 0) lay.hi = lay.v1 - std::max(lay.drops.cb ? nose : 0.0, open_b);
		}
	}
	lay_arcs(e, sid, lay);
	return lay;
}

std::vector<SectionPoint> footway_section(const RoadEmit &e, int sid, const SurfaceLayout &lay, int side, double at) {
	const Profile &p = e.s.catalog.profile(e.s.roads.segs[size_t(sid)].profile);
	const SideSection sec[2] = { side_section(p, false), side_section(p, true) };
	std::vector<SectionPoint> out;
	for (const Piece &pc : pieces_of(e.L, p, !e.far)) {
		if (footway_side(pc, sec) != side) continue;
		float h0, h1;
		heights(e.L, lay.drops, pc, at, h0, h1);
		out.push_back({ pc.u0, h0 });
		out.push_back({ pc.u1, h1 });
	}
	// From the stone's back out, one point per edge (where two pieces meet at a step, the higher).
	std::sort(out.begin(), out.end(), [&](const SectionPoint &a, const SectionPoint &b) { return side == STREET_LEFT ? a.u < b.u : a.u > b.u; });
	std::vector<SectionPoint> edges;
	for (const SectionPoint &s : out) {
		if (!edges.empty() && std::fabs(edges.back().u - s.u) < EDGE_M) edges.back().h = std::max(edges.back().h, s.h);
		else edges.push_back(s);
	}
	return edges;
}

void emit_segment(const RoadEmit &e, int sid) {
	const RoadSegment &g = e.s.roads.segs[sid];
	const Profile &p = e.s.catalog.profile(g.profile);
	if (g.len - g.retreat_b - g.retreat_a < ROAD_MIN_SPAN || p.edges.empty()) return;
	const SurfaceLayout lay = surface_layout(e, sid);
	// a kerb's face or a dropped kerb is under a pixel from far: the strips run whole there
	const std::vector<Piece> ps = pieces_of(e.L, p, !e.far);
	const bool noses = lay.lo > lay.v0 || lay.hi < lay.v1;
	stretches(e, sid, lay, ps, !noses || lay.hi - lay.lo >= ISLAND_MIN_M);
}

} // namespace city
