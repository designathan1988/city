// The parts of each bay component: frames, sills, balcony slabs and railings,
// doors with canopies and lamps, shopfront awnings, loggia glass parapets, brise racks, moulded surrounds with open
// louvred shutters, gourmet balconies, transoms. Every box is an instance in `parts` (a row of a band, not 20 vertices in the chunk's
// mesh); what is no box (railing cards, awnings, door panels, glass guards) stays where its material is.
#include "render/buildings/facade_bay.h"
#include "core/shader_contracts/buildings.h"

namespace city::b40 {

namespace {
constexpr float HALF = 0.5f;
constexpr uint32_t SHUTTER_BROWN = 0x7a5538, SHUTTER_BLUE = 0x557392; // shutters on a wall whose trim gives no colour (the source game's)

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
		float x = am + w / 2 + K.lamp_off;
		if (x + K.lamp_w / 2 > c.W - K.lamp_room) x = am - w / 2 - K.lamp_off;
		const float hh = o.h1 - K.lamp_drop;
		part_lamp(parts, at(x, hh, K.lamp_out, K.lamp_w, K.lamp_h, K.lamp_d));
		const float half = std::min({ K.wash_half, x + c.wall_before, c.W - x + c.wall_after }), up = std::min(K.wash_up, hh + K.wash_foot); // the wall goes on past the bay
		if (half > 0) shell.wash(c.f, x - half, x + half, hh - up, hh + up, K.wash_out);
	}
	void canopy(float pad, float t, float out, float fascia, float room) const {
		float cz = std::min(o.h1 + K.canopy_rise, c.H - room);
		part_box(parts, at(am, cz, -out / 2, w + pad, t, out), paint("sill"), FIN_CONCRETE);
		part_box(parts, at(am, cz - K.fascia_drop, -out + K.fascia_t / 2, w + pad, fascia, K.fascia_t), paint("sill"), FIN_CONCRETE);
		for (float s : { -1.f, 1.f })
			part_box(parts, at(am + s * (w / 2 + K.bracket_off), cz - K.bracket_h * HALF - K.canopy_rise * HALF, -K.bracket_d / 2, K.bracket_w, K.bracket_h, K.bracket_d), c.trim, PART_PAINT);
	}
	// The leaves are parts, drawn only to parts_end: a door-coloured panel behind them (at the back of the leaf
	// box, so it never covers a leaf) closes the opening when they are culled.
	void door_back(float y0, float y1) const {
		const Paint &p = pl.data->color("door");
		shell.rect(c.f, o.a0, o.a1, y0, y1, -(o.depth + K.frame_in + K.frame_d / 2), paint_lin(p), p.finish);
	}
	// A glass guard standing on the floor at `floor`: the pane up to its cap, the cap's top at the guard height.
	void glass_guard(float a, float floor, float depth, float width) const {
		const float pane = pl.look->guard_h_m - K.cap_h;
		// only where there is a surface for what is seen through: on the opaque mid level the glass would be a black mirror
		if (shell.sheer) part_box(*shell.sheer, at(a, floor + pane / 2, depth, width, pane, K.loggia_pane), paint("glass_rail"), FIN_GLASS);
		part_box(parts, at(a, floor + pane + K.cap_h / 2, depth, width, K.cap_h, K.cap_t), c.trim, PART_PAINT);
	}
	bool stands_out();
	void door();
	void double_door();
	void gourmet();
	void shuttered();
	const FacadePartsLook &K = pl.look->shapes.facade_parts; // last: it reads pl
};

