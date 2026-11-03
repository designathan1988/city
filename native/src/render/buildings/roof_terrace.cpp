// Used roof terraces (4.0 roofMesh.ts :132-176, roofPlan.ts :61-83): on a terrace with nothing built on it, at least
// 4 x 4 m: a timber deck, plant pots along both long edges, tables with parasols and stools, loungers, a pergola and a
// water tank, each kept or dropped by its own seed(k) of the building id and volume. Lot frame, z over the floor.
#include "render/buildings/facade.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city::b40 {

namespace {
constexpr float MIN_SIDE = 4.f, DECK_IN = 0.8f, DECK_SHARE = 0.55f, DECK_SPAN = 0.2f, DECK_T = 0.06f, POT_IN = 0.7f, POT_R = 0.28f;
constexpr float POT_H = 0.5f, POT_EVERY = 1.8f, POT_SPAN = 1.6f, SOIL = 0.04f, TABLE = 0.8f, TABLE_LO = 0.7f, TABLE_T = 0.05f;
constexpr float TABLE_EVERY = 3.5f, POLE = 0.08f, POLE_TOP = 2.3f, PARASOL = 2.2f, PARASOL_T = 0.1f, STOOL = 0.4f, STOOL_H = 0.45f;
constexpr float STOOL_OFF = 0.7f, TABLE_SHARE = 0.3f, TABLE_SPAN = 0.7f, LOUNGE_L = 1.8f, LOUNGE_W = 0.65f, LOUNGE_H = 0.35f;
constexpr float LOUNGE_EVERY = 1.2f, LOUNGE_ROOM = 1.5f, LOUNGE_AFTER = 1.2f, POST = 0.14f, POST_H = 2.4f, POST_EVERY = 2.4f;
constexpr float BEAM_W = 0.12f, BEAM_LO = 2.25f, SLAT_EVERY = 0.6f, SLAT_W = 0.08f, SLAT_LO = 2.4f, SLAT_HI = 2.5f, PERGOLA_HALF = 1.2f;
constexpr float TANK_IN = 1.4f, TANK = 1.4f, TANK_H = 0.95f, RIM = 1.56f, RIM_H = 0.2f, RIM_SHADE = 0.82f;
constexpr float HALF = 0.5f, POTS = 0.8f, LOUNGERS = 0.6f, PERGOLA = 0.45f, TANKS = 0.65f;
constexpr uint32_t ID_MUL = 2654435761u, VOL_MUL = 40503u, K_MUL = 131u, SEED_MOD = 1000u;
constexpr int K_DECK = 1, K_POT = 5, K_POTS = 6, K_TABLES = 7, K_LOUNGE = 8, K_PERGOLA = 9, K_TANK = 10, K_TANK_COL = 11;

float seed_k(uint32_t id, int vol, int k) {
	return float((id * ID_MUL + uint32_t(vol) * VOL_MUL + uint32_t(k) * K_MUL) % SEED_MOD) / float(SEED_MOD);
}
} // namespace

