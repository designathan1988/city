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
constexpr float AC_GAP = 0.08f, AC_UNDER_SILL = 0.12f, AC_ROOM = 0.2f, AC_EDGE = 0.15f, MID = 0.5f, QUARTER_BACK = 0.75f;
constexpr float DISH_ROOM = 0.75f, DISH_OFF = 0.45f, DISH_DROP = 0.25f;
constexpr float POT_HIGH = 0.32f, SOIL_DOWN = 0.03f, POT_MIN = 0.9f, POT_SPAN = 0.7f, POT_TALL = 0.9f, POT_TALL_SPAN = 0.4f, LEAF_WIDE = 0.7f, LEAF_WIDE_SPAN = 0.5f;
constexpr float LEAF_HIGH = 0.7f, LEAF_HIGH_SPAN = 0.6f;
constexpr float FLOOR_LIFT = 0.002f, PLANT_LIFT = 0.003f, CORNER_IN = 0.25f, RAIL_IN = 0.3f, MID_DRIFT = 0.3f, CHAIR_TURN = 0.8f, HALF_DEEP = 0.5f;
constexpr float AIRER_MIN = 1.3f, TABLE_DEEP = 0.55f, TABLE_BIG = 1.15f, TABLE_SMALL = 0.65f, TABLE_TALL = 0.75f, TABLE_SIDE = 0.34f, TABLE_CHAIR = 0.32f, TABLE_EDGE = 0.2f;
constexpr float GOURMET_LOUNGER_NARROW = 1.8f, BIKE_OFF = 0.22f, CHAIR_BACK_DEEP = 0.35f, GARDEN_END = 0.5f;
constexpr float LEDGE_IN = 0.3f, GOURMET_IN = 0.45f, LEDGE_DEEP = 1.3f, GOURMET_DEEP = 1.6f, LEDGE_MIN = 1.4f;
constexpr float INSET_IN = 0.3f, INSET_MIN = 1.f, INSET_CORNER = 0.1f, INSET_PLANT = 0.35f, INSET_CHAIR = 0.7f, INSET_TURN = 0.5f;
constexpr float FULL_TURN = PI_F;
constexpr int GARDEN_FEW = 3, GARDEN_MANY = 4;
enum Standard { STANDARD_HIGH, STANDARD_MIDDLE, STANDARD_POPULAR, STANDARD_OFFICE };

struct Spot {
	float a = 0, h = 0, out = 0, turn = 0, sx = 1, sy = 1, sz = 1;
};

class Furnish {
public:
	Furnish(Shell &parts, const BayCtx &c, const BayPropLook &k, float old) : parts_(parts), c_(c), k_(k), s_(k.share), old_(old), r_(c.hash) {}
	void window(const Opening &o);
	void balcony();
	void loggia(const Opening &o);

private:
	float rnd() { return float(r_.next()); }
	uint32_t pick(const std::vector<uint32_t> &list) { return list[size_t(int(rnd() * float(list.size())) % int(list.size()))]; }
	void put(BayProp kind, const Spot &at, uint32_t colour);
	void plant(float a, float out, float h);
	void chair(float a, float out, float turn, uint32_t colour) { put(BAY_PROP_CHAIR, Spot{ a, FLOOR_LIFT, out, turn }, colour); }
	void airer(float a0, float a1, float deep);
	void table(bool gourmet, float a0, float a1, float deep, float mid, uint32_t colour);
	void one_thing(Standard st, float a0, float a1, float deep, float mid, uint32_t colour);
	float corner(float a0, float a1) { return rnd() < MID ? a0 + CORNER_IN : a1 - CORNER_IN; }
	void its_kind(Standard st, float a0, float a1, float deep, float mid, uint32_t colour);
	Standard standard() const;