// What stands out of the wall alike on the near and the mid level. False: this component has nothing of the kind.
bool Ctx::stands_out() {
	switch (c.comp) {
		case BAY_BALCONY:
			part_box(parts, at(c.W / 2, -K.bal_slab / 2, -K.bal_out / 2, c.W - K.bal_edge, K.bal_slab, K.bal_out), paint("sill"), FIN_CONCRETE);
			part_railing(shell, at(c.W / 2, 0, 0, c.W - K.bal_edge, pl.look->guard_h_m, K.bal_out), c.trim, c.trim_fin);
			return true;
		case BAY_SHOPFRONT: {
			uint32_t aw = pl.data->shop_awnings[size_t(pl.b->palette) % pl.data->shop_awnings.size()];
			part_awning(pl.look->shapes.part_models, shell, at(c.W / 2, o.h1 + K.awning_up, 0, c.W - K.awning_trim, K.awning_h, K.bal_out), lin_of(aw), FIN_PLASTER);
			return true;
		}
		case BAY_LOGGIA: glass_guard(am, o.h0, K.glass_rail_in, w); return true;
		case BAY_BRISE: part_brise(parts, at(c.W / 2, c.H / 2, -K.brise_out / 2, c.W, c.H, K.brise_out), c.trim, PART_PAINT); return true;
		case BAY_SHUTTERED: shuttered(); return true;
		case BAY_GOURMET: gourmet(); return true;
		default: return false;
	}
}

void Ctx::door() {
	part_door(pl.look->shapes.part_models, parts, at(am, hm, o.depth + K.frame_in, w, h, K.frame_d), paint("door"), FIN_WOOD);
	door_back(o.h0, o.h1);
	if (c.level == 0) { canopy(K.canopy_pad, K.canopy_t, K.canopy_out, K.fascia_h, K.canopy_room); lamp(); }
}

void Ctx::double_door() {
	float leaf = std::min(h, K.leaf_max);
	for (float s : { -1.f, 1.f }) {
		part_door(pl.look->shapes.part_models, parts, at(am + s * w / 4, o.h0 + leaf / 2, o.depth + K.frame_in, w / 2 - K.leaf_gap, leaf, K.frame_d), paint("door"), FIN_WOOD);
		part_box(parts, at(am + s * w / 4, o.h0 + K.light_at * leaf, o.depth - K.leaf_gap, w / 2 - K.light_inset, leaf * K.light_share, K.loggia_pane),
				paint("pane"), FIN_GLASS);
	}
	const bool fanlight = h > leaf + K.fan_min;
	door_back(o.h0, fanlight ? o.h0 + leaf + K.fan_gap : o.h1);
	if (fanlight) { // glass over the leaves (a double door's fanlight: the opening's only glass)
		float fan = h - leaf - K.fan_gap, fy = o.h0 + leaf + K.fan_gap + fan / 2;
		shell.pane(c.f, o.a0, o.a1, fy - fan / 2, o.h1, -o.depth, pane_look(pl, c, o.a0, o.a1, fy - fan / 2, o.h1, false, K.frame_thin));
		part_box(parts, at(am, o.h0 + leaf + K.fan_gap / 2, o.depth - K.frame_in, w, K.fan_gap, K.transom_t), c.trim, PART_PAINT);
	}
	if (c.level == 0) { canopy(K.dd_pad, K.dd_t, K.dd_out, K.dd_fascia, K.dd_room); lamp(); }
}

void Ctx::gourmet() {
	Lin slab = c.wall * K.g_shade;
	part_box(parts, at(c.W / 2, -K.g_slab_t / 2, -K.g_out / 2, c.W, K.g_slab_t, K.g_out), slab, c.wall_fin);
	part_box(parts, at(c.W / 2, -K.g_beam_drop, -K.g_beam_out, c.W, K.g_beam_h, K.g_beam_t), slab, c.wall_fin);
	float r0 = c.open_left ? 0 : K.g_end, r1 = c.open_right ? c.W : c.W - K.g_end;
	glass_guard((r0 + r1) / 2, 0, K.g_rail_out, r1 - r0);
	if (!c.open_left) part_box(parts, at(K.g_end, c.H / 2 - K.g_wall_drop, -K.garden_wall_u, K.g_wall_t, c.H, K.g_wall_d), slab, c.wall_fin);
	if (!c.open_right) part_box(parts, at(c.W - K.g_end, c.H / 2 - K.g_wall_drop, -K.garden_wall_u, K.g_wall_t, c.H, K.g_wall_d), slab, c.wall_fin);
	else part_box(parts, at(c.W, pl.look->guard_h_m / 2, -K.g_post_out, K.frame_d, pl.look->guard_h_m, K.g_post_d), c.trim, PART_PAINT);
}

