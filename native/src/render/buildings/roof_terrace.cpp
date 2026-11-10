// Used roof terraces: on a terrace with nothing built on it, at least
// 4 x 4 m: a timber deck, round plant pots along both long edges, tables with parasols and stools, loungers, a pergola and a
// water tank, each kept or dropped by its own seed(k) of the building id and block. Lot frame, z over the floor. The mid level leaves out what is
// a few centimetres thick: the parasols' poles and the pergola's posts.
#include "render/buildings/facade.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city::b40 {

namespace {
constexpr float TERRACE_MIN_WIDTH = 4.f, DECK_IN = 0.8f, DECK_SHARE = 0.55f, DECK_SPAN = 0.2f, DECK_T = 0.06f;
constexpr float POT_IN = 0.7f, POT_END = 0.6f, POT_R = 0.28f, POT_H = 0.5f, POT_EVERY = 1.8f, POT_SPAN = 1.6f;
constexpr float TABLE_EVERY = 3.5f, TABLE_SHARE = 0.3f, TABLE_SPAN = 0.7f, TABLE_HALF = 0.4f, TABLE_LO = 0.7f, TABLE_HI = 0.75f;
constexpr float POLE_HALF = 0.04f, PARASOL_HALF = 1.1f, PARASOL_LOW = 2.3f, PARASOL_HIGH = 2.4f, STOOL_HALF = 0.2f, STOOL_H = 0.45f, STOOL_OFF = 0.7f;
constexpr float LOUNGE_ROOM = 1.5f, LOUNGE_AFTER = 1.2f, LOUNGE_FROM = 0.4f, LOUNGE_END = 0.7f, LOUNGE_EVERY = 1.2f, LOUNGE_HALF = 0.9f, LOUNGE_W = 0.65f, LOUNGE_H = 0.35f;
constexpr float POST_HALF = 0.07f, POST_H = 2.4f, POST_EVERY = 2.4f, BEAM_HALF = 0.06f, BEAM_PAST = 0.1f, BEAM_LO = 2.25f;
constexpr float SLAT_EVERY = 0.6f, SLAT_HALF = 0.04f, SLAT_REACH = 1.45f, SLAT_LO = 2.4f, SLAT_HI = 2.5f, PERGOLA_HALF = 1.2f;
constexpr float TANK_IN = 1.4f, TANK_R = 0.7f, TANK_H = 0.95f, RIM_R = 0.78f, RIM_TOP = 1.15f, RIM_SHADE = 0.82f;
constexpr float HALF = 0.5f, POTS = 0.8f, LOUNGERS = 0.6f, PERGOLA = 0.45f, TANKS = 0.65f;
constexpr uint32_t ID_MUL = 2654435761u, VOL_MUL = 40503u, K_MUL = 131u, SEED_MOD = 1000u;
constexpr int K_BOARDS = 1, K_POT = 5, K_POTS = 6, K_TABLES = 7, K_LOUNGE = 8, K_PERGOLA = 9, K_TANK = 10, K_TANK_COL = 11;

float seed_k(uint32_t id, int block, int k) {
	return float((id * ID_MUL + uint32_t(block) * VOL_MUL + uint32_t(k) * K_MUL) % SEED_MOD) / float(SEED_MOD);
}

// A box of `half` each way about (x, y).
void post(Shell &s, const Basis &L, float x, float y, float half, float z0, float z1, const Lin &lin, int8_t fin) {
	s.box(L, x - half, x + half, y - half, y + half, z0, z1, lin, fin);
}
} // namespace

