// The Mass tab's edits: storeys, sides, wings, setbacks, splits, heights (body_ops.h).
#include "sim/buildings/b40/gen.h"
#include "sim/buildings/b40/plan_area.h"
#include "sim/commands/body_ops.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace city {

using namespace b40;

namespace {
constexpr float CENTIMETRES = 100.f, AREA_EPS = 1e-3f, WING_SHARE = .6f, SHAPED_INSET_SHARE = .1f;
constexpr const char *NO_VOLUME = "tool.failed";

bool has(const Body &b, int vol) { return vol >= 0 && vol < int(b.volumes.size()); }

float snapped(const Tuning &t, float x) { return std::round(x / float(t.build_grid_m)) * float(t.build_grid_m); }

// A storey above the street has no way in: what the street floor opens with becomes the window beside it.
uint8_t above_street(uint8_t bay) {
	switch (bay) {
		case BAY_DOOR: case BAY_LOADING_DOOR: return BAY_WINDOW;
		case BAY_SHOPFRONT: return BAY_WIDE;
		default: return bay;
	}
}

// The storey a volume grows by: its top one, lifted off the street when the top one is the street floor.
} // namespace

uint8_t next_fill(const Volume &v) {
	uint8_t top = v.fill.empty() ? uint8_t(BAY_WINDOW) : v.fill.back();
	return v.top() == 1 ? above_street(top) : top;
}

namespace {

bool overlaps(const Volume &a, const Volume &c) { return overlap_area(ring_of(a), ring_of(c)) > AREA_EPS; }

// What stands on `v`'s old roof rides up or down with it, and what stands on that.
void ride_with(Body &b, const Volume &v, int old_top, int delta, std::vector<const Volume *> &seen) {
	if (delta == 0) return;
	seen.push_back(&v);
	for (Volume &other : b.volumes) {
		if (other.base != old_top || std::find(seen.begin(), seen.end(), &other) != seen.end() || !overlaps(v, other)) continue;
		int other_top = other.top();
		other.base += delta;
		ride_with(b, other, other_top, delta, seen);
	}
}

void set_count(Volume &v, int count) {
	v.storeys = count;
	v.fill.resize(size_t(count));
	move_storeys(v.bays, KEY_STOREY_SHIFT, 0, count);
	move_storeys(v.side_fill, KEY_SIDE_SHIFT, 0, count);
	move_storeys(v.storey_paint, KEY_SIDE_SHIFT, 0, count);
}

// Single-bay overrides of one side move by `shift`: a side that grew at its start recounts from there.
void shift_bays(Volume &v, int moved, int shift) {
	if (shift == 0) return;
	std::unordered_map<uint32_t, uint8_t> next;
	for (const auto &e : v.bays) {
		int storey = int(e.first >> KEY_STOREY_SHIFT), side = int((e.first >> KEY_SIDE_SHIFT) & ((1u << KEY_SIDE_SHIFT) - 1));
		int bay = int(e.first & ((1u << KEY_SIDE_SHIFT) - 1));
		if (side == moved) bay += shift;
		if (bay >= 0) next[bay_key(storey, side, bay)] = e.second;
	}
	v.bays = std::move(next);
}

// A new block built like `v`: its walls, roof paint, rhythm and dress, with `count` storeys of its fill.
} // namespace

Volume block_like(const Volume &v, int count) {
	Volume out;
	out.base = v.base;
	out.storeys = count;
	out.roof = v.roof == ROOF_TERRACE ? ROOF_FLAT : v.roof;
	out.wall = v.wall;
	out.roof_paint = v.roof_paint;
	out.lines = v.lines;
	out.crown = v.crown;
	for (int k = 0; k < count; k++) out.fill.push_back(v.fill[size_t(std::min(k, int(v.fill.size()) - 1))]);
	for (int i = 0; i < MAX_FACES; i++) out.faces[i] = v.faces[i];
	return out;
}

void clear_roof_under(Volume &v, const Ring &plan) {
	v.gear.erase(std::remove_if(v.gear.begin(), v.gear.end(), [&](const RoofPart &p) { return contains_point(plan, { p.x, p.y }); }), v.gear.end());
}