	Shell &parts_;
	const BayCtx &c_;
	const BayPropLook &k_;
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
	const float s = POT_MIN + rnd() * POT_SPAN, tall = s * (POT_TALL + rnd() * POT_TALL_SPAN);
	put(BAY_PROP_POT, Spot{ a, h, out, 0, s, tall, s }, pick(k_.pots));
	const float lift = (POT_HIGH - SOIL_DOWN) * tall, spread = s * (LEAF_WIDE + rnd() * LEAF_WIDE_SPAN);
	const BayProp kind = rnd() < MID ? BAY_PROP_FOLIAGE : BAY_PROP_BUSH;
	const float turn = rnd() * TAU_F, high = s * (LEAF_HIGH + rnd() * LEAF_HIGH_SPAN);
	put(kind, Spot{ a, h + lift, out, turn, spread, high, spread }, pick(k_.greens));
}

void Furnish::airer(float a0, float a1, float deep) {
	if (a1 - a0 > AIRER_MIN) put(BAY_PROP_AIRER, Spot{ (a0 + a1) / 2, FLOOR_LIFT, deep * HALF_DEEP, rnd() < MID ? 0 : FULL_TURN }, k_.airer);
}

void Furnish::table(bool gourmet, float a0, float a1, float deep, float mid, uint32_t colour) {
	const float big = gourmet ? TABLE_BIG : TABLE_SMALL;
	put(BAY_PROP_TABLE, Spot{ mid, FLOOR_LIFT, deep * TABLE_DEEP, 0, big, gourmet ? 1.f : TABLE_TALL, big }, colour);
	for (int side : { -1, 1 }) {
		const float at = mid + float(side) * (big * TABLE_SIDE + TABLE_CHAIR);
		if (at > a0 + TABLE_EDGE && at < a1 - TABLE_EDGE) chair(at, deep * TABLE_DEEP, float(side) * -PI_F / 2, colour);
	}
}

void Furnish::one_thing(Standard st, float a0, float a1, float deep, float mid, uint32_t colour) {
	const float t = rnd();
	if (t < s_.one_chair) chair(mid, deep * HALF_DEEP, (rnd() - MID) * CHAIR_TURN, colour);
	else if (t < s_.one_plant) plant(corner(a0, a1), deep - RAIL_IN, PLANT_LIFT);
	else if (st == STANDARD_POPULAR || t < s_.one_airer) airer(a0, a1, deep);
	else put(BAY_PROP_LOUNGER, Spot{ mid, FLOOR_LIFT, deep * HALF_DEEP }, pick(k_.loungers));
}

void Furnish::its_kind(Standard st, float a0, float a1, float deep, float mid, uint32_t colour) {
	const float t = rnd();
	if (st == STANDARD_HIGH) {
		if (t >= s_.lounger_high || a1 - a0 <= GOURMET_LOUNGER_NARROW) put(BAY_PROP_LOUNGER, Spot{ mid, FLOOR_LIFT, deep * HALF_DEEP }, pick(k_.loungers));
	} else if (t >= s_.kind_airer) {
		airer(a0, a1, deep);
		if (st == STANDARD_POPULAR) {
			const float at = rnd() < MID ? a0 + RAIL_IN : a1 - RAIL_IN;
			chair(at, deep * CHAIR_BACK_DEEP, (rnd() - MID) * CHAIR_TURN, colour);
		}
	} else if (t >= s_.kind_bike) {
		put(BAY_PROP_BIKE, Spot{ (a0 + a1) / 2, FLOOR_LIFT, BIKE_OFF }, pick(k_.bikes));
	}
}

Standard Furnish::standard() const {
	if (c_.use != USE_RESIDENTIAL) return STANDARD_OFFICE;
	if (c_.comp == BAY_GOURMET) return STANDARD_HIGH;
	return old_ > s_.old_popular ? STANDARD_POPULAR : STANDARD_MIDDLE;
}