void emit_roof_terrace(Shell &shell, Shell &parts, const Placed &pl, const Volume &v, int vol, float z) {
	if (!v.outline.empty() || v.w < TERRACE_MIN_WIDTH || v.d < TERRACE_MIN_WIDTH) return;
	const Data &data = *pl.data;
	const Basis &L = pl.lot;
	auto sd = [&](int k) { return seed_k(pl.id, vol + 1, k); };
	const Lin wood = paint_lin(data.color("deck")), pot = paint_lin(data.color("pot")), soil = paint_lin(data.color("soil"));
	const Lin parasol = paint_lin(data.color("parasol")), trim = trim_lin(pl), pole = paint_lin(data.color("plant_dark"));
	const float x0 = v.x + DECK_IN, x1 = v.x + v.w * (DECK_SHARE + DECK_SPAN * sd(K_BOARDS)), y0 = v.y + DECK_IN, y1 = v.y + v.d - DECK_IN;
	shell.box(L, x0, x1, y0, y1, z, z + DECK_T, wood, FIN_WOOD, SKIP_E2_NEG);
	const float pot_step = POT_EVERY + POT_SPAN * sd(K_POT);
	if (sd(K_POTS) < POTS)
		for (float x = v.x + POT_IN; x < v.x + v.w - POT_END; x += pot_step)
			for (float y : { v.y + POT_IN, v.y + v.d - POT_IN }) cylinder(shell, L, x, y, POT_R, z, z + POT_H, pot, soil, FIN_CERAMIC);
	const float len = x1 - x0, cy = (y0 + y1) / 2;
	const int tables = int(std::floor(std::floor(len / TABLE_EVERY) * (TABLE_SHARE + TABLE_SPAN * sd(K_TABLES))));
	for (int i = 0; i < tables; i++) {
		const float x = x0 + len * (float(i) + HALF) / float(tables);
		post(parts, L, x, cy, TABLE_HALF, z + TABLE_LO, z + TABLE_HI, trim, FIN_METAL);
		if (!pl.coarse) post(parts, L, x, cy, POLE_HALF, z, z + PARASOL_LOW, pole, FIN_METAL);
		post(shell, L, x, cy, PARASOL_HALF, z + PARASOL_LOW, z + PARASOL_HIGH, parasol, FIN_PLASTER);
		for (float s : { -1.f, 1.f }) post(parts, L, x + s * STOOL_OFF, cy, STOOL_HALF, z, z + STOOL_H, trim, FIN_METAL);
	}
	const float lx = std::min(v.x + v.w - LOUNGE_ROOM, x1 + LOUNGE_AFTER);
	if (sd(K_LOUNGE) < LOUNGERS)
		for (float y = y0 + LOUNGE_FROM; y + LOUNGE_END < y1; y += LOUNGE_EVERY)
			parts.box(L, lx - LOUNGE_HALF, lx + LOUNGE_HALF, y, y + LOUNGE_W, z, z + LOUNGE_H, parasol, FIN_PLASTER);
	if (sd(K_PERGOLA) < PERGOLA) {
		const float span = y1 - y0;
		const int bays = std::max(1, int(std::lround(span / POST_EVERY)));
		for (float px : { lx - PERGOLA_HALF, lx + PERGOLA_HALF }) {
			if (!pl.coarse)
				for (int k = 0; k <= bays; k++) post(parts, L, px, y0 + span * float(k) / float(bays), POST_HALF, z, z + POST_H, wood, FIN_WOOD);
			parts.box(L, px - BEAM_HALF, px + BEAM_HALF, y0 - BEAM_PAST, y1 + BEAM_PAST, z + BEAM_LO, z + POST_H, wood, FIN_WOOD);
		}
		for (float y = y0; y <= y1; y += SLAT_EVERY)
			parts.box(L, lx - SLAT_REACH, lx + SLAT_REACH, y - SLAT_HALF, y + SLAT_HALF, z + SLAT_LO, z + SLAT_HI, wood, FIN_WOOD);
	}
	if (sd(K_TANK) < TANKS && !data.tanks.empty()) {
		const float tx = v.x + v.w - TANK_IN, ty = v.y + TANK_IN;
		const Lin tank = lin_of(data.tanks[size_t(sd(K_TANK_COL) * float(data.tanks.size())) % data.tanks.size()]);
		cylinder(shell, L, tx, ty, TANK_R, z, z + TANK_H, tank, tank, FIN_PLASTER, false);
		cylinder(shell, L, tx, ty, RIM_R, z + TANK_H, z + RIM_TOP, tank, tank * RIM_SHADE, FIN_PLASTER);
	}
}

} // namespace city::b40