namespace {
// A wing off a wall of a shaped block: from the middle of the wall, along the wall's normal, itself a box at the wall's bearing.
std::string wing_off_wall(Body &b, const Tuning &t, const Volume &v, int side, float depth, float length) {
	const Ring ring = ring_of(v);
	const V2 a = ring[size_t(side)], c = ring[(size_t(side) + 1) % ring.size()];
	const float run = std::hypot(c.x - a.x, c.y - a.y), lo = float(t.build_min_side_m);
	if (run < lo) return "building.problem.size";
	const float tx = (c.x - a.x) / run, ty = (c.y - a.y) / run, nx = ty, ny = -tx;
	const float len = std::min(run, std::max(lo, snapped(t, length > 0 ? length : run * WING_SHARE)));
	const float out = std::max(lo, snapped(t, depth > 0 ? depth : float(t.build_wing_modules * t.build_module_m)));
	const float from = (run - len) / 2;
	const V2 p{ a.x + tx * from, a.y + ty * from }, q{ p.x + tx * len, p.y + ty * len };
	Volume wing = block_like(v, v.storeys);
	for (FaceGeometry &f : wing.faces) f.bays = -1; // its openings' proportions, but as many bays as its own walls hold
	set_block_ring(wing, { q, p, { p.x + nx * out, p.y + ny * out }, { q.x + nx * out, q.y + ny * out } });
	if (wing.base == 0) { // its own way in, on its outer face
		const Ring plan = ring_of(wing);
		int far = 0;
		float most = -1;
		for (int i = 0; i < int(plan.size()); i++) {
			const V2 e0 = plan[size_t(i)], e1 = plan[(size_t(i) + 1) % plan.size()];
			const float along = (e1.y - e0.y) * nx - (e1.x - e0.x) * ny; // the wall's outward normal on the block's, scaled by the wall's length
			if (along > most) {
				most = along;
				far = i;
			}
		}
		wing.bays[bay_key(0, far, bays_of(wing, far) / 2)] = BAY_DOOR;
	}
	b.volumes.push_back(std::move(wing));
	return "";
}
} // namespace

std::string op_set_storeys(Body &b, const Tuning &t, int vol, int count) {
	if (!has(b, vol)) return NO_VOLUME;
	Volume &v = b.volumes[size_t(vol)];
	int next = std::clamp(count, 1, std::max(1, int(t.edit_max_floors) - v.base));
	if (next == v.storeys) return "";
	int old_top = v.top();
	uint8_t fill = next_fill(v);
	int had = v.storeys;
	set_count(v, next);
	for (int k = had; k < next; k++) v.fill[size_t(k)] = fill;
	std::vector<const Volume *> seen;
	ride_with(b, v, old_top, v.top() - old_top, seen);
	return "";
}

std::string op_insert_storey(Body &b, const Tuning &t, int vol, int at, bool above) {
	if (!has(b, vol)) return NO_VOLUME;
	Volume &v = b.volumes[size_t(vol)];
	if (at < 0 || at >= v.storeys || v.top() >= int(t.edit_max_floors)) return "building.problem.size";
	int old_top = v.top(), place = above ? at + 1 : at;
	uint8_t copy = v.fill[size_t(at)];
	if (v.base + at == 0 && above) copy = above_street(copy);
	std::unordered_map<uint32_t, uint8_t> bays;
	for (const auto &e : v.bays) {
		int k = int(e.first >> KEY_STOREY_SHIFT);
		bays[e.first + (k >= place ? 1u << KEY_STOREY_SHIFT : 0u)] = e.second;
	}
	v.bays = std::move(bays);
	std::unordered_map<uint32_t, int> paints;
	for (const auto &e : v.storey_paint) {
		int k = int(e.first >> KEY_SIDE_SHIFT);
		paints[e.first + (k >= place ? 1u << KEY_SIDE_SHIFT : 0u)] = e.second;
	}
	v.storey_paint = std::move(paints);
	std::unordered_map<uint32_t, uint8_t> sides;
	for (const auto &e : v.side_fill) {
		int k = int(e.first >> KEY_SIDE_SHIFT);
		sides[e.first + (k >= place ? 1u << KEY_SIDE_SHIFT : 0u)] = e.second;
	}
	v.side_fill = std::move(sides);
	v.fill.insert(v.fill.begin() + place, copy);
	v.storeys++;
	std::vector<const Volume *> seen;
	ride_with(b, v, old_top, 1, seen);
	return "";
}

