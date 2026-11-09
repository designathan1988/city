// What hangs on a bay and stands on its balcony: the life of a block of flats. One draw per bay from a stream seeded by the bay's hash
// (mulberry32, core/seeded.h), so a bay always holds the same things. Only above the ground floor. The models are bay_props.h's.
#include "render/buildings/facade_bay.h"

#include "core/math.h"
#include "core/seeded.h"
#include "render/buildings/bay_props.h"

#include <algorithm>

using godot::Vector3;

namespace city::b40 {

namespace {
constexpr float FULL_TURN = PI_F;
enum Standard { STANDARD_HIGH, STANDARD_MIDDLE, STANDARD_POPULAR, STANDARD_OFFICE };

struct Spot {
	float a = 0, h = 0, out = 0, turn = 0, sx = 1, sy = 1, sz = 1;
};

class Furnish {
public:
	Furnish(Shell &parts, const BayCtx &c, const BayPropLook &k, const FacadePropsLook &look, float old)
			: parts_(parts), c_(c), k_(k), K(look), s_(k.share), old_(old), r_(c.hash) {}
	void window(const Opening &o);
	void balcony();
	void loggia(const Opening &o);

private:
	float rnd() { return float(r_.next()); }
	uint32_t pick(const std::vector<uint32_t> &list) { return list[size_t(int(rnd() * float(list.size())) % int(list.size()))]; }
	void put(BayProp kind, const Spot &at, uint32_t colour);
	void plant(float a, float out, float h);
	void chair(float a, float out, float turn, uint32_t colour) { put(BAY_PROP_CHAIR, Spot{ a, K.floor_lift, out, turn }, colour); }
	void airer(float a0, float a1, float deep);
	void table(bool gourmet, float a0, float a1, float deep, float mid, uint32_t colour);
	void one_thing(Standard st, float a0, float a1, float deep, float mid, uint32_t colour);
	float corner(float a0, float a1) { return rnd() < K.mid ? a0 + K.corner_in : a1 - K.corner_in; }
	void its_kind(Standard st, float a0, float a1, float deep, float mid, uint32_t colour);
	Standard standard() const;

