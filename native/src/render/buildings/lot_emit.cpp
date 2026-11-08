// The lot elements of a placed body: surfaces laid
// on a 3 m grid following the ground (lawns drawn as grass by the shader), boundary walls with plinth and coping, fences
// and railings on their base walls, hedges, gates with posts and bars, planters, benches, bollards, lamps (lit heads),
// bins, stall lines, wall ACs, an uphill house's garage dug into the slope, and the building-hung canopies, awnings, veranda floors,
// slabs and posts.
#include "render/buildings/facade.h"

#include <cmath>

using namespace godot;

namespace city::b40 {

namespace {
constexpr float HALF = 0.5f;
constexpr uint32_t BAND_LIGHT = 0xffe8c8, LAMP_WARM = 0xf1ebdd; // a bollard's lit band and a lamp head's warm white (the source game's)

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
	void dug(const LotElement &e);
	void element(const LotElement &e);
	const LotEmitLook &K = pl.look->shapes.lot_emit; // last: it reads pl
};

void LotEmit::surface(const LotElement &e) {
	bool lawn = e.paint >= 0 && pl.b->paints[size_t(e.paint)].finish == FIN_NONE;
	Lin lin = paint(e, "paving");
	float x0 = e.x - e.w / 2, x1 = e.x + e.w / 2, y0 = e.y - e.d / 2, y1 = e.y + e.d / 2;
	float lift = K.lot_raise + e.h - HALF * e.h;
	Shell &to = lawn && shell.lawns ? *shell.lawns : shell;
	// On level ground one quad. On a slope it follows the ground cell by cell: a single quad folds along its own diagonal, and two
	// surfaces that overlap fold along different ones and cross (a lawn coming up through the car park over it).
	const float mid_gap = std::fabs(ground((x0 + x1) / 2, (y0 + y1) / 2) - (ground(x0, y0) + ground(x1, y1)) / 2);
	const float corner_gap = std::fabs(ground(x0, y0) + ground(x1, y1) - ground(x1, y0) - ground(x0, y1));
	const bool level = mid_gap < K.level_m && corner_gap < K.level_m;
	const int nx = level ? 1 : std::max(1, int(std::ceil((x1 - x0) / K.ground_cell_m))), ny = level ? 1 : std::max(1, int(std::ceil((y1 - y0) / K.ground_cell_m)));
	for (int j = 0; j < ny; j++)
		for (int i = 0; i < nx; i++) {
			const float a0 = x0 + (x1 - x0) * float(i) / float(nx), a1 = x0 + (x1 - x0) * float(i + 1) / float(nx);
			const float b0 = y0 + (y1 - y0) * float(j) / float(ny), b1 = y0 + (y1 - y0) * float(j + 1) / float(ny);
			to.quad(pl.lot.at(a0, b0, ground(a0, b0) + lift), pl.lot.at(a1, b0, ground(a1, b0) + lift), pl.lot.at(a1, b1, ground(a1, b1) + lift),
					pl.lot.at(a0, b1, ground(a0, b1) + lift), pl.lot.e2, lin, fin(e), lawn ? SK_LAWN : SK_GROUND);
		}
}

void LotEmit::wall(const LotElement &e, float x0, float x1, float y0, float y1) {
	float g = ground((x0 + x1) / 2, (y0 + y1) / 2);
	float lo = std::min(ground(x0, y0), ground(x1, y1)) - K.foot, top = g + e.h;
	Lin lin = paint(e, "concrete");
	int8_t f = fin(e);
	shell.grime_floor = floor_y + g;
	if (pl.coarse) {
		shell.box(pl.lot, x0, x1, y0, y1, lo, top, lin, f, SKIP_E2_NEG);
		return;
	}
	shell.box(pl.lot, x0, x1, y0, y1, g + K.splash, top - K.coping_h, lin, f, SKIP_E2_NEG | SKIP_E2_POS);
	float o = K.coping_out;
	shell.box(pl.lot, x0 - (x1 - x0 < y1 - y0 ? o : 0), x1 + (x1 - x0 < y1 - y0 ? o : 0), y0 - (x1 - x0 >= y1 - y0 ? o : 0), y1 + (x1 - x0 >= y1 - y0 ? o : 0),
			lo, g + K.splash, lin * K.splash_shade, f, SKIP_E2_NEG);
	shell.box(pl.lot, x0 - (x1 - x0 < y1 - y0 ? o : 0), x1 + (x1 - x0 < y1 - y0 ? o : 0), y0 - (x1 - x0 >= y1 - y0 ? o : 0), y1 + (x1 - x0 >= y1 - y0 ? o : 0),
			top - K.coping_h, top, paint_lin(pl.data->color("wall_coping")) * K.coping_shade, FIN_STONE, SKIP_E2_NEG);
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
	bar(s0, s0 + len, top - K.coping_h, top, K.rail_t);
	bar(s0, s0 + len, base + (top - base) * K.rail_low, base + (top - base) * K.rail_low + K.low_rail_h, K.rail_t);
	if (pl.coarse && !light) return; // a fence's posts stand far apart and go with the near level; a railing's close ones read together
	float every = light ? K.light_every : K.post_every, pw = light ? K.light_post : K.post;
	for (float a = s0; a <= s0 + len + EPS; a += every) bar(std::min(a, s0 + len - pw), std::min(a, s0 + len - pw) + pw, base, top + K.post_over, pw);
}

void LotEmit::gate(const LotElement &e) {
	float x0 = e.x - e.w / 2, x1 = e.x + e.w / 2, g = ground(e.x, e.y);
	Lin m = paint(e, "gate");
	for (float x : { x0 - K.gate_post, x1 }) parts.box(pl.lot, x, x + K.gate_post, e.y - K.gate_post / 2, e.y + K.gate_post / 2, g - K.foot, g + e.h + K.gate_rail, m, FIN_METAL);
	parts.box(pl.lot, x0, x1, e.y - pl.look->fence_bar_m / 2, e.y + pl.look->fence_bar_m / 2, g + e.h - K.gate_rail, g + e.h, m, FIN_METAL);
	parts.box(pl.lot, x0, x1, e.y - pl.look->fence_bar_m / 2, e.y + pl.look->fence_bar_m / 2, g + K.gate_rail, g + 2 * K.gate_rail, m, FIN_METAL);
	for (float x = x0 + K.bar_every; x < x1; x += K.bar_every) parts.box(pl.lot, x, x + pl.look->fence_bar_m, e.y - pl.look->fence_bar_m / 2, e.y + pl.look->fence_bar_m / 2, g + K.gate_rail, g + e.h, m, FIN_METAL);
}

void LotEmit::prop(const LotElement &e) {
	float x0 = e.x - e.w / 2, x1 = e.x + e.w / 2, y0 = e.y - e.d / 2, y1 = e.y + e.d / 2, g = ground(e.x, e.y);
	Lin lin = paint(e, e.kind == EL_PARKING ? "parking_asphalt" : "concrete");
	int8_t f = fin(e);
	switch (e.kind) {
		case EL_PLANTER:
			shell.box(pl.lot, x0, x1, y0, y1, g - K.foot, g + e.h, lin, f, SKIP_E2_NEG | SKIP_E2_POS);
			shell.rect(pl.lot, x0 + K.soil_in, x1 - K.soil_in, y0 + K.soil_in, y1 - K.soil_in, g + e.h - K.soil_in, paint_lin(pl.data->color("planter_soil")), FIN_NONE);
			break;
		case EL_BENCH:
			parts.box(pl.lot, x0, x1, y0, y1, g + K.bench_seat, g + K.bench_seat + K.bench_seat_t, lin, FIN_WOOD);
			parts.box(pl.lot, x0, x1, y1 - K.bench_back_t, y1, g + K.bench_seat, g + K.bench_back, lin, FIN_WOOD);
			parts.box(pl.lot, x0, x0 + K.bench_back_t, y0, y1, g, g + K.bench_seat, lin, FIN_WOOD);
			parts.box(pl.lot, x1 - K.bench_back_t, x1, y0, y1, g, g + K.bench_seat, lin, FIN_WOOD);
			break;
		case EL_BOLLARD:
			parts.box(pl.lot, x0, x1, y0, y1, g, g + e.h, lin, FIN_METAL);
			parts.box(pl.lot, x0 - K.bollard_band_out, x1 + K.bollard_band_out, y0 - K.bollard_band_out, y1 + K.bollard_band_out, g + e.h - K.bollard_band, g + e.h - K.bollard_band + K.bollard_band_h, lin_of(BAND_LIGHT), FIN_NONE);
			break;
		case EL_LAMP: {
			if (e.z > 0) { // on a wall: the lamp's head and its lit glass under it
				shell.box(pl.lot, x0, x1, y0, y1, e.z, e.z + e.h, lin, FIN_METAL);
				shell.quad(pl.lot.at(x0, y0, e.z - K.grille_t), pl.lot.at(x1, y0, e.z - K.grille_t), pl.lot.at(x1, y1, e.z - K.grille_t), pl.lot.at(x0, y1, e.z - K.grille_t),
						-pl.lot.e2, lin_of(LAMP_WARM), FIN_NONE, SK_LAMP);
				break;
			}
			parts.box(pl.lot, e.x - K.lamp_post / 2, e.x + K.lamp_post / 2, e.y - K.lamp_post / 2, e.y + K.lamp_post / 2, g, g + e.h, lin, FIN_METAL);
			shell.box(pl.lot, e.x - K.lamp_head / 2, e.x + K.lamp_head / 2, e.y - K.lamp_head / 2, e.y + K.lamp_head / 2, g + e.h, g + e.h + K.lamp_head_h, lin, FIN_METAL);
			shell.quad(pl.lot.at(e.x - K.lamp_head / 3, e.y - K.lamp_head / 3, g + e.h - K.grille_t), pl.lot.at(e.x + K.lamp_head / 3, e.y - K.lamp_head / 3, g + e.h - K.grille_t),
					pl.lot.at(e.x + K.lamp_head / 3, e.y + K.lamp_head / 3, g + e.h - K.grille_t), pl.lot.at(e.x - K.lamp_head / 3, e.y + K.lamp_head / 3, g + e.h - K.grille_t),
					-pl.lot.e2, lin_of(LAMP_WARM), FIN_NONE, SK_LAMP);
			break;
		}
		case EL_AC:
			shell.box(pl.lot, x0, x1, y0, y1, e.z, e.z + e.h, lin, FIN_METAL, SKIP_E1_NEG);
			break;
		default:
			if (emit_built_part(shell, pl, e, g, lin, f)) break;
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
			shell.box(pl.lot, x0, x1, y0, y0 + K.rail_t, e.z - K.fascia_h + e.h, e.z, lin * K.fascia_shade, f);
			break;
		case EL_AWNING:
			shell.quad(pl.lot.at(x0, y1, e.z + e.h), pl.lot.at(x1, y1, e.z + e.h), pl.lot.at(x1, y0, e.z - e.d * HALF), pl.lot.at(x0, y0, e.z - e.d * HALF),
					(pl.lot.e2 - pl.lot.e1 * HALF).normalized(), lin, f);
			shell.quad(pl.lot.at(x0, y1, e.z + e.h), pl.lot.at(x1, y1, e.z + e.h), pl.lot.at(x1, y0, e.z - e.d * HALF), pl.lot.at(x0, y0, e.z - e.d * HALF),
					-(pl.lot.e2 - pl.lot.e1 * HALF).normalized(), lin * K.awn_shade, f);
			break;
		default: shell.box(pl.lot, x0, x1, y0, y1, std::min(e.z, pl.ground_lo), e.z + e.h, lin, f, SKIP_E2_NEG); break;
	}
}

// An uphill house's garage dug into the slope: a box from the ground at its door (the street's level) up to the lot's floor, or to
// a door's height and its roof when the floor stands lower; the slope buries its back. Its door stands on the front face.
void LotEmit::dug(const LotElement &e) {
	const float x0 = e.x - e.w / 2, x1 = e.x + e.w / 2, y0 = e.y - e.d / 2, y1 = e.y + e.d / 2;
	const float door = ground(e.x, y0), top = std::max(0.f, door + e.h + K.dug_roof);
	shell.box(pl.lot, x0, x1, y0, y1, door - K.foot, top, paint(e, "concrete"), fin(e), SKIP_E2_NEG);
	shell.box(pl.lot, x0 + K.dug_jamb, x1 - K.dug_jamb, y0 - K.dug_door_out, y0, door, door + e.h, paint_lin(pl.data->color("gate")), FIN_METAL, SKIP_E2_NEG);
}

void LotEmit::element(const LotElement &e) {
	float x0 = e.x - e.w / 2, x1 = e.x + e.w / 2, y0 = e.y - e.d / 2, y1 = e.y + e.d / 2;
	bool along_x = e.w >= e.d;
	float len = along_x ? e.w : e.d;
	int pieces = std::max(1, int(std::ceil(len / K.piece)));
	if (pl.coarse && e.kind != EL_SURFACE && e.kind != EL_WALL && e.kind != EL_HEDGE && e.kind != EL_FENCE && e.kind != EL_RAILING && e.kind != EL_POOL && e.kind != EL_DUG_GARAGE) return;
	if (!pl.coarse && e.kind == EL_HEDGE) return; // near, a hedge is the gardens' plants (render/trees/garden_place.h)
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
		case EL_DUG_GARAGE: dug(e); break;
		default: prop(e); break;
	}
}
// A piece at an angle to the lot is the same piece in a frame turned about its middle: everything a part draws, it draws through the frame.
void turned_element(Shell &shell, Shell &parts, const Placed &pl, const LotElement &e) {
	Placed at = pl;
	const float c = std::cos(e.angle), s = std::sin(e.angle);
	at.lot.o = pl.lot.at(e.x, e.y, 0);
	at.lot.e0 = pl.lot.e0 * c + pl.lot.e1 * s;
	at.lot.e1 = pl.lot.e1 * c - pl.lot.e0 * s;
	LotElement local = e;
	local.x = 0;
	local.y = 0;
	LotEmit{ shell, parts, at, pl.lot.o.y }.element(local);
}
} // namespace

void emit_lot(Shell &shell, Shell &parts, const Placed &pl) {
	LotEmit le{ shell, parts, pl, pl.lot.o.y };
	const Body &b = *pl.b;
	for (const LotElement &e : b.elements) {
		if (b.cut_level >= 0 && e.z >= b.level_z(b.cut_level + 1) - EPS) continue; // cut open: what hangs above the floor seen is gone
		if (e.angle != 0) turned_element(shell, parts, pl, e);
		else le.element(e);
	}
}

} // namespace city::b40
