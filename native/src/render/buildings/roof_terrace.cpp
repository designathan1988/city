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
constexpr float HALF = 0.5f;
// A terrace's draws: Knuth's multiplicative hash of the id, the volume's and the stream's multipliers, the seed's span,
// and the key of each piece's stream.
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
	const RoofTerraceLook &K = pl.look->shapes.roof_terrace;
	if (!v.outline.empty() || v.w < K.terrace_min_width || v.d < K.terrace_min_width) return;
	const Data &data = *pl.data;
	const Basis &L = pl.lot;
	auto sd = [&](int k) { return seed_k(pl.id, vol + 1, k); };
	const Lin wood = paint_lin(data.color("deck")), pot = paint_lin(data.color("pot")), soil = paint_lin(data.color("soil"));
	const Lin parasol = paint_lin(data.color("parasol")), trim = trim_lin(pl), pole = paint_lin(data.color("plant_dark"));
	const float x0 = v.x + K.deck_in, x1 = v.x + v.w * (K.deck_share + K.deck_span * sd(K_BOARDS)), y0 = v.y + K.deck_in, y1 = v.y + v.d - K.deck_in;
	shell.box(L, x0, x1, y0, y1, z, z + K.deck_t, wood, FIN_WOOD, SKIP_E2_NEG);
	const float pot_step = K.pot_every + K.pot_span * sd(K_POT);
	if (sd(K_POTS) < K.pots)
		for (float x = v.x + K.pot_in; x < v.x + v.w - K.pot_end; x += pot_step)
			for (float y : { v.y + K.pot_in, v.y + v.d - K.pot_in }) cylinder(shell, L, x, y, K.pot_r, z, z + K.pot_h, pot, soil, FIN_CERAMIC);
	const float len = x1 - x0, cy = (y0 + y1) / 2;
	const int tables = int(std::floor(std::floor(len / K.table_every) * (K.table_share + K.table_span * sd(K_TABLES))));
	for (int i = 0; i < tables; i++) {
		const float x = x0 + len * (float(i) + HALF) / float(tables);
		post(parts, L, x, cy, K.table_half, z + K.table_lo, z + K.table_hi, trim, FIN_METAL);
		if (!pl.coarse) post(parts, L, x, cy, K.pole_half, z, z + K.parasol_low, pole, FIN_METAL);
		post(shell, L, x, cy, K.parasol_half, z + K.parasol_low, z + K.parasol_high, parasol, FIN_PLASTER);
		for (float s : { -1.f, 1.f }) post(parts, L, x + s * K.stool_off, cy, K.stool_half, z, z + K.stool_h, trim, FIN_METAL);
	}
	const float lx = std::min(v.x + v.w - K.lounge_room, x1 + K.lounge_after);
	if (sd(K_LOUNGE) < K.loungers)
		for (float y = y0 + K.lounge_from; y + K.lounge_end < y1; y += K.lounge_every)
			parts.box(L, lx - K.lounge_half, lx + K.lounge_half, y, y + K.lounge_w, z, z + K.lounge_h, parasol, FIN_PLASTER);
	if (sd(K_PERGOLA) < K.pergola) {
		const float span = y1 - y0;
		const int bays = std::max(1, int(std::lround(span / K.post_every)));
		for (float px : { lx - K.pergola_half, lx + K.pergola_half }) {
			if (!pl.coarse)
				for (int k = 0; k <= bays; k++) post(parts, L, px, y0 + span * float(k) / float(bays), K.post_half, z, z + K.post_h, wood, FIN_WOOD);
			parts.box(L, px - K.beam_half, px + K.beam_half, y0 - K.beam_past, y1 + K.beam_past, z + K.beam_lo, z + K.post_h, wood, FIN_WOOD);
		}
		for (float y = y0; y <= y1; y += K.slat_every)
			parts.box(L, lx - K.slat_reach, lx + K.slat_reach, y - K.slat_half, y + K.slat_half, z + K.slat_lo, z + K.slat_hi, wood, FIN_WOOD);
	}
	if (sd(K_TANK) < K.tanks && !data.tanks.empty()) {
		const float tx = v.x + v.w - K.tank_in, ty = v.y + K.tank_in;
		const Lin tank = lin_of(data.tanks[size_t(sd(K_TANK_COL) * float(data.tanks.size())) % data.tanks.size()]);
		cylinder(shell, L, tx, ty, K.tank_r, z, z + K.tank_h, tank, tank, FIN_PLASTER, false);
		cylinder(shell, L, tx, ty, K.rim_r, z + K.tank_h, z + K.rim_top, tank, tank * K.rim_shade, FIN_PLASTER);
	}
}

} // namespace city::b40
