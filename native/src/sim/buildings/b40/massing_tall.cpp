// Flats, residential towers, shops and offices made to measure: podiums, setbacks, crowns, twins, ells, U and L
// courts, pairs, the lift core and the shop dress.
#include "sim/buildings/b40/massing.h"

#include "core/math.h"

#include <cmath>

namespace city::b40 {

namespace {
constexpr int SHOP_FUNCTIONS = 8; // the source game's shop functions: drawn only to keep its random stream (B4 replaces it)
constexpr float HALF = 0.5f;
constexpr float CORE_HALF = CORE_SIDE_M / 2; // the heights and the forms' sizes are buildings.json shapes.tall, with their sources

std::vector<V2> tower_shape(const Data &data, Xoshiro128 &r) {
	const TallShape &K = data.shapes.tall;
	float t = float(r.f());
	if (t < K.shape_rect) return {};
	if (t < K.shape_oct) {
		float c = mixr(r, K.cut_lo, K.cut_hi);
		return { { c, 0 }, { 1 - c, 0 }, { 1, c }, { 1, 1 - c }, { 1 - c, 1 }, { c, 1 }, { 0, 1 - c }, { 0, c } };
	}
	std::vector<V2> out;
	for (int k = 0; k < K.round_sides; k++) {
		float a = -PI_F / 2 + float(k) / K.round_sides * PI_F * 2;
		out.push_back({ HALF + HALF * std::cos(a), HALF + HALF * std::sin(a) });
	}
	return out;
}

void setbacks(const Data &data, Body &b, Xoshiro128 &r, float x, float y, float w, float d, int base, int floors, Roof roof, const std::vector<V2> *shape) {
	const TallShape &K = data.shapes.tall;
	int left = floors, at = base;
	float scale = mixr(r, K.setback_lo, K.setback_hi);
	for (int step = 0; left > 0; step++) {
		int part = left > K.setback_split && step < K.setback_steps ? std::max(K.setback_part, int(std::round(float(left) * choose(r, K.setback_shares)))) : left;
		BlockSpec s{ x, y, w, d, at, part, part == left ? roof : ROOF_TERRACE };
		s.shape = shape;
		add_block(b, s);
		at += part; left -= part;
		float nw = snap(std::max(K.tower_min, w * scale)), nd = snap(std::max(K.tower_min, d * scale));
		x += snap((w - nw) / 2); y += snap((d - nd) / 2); w = nw; d = nd;
	}
}

void tower(const Data &data, Body &b, Xoshiro128 &r, float w, float d, int floors) {
	const TallShape &K = data.shapes.tall;
	int podium = r.irange(1, 2);
	float inset = snap(mixr(r, K.facade_setback_lo, K.inset_hi));
	float tw = std::max(K.tower_min, w - 2 * inset), td = std::max(K.tower_min, std::min(d - 2 * inset, K.tower_max_d));
	add_block(b, { 0, 0, w, std::min(d, td + 2 * inset), 0, podium, ROOF_TERRACE });
	std::vector<int> forms{ 0, 1, 2, 2 }; // box, setback, crown, crown, ell, twin, twin
	if (tw >= K.ell_tower_w) forms.push_back(3);
	if (tw >= K.twin_tower_w) { forms.push_back(4); forms.push_back(4); }
	int kind = choose(r, forms);
	std::vector<V2> shape = kind == 3 ? std::vector<V2>{} : tower_shape(data, r);
	const std::vector<V2> *sp = shape.empty() ? nullptr : &shape;
	auto block = [&](float x, float y, float bw, float bd, int base, int n, Roof rf, bool shaped) {
		BlockSpec s{ x, y, bw, bd, base, n, rf };
		if (shaped) s.shape = sp;
		add_block(b, s);
	};
	if (kind == 1) setbacks(data, b, r, inset, inset, tw, td, podium, floors, ROOF_FLAT, sp);
	else if (kind == 3) {
		float leg = snap(std::max(K.tower_min, tw * mixr(r, K.ell_lo, K.ell_hi)));
		bool left = r.f() < HALF;
		float front_d = snap(std::max(K.tower_min, td * HALF));
		block(inset, inset, tw, front_d, podium, floors, ROOF_FLAT, false);
		block(left ? inset : inset + tw - leg, inset + front_d, leg, snap(std::max(K.leg_min, td * HALF)), podium, std::max(4, floors - r.irange(0, 4)), ROOF_FLAT, false);
	} else if (kind == 4) {
		float gap = snap(mixr(r, K.gap_lo, K.gap_hi)), each = snap((tw - gap) / 2);
		int other = std::max(K.twin_min, floors + choose(r, K.twin_steps));
		block(inset, inset, each, td, podium, floors, ROOF_FLAT, true);
		block(inset + each + gap, inset, each, td, podium, other, ROOF_FLAT, true);
	} else if (kind == 2) {
		int top = r.irange(1, 3);
		block(inset, inset, tw, td, podium, floors - top, ROOF_TERRACE, true);
		float cin = snap(mixr(r, K.cornice_in_lo, K.crown_in_hi));
		block(inset + cin, inset + cin, std::max(K.crown_min, tw - 2 * cin), std::max(K.crown_min, td - 2 * cin), podium + floors - top, top, ROOF_FLAT, true);
	} else block(inset, inset, tw, td, podium, floors, ROOF_FLAT, true);
}

bool medium_flats(const Data &data, Body &b, Xoshiro128 &r, float w, float d, int floors, int cap, Roof roof, float bar_d) {
	const TallShape &K = data.shapes.tall;
	if (d - bar_d >= K.u_room && w >= K.u_w && r.f() < HALF) {
		float ww = snap(mixr(r, K.u_wing_lo, std::min(K.u_wing_max, w / 3)));
		int wf = std::max(2, floors - r.irange(0, 2));
		add_block(b, { 0, 0, w, bar_d, 0, floors, roof });
		add_block(b, { 0, bar_d, ww, d - bar_d, 0, wf, roof });
		add_block(b, { w - ww, bar_d, ww, d - bar_d, 0, wf, roof });
	} else if (d - bar_d >= K.u_room && w >= K.l_w && r.f() < K.l_share) {
		float ww = snap(mixr(r, K.l_wing_lo, std::min(K.l_wing_max, w / 2)));
		bool left = r.f() < HALF;
		add_block(b, { 0, 0, w, bar_d, 0, floors, roof });
		int wf = r.f() < K.l_low ? std::max(2, int(std::round(float(floors) * K.l_low_share))) : floors;
		add_block(b, { left ? 0 : w - ww, bar_d, ww, d - bar_d, 0, wf, roof });
	} else if (w >= K.pair_w && r.f() < K.pair_share) {
		float split = snap(w * mixr(r, K.pair_lo, K.pair_hi)), back = snap(mixr(r, K.pair_back_lo, K.pair_back_hi));
		add_block(b, { 0, 0, split, d, 0, floors, roof });
		int other = std::max(2, floors + choose(r, { -2, -1, 1, 2 }));
		add_block(b, { split, back, w - split, d - back, 0, cap > 0 ? std::min(other, cap) : other, roof });
		return true;
	} else {
		int lower = floors >= K.bar_step_floors && r.f() < HALF ? floors - r.irange(1, 2) : floors;
		add_block(b, { 0, 0, w, d, 0, lower, lower < floors ? ROOF_TERRACE : roof });
		if (lower < floors && d >= K.bar_step_d) add_block(b, { 0, K.bar_step, w, d - K.bar_step, lower, floors - lower, roof });
	}
	return false;
}
} // namespace

void flats(const Data &data, Body &b, const Envelope &env, Xoshiro128 &r, bool high) {
	const TallShape &K = data.shapes.tall;
	b.style = uint8_t(style_for(data, high ? KIND_TOWER : KIND_FLATS, r, env.character, env.era, !high));
	Paint wall, trim;
	paints_of(data, b.style, r, wall, trim);
	b.palette = r.irange(0, data.last_palette());
	b.ground_h = mixr(r, K.flats_ground_lo, K.flats_ground_hi);
	b.storey_h = mixr(r, K.flats_storey_lo, K.flats_storey_hi);
	look(b, wall, roof_of(data, r, clay_of(data)), trim);
	int floors = high ? (r.f() < K.tall_share ? r.irange(K.min_tall, K.max_tall) : r.irange(K.min_tower, K.max_tower)) : r.irange(K.apartment_min, K.max_flats);
	if (!high && env.cap > 0) floors = std::min(std::max(K.apartment_min, env.cap), floors);
	b.ground = r.f() < K.shop_ground ? GROUND_SHOP : GROUND_LOBBY;
	bool hip = !high && (b.style == STYLE_COLONIAL || b.style == STYLE_BRICK) && r.f() < K.hip_share;
	Roof roof = hip ? ROOF_HIP : r.f() < K.terrace_share ? ROOF_TERRACE : ROOF_FLAT;
	float w = snap(env.W), d = snap(env.D);
	float bar_d = snap(std::min(d, mixr(r, K.bar_lo, K.bar_hi)));
	bool pair = false;
	if (high) tower(data, b, r, w, d, floors);
	else pair = medium_flats(data, b, r, w, d, floors, env.cap, roof, bar_d);
	b.cores.push_back(Core{ snap(w / 2 - CORE_HALF), snap(std::min(d, K.flats_core_d) / 2 - CORE_HALF), floors + (high ? K.lift_extra : 0) });
	if (pair && r.f() < K.pair_skin) b.volumes[1].wall = b.add_paint(other_skin(data, b.style, r, wall));
	for (Volume &v : b.volumes) roof_shape(data, v, r);
	finish_body(data, b, env, r, BAY_DOUBLE_DOOR, -1);
}

void shop(const Data &data, Body &b, const Envelope &env, Xoshiro128 &r) {
	const TallShape &K = data.shapes.tall;
	r.irange(0, SHOP_FUNCTIONS - 1); // the function name (shop, bakery, ...): drawn for the stream, not drawn differently
	b.style = uint8_t(style_for(data, KIND_SHOP, r, env.character, env.era, true));
	Paint wall, trim;
	paints_of(data, b.style, r, wall, trim);
	b.palette = r.irange(0, data.last_palette());
	b.ground_h = mixr(r, K.shop_ground_lo, K.shop_ground_hi);
	b.storey_h = K.shop_storey;
	look(b, wall, roof_of(data, r, clay_of(data)), trim);
	b.ground = GROUND_SHOP;
	int storeys = env.low ? r.irange(1, K.shop_low_max) : r.irange(2, K.shop_max);
	if (!env.low && env.cap > 0) storeys = std::min(std::max(2, env.cap), storeys);
	Roof roof = storeys == 1 && b.style == STYLE_COLONIAL && r.f() < K.shop_pitch ? choose(r, { ROOF_GABLE, ROOF_HIP })
			: r.f() < K.shop_terrace ? ROOF_TERRACE : ROOF_FLAT;
	float w = snap(env.W), d = snap(env.D);
	float main_d = d > K.shop_deep ? snap(mixr(r, K.shop_main_lo, std::min(K.shop_deep, d - K.shop_back))) : d;
	bool pair = w >= K.shop_pair_w && storeys >= 2 && r.f() < K.shop_pair;
	if (pair) {
		float split = snap(w * mixr(r, K.pair_lo, K.pair_hi));
		add_block(b, { 0, 0, split, main_d, 0, storeys, roof });
		int other = std::max(1, storeys + choose(r, { -1, 1 }));
		add_block(b, { split, 0, w - split, main_d, 0, env.cap > 0 ? std::min(other, env.cap) : other, roof });
	} else add_block(b, { 0, 0, w, main_d, 0, storeys, roof });
	if (d - main_d >= K.store_min) {
		int store = add_block(b, { 0, main_d, w, d - main_d, 0, 1, ROOF_FLAT, BAY_WALL, BAY_WALL });
		Volume &sv = b.volumes[size_t(store)];
		sv.keep = true; sv.lines = LINES_NONE; sv.crown = CROWN_NONE;
	}
	if (pair) b.volumes[1].wall = b.add_paint(other_skin(data, b.style, r, wall));
	for (Volume &v : b.volumes) roof_shape(data, v, r);
	uint8_t door = r.f() < K.shop_double ? BAY_DOUBLE_DOOR : BAY_DOOR;
	finish_body(data, b, env, r, door, -1);
	if (storeys > 1 && w >= K.shop_side_door_w) {
		Volume &m = b.volumes[0];
		int across = bays_of(m, 0), end = r.f() < HALF ? 0 : across - 1;
		auto it = m.bays.find(bay_key(0, 0, end));
		if (it == m.bays.end() || (it->second != BAY_DOUBLE_DOOR && it->second != BAY_DOOR)) m.bays[bay_key(0, 0, end)] = BAY_DOOR;
	}
	if ((b.style == STYLE_COLONIAL || b.style == STYLE_BRICK) && r.f() < K.awning_share) {
		LotElement e;
		e.kind = EL_AWNING; e.x = w / 2; e.y = K.awning_y; e.w = std::max(K.awning_min, w - 1); e.d = mixr(r, K.awning_d_lo, K.awning_d_hi);
		e.z = K.awning_z; e.h = K.awning_h; e.paint = b.add_paint(Paint{ FIN_METAL, choose(r, data.awnings) });
		b.elements.push_back(e);
	}
}

void office(const Data &data, Body &b, const Envelope &env, Xoshiro128 &r) {
	const TallShape &K = data.shapes.tall;
	b.style = uint8_t(style_for(data, KIND_OFFICE, r, env.character, env.era, false));
	Paint wall, trim;
	paints_of(data, b.style, r, wall, trim);
	b.palette = r.irange(0, data.last_palette());
	b.ground_h = mixr(r, K.office_ground_lo, K.office_ground_hi);
	b.storey_h = mixr(r, K.office_storey_lo, K.office_storey_hi);
	look(b, wall, data.color("office_roof"), trim);
	b.ground = GROUND_LOBBY;
	int floors = r.irange(K.workplace_min_f, K.office_max_f);
	float w = snap(env.W), d = snap(env.D);
	if (r.f() < K.podium_share && w >= K.podium_min && d >= K.podium_min) {
		int podium = r.irange(1, 3);
		float inset = snap(mixr(r, K.facade_setback_lo, K.inset_hi));
		float tw = w - 2 * inset, td = std::max(K.tower_min, std::min(d - 2 * inset, K.office_max_d));
		add_block(b, { 0, 0, w, std::min(d, td + 2 * inset), 0, podium, ROOF_TERRACE });
		int shaft = floors - podium;
		int form = b.style == STYLE_ART_DECO ? 0 : tw >= K.twin_tower_w ? choose(r, { 0, 1, 2, 3 }) : choose(r, { 0, 1, 3 });
		if (form == 0) setbacks(data, b, r, inset, inset, tw, td, podium, shaft, ROOF_FLAT, nullptr);
		else if (form == 2) {
			float gap = snap(mixr(r, K.gap_lo, K.gap_hi)), each = snap((tw - gap) / 2);
			add_block(b, { inset, inset, each, td, podium, shaft, ROOF_FLAT });
			add_block(b, { inset + each + gap, inset, each, td, podium, std::max(K.office_twin_min, shaft + choose(r, K.office_twin_steps)), ROOF_FLAT });
		} else if (form == 3) {
			int top = r.irange(2, 3);
			float cin = snap(mixr(r, K.office_crown_lo, K.office_crown_hi));
			add_block(b, { inset, inset, tw, td, podium, shaft - top, ROOF_TERRACE });
			add_block(b, { inset + cin, inset + cin, std::max(K.crown_min, tw - 2 * cin), std::max(K.crown_min, td - 2 * cin), podium + shaft - top, top, ROOF_FLAT });
		} else add_block(b, { inset, inset, tw, td, podium, shaft, ROOF_FLAT });
	} else add_block(b, { 0, 0, w, d, 0, floors, ROOF_FLAT });
	b.cores.push_back(Core{ snap(w / 2 - CORE_HALF), snap(std::min(d, K.office_core_d) / 2 - CORE_HALF), floors });
	finish_body(data, b, env, r, BAY_DOUBLE_DOOR, -1);
}

} // namespace city::b40
