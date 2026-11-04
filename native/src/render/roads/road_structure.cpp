// The structure of a raised road (Roadcraft render/structures.ts): the deck's underside and sides, its parapets and the bents that
// carry it, from the road's own profile. Past pier_from over the ground a road stands on piers, whatever it was built as.
#include "render/roads/road_mesh.h"
#include "render/roads/road_section.h"
#include "sim/roads/road_grade.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
const Vector3 DOWN(0, -1, 0), UP(0, 1, 0);

void quad(const RoadEmit &e, const Vector3 p0, const Vector3 p1, const Vector3 p2, const Vector3 p3, const Vector3 &out) {
	const Vector3 p[4] = { p0, p1, p2, p3 };
	const Vector2 uv[4] = { Vector2(p0.x, p0.z), Vector2(p1.x, p1.z), Vector2(p2.x, p2.z), Vector2(p3.x, p3.z) };
	const Vector2 none(POOL_NONE, 0), pools[4] = { none, none, none, none };
	emit_quad(e.mb, p, uv, pools, road_color(e.L.structure.concrete, RoadMat::CONCRETE), out);
}

// A block standing from y0 to y1 round `c`, `half_a` across the road and `half_f` along it.
void block(const RoadEmit &e, const Vector3 &c, const Vector3 &left, const Vector3 &fwd, float half_a, float half_f, float y0, float y1) {
	const Vector3 a = left * half_a, f = fwd * half_f, lo(c.x, y0, c.z), hi(c.x, y1, c.z);
	quad(e, lo - a - f, lo + a - f, hi + a - f, hi - a - f, -fwd);
	quad(e, lo + a + f, lo - a + f, hi - a + f, hi + a + f, fwd);
	quad(e, lo - a + f, lo - a - f, hi - a - f, hi - a + f, -left);
	quad(e, lo + a - f, lo + a + f, hi + a + f, hi + a - f, left);
	quad(e, hi - a - f, hi + a - f, hi + a + f, hi - a + f, UP);
	quad(e, lo - a + f, lo + a + f, lo + a - f, lo - a - f, DOWN);
}

// One side of a parapet between two sections: outside, inside and top.
void parapet(const RoadEmit &e, const Vector3 &a, const Vector3 &b, const Vector3 &in_a, const Vector3 &in_b, float h, const Vector3 &out) {
	const Vector3 up(0, h, 0);
	quad(e, a, b, b + up, a + up, out);
	quad(e, in_b, in_a, in_a + up, in_b + up, -out);
	quad(e, a + up, b + up, in_b + up, in_a + up, UP);
}

// A bent where the road stands high enough: columns under a cap, one column under a bridge.
void bent(const RoadEmit &e, const SegFrame &f, float u0, float u1, float top, float ground, RoadStructure structure) {
	const StructureLook &S = e.L.structure;
	const float mid = (u0 + u1) / 2, half = (u1 - u0) / 2, cap = top - S.cap_h;
	const float width = std::max(S.column_min, (cap - ground) / S.slenderness);
	const Vector3 centre = f.c + f.left * mid;
	block(e, centre, f.left, f.fwd, half, S.cap_depth / 2, cap, top);
	const float foot = ground - S.foot;
	if (structure == RoadStructure::BRIDGE) {
		block(e, centre, f.left, f.fwd, half * S.wall_share, width / 2, foot, cap);
		return;
	}
	for (float side : { -S.spread, S.spread }) block(e, centre + f.left * (half * side), f.left, f.fwd, width / 2, width / 2, foot, cap);
}
} // namespace

void emit_structure(const RoadEmit &e, int sid) {
	const RoadSegment &g = e.s.roads.segs[sid];
	const Profile &p = e.s.catalog.profile(g.profile);
	if (g.lift.empty() || p.edges.empty()) return;
	const StructureLook &S = e.L.structure;
	const RoadStructure structure = RoadStructure((g.flags & SEG_STRUCTURE_MASK) >> SEG_STRUCTURE_SHIFT);
	const float deck = structure == RoadStructure::BRIDGE ? S.deck_bridge : S.deck_elevated;
	const float spacing = structure == RoadStructure::BRIDGE ? S.spacing_bridge : S.spacing_elevated;
	const std::vector<Piece> ps = pieces_of(e.L, p);
	const float u0 = ps.front().u0, u1 = ps.back().u1;
	const int n = std::max(1, int(std::ceil(g.len / e.L.step)));
	const float rail = e.s.catalog.tune.terrain_lift_on_m;
	for (int k = 0; k < n; k++) {
		const SegFrame a = seg_frame(e.s, sid, g.len * k / n), b = seg_frame(e.s, sid, g.len * (k + 1) / n);
		const float clear_a = a.c.y - e.s.terrain.height_at(a.c.x, a.c.z), clear_b = b.c.y - e.s.terrain.height_at(b.c.x, b.c.z);
		if (std::max(clear_a, clear_b) < rail) continue;
		const Vector3 drop(0, -deck, 0), al = a.c + a.left * u0, ar = a.c + a.left * u1, bl = b.c + b.left * u0, br = b.c + b.left * u1;
		quad(e, al + drop, ar + drop, br + drop, bl + drop, DOWN);
		quad(e, al + drop, bl + drop, bl, al, -a.left);
		quad(e, br + drop, ar + drop, ar, br, a.left);
		if (std::min(clear_a, clear_b) < S.parapet_from) continue;
		const float kerb = e.L.h_sidewalk;
		const Vector3 lift(0, kerb, 0), wa = a.left * S.parapet_w, wb = b.left * S.parapet_w;
		parapet(e, al + lift, bl + lift, al + lift + wa, bl + lift + wb, S.parapet_h, -a.left);
		parapet(e, br + lift, ar + lift, br + lift - wb, ar + lift - wa, S.parapet_h, a.left);
	}
	for (double at = spacing / 2; at < g.len; at += spacing) {
		const SegFrame f = seg_frame(e.s, sid, at);
		const float ground = e.s.terrain.height_at(f.c.x, f.c.z);
		if (f.c.y - ground >= S.pier_from) bent(e, f, u0, u1, f.c.y - deck, ground, structure);
	}
}

} // namespace city
