// Flats, residential towers, shops and offices made to measure: podiums, setbacks, crowns, twins, ells, U and L
// courts, pairs, the lift core and the shop dress.
#include "sim/buildings/b40/massing.h"

#include "core/math.h"

#include <cmath>

namespace city::b40 {

namespace {
constexpr int PALETTES = 7, SHOP_FUNCTIONS = 8, ROUND_SIDES = 16;
constexpr float HALF = 0.5f;
// Floor-to-floor heights (m). Every one leaves the 2.50 m clear of a room or a workplace (Sao Paulo COE, Decreto
// 57.776/2017 Anexo I 5.A.6) over a floor slab of at least 0.08 m (ABNT NBR 6118:2014 13.2.4.1). The city they make
// (map 0: flats 3.25 m and offices 3.73 m of wall per storey, the tall ground floor included) stays under the CTBUH
// database's height to the architectural top per floor (residential 3.42 m over 9,957 buildings, office 4.39 m over
// 7,344; skyscrapercenter.com height calculator), which adds crowns and plant floors these walls leave out.
// Shop storeys: the same minimum; their spread has no primary source yet.
constexpr float FLATS_GROUND_LO = 3.8f, FLATS_GROUND_HI = 4.5f, FLATS_STOREY_LO = 2.9f, FLATS_STOREY_HI = 3.1f;
constexpr float TALL_SHARE = 0.125f, SHOP_GROUND = 0.25f, HIP_SHARE = 0.4f, TERRACE_SHARE = 0.3f, BAR_LO = 12.f, BAR_HI = 15.f;
constexpr float FACADE_SETBACK_LO = 0.5f, INSET_HI = 1.2f, TOWER_MIN = 8.f, TOWER_MAX_D = 22.f, OFFICE_MAX_D = 24.f;
constexpr float ELL_TOWER_W = 14.f, TWIN_TOWER_W = 26.f, ELL_LO = 0.38f, ELL_HI = 0.5f, LEG_MIN = 4.f, GAP_LO = 3.f, GAP_HI = 5.f;
constexpr int TWIN_MIN = 6, LIFT_EXTRA = 2;
constexpr float CORNICE_IN_LO = 1.5f, CROWN_IN_HI = 3.f, CROWN_MIN = 6.f, SHAPE_RECT = 0.5f, SHAPE_OCT = 0.85f;
constexpr float CUT_LO = 0.14f, CUT_HI = 0.26f, SETBACK_LO = 0.75f, SETBACK_HI = 0.9f;
constexpr int SETBACK_SPLIT = 6, SETBACK_STEPS = 3, SETBACK_PART = 3;
constexpr float U_ROOM = 6.f, U_W = 20.f, U_WING_LO = 6.f, U_WING_MAX = 9.f, L_W = 14.f, L_SHARE = 0.7f, L_WING_LO = 7.f;
constexpr float L_WING_MAX = 11.f, L_LOW = 0.25f, L_LOW_SHARE = 0.7f, PAIR_W = 16.f, PAIR_SHARE = 0.4f, PAIR_LO = 0.4f;
constexpr float PAIR_HI = 0.6f, PAIR_BACK_LO = 1.f, PAIR_BACK_HI = 2.5f, PAIR_SKIN = 0.6f, BAR_STEP = 2.f, BAR_STEP_D = 9.f;
constexpr int BAR_STEP_FLOORS = 5;
constexpr float CORE_HALF = 1.5f, FLATS_CORE_D = 10.f, OFFICE_CORE_D = 12.f;
constexpr float SHOP_GROUND_LO = 4.f, SHOP_GROUND_HI = 4.6f, SHOP_STOREY = 3.1f, SHOP_PITCH = 0.4f, SHOP_TERRACE = 0.2f;
constexpr float SHOP_DEEP = 16.f, SHOP_MAIN_LO = 12.f, SHOP_BACK = 3.f, SHOP_PAIR_W = 14.f, SHOP_PAIR = 0.35f, STORE_MIN = 2.f;
constexpr float SHOP_DOUBLE = 0.5f, SHOP_SIDE_DOOR_W = 7.f, AWNING_SHARE = 0.3f, AWNING_Y = -0.6f, AWNING_MIN = 2.f;
constexpr float AWNING_D_LO = 1.f, AWNING_D_HI = 1.6f, AWNING_Z = 3.f, AWNING_H = 0.12f;
constexpr float OFFICE_GROUND_LO = 4.5f, OFFICE_GROUND_HI = 5.5f, OFFICE_STOREY_LO = 3.4f, OFFICE_STOREY_HI = 3.9f;
constexpr float PODIUM_SHARE = 0.6f, PODIUM_MIN = 14.f, OFFICE_CROWN_LO = 1.f, OFFICE_CROWN_HI = 2.5f;
constexpr int APARTMENT_MIN = 3, MAX_FLATS = 7, MIN_TOWER = 8, MAX_TOWER = 24, MIN_TALL = 30, MAX_TALL = 44, SHOP_MAX = 6, SHOP_LOW_MAX = 3;
constexpr float SETBACK_SHARES[] = { 0.4f, 0.6f };
constexpr int TWIN_STEPS[] = { -5, -3, 3, 5 }, OFFICE_TWIN_STEPS[] = { -6, -3, 4 };
constexpr int WORKPLACE_MIN_F = 8, OFFICE_MAX_F = 24, OFFICE_TWIN_MIN = 4;

std::vector<Point2> tower_shape(Xoshiro128 &r) {
	float t = float(r.f());
	if (t < SHAPE_RECT) return {};
	if (t < SHAPE_OCT) {
		float c = mixr(r, CUT_LO, CUT_HI);
		return { { c, 0 }, { 1 - c, 0 }, { 1, c }, { 1, 1 - c }, { 1 - c, 1 }, { c, 1 }, { 0, 1 - c }, { 0, c } };
	}
	std::vector<Point2> out;
	for (int k = 0; k < ROUND_SIDES; k++) {
		float a = -PI_F / 2 + float(k) / ROUND_SIDES * PI_F * 2;
		out.push_back({ HALF + HALF * std::cos(a), HALF + HALF * std::sin(a) });
	}
	return out;
}

void setbacks(Body &b, Xoshiro128 &r, float x, float y, float w, float d, int base, int floors, Roof roof, const std::vector<Point2> *shape) {
	int left = floors, at = base;
	float scale = mixr(r, SETBACK_LO, SETBACK_HI);
	for (int step = 0; left > 0; step++) {
		int part = left > SETBACK_SPLIT && step < SETBACK_STEPS ? std::max(SETBACK_PART, int(std::round(float(left) * choose(r, SETBACK_SHARES)))) : left;
		BlockSpec s{ x, y, w, d, at, part, part == left ? roof : ROOF_TERRACE };
		s.shape = shape;
		add_block(b, s);
		at += part; left -= part;
		float nw = snap(std::max(TOWER_MIN, w * scale)), nd = snap(std::max(TOWER_MIN, d * scale));
		x += snap((w - nw) / 2); y += snap((d - nd) / 2); w = nw; d = nd;
	}
}

void tower(Body &b, Xoshiro128 &r, float w, float d, int floors) {
	int podium = r.irange(1, 2);
	float inset = snap(mixr(r, FACADE_SETBACK_LO, INSET_HI));
	float tw = std::max(TOWER_MIN, w - 2 * inset), td = std::max(TOWER_MIN, std::min(d - 2 * inset, TOWER_MAX_D));
	add_block(b, { 0, 0, w, std::min(d, td + 2 * inset), 0, podium, ROOF_TERRACE });
	std::vector<int> forms{ 0, 1, 2, 2 }; // box, setback, crown, crown, ell, twin, twin
	if (tw >= ELL_TOWER_W) forms.push_back(3);
	if (tw >= TWIN_TOWER_W) { forms.push_back(4); forms.push_back(4); }
	int kind = choose(r, forms);
	std::vector<Point2> shape = kind == 3 ? std::vector<Point2>{} : tower_shape(r);
	const std::vector<Point2> *sp = shape.empty() ? nullptr : &shape;
	auto block = [&](float x, float y, float bw, float bd, int base, int n, Roof rf, bool shaped) {
		BlockSpec s{ x, y, bw, bd, base, n, rf };
		if (shaped) s.shape = sp;
		add_block(b, s);
	};
	if (kind == 1) setbacks(b, r, inset, inset, tw, td, podium, floors, ROOF_FLAT, sp);
	else if (kind == 3) {
		float leg = snap(std::max(TOWER_MIN, tw * mixr(r, ELL_LO, ELL_HI)));
		bool left = r.f() < HALF;
		float front_d = snap(std::max(TOWER_MIN, td * HALF));
		block(inset, inset, tw, front_d, podium, floors, ROOF_FLAT, false);
		block(left ? inset : inset + tw - leg, inset + front_d, leg, snap(std::max(LEG_MIN, td * HALF)), podium, std::max(4, floors - r.irange(0, 4)), ROOF_FLAT, false);
	} else if (kind == 4) {
		float gap = snap(mixr(r, GAP_LO, GAP_HI)), each = snap((tw - gap) / 2);
		int other = std::max(TWIN_MIN, floors + choose(r, TWIN_STEPS));
		block(inset, inset, each, td, podium, floors, ROOF_FLAT, true);
		block(inset + each + gap, inset, each, td, podium, other, ROOF_FLAT, true);
	} else if (kind == 2) {
		int top = r.irange(1, 3);
		block(inset, inset, tw, td, podium, floors - top, ROOF_TERRACE, true);
		float cin = snap(mixr(r, CORNICE_IN_LO, CROWN_IN_HI));
		block(inset + cin, inset + cin, std::max(CROWN_MIN, tw - 2 * cin), std::max(CROWN_MIN, td - 2 * cin), podium + floors - top, top, ROOF_FLAT, true);
	} else block(inset, inset, tw, td, podium, floors, ROOF_FLAT, true);
}

bool medium_flats(Body &b, Xoshiro128 &r, float w, float d, int floors, int cap, Roof roof, float bar_d) {
	if (d - bar_d >= U_ROOM && w >= U_W && r.f() < HALF) {
		float ww = snap(mixr(r, U_WING_LO, std::min(U_WING_MAX, w / 3)));
		int wf = std::max(2, floors - r.irange(0, 2));
		add_block(b, { 0, 0, w, bar_d, 0, floors, roof });
		add_block(b, { 0, bar_d, ww, d - bar_d, 0, wf, roof });
		add_block(b, { w - ww, bar_d, ww, d - bar_d, 0, wf, roof });
	} else if (d - bar_d >= U_ROOM && w >= L_W && r.f() < L_SHARE) {
		float ww = snap(mixr(r, L_WING_LO, std::min(L_WING_MAX, w / 2)));
		bool left = r.f() < HALF;
		add_block(b, { 0, 0, w, bar_d, 0, floors, roof });
		int wf = r.f() < L_LOW ? std::max(2, int(std::round(float(floors) * L_LOW_SHARE))) : floors;
		add_block(b, { left ? 0 : w - ww, bar_d, ww, d - bar_d, 0, wf, roof });
	} else if (w >= PAIR_W && r.f() < PAIR_SHARE) {
		float split = snap(w * mixr(r, PAIR_LO, PAIR_HI)), back = snap(mixr(r, PAIR_BACK_LO, PAIR_BACK_HI));
		add_block(b, { 0, 0, split, d, 0, floors, roof });
		int other = std::max(2, floors + choose(r, { -2, -1, 1, 2 }));
		add_block(b, { split, back, w - split, d - back, 0, cap > 0 ? std::min(other, cap) : other, roof });
		return true;
	} else {
		int lower = floors >= BAR_STEP_FLOORS && r.f() < HALF ? floors - r.irange(1, 2) : floors;
		add_block(b, { 0, 0, w, d, 0, lower, lower < floors ? ROOF_TERRACE : roof });
		if (lower < floors && d >= BAR_STEP_D) add_block(b, { 0, BAR_STEP, w, d - BAR_STEP, lower, floors - lower, roof });
	}
	return false;
}
} // namespace

void flats(const Data &data, Body &b, const Envelope &env, Xoshiro128 &r, bool high) {
	b.style = uint8_t(style_for(data, high ? KIND_TOWER : KIND_FLATS, r, env.character, env.era));
	Paint wall, trim;
	paints_of(data, b.style, r, wall, trim);
	b.palette = r.irange(0, PALETTES);
	b.ground_h = mixr(r, FLATS_GROUND_LO, FLATS_GROUND_HI);
	b.storey_h = mixr(r, FLATS_STOREY_LO, FLATS_STOREY_HI);
	look(b, wall, roof_of(r, clay_of(data)), trim);
	int floors = high ? (r.f() < TALL_SHARE ? r.irange(MIN_TALL, MAX_TALL) : r.irange(MIN_TOWER, MAX_TOWER)) : r.irange(APARTMENT_MIN, MAX_FLATS);
	if (!high && env.cap > 0) floors = std::min(std::max(APARTMENT_MIN, env.cap), floors);
	b.ground = r.f() < SHOP_GROUND ? GROUND_SHOP : GROUND_LOBBY;
	bool hip = !high && (b.style == STYLE_COLONIAL || b.style == STYLE_BRICK) && r.f() < HIP_SHARE;
	Roof roof = hip ? ROOF_HIP : r.f() < TERRACE_SHARE ? ROOF_TERRACE : ROOF_FLAT;
	float w = snap(env.W), d = snap(env.D);
	float bar_d = snap(std::min(d, mixr(r, BAR_LO, BAR_HI)));
	bool pair = false;
	if (high) tower(b, r, w, d, floors);
	else pair = medium_flats(b, r, w, d, floors, env.cap, roof, bar_d);
	b.cores.push_back(Core{ snap(w / 2 - CORE_HALF), snap(std::min(d, FLATS_CORE_D) / 2 - CORE_HALF), floors + (high ? LIFT_EXTRA : 0) });
	if (pair && r.f() < PAIR_SKIN) b.volumes[1].wall = b.add_paint(other_skin(data, b.style, r, wall));
	for (Volume &v : b.volumes) roof_shape(v, r);
	finish_body(data, b, env, r, BAY_DOUBLE_DOOR, -1);
}

void shop(const Data &data, Body &b, const Envelope &env, Xoshiro128 &r) {
	r.irange(0, SHOP_FUNCTIONS - 1); // the function name (shop, bakery, ...): drawn for the stream, not drawn differently
	b.style = uint8_t(style_for(data, KIND_SHOP, r, env.character, env.era));
	Paint wall, trim;
	paints_of(data, b.style, r, wall, trim);
	b.palette = r.irange(0, PALETTES);
	b.ground_h = mixr(r, SHOP_GROUND_LO, SHOP_GROUND_HI);
	b.storey_h = SHOP_STOREY;
	look(b, wall, roof_of(r, clay_of(data)), trim);
	b.ground = GROUND_SHOP;
	int storeys = env.low ? r.irange(1, SHOP_LOW_MAX) : r.irange(2, SHOP_MAX);
	if (!env.low && env.cap > 0) storeys = std::min(std::max(2, env.cap), storeys);
	Roof roof = storeys == 1 && b.style == STYLE_COLONIAL && r.f() < SHOP_PITCH ? choose(r, { ROOF_GABLE, ROOF_HIP })
			: r.f() < SHOP_TERRACE ? ROOF_TERRACE : ROOF_FLAT;
	float w = snap(env.W), d = snap(env.D);
	float main_d = d > SHOP_DEEP ? snap(mixr(r, SHOP_MAIN_LO, std::min(SHOP_DEEP, d - SHOP_BACK))) : d;
	bool pair = w >= SHOP_PAIR_W && storeys >= 2 && r.f() < SHOP_PAIR;
	if (pair) {
		float split = snap(w * mixr(r, PAIR_LO, PAIR_HI));
		add_block(b, { 0, 0, split, main_d, 0, storeys, roof });
		int other = std::max(1, storeys + choose(r, { -1, 1 }));
		add_block(b, { split, 0, w - split, main_d, 0, env.cap > 0 ? std::min(other, env.cap) : other, roof });
	} else add_block(b, { 0, 0, w, main_d, 0, storeys, roof });
	if (d - main_d >= STORE_MIN) {
		int store = add_block(b, { 0, main_d, w, d - main_d, 0, 1, ROOF_FLAT, BAY_WALL, BAY_WALL });
		Volume &sv = b.volumes[size_t(store)];
		sv.keep = true; sv.lines = LINES_NONE; sv.crown = CROWN_NONE;
	}
	if (pair) b.volumes[1].wall = b.add_paint(other_skin(data, b.style, r, wall));
	for (Volume &v : b.volumes) roof_shape(v, r);
	uint8_t door = r.f() < SHOP_DOUBLE ? BAY_DOUBLE_DOOR : BAY_DOOR;
	finish_body(data, b, env, r, door, -1);
	if (storeys > 1 && w >= SHOP_SIDE_DOOR_W) {
		Volume &m = b.volumes[0];
		int across = bays_of(m, 0), end = r.f() < HALF ? 0 : across - 1;
		auto it = m.bays.find(bay_key(0, 0, end));
		if (it == m.bays.end() || (it->second != BAY_DOUBLE_DOOR && it->second != BAY_DOOR)) m.bays[bay_key(0, 0, end)] = BAY_DOOR;
	}
	if ((b.style == STYLE_COLONIAL || b.style == STYLE_BRICK) && r.f() < AWNING_SHARE) {
		LotElement e;
		e.kind = EL_AWNING; e.x = w / 2; e.y = AWNING_Y; e.w = std::max(AWNING_MIN, w - 1); e.d = mixr(r, AWNING_D_LO, AWNING_D_HI);
		e.z = AWNING_Z; e.h = AWNING_H; e.paint = b.add_paint(Paint{ FIN_METAL, choose(r, data.awnings) });
		b.elements.push_back(e);
	}
}

void office(const Data &data, Body &b, const Envelope &env, Xoshiro128 &r) {
	b.style = uint8_t(style_for(data, KIND_OFFICE, r, env.character, env.era));
	Paint wall, trim;
	paints_of(data, b.style, r, wall, trim);
	b.palette = r.irange(0, PALETTES);
	b.ground_h = mixr(r, OFFICE_GROUND_LO, OFFICE_GROUND_HI);
	b.storey_h = mixr(r, OFFICE_STOREY_LO, OFFICE_STOREY_HI);
	look(b, wall, data.color("office_roof"), trim);
	b.ground = GROUND_LOBBY;
	int floors = r.irange(WORKPLACE_MIN_F, OFFICE_MAX_F);
	float w = snap(env.W), d = snap(env.D);
	if (r.f() < PODIUM_SHARE && w >= PODIUM_MIN && d >= PODIUM_MIN) {
		int podium = r.irange(1, 3);
		float inset = snap(mixr(r, FACADE_SETBACK_LO, INSET_HI));
		float tw = w - 2 * inset, td = std::max(TOWER_MIN, std::min(d - 2 * inset, OFFICE_MAX_D));
		add_block(b, { 0, 0, w, std::min(d, td + 2 * inset), 0, podium, ROOF_TERRACE });
		int shaft = floors - podium;
		int form = b.style == STYLE_ART_DECO ? 0 : tw >= TWIN_TOWER_W ? choose(r, { 0, 1, 2, 3 }) : choose(r, { 0, 1, 3 });
		if (form == 0) setbacks(b, r, inset, inset, tw, td, podium, shaft, ROOF_FLAT, nullptr);
		else if (form == 2) {
			float gap = snap(mixr(r, GAP_LO, GAP_HI)), each = snap((tw - gap) / 2);
			add_block(b, { inset, inset, each, td, podium, shaft, ROOF_FLAT });
			add_block(b, { inset + each + gap, inset, each, td, podium, std::max(OFFICE_TWIN_MIN, shaft + choose(r, OFFICE_TWIN_STEPS)), ROOF_FLAT });
		} else if (form == 3) {
			int top = r.irange(2, 3);
			float cin = snap(mixr(r, OFFICE_CROWN_LO, OFFICE_CROWN_HI));
			add_block(b, { inset, inset, tw, td, podium, shaft - top, ROOF_TERRACE });
			add_block(b, { inset + cin, inset + cin, std::max(CROWN_MIN, tw - 2 * cin), std::max(CROWN_MIN, td - 2 * cin), podium + shaft - top, top, ROOF_FLAT });
		} else add_block(b, { inset, inset, tw, td, podium, shaft, ROOF_FLAT });
	} else add_block(b, { 0, 0, w, d, 0, floors, ROOF_FLAT });
	b.cores.push_back(Core{ snap(w / 2 - CORE_HALF), snap(std::min(d, OFFICE_CORE_D) / 2 - CORE_HALF), floors });
	finish_body(data, b, env, r, BAY_DOUBLE_DOOR, -1);
}

} // namespace city::b40