std::string op_resize(Body &b, const Tuning &t, int vol, int side, float delta, bool snap) {
	if (!has(b, vol) || side < 0 || side > 3 || delta == 0) return NO_VOLUME;
	Volume &v = b.volumes[size_t(vol)];
	bool across = side == 1 || side == 3;
	float &along = across ? v.w : v.d;
	float wanted = snap ? snapped(t, along + delta) : std::round((along + delta) * CENTIMETRES) / CENTIMETRES;
	float size = std::clamp(wanted, float(t.build_min_side_m), float(t.build_max_side_m)), change = size - along;
	if (std::fabs(change) < AREA_EPS) return "";
	// Sides run round the block (placed.cpp sides_of): the one after the side pulled starts at the edge that moved.
	int next = (side + 1) % 4, before = bays_of(v, next);
	along = size;
	if (side == 3) v.x -= change;
	if (side == 0) v.y -= change;
	shift_bays(v, next, bays_of(v, next) - before);
	return "";
}

std::string op_add_wing(Body &b, const Tuning &t, int vol, int side, float depth, float length) {
	if (!has(b, vol) || side < 0 || side >= b.volumes[size_t(vol)].sides()) return NO_VOLUME;
	if (int(b.volumes.size()) >= int(t.build_max_volumes)) return "building.problem.size";
	const Volume v = b.volumes[size_t(vol)];
	if (!v.outline.empty()) return wing_off_wall(b, t, v, side, depth, length);
	float module = float(t.build_module_m), lo = float(t.build_min_side_m), hi = float(t.build_max_side_m);
	float run = side == 0 || side == 2 ? v.w : v.d;
	float half = std::max(2 * module, std::ceil(run / 2 / module) * module);
	float len = std::clamp(snapped(t, std::min(run, length > 0 ? length : half)), lo, hi);
	float out = std::clamp(snapped(t, depth > 0 ? depth : float(t.build_wing_modules) * module), lo, hi);
	float offset = snapped(t, (run - len) / 2);
	Volume wing = block_like(v, v.storeys);
	for (FaceGeometry &f : wing.faces) f.bays = -1; // its openings' proportions, but as many bays as its own walls hold
	switch (side) {
		case 0: wing.x = v.x + offset; wing.y = v.y - out; wing.w = len; wing.d = out; break;
		case 2: wing.x = v.x + offset; wing.y = v.y + v.d; wing.w = len; wing.d = out; break;
		case 1: wing.x = v.x + v.w; wing.y = v.y + offset; wing.w = out; wing.d = len; break;
		default: wing.x = v.x - out; wing.y = v.y + offset; wing.w = out; wing.d = len; break;
	}
	if (wing.base == 0) wing.bays[bay_key(0, side, bays_of(wing, side) / 2)] = BAY_DOOR; // its own way in, on its outer face
	b.volumes.push_back(std::move(wing));
	return "";
}

