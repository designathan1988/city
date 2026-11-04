// The common massing steps (jobs/b40/massing.ts: block, roofShape, roofOf, otherSkin, finish) and the body of a record
// (jobs/building40.ts building40): kind from use and floors, plan, massing, lot furnishing, shift to the lot frame.
#include "sim/buildings/b40/massing.h"

#include <algorithm>
#include <cmath>

namespace city::b40 {

namespace {
constexpr float MIN_PITCH = 5.f, FLAT_BELOW = 0.66f, RIDGE_TO_STREET = 0.33f, SAWTOOTH_RUN = 6.f, DEG = 180.f / 3.14159265f;
constexpr float RISE_GABLE = 3.2f, RISE_SHED = 1.2f, RISE_SAWTOOTH = 2.f, RISE_OTHER = 3.f;
constexpr float PITCH_GABLE[2] = { 20, 35 }, PITCH_HIP[2] = { 20, 32 }, PITCH_SHED[2] = { 5, 14 }, PITCH_SAW[2] = { 22, 30 };
constexpr float PITCH_OTHER[2] = { 20, 30 }, TINY = 1e-6f, HALF = 0.5f, SECOND_SKIN_LIGHT = 0.04f, SECOND_SKIN_NUDGE = 3.f;
constexpr int SECOND_SKIN_TRIES = 4, ZONE_COMMERCIAL = 2, ZONE_INDUSTRIAL = 3, ZONE_MIXED = 4, ZONE_COM_LOW = 7;
constexpr int ZONE_COM_HIGH = 8, ZONE_RES_LOW = 5, ZONE_RES_HIGH = 6, HOUSE_FLOORS = 2, OFFICE_FLOORS = 8, ERAS = 3;
constexpr uint32_t CHARACTER_MASK = 0xffff, ERA_SHIFT = 16;
constexpr float MEMBRANE_NUDGE = 0.6f;

bool closed_roof(Roof r) { return r == ROOF_FLAT || r == ROOF_TERRACE; }

Kind kind_of(int zone, int floors) {
	bool commercial = zone == ZONE_COMMERCIAL || zone == ZONE_COM_LOW || zone == ZONE_COM_HIGH || zone == ZONE_MIXED;
	if (zone == ZONE_INDUSTRIAL) return KIND_WORKS;
	bool low = zone == ZONE_RES_LOW || zone == ZONE_COM_LOW || (!commercial && floors <= HOUSE_FLOORS);
	bool high = zone == ZONE_RES_HIGH || zone == ZONE_COM_HIGH || (commercial && floors >= OFFICE_FLOORS);
	if (commercial) return high ? KIND_OFFICE : KIND_SHOP;
	return low ? KIND_HOUSE : high ? KIND_TOWER : KIND_FLATS;
}

void membranes(const Data &data, Body &b, Xoshiro128 &r) {
	if (b.roof < 0) return;
	int8_t f = b.paints[size_t(b.roof)].finish;
	if (f != FIN_TILE && f != FIN_SLATE && f != FIN_METAL) return;
	int m = b.add_paint(Paint{ FIN_ROOFING, nudge(data.color("membrane").rgb, r, MEMBRANE_NUDGE) });
	for (Volume &v : b.volumes)
		if (closed_roof(v.roof) && v.roof_paint < 0) v.roof_paint = m;
}
} // namespace

int add_block(Body &b, const BlockSpec &s) {
	Volume v;
	v.x = s.x; v.y = s.y; v.w = s.w; v.d = s.d;
	v.base = s.base; v.storeys = std::max(1, s.storeys); v.roof = s.roof;
	v.fill.assign(size_t(v.storeys), s.fill);
	if (s.ground != BAY_NONE) v.fill[0] = s.ground;
	if (s.shape && s.shape->size() >= 3) v.outline = *s.shape;
	b.volumes.push_back(v);
	return int(b.volumes.size()) - 1;
}

void roof_shape(Volume &v, Xoshiro128 &r, int ridge_to_street) {
	bool to_street = ridge_to_street < 0 ? r.f() < RIDGE_TO_STREET : ridge_to_street == 1;
	if (closed_roof(v.roof)) return;
	if (v.roof == ROOF_GABLE || v.roof == ROOF_HIP) v.ridge = to_street ? 1 : 0;
	if (v.roof == ROOF_SHED && v.fall < 0 && v.w < v.d) v.fall = 1;
	const float *pr = v.roof == ROOF_GABLE ? PITCH_GABLE : v.roof == ROOF_HIP ? PITCH_HIP : v.roof == ROOF_SHED ? PITCH_SHED
			: v.roof == ROOF_SAWTOOTH ? PITCH_SAW : PITCH_OTHER;
	float rise = v.roof == ROOF_GABLE || v.roof == ROOF_HIP ? RISE_GABLE : v.roof == ROOF_SHED ? RISE_SHED
			: v.roof == ROOF_SAWTOOTH ? RISE_SAWTOOTH : RISE_OTHER;
	float run;
	if (v.roof == ROOF_SHED) run = (std::max(0, v.fall) % 2 == 0) ? v.d : v.w;
	else if (v.roof == ROOF_SAWTOOTH) run = std::min(SAWTOOTH_RUN, v.d) / 2;
	else run = ((v.ridge >= 0 ? v.ridge == 0 : v.w >= v.d) ? v.d : v.w) / 2;
	float cap = std::atan(rise / std::max(run, TINY)) * DEG;
	if (cap < MIN_PITCH * FLAT_BELOW) {
		v.roof = ROOF_FLAT; v.pitch = -1; v.ridge = -1; v.fall = -1;
		return;
	}
	v.pitch = std::max(MIN_PITCH, std::floor(std::min(mixr(r, pr[0], pr[1]), cap)));
}

Paint roof_of(Xoshiro128 &r, const std::vector<Paint> &list) {
	const Paint &p = choose(r, list);
	return Paint{ p.finish, nudge(p.rgb, r, HALF) };
}

std::vector<Paint> clay_of(const Data &data) {
	std::vector<Paint> out;
	for (const Paint &p : data.roof_tiles)
		if (data.is_clay(p)) out.push_back(p);
	return out;
}

Paint other_skin(const Data &data, int style, Xoshiro128 &r, const Paint &wall) {
	for (int k = 0; k < SECOND_SKIN_TRIES; k++) {
		Paint p, trim;
		paints_of(data, style, r, p, trim);
		if (std::abs(lightness(p.rgb) - lightness(wall.rgb)) > SECOND_SKIN_LIGHT || p.finish != wall.finish) return p;
	}
	return Paint{ wall.finish, nudge(wall.rgb, r, SECOND_SKIN_NUDGE) };
}

void look(Body &b, const Paint &wall, const Paint &roof, const Paint &trim) {
	b.wall = b.add_paint(wall);
	b.roof = b.add_paint(roof);
	b.trim = b.add_paint(trim);
}

void finish_body(const Data &data, Body &b, const Envelope &env, Xoshiro128 &r, uint8_t door_kind, int door_block) {
	for (Volume &v : b.volumes)
		if (v.pitch < 0) roof_shape(v, r);
	membranes(data, b, r);
	compose_facades(data, b, r);
	int front = -1, back = -1;
	for (size_t i = 0; i < b.volumes.size(); i++) {
		const Volume &v = b.volumes[i];
		if (v.base != 0) continue;
		if (back < 0 || v.y + v.d > b.volumes[size_t(back)].y + b.volumes[size_t(back)].d) back = int(i);
		if (v.keep) continue;
		const Volume *f = front < 0 ? nullptr : &b.volumes[size_t(front)];
		if (!f || v.y < f->y || (v.y == f->y && v.w > f->w)) front = int(i);
	}
	if (door_block >= 0) front = door_block;
	if (front < 0) front = 0;
	Volume &fv = b.volumes[size_t(front)];
	int across = bays_of(fv, 0);
	float want = std::clamp(env.W / 2, fv.x, fv.x + fv.w);
	int bay = std::clamp(int(std::floor((want - fv.x) / fv.w * float(across))), 0, across - 1);
	fv.bays[bay_key(0, 0, bay)] = door_kind;
	float entrance = fv.x + (float(bay) + HALF) * fv.w / float(across);
	if (env.back_door && back >= 0) {
		Volume &bv = b.volumes[size_t(back)];
		bv.bays[bay_key(0, 2, bays_of(bv, 2) / 2)] = BAY_DOOR;
	}
	b.entrance_x = entrance;
	b.entrance_volume = front;
	dress_building(data, b, r, entrance, front);
}

void made_to_measure(const Data &data, Body &b, const Envelope &env, Xoshiro128 &r) {
	switch (b.kind) {
		case KIND_HOUSE: house(data, b, env, r); break;
		case KIND_FLATS: flats(data, b, env, r, false); break;
		case KIND_TOWER: flats(data, b, env, r, true); break;
		case KIND_SHOP: shop(data, b, env, r); break;
		case KIND_OFFICE: office(data, b, env, r); break;
		default: works(data, b, env, r); break;
	}
}

void make_body(const Data &data, Body &b, int zone, int floors, float W, float D, uint32_t seed) {
	b = Body{};
	b.kind = kind_of(zone, floors);
	b.use = b.kind == KIND_WORKS ? USE_INDUSTRIAL : b.kind == KIND_SHOP || b.kind == KIND_OFFICE ? USE_COMMERCIAL : USE_RESIDENTIAL;
	Xoshiro128 r(seed);
	b.plan = plan_lot(b.kind, W, D, r);
	Envelope env;
	env.W = b.plan.x1 - b.plan.x0;
	env.D = b.plan.y1 - b.plan.y0;
	env.back_door = b.plan.back != BACK_NONE && b.plan.back != BACK_LOADING;
	env.character = seed & CHARACTER_MASK;
	env.era = int((seed >> ERA_SHIFT) % ERAS);
	made_to_measure(data, b, env, r);
	float dx = b.plan.x0 - W / 2, dy = b.plan.y0;
	for (Volume &v : b.volumes) { v.x += dx; v.y += dy; }
	for (LotElement &e : b.elements) { e.x += dx; e.y += dy; }
	for (Core &c : b.cores) { c.x += dx; c.y += dy; }
	b.entrance_x += dx;
	furnish_lot(data, b, r);
}

} // namespace city::b40
