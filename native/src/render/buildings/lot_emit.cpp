// The lot elements of a placed body: surfaces laid
// on a 3 m grid following the ground (lawns drawn as grass by the shader), boundary walls with plinth and coping, fences
// and railings on their base walls, hedges, gates with posts and bars, planters, benches, bollards, lamps (lit heads),
// bins, stall lines, wall ACs, and the building-hung canopies, awnings, veranda floors, slabs and posts.
#include "render/buildings/facade.h"

#include <cmath>

using namespace godot;

namespace city::b40 {

namespace {
constexpr float LOT_RAISE = 0.03f, FOOT = 0.3f, PIECE = 4.f, COPING_H = 0.07f, COPING_OUT = 0.04f, COPING_SHADE = 1.f;
constexpr float SPLASH = 0.22f, SPLASH_SHADE = 0.78f, RAIL_T = 0.05f, RAIL_LOW = 0.45f, POST = 0.12f, POST_EVERY = 2.f;
constexpr float LIGHT_POST = 0.06f, LIGHT_EVERY = 0.45f, BAR_EVERY = 0.14f, GATE_POST = 0.17f, GATE_RAIL = 0.08f;
constexpr float LAMP_HEAD = 0.44f, LAMP_HEAD_H = 0.12f, LAMP_POST = 0.12f, BOLLARD_BAND = 0.25f, BOLLARD_BAND_H = 0.1f;
constexpr float SOIL_IN = 0.06f, BENCH_SEAT = 0.4f, BENCH_SEAT_T = 0.06f, BENCH_BACK = 0.95f, BENCH_BACK_T = 0.06f;
constexpr float FASCIA_SHADE = 0.95f, FASCIA_H = 0.2f, AWN_SHADE = 0.8f, HALF = 0.5f, GRILLE_T = 0.02f;
constexpr uint32_t BAND_LIGHT = 0xffe8c8, LAMP_WARM = 0xffefcf;

struct LotEmit {
	Shell &shell, &parts;
	const Placed &pl;
	float floor_y;
	float ground(float x, float y) const { return pl.ground(pl.lot.at(x, y, 0)) - floor_y; }
	Lin paint(const LotElement &e, const char *fallback) const {
		return e.paint >= 0 ? lin_of(pl.b->paints[size_t(e.paint)].rgb) : paint_lin(pl.data->color(fallback));
	}
	int8_t fin(const LotElement &e) const { return e.paint >= 0 ? pl.b->paints[size_t(e.paint)].finish : FIN_CONCRETE; }
	void surface(const LotElement &e);
	void wall(const LotElement &e, float x0, float x1, float y0, float y1);
	void fence(const LotElement &e, float x0, float x1, float y0, float y1, bool light);
	void gate(const LotElement &e);
	void prop(const LotElement &e);
	void hung(const LotElement &e);
	void element(const LotElement &e);
};

void LotEmit::surface(const LotElement &e) {
	bool lawn = e.paint >= 0 && pl.b->paints[size_t(e.paint)].finish == FIN_NONE;
	Lin lin = paint(e, "paving");
	float x0 = e.x - e.w / 2, x1 = e.x + e.w / 2, y0 = e.y - e.d / 2, y1 = e.y + e.d / 2;
	float lift = LOT_RAISE + e.h - HALF * e.h; // one quad per element: the ground layers do the tiling
	shell.quad(pl.lot.at(x0, y0, ground(x0, y0) + lift), pl.lot.at(x1, y0, ground(x1, y0) + lift), pl.lot.at(x1, y1, ground(x1, y1) + lift),
			pl.lot.at(x0, y1, ground(x0, y1) + lift), pl.lot.e2, lin, fin(e), lawn ? SK_LAWN : SK_SHELL);
}

void LotEmit::wall(const LotElement &e, float x0, float x1, float y0, float y1) {
	float g = ground((x0 + x1) / 2, (y0 + y1) / 2);
	float lo = std::min(ground(x0, y0), ground(x1, y1)) - FOOT, top = g + e.h;
	Lin lin = paint(e, "concrete");
	int8_t f = fin(e);
	shell.grime_floor = floor_y + g;
	if (pl.coarse) {
		shell.box(pl.lot, x0, x1, y0, y1, lo, top, lin, f, SKIP_E2_NEG);
		return;
	}
	shell.box(pl.lot, x0, x1, y0, y1, g + SPLASH, top - COPING_H, lin, f, SKIP_E2_NEG | SKIP_E2_POS);
	float o = COPING_OUT;
	shell.box(pl.lot, x0 - (x1 - x0 < y1 - y0 ? o : 0), x1 + (x1 - x0 < y1 - y0 ? o : 0), y0 - (x1 - x0 >= y1 - y0 ? o : 0), y1 + (x1 - x0 >= y1 - y0 ? o : 0),
			lo, g + SPLASH, lin * SPLASH_SHADE, f, SKIP_E2_NEG);
	shell.box(pl.lot, x0 - (x1 - x0 < y1 - y0 ? o : 0), x1 + (x1 - x0 < y1 - y0 ? o : 0), y0 - (x1 - x0 >= y1 - y0 ? o : 0), y1 + (x1 - x0 >= y1 - y0 ? o : 0),
			top - COPING_H, top, paint_lin(pl.data->color("wall_coping")) * COPING_SHADE, FIN_STONE, SKIP_E2_NEG);
}

void LotEmit::fence(const LotElement &e, float x0, float x1, float y0, float y1, bool light) {
	float g = ground((x0 + x1) / 2, (y0 + y1) / 2), base = g + e.base;
	if (e.base > 0) {
		LotElement w = e;
		w.h = e.base;
		w.paint = -1;
		wall(w, x0, x1, y0, y1);
	}
	Lin m = paint(e, "fence");
	bool along_x = x1 - x0 >= y1 - y0;
	float len = along_x ? x1 - x0 : y1 - y0, top = g + e.h;
	float cx = (x0 + x1) / 2, cy = (y0 + y1) / 2;
	auto bar = [&](float a0, float a1, float z0, float z1, float t) {
		if (along_x) parts.box(pl.lot, a0, a1, cy - t / 2, cy + t / 2, z0, z1, m, FIN_METAL);
		else parts.box(pl.lot, cx - t / 2, cx + t / 2, a0, a1, z0, z1, m, FIN_METAL);
	};
	float s0 = along_x ? x0 : y0;
	bar(s0, s0 + len, top - COPING_H, top, RAIL_T);
	bar(s0, s0 + len, base + (top - base) * RAIL_LOW, base + (top - base) * RAIL_LOW + RAIL_T, RAIL_T);
	float every = light ? LIGHT_EVERY : POST_EVERY, pw = light ? LIGHT_POST : POST;
	for (float a = s0; a <= s0 + len + EPS; a += every) bar(std::min(a, s0 + len - pw), std::min(a, s0 + len - pw) + pw, base, top, pw);
	if (!light) {
		// One panel instead of a box every BAR_EVERY metres: a single instance the finish map tiles along the length.
		float lo = base + (top - base) * RAIL_LOW, hi = top - COPING_H;
		if (along_x) parts.box(pl.lot, x0, x1, cy - pl.look->fence_bar_m / 2, cy + pl.look->fence_bar_m / 2, lo, hi, m, FIN_METAL);
		else parts.box(pl.lot, cx - pl.look->fence_bar_m / 2, cx + pl.look->fence_bar_m / 2, y0, y1, lo, hi, m, FIN_METAL);
	}
}

void LotEmit::gate(const LotElement &e) {
	float x0 = e.x - e.w / 2, x1 = e.x + e.w / 2, g = ground(e.x, e.y);
	Lin m = paint(e, "gate");
	for (float x : { x0 - GATE_POST, x1 }) parts.box(pl.lot, x, x + GATE_POST, e.y - GATE_POST / 2, e.y + GATE_POST / 2, g - FOOT, g + e.h + GATE_RAIL, m, FIN_METAL);
	parts.box(pl.lot, x0, x1, e.y - pl.look->fence_bar_m / 2, e.y + pl.look->fence_bar_m / 2, g + e.h - GATE_RAIL, g + e.h, m, FIN_METAL);
	parts.box(pl.lot, x0, x1, e.y - pl.look->fence_bar_m / 2, e.y + pl.look->fence_bar_m / 2, g + GATE_RAIL, g + 2 * GATE_RAIL, m, FIN_METAL);
	for (float x = x0 + BAR_EVERY; x < x1; x += BAR_EVERY) parts.box(pl.lot, x, x + pl.look->fence_bar_m, e.y - pl.look->fence_bar_m / 2, e.y + pl.look->fence_bar_m / 2, g + GATE_RAIL, g + e.h, m, FIN_METAL);
}

void LotEmit::prop(const LotElement &e) {
	float x0 = e.x - e.w / 2, x1 = e.x + e.w / 2, y0 = e.y - e.d / 2, y1 = e.y + e.d / 2, g = ground(e.x, e.y);
	Lin lin = paint(e, "concrete");
	int8_t f = fin(e);
	switch (e.kind) {
		case EL_PLANTER:
			shell.box(pl.lot, x0, x1, y0, y1, g - FOOT, g + e.h, lin, f, SKIP_E2_NEG | SKIP_E2_POS);
			shell.rect(pl.lot, x0 + SOIL_IN, x1 - SOIL_IN, y0 + SOIL_IN, y1 - SOIL_IN, g + e.h - SOIL_IN, paint_lin(pl.data->color("planter_soil")), FIN_NONE);
			break;
		case EL_BENCH:
			parts.box(pl.lot, x0, x1, y0, y1, g + BENCH_SEAT, g + BENCH_SEAT + BENCH_SEAT_T, lin, FIN_WOOD);
			parts.box(pl.lot, x0, x1, y1 - BENCH_BACK_T, y1, g + BENCH_SEAT, g + BENCH_BACK, lin, FIN_WOOD);
			parts.box(pl.lot, x0, x0 + BENCH_BACK_T, y0, y1, g, g + BENCH_SEAT, lin, FIN_WOOD);
			parts.box(pl.lot, x1 - BENCH_BACK_T, x1, y0, y1, g, g + BENCH_SEAT, lin, FIN_WOOD);
			break;
		case EL_BOLLARD:
			parts.box(pl.lot, x0, x1, y0, y1, g, g + e.h, lin, FIN_METAL);
			parts.box(pl.lot, x0, x1, y0, y1, g + e.h - BOLLARD_BAND, g + e.h - BOLLARD_BAND + BOLLARD_BAND_H, lin_of(BAND_LIGHT), FIN_NONE);
			break;
		case EL_LAMP: {
			if (e.z > 0) { // on a wall: the lamp's head and its lit glass under it
				shell.box(pl.lot, x0, x1, y0, y1, e.z, e.z + e.h, lin, FIN_METAL);
				shell.quad(pl.lot.at(x0, y0, e.z - GRILLE_T), pl.lot.at(x1, y0, e.z - GRILLE_T), pl.lot.at(x1, y1, e.z - GRILLE_T), pl.lot.at(x0, y1, e.z - GRILLE_T),
						-pl.lot.e2, lin_of(LAMP_WARM), FIN_NONE, SK_LAMP);
				break;
			}
			parts.box(pl.lot, e.x - LAMP_POST / 2, e.x + LAMP_POST / 2, e.y - LAMP_POST / 2, e.y + LAMP_POST / 2, g, g + e.h, lin, FIN_METAL);
			shell.box(pl.lot, e.x - LAMP_HEAD / 2, e.x + LAMP_HEAD / 2, e.y - LAMP_HEAD / 2, e.y + LAMP_HEAD / 2, g + e.h, g + e.h + LAMP_HEAD_H, lin, FIN_METAL);
			Basis down{ pl.lot.o, pl.lot.e0, pl.lot.e1, -pl.lot.e2 };
			shell.quad(pl.lot.at(e.x - LAMP_HEAD / 3, e.y - LAMP_HEAD / 3, g + e.h - GRILLE_T), pl.lot.at(e.x + LAMP_HEAD / 3, e.y - LAMP_HEAD / 3, g + e.h - GRILLE_T),
					pl.lot.at(e.x + LAMP_HEAD / 3, e.y + LAMP_HEAD / 3, g + e.h - GRILLE_T), pl.lot.at(e.x - LAMP_HEAD / 3, e.y + LAMP_HEAD / 3, g + e.h - GRILLE_T),
					-pl.lot.e2, lin_of(LAMP_WARM), FIN_NONE, SK_LAMP);
			(void)down;
			break;
		}
		case EL_AC:
			shell.box(pl.lot, x0, x1, y0, y1, e.z, e.z + e.h, lin, FIN_METAL, SKIP_E1_NEG);
			break;
		default:
			if (!emit_lot_prop(shell, parts, pl, e, g, lin)) shell.box(pl.lot, x0, x1, y0, y1, g, g + e.h, lin, f, SKIP_E2_NEG);
			break;
	}
}

// Elements hung on the building, at heights over its floor.
void LotEmit::hung(const LotElement &e) {
	float x0 = e.x - e.w / 2, x1 = e.x + e.w / 2, y0 = e.y - e.d / 2, y1 = e.y + e.d / 2;
	Lin lin = paint(e, "canopy");
	int8_t f = fin(e);
	switch (e.kind) {
		case EL_CANOPY:
			shell.box(pl.lot, x0, x1, y0, y1, e.z, e.z + e.h, lin, f);
			shell.box(pl.lot, x0, x1, y0, y0 + RAIL_T, e.z - FASCIA_H + e.h, e.z, lin * FASCIA_SHADE, f);
			break;
		case EL_AWNING:
			shell.quad(pl.lot.at(x0, y1, e.z + e.h), pl.lot.at(x1, y1, e.z + e.h), pl.lot.at(x1, y0, e.z - e.d * HALF), pl.lot.at(x0, y0, e.z - e.d * HALF),
					(pl.lot.e2 - pl.lot.e1 * HALF).normalized(), lin, f);
			shell.quad(pl.lot.at(x0, y1, e.z + e.h), pl.lot.at(x1, y1, e.z + e.h), pl.lot.at(x1, y0, e.z - e.d * HALF), pl.lot.at(x0, y0, e.z - e.d * HALF),
					-(pl.lot.e2 - pl.lot.e1 * HALF).normalized(), lin * AWN_SHADE, f);
			break;
		default: shell.box(pl.lot, x0, x1, y0, y1, std::min(e.z, pl.ground_lo), e.z + e.h, lin, f, SKIP_E2_NEG); break;
	}
}

void LotEmit::element(const LotElement &e) {
	float x0 = e.x - e.w / 2, x1 = e.x + e.w / 2, y0 = e.y - e.d / 2, y1 = e.y + e.d / 2;
	bool along_x = e.w >= e.d;
	float len = along_x ? e.w : e.d;
	int pieces = std::max(1, int(std::ceil(len / PIECE)));
	if (pl.coarse && e.kind != EL_SURFACE && e.kind != EL_WALL && e.kind != EL_HEDGE) return;
	switch (e.kind) {
		case EL_SURFACE: surface(e); break;
		case EL_WALL: case EL_FENCE: case EL_RAILING: case EL_HEDGE:
			for (int i = 0; i < pieces; i++) {
				float a0 = float(i) / float(pieces), a1 = float(i + 1) / float(pieces);
				float px0 = along_x ? x0 + e.w * a0 : x0, px1 = along_x ? x0 + e.w * a1 : x1;
				float py0 = along_x ? y0 : y0 + e.d * a0, py1 = along_x ? y1 : y0 + e.d * a1;
				if (e.kind == EL_WALL || e.kind == EL_HEDGE) wall(e, px0, px1, py0, py1);
				else fence(e, px0, px1, py0, py1, e.kind == EL_RAILING);
			}
			break;
		case EL_GATE: case EL_CAR_GATE: gate(e); break;
		case EL_CANOPY: case EL_AWNING: case EL_SLAB: case EL_PILLAR: case EL_PAVEMENT: hung(e); break;
		default: prop(e); break;
	}
}
} // namespace

void emit_lot(Shell &shell, Shell &parts, const Placed &pl) {
	LotEmit le{ shell, parts, pl, pl.lot.o.y };
	for (const LotElement &e : pl.b->elements) le.element(e);
}

} // namespace city::b40