std::string op_add_setback(Body &b, const Tuning &t, int vol, float inset, int storeys) {
	if (!has(b, vol)) return NO_VOLUME;
	if (int(b.volumes.size()) >= int(t.build_max_volumes)) return "building.problem.size";
	Volume &v = b.volumes[size_t(vol)];
	float step = std::max(0.f, snapped(t, inset >= 0 ? inset : float(t.build_module_m))), lo = float(t.build_min_side_m);
	if (!v.outline.empty() && inset < 0) step = std::min(step, std::max(float(t.build_grid_m), snapped(t, std::min(v.w, v.d) * SHAPED_INSET_SHARE)));
	float ix = v.w - 2 * step >= lo ? step : 0, iy = v.d - 2 * step >= lo ? step : 0;
	int count = std::clamp(storeys, 1, std::max(1, int(t.edit_max_floors) - v.top()));
	Volume up = block_like(v, count);
	up.base = v.top();
	up.x = v.x + ix;
	up.y = v.y + iy;
	up.w = v.w - 2 * ix;
	up.d = v.d - 2 * iy;
	if (!v.outline.empty()) {
		const Ring ring = offset_ring(ring_of(v), -step);
		if (!simple_ring(ring) || ring_area(ring) <= 0) return "building.problem.outline";
		set_block_ring(up, ring);
	}
	std::fill(up.fill.begin(), up.fill.end(), next_fill(v));
	v.roof = ROOF_TERRACE;
	clear_roof_under(v, ring_of(up)); // what stood where the new block stands goes: nothing is left inside it
	b.volumes.push_back(std::move(up));
	return "";
}

std::string op_split_at_floor(Body &b, const Tuning &t, int vol, int level) {
	if (!has(b, vol) || int(b.volumes.size()) >= int(t.build_max_volumes)) return "building.problem.size";
	Volume &low = b.volumes[size_t(vol)];
	if (level <= low.base || level >= low.top()) return "building.problem.size";
	int keep = level - low.base;
	Volume up = low;
	up.base = level;
	up.storeys = low.storeys - keep;
	up.fill.assign(low.fill.begin() + keep, low.fill.end());
	move_storeys(up.bays, KEY_STOREY_SHIFT, -keep, up.storeys);
	move_storeys(up.side_fill, KEY_SIDE_SHIFT, -keep, up.storeys);
	move_storeys(up.storey_paint, KEY_SIDE_SHIFT, -keep, up.storeys);
	set_count(low, keep);
	up.reliefs.clear();
	std::vector<Relief> below;
	for (const Relief &r : low.reliefs) {
		if (r.storey0 < keep) {
			below.push_back(r);
			below.back().storey1 = std::min(r.storey1, keep - 1);
		}
		if (r.storey1 >= keep) {
			up.reliefs.push_back(r);
			up.reliefs.back().storey0 = std::max(0, r.storey0 - keep);
			up.reliefs.back().storey1 = r.storey1 - keep;
		}
	}
	low.reliefs = std::move(below);
	low.roof = ROOF_TERRACE;
	low.gear.clear(); // the roof it stood on is now the upper block's
	low.pitch = -1;
	low.ridge = low.fall = -1;
	b.volumes.push_back(std::move(up));
	return "";
}

std::string op_remove_volume(Body &b, const Tuning &t, int vol) {
	if (!has(b, vol)) return NO_VOLUME;
	std::vector<Volume> rest = b.volumes;
	rest.erase(rest.begin() + vol);
	bool ground = std::any_of(rest.begin(), rest.end(), [](const Volume &v) { return v.base == 0; });
	if (!ground) return "building.problem.lastVolume";
	// What hangs over nothing goes too, again and again: a tower on a podium goes with the podium.
	for (bool changed = true; changed;) {
		changed = false;
		for (size_t i = 0; i < rest.size() && !changed; i++) {
			if (rest[i].base == 0) continue;
			std::vector<Ring> under;
			for (size_t k = 0; k < rest.size(); k++)
				if (k != i && rest[k].base < rest[i].base && rest[k].top() >= rest[i].base) under.push_back(ring_of(rest[k]));
			if (support_share(ring_of(rest[i]), under) < float(t.build_min_bearing)) {
				rest.erase(rest.begin() + std::ptrdiff_t(i));
				changed = true;
			}
		}
	}
	b.volumes = std::move(rest);
	if (b.entrance_volume >= int(b.volumes.size())) b.entrance_volume = 0;
	return "";
}

std::string op_set_heights(Body &b, const Tuning &t, float ground_h, float storey_h) {
	b.ground_h = std::clamp(ground_h, float(t.build_min_storey_m), float(t.build_max_ground_m));
	b.storey_h = std::clamp(storey_h, float(t.build_min_storey_m), float(t.build_max_storey_m));
	return "";
}

} // namespace city