void Furnish::balcony() {
	const bool gourmet = c_.comp == BAY_GOURMET;
	const float a0 = gourmet ? GOURMET_IN : LEDGE_IN, a1 = c_.W - a0, deep = gourmet ? GOURMET_DEEP : LEDGE_DEEP;
	if (a1 - a0 < LEDGE_MIN) return;
	const Standard st = standard();
	const uint32_t colour = pick(st == STANDARD_POPULAR ? k_.plastic : k_.fine);
	const float mid = (a0 + a1) / 2 + (rnd() - MID) * MID_DRIFT;
	const float centre = (a0 + a1) / 2;
	const float r = rnd();
	if (st == STANDARD_OFFICE) {
		if (r < s_.office_plant) plant(corner(a0, a1), deep - RAIL_IN, PLANT_LIFT);
	} else if (r < s_.bare) {
		return;
	} else if (r < s_.one_thing) {
		one_thing(st, a0, a1, deep, mid, colour);
	} else if (r < s_.two_things) {
		if (rnd() < s_.set_table) {
			table(gourmet, a0, a1, deep, mid, colour);
		} else {
			chair(mid, deep * HALF_DEEP, (rnd() - MID) * CHAIR_TURN, colour);
			plant(mid < centre ? a1 - CORNER_IN : a0 + CORNER_IN, deep - RAIL_IN, PLANT_LIFT);
		}
	} else if (r < s_.its_kind) {
		its_kind(st, a0, a1, deep, mid, colour);
	} else {
		const int n = rnd() < s_.four_pots ? GARDEN_FEW : GARDEN_MANY;
		const float step = (a1 - a0 - GARDEN_END) / float(n - 1);
		for (int i = 0; i < n; i++) plant(a0 + CORNER_IN + float(i) * step, deep - RAIL_IN, PLANT_LIFT);
	}
}

void Furnish::loggia(const Opening &o) {
	if (c_.use != USE_RESIDENTIAL) return;
	const float a0 = o.a0 + INSET_IN, a1 = o.a1 - INSET_IN;
	if (a1 - a0 < INSET_MIN) return;
	const float r = rnd(), mid = (a0 + a1) / 2;
	if (r < s_.loggia_plant) {
		const float at = rnd() < MID ? a0 + INSET_CORNER : a1 - INSET_CORNER;
		plant(at, -INSET_PLANT, o.h0 + PLANT_LIFT);
	} else if (r < s_.loggia_chair) {
		const float turn = (rnd() - MID) * INSET_TURN;
		put(BAY_PROP_CHAIR, Spot{ mid, o.h0 + FLOOR_LIFT, -INSET_CHAIR, turn }, pick(k_.fine));
	} else if (r < s_.loggia_lounger) {
		put(BAY_PROP_LOUNGER, Spot{ mid, o.h0 + FLOOR_LIFT, -INSET_CHAIR }, pick(k_.loungers));
	}
}

// A split under the sill, or a dish beside the window at its head.
void Furnish::window(const Opening &o) {
	const float ac = c_.use == USE_RESIDENTIAL ? s_.split_home : c_.use == USE_COMMERCIAL ? s_.split_shop : s_.split_works;
	const Vector3 &box = k_.ac_case; // the condenser's width, height and depth
	if (o.h0 - AC_UNDER_SILL - box.y > AC_ROOM && rnd() < ac) {
		const float side = rnd();
		const float a = side < MID ? (o.a0 + o.a1) / 2 : side < QUARTER_BACK ? o.a0 + box.x / 2 : o.a1 - box.x / 2;
		const float lo = box.x / 2 + AC_EDGE, hi = c_.W - box.x / 2 - AC_EDGE;
		if (hi > lo) {
			const uint32_t colour = rnd() < old_ * s_.ac_old ? k_.ac_old : pick(k_.ac_cases);
			put(BAY_PROP_AC, Spot{ std::clamp(a, lo, hi), o.h0 - AC_UNDER_SILL - box.y / 2, AC_GAP + box.z / 2 }, colour);
		}
	} else if (c_.use != USE_INDUSTRIAL && rnd() < s_.dish && o.a1 + DISH_ROOM < c_.W) {
		put(BAY_PROP_DISH, Spot{ o.a1 + DISH_OFF, o.h1 - DISH_DROP, 0 }, k_.dish);
	}
}
} // namespace

void emit_bay_props(Shell &parts, const Placed &pl, const BayCtx &c, const Opening &o) {
	Furnish f(parts, c, pl.look->bay, 0);
	switch (c.comp) {
		case BAY_WINDOW: case BAY_SASH: case BAY_SHUTTERED: case BAY_RIBBON: f.window(o); break;
		case BAY_BALCONY: case BAY_GOURMET: f.balcony(); break;
		case BAY_LOGGIA: f.loggia(o); break;
		default: break;
	}
}

} // namespace city::b40
