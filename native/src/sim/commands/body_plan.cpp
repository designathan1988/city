// A block's plan in a shape (the 4.0's editor/buildingPlans.ts and reshapeTier): the rings the Draw tab and the Mass tab's block shapes offer, set on the block's box. What
// was set on the old walls goes to the new wall that faces the same way, and a wall's bays are shared by how much longer or shorter it came out.
#include "sim/buildings/b40/gen.h"
#include "sim/buildings/b40/plan_area.h"
#include "sim/commands/body_ops.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>
#include <vector>

namespace city {

using namespace b40;

namespace {
constexpr const char *NO_BLOCK = "tool.failed";
constexpr int CIRCLE_SIDES = 24, HEXAGON_SIDES = 6, OCTAGON_SIDES = 8, MOST_BAYS = 64; // a round plan's sides, the polygons', the most bays a side
constexpr uint32_t KEY_FIELD = (1u << KEY_SIDE_SHIFT) - 1;
// Shares of the box, the 4.0's: the width of an L's arms; where the arms of a U and a cross stand; a stepped cross's steps; a chamfer, the same as the step's rise.
constexpr float ELL_LEG = .4f, ARM = .3f, FAR_ARM = 1 - ARM, STEP = .15f, FAR_STEP = 1 - STEP, RISE = .12f, FAR_RISE = 1 - RISE;

Ring round_ring(int sides) {
	Ring out;
	for (int i = 0; i < sides; i++) {
		const float a = float(i) * TAU_F / float(sides) - TAU_F / 4.f;
		out.push_back({ 0.5f + 0.5f * std::cos(a), 0.5f + 0.5f * std::sin(a) });
	}
	return out;
}

// The shape in the unit square, counterclockwise. The arms and steps are shares of the box, the 4.0's.
Ring shape_points(int shape) {
	switch (shape) {
		case PLAN_L: return { { 0, 0 }, { 1, 0 }, { 1, ELL_LEG }, { ELL_LEG, ELL_LEG }, { ELL_LEG, 1 }, { 0, 1 } };
		case PLAN_U: return { { 0, 0 }, { 1, 0 }, { 1, 1 }, { FAR_ARM, 1 }, { FAR_ARM, ARM }, { ARM, ARM }, { ARM, 1 }, { 0, 1 } };
		case PLAN_CROSS: return { { ARM, 0 }, { FAR_ARM, 0 }, { FAR_ARM, ARM }, { 1, ARM }, { 1, FAR_ARM }, { FAR_ARM, FAR_ARM }, { FAR_ARM, 1 }, { ARM, 1 }, { ARM, FAR_ARM }, { 0, FAR_ARM },
			{ 0, ARM }, { ARM, ARM } };
		case PLAN_STEPPED: return { { ARM, 0 }, { FAR_ARM, 0 }, { FAR_ARM, RISE }, { FAR_STEP, RISE }, { FAR_STEP, ARM }, { 1, ARM }, { 1, FAR_ARM }, { FAR_STEP, FAR_ARM }, { FAR_STEP, FAR_RISE },
			{ FAR_ARM, FAR_RISE }, { FAR_ARM, 1 }, { ARM, 1 }, { ARM, FAR_RISE }, { STEP, FAR_RISE }, { STEP, FAR_ARM }, { 0, FAR_ARM }, { 0, ARM }, { STEP, ARM }, { STEP, RISE }, { ARM, RISE } };
		case PLAN_CHAMFERED: return { { RISE, 0 }, { FAR_RISE, 0 }, { 1, RISE }, { 1, FAR_RISE }, { FAR_RISE, 1 }, { RISE, 1 }, { 0, FAR_RISE }, { 0, RISE } };
		case PLAN_CIRCLE: return round_ring(CIRCLE_SIDES);
		case PLAN_HEXAGON: return round_ring(HEXAGON_SIDES);
		case PLAN_OCTAGON: return round_ring(OCTAGON_SIDES);
		default: return {};
	}
}

struct Wall {
	float nx = 0, ny = 0, length = 0;
};

std::vector<Wall> walls_of(const Volume &v) {
	const Ring ring = ring_of(v);
	std::vector<Wall> out;
	for (size_t i = 0; i < ring.size(); i++) {
		const V2 &a = ring[i], &b = ring[(i + 1) % ring.size()];
		const float length = std::hypot(b.x - a.x, b.y - a.y), scale = length > 0 ? 1.f / length : 0.f;
		out.push_back({ (b.y - a.y) * scale, (a.x - b.x) * scale, length });
	}
	return out;
}

// For each wall of `from`, the wall of `to` that faces the most the same way.
std::vector<int> facing(const std::vector<Wall> &from, const std::vector<Wall> &to) {
	std::vector<int> out;
	for (const Wall &w : from) {
		int best = 0;
		float score = -std::numeric_limits<float>::max();
		for (int i = 0; i < int(to.size()); i++) {
			const float dot = w.nx * to[size_t(i)].nx + w.ny * to[size_t(i)].ny;
			if (dot > score) {
				score = dot;
				best = i;
			}
		}
		out.push_back(best);
	}
	return out;
}

template <typename M>
M by_side(const M &old, const std::vector<int> &source, int storeys) {
	M next;
	for (int k = 0; k < storeys; k++)
		for (int side = 0; side < int(source.size()); side++) {
			auto it = old.find(side_key(k, source[size_t(side)]));
			if (it != old.end()) next[side_key(k, side)] = it->second;
		}
	return next;
}

// What was set on `old`'s walls, on `v`'s. Bay by bay in key order, so two old bays that land on one leave the same one every time.
void carry_walls(const Volume &old, Volume &v) {
	const std::vector<Wall> was = walls_of(old), now = walls_of(v);
	const std::vector<int> source = facing(now, was), target = facing(was, now);
	for (FaceGeometry &f : v.faces) f = FaceGeometry();
	for (int side = 0; side < int(now.size()) && side < MAX_FACES; side++) {
		const int from = source[size_t(side)];
		FaceGeometry f = old.faces[from];
		if (f.bays >= 0) f.bays = std::clamp(int(std::round(float(f.bays) * now[size_t(side)].length / was[size_t(from)].length)), 1, MOST_BAYS);
		v.faces[side] = f;
	}
	v.side_fill = by_side(old.side_fill, source, v.storeys);
	v.storey_paint = by_side(old.storey_paint, source, v.storeys);
	std::vector<std::pair<uint32_t, uint8_t>> set(old.bays.begin(), old.bays.end());
	std::sort(set.begin(), set.end());
	v.bays.clear();
	for (const auto &e : set) {
		const int storey = int(e.first >> KEY_STOREY_SHIFT), side = int((e.first >> KEY_SIDE_SHIFT) & KEY_FIELD), bay = int(e.first & KEY_FIELD);
		if (side >= int(target.size())) continue;
		const int wall = target[size_t(side)], from = bays_of(old, side), to = bays_of(v, wall);
		v.bays[bay_key(storey, wall, std::min(to - 1, bay * to / std::max(from, 1)))] = e.second;
	}
}
} // namespace

Ring plan_ring(int shape) {
	if (shape == PLAN_RECTANGLE) return { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } };
	return shape_points(shape);
}

