// A building mirrored left to right within its lot (the Roadcraft's opMirror), and two blocks side by side made one (opUnionVolumes)
// (body_ops.h). Mirroring is about the lot's middle line (x = 0 of the lot frame): every wall walks the other way round, so each side
// keyed by number (bays, fills, paints, proportions, reliefs) moves to the side it became and counts its bays from the other end.
#include "sim/buildings/b40/gen.h"
#include "sim/buildings/b40/plan_area.h"
#include "sim/commands/body_ops.h"

#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <utility>
#include <vector>

namespace city {

using namespace b40;

namespace {
constexpr const char *NO_BLOCK = "tool.failed";
constexpr uint32_t SIDE_FIELD = (1u << KEY_SIDE_SHIFT) - 1;
constexpr float FLUSH_EPS = 1e-3f; // two walls this near are one line

// The side a side of a block becomes. A box keeps its numbering (its ring is rebuilt from its box: the bottom and top sides stay,
// left and right swap); an outline, its points reversed to stay counter-clockwise, walks side i backwards as side n - 2 - i.
int mirrored_side(const Volume &v, int side) {
	const int n = v.sides();
	if (v.outline.empty()) return side == 1 ? 3 : side == 3 ? 1 : side;
	return ((n - 2 - side) % n + n) % n;
}

// A facing of the lot (types.h Facing) seen in the mirror: right and left swap.
int mirrored_facing(int facing) { return facing == FACE_RIGHT ? FACE_LEFT : facing == FACE_LEFT ? FACE_RIGHT : facing; }

void mirror_volume(Volume &v) {
	const int n = v.sides();
	std::vector<int> bays(size_t(n), 1);
	for (int side = 0; side < n; side++) bays[size_t(side)] = bays_of(v, side);
	std::unordered_map<uint32_t, uint8_t> bay_map;
	for (const auto &e : v.bays) {
		const int storey = int(e.first >> KEY_STOREY_SHIFT), side = int((e.first >> KEY_SIDE_SHIFT) & SIDE_FIELD), bay = int(e.first & SIDE_FIELD);
		bay_map[bay_key(storey, mirrored_side(v, side), bays[size_t(side)] - 1 - bay)] = e.second;
	}
	auto by_side = [&](const auto &m) {
		std::decay_t<decltype(m)> out;
		for (const auto &e : m) out[side_key(int(e.first >> KEY_SIDE_SHIFT), mirrored_side(v, int(e.first & SIDE_FIELD)))] = e.second;
		return out;
	};
	auto side_fill = by_side(v.side_fill);
	auto storey_paint = by_side(v.storey_paint);
	FaceGeometry faces[MAX_FACES];
	for (int side = 0; side < n; side++) faces[mirrored_side(v, side)] = v.faces[side];
	for (Relief &r : v.reliefs) {
		const int count = bays[size_t(r.side)];
		r.side = mirrored_side(v, r.side);
		const int b0 = count - 1 - r.bay1, b1 = count - 1 - r.bay0;
		r.bay0 = b0;
		r.bay1 = b1;
	}
	v.bays = std::move(bay_map);
	v.side_fill = std::move(side_fill);
	v.storey_paint = std::move(storey_paint);
	std::copy(std::begin(faces), std::end(faces), std::begin(v.faces));
	v.x = -(v.x + v.w);
	if (!v.outline.empty()) {
		for (V2 &p : v.outline) p.x = 1 - p.x;
		std::reverse(v.outline.begin(), v.outline.end());
	}
	if (v.fall >= 0) v.fall = mirrored_facing(v.fall);
	for (RoofPart &p : v.gear) {
		p.x = -p.x;
		p.turn = -p.turn;
	}
}
} // namespace

std::string op_mirror(Body &b) {
	if (b.volumes.empty()) return NO_BLOCK;
	for (Volume &v : b.volumes) mirror_volume(v);
	for (LotElement &e : b.elements) {
		e.x = -e.x;
		e.facing = mirrored_facing(e.facing);
		e.angle = -e.angle;
	}
	for (Core &c : b.cores) c.x = -(c.x + core_width(c));
	for (auto &floor : b.furnishing)
		for (PlacedPiece &p : floor.second) {
			p.x = -p.x;
			p.angle = -p.angle; // a piece faces (sin a, -cos a): mirrored, (-sin a, -cos a)
		}
	b.entrance_x = -b.entrance_x;
	LotPlan &plan = b.plan; // its envelope from the lot's left edge, and what lies left and right of it
	const float x0 = plan.W - plan.x1, x1 = plan.W - plan.x0;
	plan.x0 = x0;
	plan.x1 = x1;
	std::swap(plan.left, plan.right);
	return "";
}

std::string op_union_blocks(Body &b, const Tuning &t, int vol) {
	if (vol < 0 || vol >= int(b.volumes.size())) return NO_BLOCK;
	Volume &v = b.volumes[size_t(vol)];
	if (!v.outline.empty()) return "building.problem.outline";
	for (int k = 0; k < int(b.volumes.size()); k++) {
		const Volume &o = b.volumes[size_t(k)];
		if (k == vol || !o.outline.empty() || o.base != v.base || o.storeys != v.storeys || o.lift != v.lift || o.mode != v.mode) continue;
		const bool same_row = std::fabs(v.y - o.y) < FLUSH_EPS && std::fabs(v.d - o.d) < FLUSH_EPS;
		const bool same_column = std::fabs(v.x - o.x) < FLUSH_EPS && std::fabs(v.w - o.w) < FLUSH_EPS;
		const bool flush_x = std::fabs(v.x + v.w - o.x) < FLUSH_EPS || std::fabs(o.x + o.w - v.x) < FLUSH_EPS;
		const bool flush_y = std::fabs(v.y + v.d - o.y) < FLUSH_EPS || std::fabs(o.y + o.d - v.y) < FLUSH_EPS;
		if (!((same_row && flush_x) || (same_column && flush_y))) continue;
		if ((same_row ? v.w + o.w : v.w) > float(t.build_max_side_m) || (same_column ? v.d + o.d : v.d) > float(t.build_max_side_m))
			return "building.problem.size";
		const float x = std::min(v.x, o.x), y = std::min(v.y, o.y);
		v.w = same_row ? v.w + o.w : v.w;
		v.d = same_column ? v.d + o.d : v.d;
		v.x = x;
		v.y = y;
		v.gear.insert(v.gear.end(), o.gear.begin(), o.gear.end());
		v.bays.clear(); // the walls are new lengths: each bay counted again
		v.reliefs.clear();
		for (FaceGeometry &f : v.faces) f.bays = -1;
		if (b.entrance_volume == k) b.entrance_volume = vol; // the door was on the block that joined: it is on the one block now
		if (b.entrance_volume > k) b.entrance_volume--;
		b.volumes.erase(b.volumes.begin() + k);
		return "";
	}
	return "building.problem.noUnion";
}

} // namespace city