void emit_roof_terrace(Shell &shell, Shell &parts, const Placed &pl, const Volume &v, int vol, float z) {
	if (!v.outline.empty() || v.w < MIN_SIDE || v.d < MIN_SIDE) return;
	const Data &data = *pl.data;
	const Basis &L = pl.lot;
	auto sd = [&](int k) { return seed_k(pl.id, vol, k); };
	Lin wood = paint_lin(data.color("deck")), pot = paint_lin(data.color("pot")), soil = paint_lin(data.color("soil"));
	Lin plaster = paint_lin(data.color("parasol")), trim = trim_lin(pl);
	float x0 = v.x + DECK_IN, x1 = v.x + v.w * (DECK_SHARE + DECK_SPAN * sd(K_DECK)), y0 = v.y + DECK_IN, y1 = v.y + v.d - DECK_IN;
	shell.box(L, x0, x1, y0, y1, z, z + DECK_T, wood, FIN_WOOD, SKIP_E2_NEG);
	float top = z + DECK_T;
	if (sd(K_POTS) < POTS)
		for (float y : { v.y + POT_IN, v.y + v.d - POT_IN })
			for (float x = v.x + POT_IN; x < v.x + v.w - POT_IN; x += POT_EVERY + POT_SPAN * sd(K_POT)) {
				parts.box(L, x - POT_R, x + POT_R, y - POT_R, y + POT_R, z, z + POT_H, pot, FIN_CERAMIC, SKIP_E2_NEG | SKIP_E2_POS);
				parts.rect(L, x - POT_R, x + POT_R, y - POT_R, y + POT_R, z + POT_H - SOIL, soil, FIN_NONE);
			}
	float len = x1 - x0, cy = (y0 + y1) / 2;
	int tables = int(std::floor(std::floor(len / TABLE_EVERY) * (TABLE_SHARE + TABLE_SPAN * sd(K_TABLES))));
	for (int i = 0; i < tables; i++) {
		float x = x0 + len * (float(i) + HALF) / float(tables);
		parts.box(L, x - TABLE / 2, x + TABLE / 2, cy - TABLE / 2, cy + TABLE / 2, top + TABLE_LO, top + TABLE_LO + TABLE_T, trim, FIN_METAL);
		parts.box(L, x - POLE / 2, x + POLE / 2, cy - POLE / 2, cy + POLE / 2, top, top + POLE_TOP, trim, FIN_METAL);
		shell.box(L, x - PARASOL / 2, x + PARASOL / 2, cy - PARASOL / 2, cy + PARASOL / 2, top + POLE_TOP, top + POLE_TOP + PARASOL_T, plaster, FIN_PLASTER);
		for (float s : { -1.f, 1.f })
			parts.box(L, x + s * STOOL_OFF - STOOL / 2, x + s * STOOL_OFF + STOOL / 2, cy - STOOL / 2, cy + STOOL / 2, top, top + STOOL_H, trim, FIN_METAL);
	}
	float lx = std::min(v.x + v.w - LOUNGE_ROOM, x1 + LOUNGE_AFTER);
	bool lounge = sd(K_LOUNGE) < LOUNGERS && lx + LOUNGE_W < v.x + v.w;
	if (lounge)
		for (float y = y0; y + LOUNGE_L < y1; y += LOUNGE_EVERY + LOUNGE_W)
			parts.box(L, lx - LOUNGE_W / 2, lx + LOUNGE_W / 2, y, y + LOUNGE_L, z, z + LOUNGE_H, plaster, FIN_PLASTER);
	if (lounge && sd(K_PERGOLA) < PERGOLA) {
		for (float px : { lx - PERGOLA_HALF, lx + PERGOLA_HALF }) {
			for (float y = y0; y <= y1; y += POST_EVERY) parts.box(L, px - POST / 2, px + POST / 2, y, y + POST, z, z + POST_H, wood, FIN_WOOD);
			parts.box(L, px - BEAM_W / 2, px + BEAM_W / 2, y0, y1, z + BEAM_LO, z + POST_H, wood, FIN_WOOD);
		}
		for (float y = y0; y < y1; y += SLAT_EVERY)
			parts.box(L, lx - PERGOLA_HALF - BEAM_W, lx + PERGOLA_HALF + BEAM_W, y, y + SLAT_W, z + SLAT_LO, z + SLAT_HI, wood, FIN_WOOD);
	}
	if (sd(K_TANK) < TANKS && !pl.data->tanks.empty()) {
		float tx = v.x + v.w - TANK_IN, ty = v.y + TANK_IN;
		Lin tank = lin_of(pl.data->tanks[size_t(sd(K_TANK_COL) * float(pl.data->tanks.size())) % pl.data->tanks.size()]);
		shell.box(L, tx - TANK / 2, tx + TANK / 2, ty - TANK / 2, ty + TANK / 2, z, z + TANK_H, tank, FIN_PLASTER, SKIP_E2_NEG);
		shell.box(L, tx - RIM / 2, tx + RIM / 2, ty - RIM / 2, ty + RIM / 2, z + TANK_H, z + TANK_H + RIM_H, tank * RIM_SHADE, FIN_PLASTER, SKIP_E2_NEG);
	}
}

} // namespace city::b40