std::string op_set_plan(Body &b, const Tuning &, int vol, int shape) {
	if (vol < 0 || vol >= int(b.volumes.size()) || shape < 0 || shape >= PLAN_COUNT) return NO_BLOCK;
	Volume &v = b.volumes[size_t(vol)];
	if (!v.reliefs.empty()) return "building.reshapeRelief"; // a relief is a place on a wall, and the wall goes
	const Volume old = v;
	// Stretched over the box, unlike the 4.0's, where a hexagon narrower than its box shrinks the block at every click.
	Ring ring = shape_points(shape);
	float lo_x = 1, lo_y = 1, hi_x = 0, hi_y = 0;
	for (const V2 &p : ring) {
		lo_x = std::min(lo_x, p.x);
		lo_y = std::min(lo_y, p.y);
		hi_x = std::max(hi_x, p.x);
		hi_y = std::max(hi_y, p.y);
	}
	v.outline.clear();
	for (const V2 &p : ring) v.outline.push_back(V2{ (p.x - lo_x) / (hi_x - lo_x), (p.y - lo_y) / (hi_y - lo_y) });
	const auto same_point = [](const V2 &a, const V2 &c) { return a.x == c.x && a.y == c.y; };
	const bool same = v.outline.size() == old.outline.size() && std::equal(v.outline.begin(), v.outline.end(), old.outline.begin(), same_point);
	if (!same) carry_walls(old, v); // the plan it had: its walls are where they were, and in a concave plan several face one way
	return "";
}

std::string op_whole_plan(Body &b, const Tuning &t, int shape) {
	if (b.volumes.empty()) return NO_BLOCK;
	int biggest = 0, top = 0;
	float most = -1;
	for (int i = 0; i < int(b.volumes.size()); i++) {
		const Volume &v = b.volumes[size_t(i)];
		top = std::max(top, v.top());
		if (v.base == 0 && v.w * v.d > most) {
			most = v.w * v.d;
			biggest = i;
		}
	}
	const Volume kept = b.volumes[size_t(biggest)];
	b.volumes.assign(1, kept);
	Volume &v = b.volumes[0];
	v.storeys = top - v.base;
	v.fill.resize(size_t(v.storeys), v.fill.empty() ? uint8_t(BAY_WINDOW) : v.fill.back());
	v.reliefs.clear(); // what the generator set: nobody has to flatten what they never saw
	return op_set_plan(b, t, 0, shape);
}

} // namespace city
