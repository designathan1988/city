// A model of data/render/models.json made into a body (model_ops.h): blocks with their fills, doors and patterns, then the parts, all
// moved to the lot frame. A model has no seed: its random bits (the flat roofs' membrane) come from its name.
#include "core/random.h"
#include "sim/buildings/b40/gen.h"
#include "sim/buildings/b40/massing.h"
#include "sim/commands/body_ops.h"
#include "sim/commands/model_ops.h"

#include <algorithm>
#include <cmath>

namespace city {

using namespace b40;

namespace {
constexpr int FRONT = 0;

Ground ground_of(Kind k) {
	switch (k) {
		case KIND_HOUSE: return GROUND_HOUSE;
		case KIND_SHOP: return GROUND_SHOP;
		case KIND_WORKS: return GROUND_WORKS;
		default: return GROUND_LOBBY;
	}
}

// What hangs over the ground or is set on a wall does not take ground of its own: an awning leans out over the pavement.
bool on_ground(const LotElement &e) { return e.z <= 0 && e.kind != EL_AWNING && e.kind != EL_CANOPY; }

void dress_block(Volume &v, const ModelDress &d) {
	int top = d.to == TOP_STOREY ? v.storeys - 1 : std::min(d.to, v.storeys - 1);
	for (int k = d.from; k <= top; k++) {
		if (d.every == 0) {
			v.side_fill[side_key(k, d.side)] = d.fill;
			continue;
		}
		for (int i = d.start; i < bays_of(v, d.side); i += d.every) v.bays[bay_key(k, d.side, i)] = d.fill;
	}
}

void add_model_block(const ModelTable &t, const ModelBlock &m, Body &b) {
	BlockSpec spec{ m.x, m.y, m.w, m.d, m.base, m.storeys, m.roof, m.fill, m.base == 0 ? m.ground : uint8_t(BAY_NONE), m.outline.empty() ? nullptr : &m.outline };
	int at = add_block(b, spec);
	Volume &v = b.volumes[size_t(at)];
	v.pitch = m.pitch;
	v.keep = true;
	for (int side = 0; side < v.sides() && side < MAX_FACES; side++) v.faces[side].bays = std::max(1, int(std::lround(side_length(v, side) / t.module_m)));
	if (m.pattern != NO_PATTERN) {
		op_set_pattern(b, at, -1, -1, m.pattern);
		if (m.base == 0 && m.ground != BAY_NONE)
			for (int side = 0; side < v.sides(); side++) v.side_fill[side_key(0, side)] = m.ground;
	}
	for (const ModelDress &d : m.dress) dress_block(v, d);
	if (m.door_bay == NO_BAY || m.base != 0) return;
	int across = bays_of(v, FRONT), bay = m.door_bay == MIDDLE_BAY ? across / 2 : std::min(m.door_bay, across - 1);
	v.bays[bay_key(0, FRONT, bay)] = m.door_kind;
	if (b.entrance_volume >= 0) return;
	b.entrance_volume = at;
	b.entrance_x = v.x + (float(bay) + 0.5f) * v.w / float(across);
}
} // namespace

V2 fit_plan(Body &b) {
	float x0 = FAR_AWAY, y0 = FAR_AWAY, x1 = -FAR_AWAY, y1 = -FAR_AWAY;
	auto take = [&](float ax, float ay, float bx, float by) {
		x0 = std::min(x0, ax);
		y0 = std::min(y0, ay);
		x1 = std::max(x1, bx);
		y1 = std::max(y1, by);
	};
	for (const Volume &v : b.volumes)
		if (v.base == 0) take(v.x, v.y, v.x + v.w, v.y + v.d);
	for (const LotElement &e : b.elements)
		if (on_ground(e)) take(e.x - e.w / 2, e.y - e.d / 2, e.x + e.w / 2, e.y + e.d / 2);
	const float mid = (x0 + x1) / 2;
	for (Volume &v : b.volumes) {
		v.x -= mid;
		v.y -= y0;
		for (RoofPart &p : v.gear) {
			p.x -= mid;
			p.y -= y0;
		}
	}
	for (LotElement &e : b.elements) {
		e.x -= mid;
		e.y -= y0;
	}
	for (Core &c : b.cores) {
		c.x -= mid;
		c.y -= y0;
	}
	b.entrance_x -= mid;
	b.plan.W = x1 - x0;
	b.plan.D = y1 - y0;
	b.plan.x0 = b.plan.y0 = 0;
	b.plan.x1 = b.plan.W;
	b.plan.y1 = b.plan.D;
	return V2(mid, y0);
}

void model_body(const Data &data, const ModelSpec &m, Body &b) {
	b = Body{};
	uint32_t seed = 0;
	for (char c : m.id) seed = hash_combine(seed, uint32_t(uint8_t(c)));
	Xoshiro128 r(seed);
	b.kind = b.plan.kind = m.kind;
	b.use = m.use;
	b.ground = ground_of(m.kind);
	b.palette = m.palette;
	b.ground_h = m.ground_h;
	b.storey_h = m.storey_h;
	b.entrance_volume = -1;
	look(b, m.wall, m.roof, m.trim);
	for (const ModelBlock &block : m.blocks) add_model_block(data.models, block, b);
	if (b.entrance_volume < 0) b.entrance_volume = 0;
	membranes(data, b, r);
	const int first = int(b.paints.size());
	for (const Paint &p : m.paints) b.add_paint(p);
	b.elements = m.elements;
	for (LotElement &e : b.elements)
		if (e.paint >= 0) e.paint += first;
	b.cores = m.cores;
	fit_plan(b);
}

} // namespace city
