// The parts of each bay component: frames, sills, balcony slabs and railings,
// doors with canopies and lamps, shopfront awnings, loggia glass parapets, brise racks, moulded surrounds with open
// louvred shutters, gourmet balconies, transoms. Every box is an instance in `parts` (a row of a band, not 20 vertices in the chunk's
// mesh); what is no box (railing cards, awnings, door panels, glass guards) stays where its material is.
#include "render/buildings/facade_bay.h"

namespace city::b40 {

namespace {
constexpr float FRAME_D = 0.06f, FRAME_THIN = 0.05f, FRAME_IN = 0.03f, SILL_PAST = 0.14f, SILL_T = 0.06f, SILL_OUT = 0.07f;
constexpr float BAL_EDGE = 0.3f, BAL_SLAB = 0.18f, BAL_OUT = 1.3f;
constexpr float CANOPY_PAD = 0.7f, CANOPY_T = 0.12f, CANOPY_OUT = 0.9f, FASCIA_H = 0.2f, FASCIA_T = 0.05f, FASCIA_DROP = 0.04f;
constexpr float CANOPY_RISE = 0.3f, CANOPY_ROOM = 0.48f, BRACKET_W = 0.06f, BRACKET_H = 0.3f, BRACKET_D = 0.5f, BRACKET_OFF = 0.2f;
constexpr float LEAF_MAX = 2.5f, LEAF_GAP = 0.04f, LIGHT_INSET = 0.5f, LIGHT_SHARE = 0.5f, LIGHT_AT = 0.62f, FAN_MIN = 0.4f;
constexpr float FAN_GAP = 0.12f, TRANSOM_T = 0.08f, DD_PAD = 1.f, DD_T = 0.14f, DD_OUT = 1.1f, DD_FASCIA = 0.24f, DD_ROOM = 0.49f;
constexpr float AWNING_TRIM = 0.2f, AWNING_H = 0.75f, AWNING_UP = 0.45f;
constexpr float LOGGIA_PANE = 0.02f, GLASS_RAIL_IN = -0.02f, CAP_H = 0.05f, CAP_T = 0.06f;
constexpr float BRISE_OUT = 0.6f, SURROUND = 0.14f, SURROUND_T = 0.06f, SURROUND_OUT = -0.03f, LOUVRE_T = 0.04f, LOUVRE_OUT = -0.05f;
constexpr float LOUVRE_SPREAD = 0.75f, SHUT_SILL_PAST = 0.38f, SHUT_SILL_T = 0.08f, SHUT_SILL_OUT = 0.1f;
constexpr float G_SLAB_T = 0.24f, G_OUT = 1.6f, G_BEAM_H = 0.18f, G_BEAM_T = 0.16f, G_BEAM_DROP = 0.2f, G_BEAM_OUT = 1.54f;
constexpr float G_SHADE = 0.92f, G_END = 0.103f, G_RAIL_OUT = 1.56f, G_WALL_T = 0.2f, G_WALL_D = 1.661f, G_WALL_DROP = 0.307f;
constexpr float GARDEN_WALL_U = 0.8195f, G_POST_D = 1.4f, G_POST_OUT = 0.9f, TRANSOM_MIN = 2.2f, TRANSOM_AT = 0.72f;
constexpr float WINDOW_SILL_MIN = 0.3f, LAMP_W = 0.16f, LAMP_H = 0.26f, LAMP_D = 0.12f, LAMP_OFF = 0.4f, LAMP_DROP = 0.25f;
constexpr float LAMP_OUT = -0.06f, LAMP_ROOM = 0.13f, HALF = 0.5f, SHUTTER_CHROMA = 0.06f, SHUTTER_MAX = 0.75f;
constexpr uint32_t SHUTTER_BROWN = 0x7a5538, SHUTTER_BLUE = 0x557392;

struct Ctx {
	Shell &shell, &parts;
	const Placed &pl;
	const BayCtx &c;
	const Opening &o;
	float am, hm, w, h;
	bool coarse = false; // the mid level: no louvres
	Lin paint(const char *n) const { return paint_lin(pl.data->color(n)); }
	Part at(float a, float hh, float depth, float sx, float sy, float sz) const { return Part{ &c.f, a, hh, depth, sx, sy, sz, pl.look }; }
	void sill(float past, float t, float out) const {
		part_box(parts, at(am, o.h0 - t / 2, (o.depth - out) / 2, w + past, t, o.depth + out), paint("sill"), FIN_CONCRETE);
	}
	void lamp() const {
		float x = am + w / 2 + LAMP_OFF;
		if (x + LAMP_W / 2 > c.W - LAMP_ROOM) x = am - w / 2 - LAMP_OFF;
		part_lamp(parts, at(x, o.h1 - LAMP_DROP, LAMP_OUT, LAMP_W, LAMP_H, LAMP_D));
	}
	void canopy(float pad, float t, float out, float fascia, float room) const {
		float cz = std::min(o.h1 + CANOPY_RISE, c.H - room);
		part_box(parts, at(am, cz, -out / 2, w + pad, t, out), paint("sill"), FIN_CONCRETE);
		part_box(parts, at(am, cz - FASCIA_DROP, -out + FASCIA_T / 2, w + pad, fascia, FASCIA_T), paint("sill"), FIN_CONCRETE);
		for (float s : { -1.f, 1.f })
			part_box(parts, at(am + s * (w / 2 + BRACKET_OFF), cz - BRACKET_H * HALF - CANOPY_RISE * HALF, -BRACKET_D / 2, BRACKET_W, BRACKET_H, BRACKET_D), c.trim, PART_PAINT);
	}
	// The leaves are parts, drawn only to parts_end: a door-coloured panel behind them (at the back of the leaf
	// box, so it never covers a leaf) closes the opening when they are culled.
	void door_back(float y0, float y1) const {
		const Paint &p = pl.data->color("door");
		shell.rect(c.f, o.a0, o.a1, y0, y1, -(o.depth + FRAME_IN + FRAME_D / 2), paint_lin(p), p.finish);
	}
	// A glass guard standing on the floor at `floor`: the pane up to its cap, the cap's top at the guard height.
	void glass_guard(float a, float floor, float depth, float width) const {
		const float pane = pl.look->guard_h_m - CAP_H;
		part_box(shell.sheer ? *shell.sheer : shell, at(a, floor + pane / 2, depth, width, pane, LOGGIA_PANE), paint("glass_rail"), FIN_GLASS);
		part_box(parts, at(a, floor + pane + CAP_H / 2, depth, width, CAP_H, CAP_T), c.trim, PART_PAINT);
	}
	void door();
	void double_door();
	void gourmet();
	void shuttered();
};

void Ctx::door() {
	part_door(parts, at(am, hm, o.depth + FRAME_IN, w, h, FRAME_D), paint("door"), FIN_WOOD);
	door_back(o.h0, o.h1);
	if (c.level == 0) { canopy(CANOPY_PAD, CANOPY_T, CANOPY_OUT, FASCIA_H, CANOPY_ROOM); lamp(); }
}

void Ctx::double_door() {
	float leaf = std::min(h, LEAF_MAX);
	for (float s : { -1.f, 1.f }) {
		part_door(parts, at(am + s * w / 4, o.h0 + leaf / 2, o.depth + FRAME_IN, w / 2 - LEAF_GAP, leaf, FRAME_D), paint("door"), FIN_WOOD);
		part_box(parts, at(am + s * w / 4, o.h0 + LIGHT_AT * leaf, o.depth - LEAF_GAP, w / 2 - LIGHT_INSET, leaf * LIGHT_SHARE, LOGGIA_PANE),
				paint("pane"), FIN_GLASS);
	}
	const bool fanlight = h > leaf + FAN_MIN;
	door_back(o.h0, fanlight ? o.h0 + leaf + FAN_GAP : o.h1);
	if (fanlight) { // glass over the leaves (a double door's fanlight: the opening's only glass)
		float fan = h - leaf - FAN_GAP, fy = o.h0 + leaf + FAN_GAP + fan / 2;
		shell.pane(c.f, o.a0, o.a1, fy - fan / 2, o.h1, -o.depth, pane_look(pl, c, o.a0, o.a1, fy - fan / 2, o.h1, false, FRAME_THIN));
		part_box(parts, at(am, o.h0 + leaf + FAN_GAP / 2, o.depth - FRAME_IN, w, FAN_GAP, TRANSOM_T), c.trim, PART_PAINT);
	}
	if (c.level == 0) { canopy(DD_PAD, DD_T, DD_OUT, DD_FASCIA, DD_ROOM); lamp(); }
}

void Ctx::gourmet() {
	Lin slab = c.wall * G_SHADE;
	part_box(parts, at(c.W / 2, -G_SLAB_T / 2, -G_OUT / 2, c.W, G_SLAB_T, G_OUT), slab, c.wall_fin);
	part_box(parts, at(c.W / 2, -G_BEAM_DROP, -G_BEAM_OUT, c.W, G_BEAM_H, G_BEAM_T), slab, c.wall_fin);
	float r0 = c.open_left ? 0 : G_END, r1 = c.open_right ? c.W : c.W - G_END;
	glass_guard((r0 + r1) / 2, 0, G_RAIL_OUT, r1 - r0);
	if (!c.open_left) part_box(parts, at(G_END, c.H / 2 - G_WALL_DROP, -GARDEN_WALL_U, G_WALL_T, c.H, G_WALL_D), slab, c.wall_fin);
	if (!c.open_right) part_box(parts, at(c.W - G_END, c.H / 2 - G_WALL_DROP, -GARDEN_WALL_U, G_WALL_T, c.H, G_WALL_D), slab, c.wall_fin);
	else part_box(parts, at(c.W, pl.look->guard_h_m / 2, -G_POST_OUT, FRAME_D, pl.look->guard_h_m, G_POST_D), c.trim, PART_PAINT);
}

void Ctx::shuttered() {
	part_box(parts, at(am, o.h1 + SURROUND / 2, SURROUND_OUT, w + 2 * SURROUND, SURROUND, SURROUND_T), c.trim, PART_PAINT);
	for (float s : { -1.f, 1.f }) part_box(parts, at(am + s * (w / 2 + SURROUND / 2), hm, SURROUND_OUT, SURROUND, h, SURROUND_T), c.trim, PART_PAINT);
	if (o.a0 >= w / 2 + SURROUND) {
		const Paint &lv = pl.data->color("louvre_default");
		Lin tc = c.trim;
		bool chroma = std::max({ tc.r, tc.g, tc.b }) - std::min({ tc.r, tc.g, tc.b }) > SHUTTER_CHROMA && std::max({ tc.r, tc.g, tc.b }) < SHUTTER_MAX;
		Lin col = chroma ? tc : c.wall.b > c.wall.r ? lin_of(SHUTTER_BROWN) : c.wall.g > c.wall.b ? lin_of(SHUTTER_BLUE) : paint_lin(lv);
		for (float s : { -1.f, 1.f }) {
			Part p = at(am + s * (LOUVRE_SPREAD * w + SURROUND), hm, LOUVRE_OUT, w / 2, h, LOUVRE_T);
			part_box(parts, p, col, FIN_WOOD);
			if (!coarse) part_louvre(parts, Part{ p.f, p.a, p.h, p.depth - LOUVRE_T, p.sx, p.sy, LOUVRE_T }, col, FIN_WOOD);
		}
	}
	part_box(parts, at(am, o.h0 - SHUT_SILL_T / 2, (o.depth - SHUT_SILL_OUT) / 2, w + SHUT_SILL_PAST, SHUT_SILL_T, o.depth + SHUT_SILL_OUT), paint("sill"), FIN_CONCRETE);
}
} // namespace

void emit_bay_relief(Shell &shell, Shell &parts, const Placed &pl, const BayCtx &c, const Opening &o) {
	Ctx x{ parts, parts, pl, c, o, c.W / 2, (o.h0 + o.h1) / 2, o.w, o.h1 - o.h0, true };
	switch (c.comp) {
		case BAY_BALCONY:
			part_box(parts, x.at(c.W / 2, -BAL_SLAB / 2, -BAL_OUT / 2, c.W - BAL_EDGE, BAL_SLAB, BAL_OUT), x.paint("sill"), FIN_CONCRETE);
			part_railing(shell, x.at(c.W / 2, 0, 0, c.W - BAL_EDGE, pl.look->guard_h_m, BAL_OUT), c.trim, c.trim_fin);
			break;
		case BAY_SHOPFRONT: {
			uint32_t aw = pl.data->shop_awnings[size_t(pl.b->palette) % pl.data->shop_awnings.size()];
			part_awning(shell, x.at(c.W / 2, o.h1 + AWNING_UP, 0, c.W - AWNING_TRIM, AWNING_H, BAL_OUT), lin_of(aw), FIN_PLASTER);
			break;
		}
		case BAY_DOOR:
			if (c.level == 0) x.canopy(CANOPY_PAD, CANOPY_T, CANOPY_OUT, FASCIA_H, CANOPY_ROOM);
			break;
		case BAY_DOUBLE_DOOR:
			if (c.level == 0) x.canopy(DD_PAD, DD_T, DD_OUT, DD_FASCIA, DD_ROOM);
			break;
		case BAY_LOGGIA: x.glass_guard(x.am, o.h0, GLASS_RAIL_IN, x.w); break;
		case BAY_BRISE: part_brise(parts, x.at(c.W / 2, c.H / 2, -BRISE_OUT / 2, c.W, c.H, BRISE_OUT), c.trim, PART_PAINT); break;
		case BAY_SHUTTERED: x.shuttered(); break;
		case BAY_GOURMET: x.gourmet(); break;
		default: break;
	}
}

void emit_bay_parts(Shell &shell, Shell &parts, const Placed &pl, const BayCtx &c, const Opening &o) {
	Ctx x{ shell, parts, pl, c, o, c.W / 2, (o.h0 + o.h1) / 2, o.w, o.h1 - o.h0 };
	switch (c.comp) {
		case BAY_WINDOW: case BAY_SASH: x.sill(SILL_PAST, SILL_T, SILL_OUT); break;
		case BAY_BALCONY:
			part_box(parts, x.at(c.W / 2, -BAL_SLAB / 2, -BAL_OUT / 2, c.W - BAL_EDGE, BAL_SLAB, BAL_OUT), x.paint("sill"), FIN_CONCRETE);
			part_railing(shell, x.at(c.W / 2, 0, 0, c.W - BAL_EDGE, pl.look->guard_h_m, BAL_OUT), c.trim, c.trim_fin);
			break;
		case BAY_DOOR: x.door(); break;
		case BAY_DOUBLE_DOOR: x.double_door(); break;
		case BAY_SHOPFRONT: {
			uint32_t aw = pl.data->shop_awnings[size_t(pl.b->palette) % pl.data->shop_awnings.size()];
			part_awning(shell, x.at(c.W / 2, o.h1 + AWNING_UP, 0, c.W - AWNING_TRIM, AWNING_H, BAL_OUT), lin_of(aw), FIN_PLASTER);
			break;
		}
		case BAY_LOADING_DOOR: case BAY_GARAGE_DOOR:
			part_box(parts, x.at(x.am, x.hm, o.depth - FRAME_IN, x.w, x.h, FRAME_IN), x.paint("shutter"), FIN_METAL);
			break;
		case BAY_LOGGIA:
			x.glass_guard(x.am, o.h0, GLASS_RAIL_IN, x.w);
			break;
		case BAY_BRISE:
			part_brise(parts, x.at(c.W / 2, c.H / 2, -BRISE_OUT / 2, c.W, c.H, BRISE_OUT), c.trim, PART_PAINT);
			break;
		case BAY_SHUTTERED: x.shuttered(); break;
		case BAY_GOURMET: x.gourmet(); break;
		case BAY_FRENCH: case BAY_BAY_WINDOW: case BAY_RIBBON:
			if (x.h > TRANSOM_MIN) part_box(parts, x.at(x.am, o.h0 + TRANSOM_AT * x.h, o.depth - FRAME_IN, x.w, TRANSOM_T, SILL_OUT), c.trim, PART_PAINT);
			if (o.h0 > WINDOW_SILL_MIN) x.sill(SILL_PAST, SILL_T, SILL_OUT);
			break;
		default: break;
	}
}

} // namespace city::b40
