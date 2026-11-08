// The opening of a bay and the wall round it: the four wall rectangles; an opening with nothing in its recess but glass and
// frame gets a pane on the wall's face, which draws the recess itself (building_pane.gdshaderinc); any other its jambs
// (trim x 0.68), sill (trim x 0.92) and head strips and the closing pane or leaf at the reveal depth; roller shutters, the
// rain streak under the sill; then the component's parts.
#include "render/buildings/facade_bay.h"
#include "core/shader_contracts/buildings.h"

using namespace godot;

namespace city::b40 {

namespace {
bool glazed(uint8_t c) {
	return c != BAY_DOOR && c != BAY_GARAGE_DOOR && c != BAY_LOADING_DOOR && c != BAY_WALL && c != BAY_PILLAR;
}
bool streaks(int8_t fin) { return fin == FIN_PLASTER || fin == FIN_STUCCO || fin == FIN_CONCRETE || fin == FIN_STONE; }

bool dark_pane(const FacadeBayLook &K, const BayCtx &c) {
	switch (pane_dark(c.comp)) {
		case PANE_DARK_PICK: return int(c.hash % K.picks) < K.dark_picks;
		case PANE_DARK_QUARTER: return c.hash % K.loggia_dark == 0;
		case PANE_DARK_FIFTH: return c.hash % K.brise_dark == 0;
		default: return false;
	}
}

// A plain window with its roller shutter part way down.
bool rolled(const FacadeBayLook &K, const BayCtx &c) {
	int pick = int(c.hash % K.picks);
	return c.comp == BAY_WINDOW && pick >= K.roll_from && pick < K.roll_to;
}

// An opening with nothing in its recess but its glass and frame: its wall stays whole and the pane on it draws the recess.
// What else it has stands on the wall or out of it. Not one with a shutter or a transom inside, or one down to the floor.
bool flush(const FacadeBayLook &K, const BayCtx &c, const Opening &o) {
	if (o.h0 <= 0) return false;
	switch (c.comp) {
		case BAY_WINDOW: return !rolled(K, c);
		case BAY_SASH: case BAY_WIDE: case BAY_BALCONY: case BAY_SHOPFRONT: case BAY_BRISE: case BAY_SHUTTERED: case BAY_GOURMET: return true;
		case BAY_FRENCH: case BAY_BAY_WINDOW: case BAY_RIBBON: return o.h1 - o.h0 <= K.tall_opening;
		default: return false;
	}
}

// The thickness of the frame the pane draws in front of its glass (building_pane.gdshaderinc pane_frame).
float frame_of(const FacadePartsLook &F, uint8_t comp) { // a frame's width: the facade parts' (the same frames)
	return comp == BAY_WIDE || comp == BAY_SHOPFRONT || comp == BAY_BRISE || comp == BAY_GOURMET ? F.frame_thin : F.frame_d;
}

void streak(const FacadeBayLook &K, Shell &s, const BayCtx &c, const Opening &o) {
	bool kind = c.comp == BAY_WINDOW || c.comp == BAY_SASH || c.comp == BAY_SHUTTERED ||
			((c.comp == BAY_FRENCH || c.comp == BAY_BAY_WINDOW || c.comp == BAY_RIBBON) && o.h0 > K.french_streak_sill);
	if (!kind || !streaks(c.wall_fin) || o.h0 < K.streak_min_sill || c.hash % K.streak_skip == 0) return;
	float reach = std::min(o.h0 - K.streak_lo, K.streak_base + float(c.hash % K.streak_reach) * K.streak_step);
	float hw = o.w * (K.streak_half + float(c.hash % K.streak_width) * K.streak_half_step), am = c.W / 2;
	float mid = o.h0 - reach / 2;
	s.rect(c.f, am - hw, am + hw, o.h0 - reach, mid, K.streak_out, c.wall * K.streak_mid, c.wall_fin);
	s.rect(c.f, am - hw, am + hw, mid, o.h0 - K.streak_top, K.streak_out, c.wall * K.streak_dark, c.wall_fin);
}

void reveals(const FacadeBayLook &K, Shell &shell, const BayCtx &c, const Opening &o) {
	bool porch = c.comp == BAY_LOGGIA;
	Lin reveal = porch ? c.wall * REVEAL_SHADE : c.trim * REVEAL_SHADE;
	int8_t rfin = porch ? c.wall_fin : c.trim_fin;
	float sill_h = o.h0 < K.threshold_below ? K.threshold : o.h0;
	emit_reveal(shell, c, o, -o.depth, 0, reveal, porch ? reveal : c.trim * SILL_SHADE, reveal * HEAD_SHADE, rfin, sill_h);
}

void closing(Shell &s, const Placed &pl, const BayCtx &c, const Opening &o) {
	const FacadeBayLook &K = pl.look->shapes.facade_bay;
	if (c.comp == BAY_DOOR || c.comp == BAY_DOUBLE_DOOR) return; // their parts close them (facade_parts.cpp door_back)
	if (glazed(c.comp)) {
		s.pane(c.f, o.a0, o.a1, o.h0, o.h1, -o.depth, pane_look(pl, c, o.a0, o.a1, o.h0, o.h1, dark_pane(K, c), frame_of(pl.look->shapes.facade_parts, c.comp)));
		if (rolled(K, c)) {
			float down = (o.h1 - o.h0) * (K.roll_base + K.roll_step * float(int(c.hash % K.picks) - K.roll_from));
			s.rect(c.f, o.a0, o.a1, o.h1 - down, o.h1, -o.depth + K.roll_out, paint_lin(pl.data->color("shutter")), FIN_METAL);
		}
		return;
	}
	const Paint &p = pl.data->color("shutter");
	s.rect(c.f, o.a0, o.a1, o.h0, o.h1, -o.depth, paint_lin(p), p.finish);
}
} // namespace

PaneDark pane_dark(uint8_t comp) {
	switch (comp) {
		case BAY_WINDOW: return PANE_DARK_PICK;
		case BAY_LOGGIA: case BAY_SHUTTERED: case BAY_GOURMET: return PANE_DARK_QUARTER;
		case BAY_BRISE: case BAY_FRENCH: case BAY_BAY_WINDOW: case BAY_RIBBON: return PANE_DARK_FIFTH;
		default: return PANE_DARK_NONE;
	}
}

Opening opening_of(const Placed &pl, const BayCtx &c) {
	const Data &data = *pl.data;
	const FacadeBayLook &K = pl.look->shapes.facade_bay;
	Opening o;
	const OpeningDef &d = data.openings[c.comp < BAY_COUNT ? c.comp : 0];
	if (!d.has || c.plain) return o;
	o.w = std::min(c.W - d.w_in, d.w_max);
	o.h0 = d.sill;
	o.h1 = std::min(c.H - d.head_in, d.head_max);
	o.depth = d.depth;
	if ((c.comp == BAY_WINDOW || c.comp == BAY_SASH || c.comp == BAY_WIDE) && c.geo && c.geo->bays >= 0) {
		o.w = c.W * c.geo->window_w;
		o.h0 = std::min(c.geo->sill, c.H - K.min_sill_room);
		o.h1 = o.h0 + c.H * c.geo->window_h;
		o.w = std::max(K.min_w, std::min(o.w, c.W - K.w_margin)); // the size is the building's: the table's caps are for the bays that have none
		o.h1 = std::max(o.h0 + K.min_h, std::min(o.h1, c.H - K.head_room));
	}
	if (o.w < K.no_opening || o.h1 <= o.h0) return o;
	o.has = true;
	o.a0 = (c.W - o.w) / 2;
	o.a1 = o.a0 + o.w;
	return o;
}

void emit_reveal(Shell &shell, const BayCtx &c, const Opening &o, float from, float to, const Lin &jamb, const Lin &sill, const Lin &head, int8_t fin,
		float sill_h) {
	Basis jl{ c.f.at(o.a0, 0, 0), c.f.e2, c.f.e1, c.f.e0 }, jr{ c.f.at(o.a1, 0, 0), c.f.e2, c.f.e1, -c.f.e0 };
	shell.rect(jl, from, to, o.h0, o.h1, 0, jamb, fin);
	shell.rect(jr, from, to, o.h0, o.h1, 0, jamb, fin);
	Basis flat{ c.f.o, c.f.e0, c.f.e2, c.f.e1 };
	shell.rect(flat, o.a0, o.a1, from, to, sill_h, sill, fin);
	shell.rect(flat, o.a0, o.a1, from, to, o.h1, head, fin, true);
}

void WallRun::add(const FacadeBayLook &K, const BayCtx &c, int bay, float at_push) {
	const bool joins = last >= 0 && bay == last + 1 && at_push == push && fin == c.wall_fin && width + c.W <= K.wall_run_m && wall.r == c.wall.r && wall.g == c.wall.g && wall.b == c.wall.b;
	if (!joins) {
		close();
		f = c.f;
		width = 0;
		height = c.H;
		push = at_push;
		wall = c.wall;
		fin = c.wall_fin;
	}
	width += c.W;
	last = bay;
}

void WallRun::close() {
	if (last >= 0) shell.rect(f, 0, width, 0, height, 0, wall, fin);
	last = -1;
}

void emit_bay(Shell &shell, Shell &parts, const Placed &pl, const BayCtx &c, WallRun &run, int bay, float push) {
	const FacadeBayLook &K = pl.look->shapes.facade_bay;
	Opening o = opening_of(pl, c);
	if (!o.has) {
		run.add(K, c, bay, push);
		return;
	}
	if (pl.b->cut_level < 0 && flush(K, c, o)) { // a body cut open has clear glass in the reveal (no recess drawn by the shader)
		run.add(K, c, bay, push);
		PaneLook look = pane_look(pl, c, o.a0, o.a1, o.h0, o.h1, dark_pane(K, c), frame_of(pl.look->shapes.facade_parts, c.comp));
		look.recess = o.depth;
		shell.pane(c.f, o.a0, o.a1, o.h0, o.h1, K.pane_on_wall_m, look);
	} else {
		run.close();
		shell.rect(c.f, 0, o.a0, 0, c.H, 0, c.wall, c.wall_fin);
		shell.rect(c.f, o.a1, c.W, 0, c.H, 0, c.wall, c.wall_fin);
		shell.rect(c.f, o.a0, o.a1, 0, o.h0, 0, c.wall, c.wall_fin);
		shell.rect(c.f, o.a0, o.a1, o.h1, c.H, 0, c.wall, c.wall_fin);
		reveals(K, shell, c, o);
		closing(shell, pl, c, o);
	}
	streak(K, shell, c, o);
	emit_bay_parts(shell, parts, pl, c, o);
	if (c.level >= 1) emit_bay_props(parts, pl, c, o);
}

} // namespace city::b40
