// The common massing steps (a block, a roof's shape and paint, a second skin, the finish) and the body of a record:
// kind from use and floors, plan, massing, the lot laid round it, shift to the lot frame.
#include "sim/buildings/b40/massing.h"

#include "core/units.h"

#include <algorithm>
#include <cmath>

namespace city::b40 {

namespace {
constexpr float TINY = 1e-6f, HALF = 0.5f; // a pitch this small is none
// A lot's seed holds its character in the low 16 bits and its era above them.
constexpr uint32_t CHARACTER_MASK = 0xffff, ERA_SHIFT = 16;

bool closed_roof(Roof r) { return r == ROOF_FLAT || r == ROOF_TERRACE; }

} // namespace

void membranes(const Data &data, Body &b, Xoshiro128 &r) {
	const MassingShape &K = data.shapes.massing;
	if (b.roof < 0) return;
	int8_t f = b.paints[size_t(b.roof)].finish;
	if (f != FIN_TILE && f != FIN_SLATE && f != FIN_METAL) return;
	int m = b.add_paint(Paint{ FIN_ROOFING, nudge(data.shapes.paint, data.color("membrane").rgb, r, K.membrane_nudge) });
	for (Volume &v : b.volumes)
		if (closed_roof(v.roof) && v.roof_paint < 0) v.roof_paint = m;
}

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

void roof_shape(const Data &data, Volume &v, Xoshiro128 &r, int ridge_to_street) {
	const MassingShape &K = data.shapes.massing;
	bool to_street = ridge_to_street < 0 ? r.f() < K.ridge_to_street : ridge_to_street == 1;
	if (closed_roof(v.roof)) return;
	if (v.roof == ROOF_GABLE || v.roof == ROOF_HIP) v.ridge = to_street ? 1 : 0;
	if (v.roof == ROOF_SHED && v.fall < 0 && v.w < v.d) v.fall = 1;
	const std::vector<float> &pr = v.roof == ROOF_GABLE ? K.pitch_gable : v.roof == ROOF_HIP ? K.pitch_hip : v.roof == ROOF_SHED ? K.pitch_shed
			: v.roof == ROOF_SAWTOOTH ? K.pitch_saw : K.pitch_other;
	float rise = v.roof == ROOF_GABLE || v.roof == ROOF_HIP ? K.rise_gable : v.roof == ROOF_SHED ? K.rise_shed
			: v.roof == ROOF_SAWTOOTH ? K.rise_sawtooth : K.rise_other;
	float run;
	if (v.roof == ROOF_SHED) run = (std::max(0, v.fall) % 2 == 0) ? v.d : v.w;
	else if (v.roof == ROOF_SAWTOOTH) run = std::min(K.sawtooth_run, v.d) / 2;
	else run = ((v.ridge >= 0 ? v.ridge == 0 : v.w >= v.d) ? v.d : v.w) / 2;
	float cap = std::atan(rise / std::max(run, TINY)) * RAD_TO_DEG;
	if (cap < K.min_pitch * K.flat_below) {
		v.roof = ROOF_FLAT; v.pitch = -1; v.ridge = -1; v.fall = -1;
		return;
	}
	v.pitch = std::max(K.min_pitch, std::floor(std::min(mixr(r, pr[0], pr[1]), cap)));
}

Paint roof_of(const Data &data, Xoshiro128 &r, const std::vector<Paint> &list) {
	const Paint &p = choose(r, list);
	return Paint{ p.finish, nudge(data.shapes.paint, p.rgb, r, HALF) };
}

std::vector<Paint> clay_of(const Data &data) {
	std::vector<Paint> out;
	for (const Paint &p : data.roof_tiles)
		if (data.is_clay(p)) out.push_back(p);
	return out;
}

Paint other_skin(const Data &data, int style, Xoshiro128 &r, const Paint &wall) {
	const MassingShape &K = data.shapes.massing;
	for (int k = 0; k < K.second_skin_tries; k++) {
		Paint p, trim;
		paints_of(data, style, r, p, trim);
		if (std::abs(lightness(p.rgb) - lightness(wall.rgb)) > K.second_skin_light || p.finish != wall.finish) return p;
	}
	return Paint{ wall.finish, nudge(data.shapes.paint, wall.rgb, r, K.second_skin_nudge) };
}

void look(Body &b, const Paint &wall, const Paint &roof, const Paint &trim) {
	b.wall = b.add_paint(wall);
	b.roof = b.add_paint(roof);
	b.trim = b.add_paint(trim);
}

void finish_body(const Data &data, Body &b, const Envelope &env, Xoshiro128 &r, uint8_t door_kind, int door_block) {
	for (Volume &v : b.volumes)
		if (v.pitch < 0) roof_shape(data, v, r);
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

void make_body(const Data &data, Body &b, int zone, int floors, float W, float D, uint32_t seed, LotCorner corner) {
	b = Body{};
	b.kind = data.kind_of_zone(zone);
	b.use = b.kind == KIND_WORKS ? USE_INDUSTRIAL : b.kind == KIND_SHOP || b.kind == KIND_OFFICE ? USE_COMMERCIAL : USE_RESIDENTIAL;
	Xoshiro128 r(seed);
	b.plan = plan_lot(data, b.kind, W, D, r, corner);
	Envelope env;
	env.W = b.plan.x1 - b.plan.x0;
	env.D = b.plan.y1 - b.plan.y0;
	env.back_door = b.plan.back != BACK_NONE && b.plan.back != BACK_LOADING;
	env.character = seed & CHARACTER_MASK;
	env.era = int((seed >> ERA_SHIFT) % ERAS);
	env.cap = floors;
	env.low = data.low_shop(zone);
	made_to_measure(data, b, env, r);
	float dx = b.plan.x0 - W / 2, dy = b.plan.y0;
	for (Volume &v : b.volumes) { v.x += dx; v.y += dy; }
	for (LotElement &e : b.elements) { e.x += dx; e.y += dy; }
	for (Core &c : b.cores) { c.x += dx; c.y += dy; }
	b.entrance_x += dx;
	lay_out_lot(data, b, r);
}

} // namespace city::b40
