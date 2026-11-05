// Facade props of the upper storeys: split AC condensers under
// windows, satellite dishes, plant pots and a chair on balconies. Random stream = mulberry32 (Tommy Ettinger's 32-bit generator) seeded by the bay's hash.
#include "render/buildings/facade_bay.h"

#include "core/seeded.h"

namespace city::b40 {

namespace {
constexpr float AC_W = 0.72f, AC_H = 0.48f, AC_D = 0.23f, AC_DROP = 0.36f, AC_OUT = -0.195f, AC_ROOM = 0.12f, AC_MIN = 0.2f;
constexpr float AC_HALF_M = 0.36f, AC_EDGE = 0.51f, HALF = 0.5f, QUARTER = 0.75f;
constexpr float DISH_OFF = 0.45f, DISH_ROOM = 0.75f, DISH_DROP = 0.25f, DISH = 0.5f, DISH_T = 0.06f;
constexpr float POT = 0.32f, POT_H = 0.32f, POT_IN = 0.25f, POT_OUT = 0.3f, PLANT = 0.45f, PLANT_H = 0.5f;
constexpr float CHAIR = 0.45f, CHAIR_SEAT = 0.44f, CHAIR_BACK = 0.85f, BALCONY_OUT = 1.3f, GOURMET_OUT = 1.6f;
constexpr float BARE = 0.45f, ONE_THING = 0.7f, CHAIR_FIRST = 0.4f;
constexpr uint32_t DISH_WHITE = 0xe8e8e4, POT_CLAY = 0xb5643c, LEAF = 0x4f7a3a, CHAIR_DARK = 0x2b2b2b, AC_DARK = 0x2d3033;

bool ac_bay(uint8_t c) { return c == BAY_WINDOW || c == BAY_SASH || c == BAY_SHUTTERED || c == BAY_RIBBON; }

void pot(Shell &s, const BayCtx &c, float a, float out) {
	part_box(s, Part{ &c.f, a, POT_H / 2, -out, POT, POT_H, POT }, lin_of(POT_CLAY), FIN_CERAMIC);
	part_box(s, Part{ &c.f, a, POT_H + PLANT_H / 2, -out, PLANT, PLANT_H, PLANT }, lin_of(LEAF), FIN_NONE);
}

void chair(Shell &s, const BayCtx &c, float a, float out) {
	part_box(s, Part{ &c.f, a, CHAIR_SEAT, -out, CHAIR, DISH_T, CHAIR }, lin_of(CHAIR_DARK), FIN_METAL);
	part_box(s, Part{ &c.f, a, (CHAIR_SEAT + CHAIR_BACK) / 2, -out - CHAIR / 2, CHAIR, CHAIR_BACK - CHAIR_SEAT, DISH_T }, lin_of(CHAIR_DARK), FIN_METAL);
}
} // namespace

void emit_bay_props(Shell &parts, const Placed &pl, const BayCtx &c, const Opening &o) {
	Mulberry32 r(c.hash);
	const Data &d = *pl.data;
	if (ac_bay(c.comp) || c.comp == BAY_BALCONY || c.comp == BAY_GOURMET || c.comp == BAY_LOGGIA) {
		float chance = d.ac_chance[c.use == USE_RESIDENTIAL ? 0 : c.use == USE_COMMERCIAL ? 1 : 2];
		if (ac_bay(c.comp) && r.next() < chance && o.h0 - AC_ROOM - AC_H > AC_MIN) {
			float t = float(r.next());
			float a = t < HALF ? c.W / 2 : t < QUARTER ? o.a0 + AC_HALF_M : o.a1 - AC_HALF_M;
			a = std::clamp(a, AC_EDGE, c.W - AC_EDGE);
			uint32_t col = d.ac_cases[size_t(r.irange_excl(0, int(d.ac_cases.size())))];
			part_ac(parts, Part{ &c.f, a, o.h0 - AC_DROP, AC_OUT, AC_W, AC_H, AC_D }, lin_of(col), lin_of(AC_DARK));
		} else if (ac_bay(c.comp) && r.next() < d.dish_chance && o.a1 + DISH_ROOM < c.W) {
			part_box(parts, Part{ &c.f, o.a1 + DISH_OFF, o.h1 - DISH_DROP, -DISH_T, DISH, DISH, DISH_T }, lin_of(DISH_WHITE), FIN_METAL);
		}
	}
	if (c.comp != BAY_BALCONY && c.comp != BAY_GOURMET) return;
	if (c.use != USE_RESIDENTIAL) return;
	float out = c.comp == BAY_GOURMET ? GOURMET_OUT : BALCONY_OUT;
	float t = float(r.next());
	if (t < BARE) return;
	float a0 = POT_IN + (c.comp == BAY_GOURMET ? POT_IN : 0), a1 = c.W - a0;
	if (t < ONE_THING) {
		if (r.next() < CHAIR_FIRST) chair(parts, c, c.W / 2, out * HALF);
		else pot(parts, c, a0 + POT_IN, out - POT_OUT);
		return;
	}
	pot(parts, c, a0 + POT_IN, out - POT_OUT);
	pot(parts, c, a1 - POT_IN, out - POT_OUT);
	chair(parts, c, c.W / 2, out * HALF);
}

} // namespace city::b40