	Shell &parts_;
	const BayCtx &c_;
	const BayPropLook &k_;
	const FacadePropsLook &K; // its sizes (building_look.json shapes.facade_props)
	const BayPropShares &s_;
	float old_;
	Mulberry32 r_;
};

// Upright, facing out of the wall, turned about the vertical: never mirrored, whichever way the facade's frame runs.
void Furnish::put(BayProp kind, const Spot &at, uint32_t colour) {
	const Vector3 y = c_.f.e1, z = c_.f.e2, x = y.cross(z);
	const float cs = std::cos(at.turn), sn = std::sin(at.turn);
	const Vector3 tx = (x * cs - z * sn) * at.sx, ty = y * at.sy, tz = (x * sn + z * cs) * at.sz;
	const Vector3 o = c_.f.at(at.a, at.h, at.out);
	PropInstance p;
	const Vector3 axes[3] = { tx, ty, tz };
	for (int i = 0; i < 3; i++) {
		p.basis[i * 3] = axes[i].x;
		p.basis[i * 3 + 1] = axes[i].y;
		p.basis[i * 3 + 2] = axes[i].z;
	}
	p.origin[0] = o.x;
	p.origin[1] = o.y;
	p.origin[2] = o.z;
	p.color = colour;
	p.kind = kind;
	parts_.props.push_back(p);
}

// A pot of 30 to 55 cm and its plant: a rosette or a round shrub, out of the soil under the rim.
void Furnish::plant(float a, float out, float h) {
	const float s = K.pot_min + rnd() * K.pot_span, tall = s * (K.pot_tall + rnd() * K.pot_tall_span);
	put(BAY_PROP_POT, Spot{ a, h, out, 0, s, tall, s }, pick(k_.pots));
	const float lift = (K.pot_high - K.soil_down) * tall, spread = s * (K.leaf_wide + rnd() * K.leaf_wide_span);
	const BayProp kind = rnd() < K.mid ? BAY_PROP_FOLIAGE : BAY_PROP_BUSH;
	const float turn = rnd() * TAU_F, high = s * (K.leaf_high + rnd() * K.leaf_high_span);
	put(kind, Spot{ a, h + lift, out, turn, spread, high, spread }, pick(k_.greens));
}

void Furnish::airer(float a0, float a1, float deep) {
	if (a1 - a0 > K.airer_min) put(BAY_PROP_AIRER, Spot{ (a0 + a1) / 2, K.floor_lift, deep * K.half_deep, rnd() < K.mid ? 0 : FULL_TURN }, k_.airer);
}

void Furnish::table(bool gourmet, float a0, float a1, float deep, float mid, uint32_t colour) {
	const float big = gourmet ? K.table_big : K.table_small;
	put(BAY_PROP_TABLE, Spot{ mid, K.floor_lift, deep * K.table_deep, 0, big, gourmet ? 1.f : K.table_tall, big }, colour);
	for (int side : { -1, 1 }) {
		const float at = mid + float(side) * (big * K.table_side + K.table_chair);
		if (at > a0 + K.table_edge && at < a1 - K.table_edge) chair(at, deep * K.table_deep, float(side) * -PI_F / 2, colour);
	}
}

void Furnish::one_thing(Standard st, float a0, float a1, float deep, float mid, uint32_t colour) {
	const float t = rnd();
	if (t < s_.one_chair) chair(mid, deep * K.half_deep, (rnd() - K.mid) * K.chair_turn, colour);
	else if (t < s_.one_plant) plant(corner(a0, a1), deep - K.rail_in, K.plant_lift);
	else if (st == STANDARD_POPULAR || t < s_.one_airer) airer(a0, a1, deep);
	else put(BAY_PROP_LOUNGER, Spot{ mid, K.floor_lift, deep * K.half_deep }, pick(k_.loungers));
}

void Furnish::its_kind(Standard st, float a0, float a1, float deep, float mid, uint32_t colour) {
	const float t = rnd();
	if (st == STANDARD_HIGH) {
		if (t >= s_.lounger_high || a1 - a0 <= K.gourmet_lounger_narrow) put(BAY_PROP_LOUNGER, Spot{ mid, K.floor_lift, deep * K.half_deep }, pick(k_.loungers));
	} else if (t >= s_.kind_airer) {
		airer(a0, a1, deep);
		if (st == STANDARD_POPULAR) {
			const float at = rnd() < K.mid ? a0 + K.rail_in : a1 - K.rail_in;
			chair(at, deep * K.chair_back_deep, (rnd() - K.mid) * K.chair_turn, colour);
		}
	} else if (t >= s_.kind_bike) {
		put(BAY_PROP_BIKE, Spot{ (a0 + a1) / 2, K.floor_lift, K.bike_off }, pick(k_.bikes));
	}
}

Standard Furnish::standard() const {
	if (c_.use != USE_RESIDENTIAL) return STANDARD_OFFICE;
	if (c_.comp == BAY_GOURMET) return STANDARD_HIGH;
	return old_ > s_.old_popular ? STANDARD_POPULAR : STANDARD_MIDDLE;
}

void Furnish::balcony() {
	const bool gourmet = c_.comp == BAY_GOURMET;
	const float a0 = gourmet ? K.gourmet_in : K.ledge_in, a1 = c_.W - a0, deep = gourmet ? K.gourmet_deep : K.ledge_deep;
	if (a1 - a0 < K.ledge_min) return;
	const Standard st = standard();
	const uint32_t colour = pick(st == STANDARD_POPULAR ? k_.plastic : k_.fine);
	const float mid = (a0 + a1) / 2 + (rnd() - K.mid) * K.mid_drift;
	const float centre = (a0 + a1) / 2;
	const float r = rnd();
	if (st == STANDARD_OFFICE) {
		if (r < s_.office_plant) plant(corner(a0, a1), deep - K.rail_in, K.plant_lift);
	} else if (r < s_.bare) {
		return;
	} else if (r < s_.one_thing) {
		one_thing(st, a0, a1, deep, mid, colour);
	} else if (r < s_.two_things) {
		if (rnd() < s_.set_table) {
			table(gourmet, a0, a1, deep, mid, colour);
		} else {
			chair(mid, deep * K.half_deep, (rnd() - K.mid) * K.chair_turn, colour);
			plant(mid < centre ? a1 - K.corner_in : a0 + K.corner_in, deep - K.rail_in, K.plant_lift);
		}
	} else if (r < s_.its_kind) {
		its_kind(st, a0, a1, deep, mid, colour);
	} else {
		const int n = rnd() < s_.four_pots ? K.garden_few : K.garden_many;
		const float step = (a1 - a0 - K.garden_end) / float(n - 1);
		for (int i = 0; i < n; i++) plant(a0 + K.corner_in + float(i) * step, deep - K.rail_in, K.plant_lift);
	}
}

void Furnish::loggia(const Opening &o) {
	if (c_.use != USE_RESIDENTIAL) return;
	const float a0 = o.a0 + K.inset_in, a1 = o.a1 - K.inset_in;
	if (a1 - a0 < K.inset_min) return;
	const float r = rnd(), mid = (a0 + a1) / 2;
	if (r < s_.loggia_plant) {
		const float at = rnd() < K.mid ? a0 + K.inset_corner : a1 - K.inset_corner;
		plant(at, -K.inset_plant, o.h0 + K.plant_lift);
	} else if (r < s_.loggia_chair) {
		const float turn = (rnd() - K.mid) * K.inset_turn;
		put(BAY_PROP_CHAIR, Spot{ mid, o.h0 + K.floor_lift, -K.inset_chair, turn }, pick(k_.fine));
	} else if (r < s_.loggia_lounger) {
		put(BAY_PROP_LOUNGER, Spot{ mid, o.h0 + K.floor_lift, -K.inset_chair }, pick(k_.loungers));
	}
}

// A split under the sill, or a dish beside the window at its head.
void Furnish::window(const Opening &o) {
	const float ac = c_.use == USE_RESIDENTIAL ? s_.split_home : c_.use == USE_COMMERCIAL ? s_.split_shop : s_.split_works;
	const Vector3 &box = k_.ac_case; // the condenser's width, height and depth
	if (o.h0 - K.ac_under_sill - box.y > K.ac_room && rnd() < ac) {
		const float side = rnd();
		const float a = side < K.mid ? (o.a0 + o.a1) / 2 : side < K.quarter_back ? o.a0 + box.x / 2 : o.a1 - box.x / 2;
		const float lo = box.x / 2 + K.ac_edge, hi = c_.W - box.x / 2 - K.ac_edge;
		if (hi > lo) {
			const uint32_t colour = rnd() < old_ * s_.ac_old ? k_.ac_old : pick(k_.ac_cases);
			put(BAY_PROP_AC, Spot{ std::clamp(a, lo, hi), o.h0 - K.ac_under_sill - box.y / 2, K.ac_gap + box.z / 2 }, colour);
		}
	} else if (c_.use != USE_INDUSTRIAL && rnd() < s_.dish && o.a1 + K.dish_room < c_.W) {
		put(BAY_PROP_DISH, Spot{ o.a1 + K.dish_off, o.h1 - K.dish_drop, 0 }, k_.dish);
	}
}
} // namespace

void emit_bay_props(Shell &parts, const Placed &pl, const BayCtx &c, const Opening &o) {
	Furnish f(parts, c, pl.look->bay, pl.look->shapes.facade_props, pl.wear); // a worn building's balconies hold the old popular things
	switch (c.comp) {
		case BAY_WINDOW: case BAY_SASH: case BAY_SHUTTERED: case BAY_RIBBON: f.window(o); break;
		case BAY_BALCONY: case BAY_GOURMET: f.balcony(); break;
		case BAY_LOGGIA: f.loggia(o); break;
		default: break;
	}
}

} // namespace city::b40