void Ctx::shuttered() {
	part_box(parts, at(am, o.h1 + K.surround / 2, K.surround_out, w + 2 * K.surround, K.surround, K.surround_t), c.trim, PART_PAINT);
	for (float s : { -1.f, 1.f }) part_box(parts, at(am + s * (w / 2 + K.surround / 2), hm, K.surround_out, K.surround, h, K.surround_t), c.trim, PART_PAINT);
	if (o.a0 >= w / 2 + K.surround) {
		const Paint &lv = pl.data->color("louvre_default");
		Lin tc = c.trim;
		bool chroma = std::max({ tc.r, tc.g, tc.b }) - std::min({ tc.r, tc.g, tc.b }) > K.shutter_chroma && std::max({ tc.r, tc.g, tc.b }) < K.shutter_max;
		Lin col = chroma ? tc : c.wall.b > c.wall.r ? lin_of(SHUTTER_BROWN) : c.wall.g > c.wall.b ? lin_of(SHUTTER_BLUE) : paint_lin(lv);
		for (float s : { -1.f, 1.f }) {
			Part p = at(am + s * (K.louvre_spread * w + K.surround), hm, K.louvre_out, w / 2, h, K.louvre_t);
			part_box(parts, p, col, PART_PAINT);
			if (!coarse) part_louvre(pl.look->shapes.part_models, parts, Part{ p.f, p.a, p.h, p.depth - K.louvre_t, p.sx, p.sy, K.louvre_t }, col, PART_PAINT);
		}
	}
	part_box(parts, at(am, o.h0 - K.shut_sill_t / 2, (o.depth - K.shut_sill_out) / 2, w + K.shut_sill_past, K.shut_sill_t, o.depth + K.shut_sill_out), paint("sill"), FIN_CONCRETE);
}
} // namespace

void emit_bay_relief(Shell &shell, Shell &parts, const Placed &pl, const BayCtx &c, const Opening &o) {
	Ctx x{ shell, parts, pl, c, o, c.W / 2, (o.h0 + o.h1) / 2, o.w, o.h1 - o.h0, true };
	if (x.stands_out()) return;
	if (c.level != 0) return;
	if (c.comp == BAY_DOOR) x.canopy(x.K.canopy_pad, x.K.canopy_t, x.K.canopy_out, x.K.fascia_h, x.K.canopy_room);
	if (c.comp == BAY_DOUBLE_DOOR) x.canopy(x.K.dd_pad, x.K.dd_t, x.K.dd_out, x.K.dd_fascia, x.K.dd_room);
}

void emit_bay_parts(Shell &shell, Shell &parts, const Placed &pl, const BayCtx &c, const Opening &o) {
	Ctx x{ shell, parts, pl, c, o, c.W / 2, (o.h0 + o.h1) / 2, o.w, o.h1 - o.h0 };
	if (x.stands_out()) return;
	switch (c.comp) {
		case BAY_WINDOW: case BAY_SASH: x.sill(SILL_PAST_M, SILL_T_M, SILL_OUT_M); break;
		case BAY_DOOR: x.door(); break;
		case BAY_DOUBLE_DOOR: x.double_door(); break;
		case BAY_LOADING_DOOR: case BAY_GARAGE_DOOR:
			part_box(parts, x.at(x.am, x.hm, o.depth - x.K.frame_in, x.w, x.h, x.K.frame_in), x.paint("shutter"), FIN_METAL);
			break;
		case BAY_FRENCH: case BAY_BAY_WINDOW: case BAY_RIBBON:
			if (x.h > x.K.transom_min) part_box(parts, x.at(x.am, o.h0 + x.K.transom_at * x.h, o.depth - x.K.frame_in, x.w, x.K.transom_t, SILL_OUT_M), c.trim, PART_PAINT);
			if (o.h0 > x.K.window_sill_min) x.sill(SILL_PAST_M, SILL_T_M, SILL_OUT_M);
			break;
		default: break;
	}
}

} // namespace city::b40
