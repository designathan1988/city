// The opening of a bay and the wall round it: the four wall
// rectangles, the jambs (trim x 0.68), the sill (trim x 0.92) and head strips, the closing pane or leaf at the reveal
// depth, roller shutters, the rain streak under the sill; then the component's parts.
#include "render/buildings/facade_bay.h"

using namespace godot;

namespace city::b40 {

namespace {
constexpr float MIN_SILL_ROOM = 0.65f, MIN_W = 0.4f, W_MARGIN = 0.2f, MIN_H = 0.5f, HEAD_ROOM = 0.15f, NO_OPENING = 0.2f;
constexpr float REVEAL = 0.68f, SILL_SHADE = 0.92f, HEAD_SHADE = 0.8f, THRESHOLD = 0.012f, THRESHOLD_BELOW = 0.02f;
constexpr int PICKS = 20, DARK_PICKS = 5, ROLL_FROM = 9, ROLL_TO = 13, LOGGIA_DARK = 4, BRISE_DARK = 5;
constexpr float ROLL_BASE = 0.25f, ROLL_STEP = 0.15f, ROLL_OUT = 0.05f, WIDE_PANE = 1.8f;
constexpr float STREAK_OUT = 0.012f, STREAK_MIN_SILL = 0.5f, STREAK_LO = 0.05f, STREAK_BASE = 0.7f, STREAK_STEP = 0.12f;
constexpr float STREAK_HALF = 0.32f, STREAK_HALF_STEP = 0.03f, STREAK_TOP = 0.02f, STREAK_DARK = 0.93f, STREAK_MID = 0.965f;
constexpr int STREAK_SKIP = 3, STREAK_REACH = 7, STREAK_WIDTH = 5;
constexpr float FRENCH_STREAK_SILL = 0.3f;

bool glazed(uint8_t c) {
	return c != BAY_DOOR && c != BAY_GARAGE_DOOR && c != BAY_LOADING_DOOR && c != BAY_WALL && c != BAY_PILLAR;
}
bool streaks(int8_t fin) { return fin == FIN_PLASTER || fin == FIN_STUCCO || fin == FIN_CONCRETE || fin == FIN_STONE; }

bool dark_pane(const BayCtx &c, float w) {
	uint32_t h = c.hash;
	switch (c.comp) {
		case BAY_WINDOW: case BAY_SASH: return int(h % PICKS) < (w >= WIDE_PANE ? DARK_PICKS - 1 : DARK_PICKS);
		case BAY_LOGGIA: case BAY_SHUTTERED: case BAY_GOURMET: return h % LOGGIA_DARK == 0;
		default: return h % BRISE_DARK == 0;
	}
}

void streak(Shell &s, const BayCtx &c, const Opening &o) {
	bool kind = c.comp == BAY_WINDOW || c.comp == BAY_SASH || c.comp == BAY_SHUTTERED ||
			((c.comp == BAY_FRENCH || c.comp == BAY_BAY_WINDOW || c.comp == BAY_RIBBON) && o.h0 > FRENCH_STREAK_SILL);
	if (!kind || !streaks(c.wall_fin) || o.h0 < STREAK_MIN_SILL || c.hash % STREAK_SKIP == 0) return;
	float reach = std::min(o.h0 - STREAK_LO, STREAK_BASE + float(c.hash % STREAK_REACH) * STREAK_STEP);
	float hw = o.w * (STREAK_HALF + float(c.hash % STREAK_WIDTH) * STREAK_HALF_STEP), am = c.W / 2;
	float mid = o.h0 - reach / 2;
	s.rect(c.f, am - hw, am + hw, o.h0 - reach, mid, STREAK_OUT, c.wall * STREAK_MID, c.wall_fin);
	s.rect(c.f, am - hw, am + hw, mid, o.h0 - STREAK_TOP, STREAK_OUT, c.wall * STREAK_DARK, c.wall_fin);
}

void closing(Shell &s, const Placed &pl, const BayCtx &c, const Opening &o) {
	if (c.comp == BAY_DOOR || c.comp == BAY_DOUBLE_DOOR) return; // their parts close them (facade_parts.cpp door_back)
	if (glazed(c.comp)) {
		bool dark = dark_pane(c, o.w);
		s.pane(c.f, o.a0, o.a1, o.h0, o.h1, -o.depth, c.hash, dark);
		int pick = int(c.hash % PICKS);
		bool roll = (c.comp == BAY_WINDOW || c.comp == BAY_SASH) && pick >= ROLL_FROM && pick < ROLL_TO && o.w < WIDE_PANE;
		if (roll && c.comp != BAY_SASH) {
			float down = (o.h1 - o.h0) * (ROLL_BASE + ROLL_STEP * float(pick - ROLL_FROM));
			s.rect(c.f, o.a0, o.a1, o.h1 - down, o.h1, -o.depth + ROLL_OUT, paint_lin(pl.data->color("shutter")), FIN_METAL);
		}
		return;
	}
	const Paint &p = pl.data->color("shutter");
	s.rect(c.f, o.a0, o.a1, o.h0, o.h1, -o.depth, paint_lin(p), p.finish);
}
} // namespace

Opening opening_of(const Data &data, const BayCtx &c) {
	Opening o;
	const OpeningDef &d = data.openings[c.comp < BAY_COUNT ? c.comp : 0];
	if (!d.has || c.plain) return o;
	o.w = std::min(c.W - d.w_in, d.w_max);
	o.h0 = d.sill;
	o.h1 = std::min(c.H - d.head_in, d.head_max);
	o.depth = d.depth;
	if ((c.comp == BAY_WINDOW || c.comp == BAY_SASH || c.comp == BAY_WIDE) && c.geo && c.geo->bays >= 0) {
		o.w = c.W * c.geo->window_w;
		o.h0 = std::min(c.geo->sill, c.H - MIN_SILL_ROOM);
		o.h1 = o.h0 + c.H * c.geo->window_h;
		o.w = std::max(MIN_W, std::min({ o.w, c.W - W_MARGIN, d.w_max }));
		o.h1 = std::max(o.h0 + MIN_H, std::min({ o.h1, c.H - HEAD_ROOM, d.head_max })); // the table's caps hold here too
	}
	if (o.w < NO_OPENING || o.h1 <= o.h0) return o;
	o.has = true;
	o.a0 = (c.W - o.w) / 2;
	o.a1 = o.a0 + o.w;
	return o;
}

void emit_bay(Shell &shell, Shell &parts, const Placed &pl, const BayCtx &c) {
	Opening o = opening_of(*pl.data, c);
	if (!o.has) {
		shell.rect(c.f, 0, c.W, 0, c.H, 0, c.wall, c.wall_fin);
		return;
	}
	shell.rect(c.f, 0, o.a0, 0, c.H, 0, c.wall, c.wall_fin);
	shell.rect(c.f, o.a1, c.W, 0, c.H, 0, c.wall, c.wall_fin);
	shell.rect(c.f, o.a0, o.a1, 0, o.h0, 0, c.wall, c.wall_fin);
	shell.rect(c.f, o.a0, o.a1, o.h1, c.H, 0, c.wall, c.wall_fin);
	bool porch = c.comp == BAY_LOGGIA;
	Lin reveal = porch ? c.wall * REVEAL : c.trim * REVEAL;
	int8_t rfin = porch ? c.wall_fin : c.trim_fin;
	Basis jl{ c.f.at(o.a0, 0, 0), c.f.e2, c.f.e1, c.f.e0 }, jr{ c.f.at(o.a1, 0, 0), c.f.e2, c.f.e1, -c.f.e0 };
	shell.rect(jl, -o.depth, 0, o.h0, o.h1, 0, reveal, rfin);
	shell.rect(jr, -o.depth, 0, o.h0, o.h1, 0, reveal, rfin);
	Basis flat{ c.f.o, c.f.e0, c.f.e2, c.f.e1 };
	float sill_h = o.h0 < THRESHOLD_BELOW ? THRESHOLD : o.h0;
	shell.rect(flat, o.a0, o.a1, -o.depth, 0, sill_h, porch ? reveal : c.trim * SILL_SHADE, rfin);
	shell.rect(flat, o.a0, o.a1, -o.depth, 0, o.h1, reveal * HEAD_SHADE, rfin, true);
	closing(shell, pl, c, o);
	streak(shell, c, o);
	emit_bay_parts(shell, parts, pl, c, o);
	if (c.level >= 1) emit_bay_props(parts, pl, c, o);
}

} // namespace city::b40
