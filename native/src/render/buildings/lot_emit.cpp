// The lot elements of a placed body: its covers as lot_covers.h lays them on the ground (lawns drawn as grass by the shader),
// boundary walls with plinth and coping, fences and railings on their base walls, hedges, gates standing open (their posts, and their leaves
// swung into the lot), planters,
// benches, bollards, lamps (lit heads), bins, stall lines, wall ACs, and the building-hung canopies, awnings, veranda floors, slabs
// and posts.
#include "render/buildings/lot_covers.h"

#include "core/polygon_ops.h"
#include "sim/buildings/b40/bench.h"

#include <cmath>

using namespace godot;

namespace city::b40 {

namespace {
constexpr float FOOT = 0.3f, PIECE = 4.f, COPING_H = 0.07f, COPING_OUT = 0.04f, COPING_SHADE = 1.f;
constexpr float SPLASH = 0.22f, SPLASH_SHADE = 0.78f, RAIL_T = 0.04f, RAIL_LOW = 0.45f, LOW_RAIL_H = 0.06f, POST = 0.12f, POST_EVERY = 2.f, POST_OVER = 0.05f;
constexpr float LIGHT_POST = 0.06f, LIGHT_EVERY = 0.45f, BAR_EVERY = 0.14f, GATE_POST = 0.17f, GATE_RAIL = 0.08f;
constexpr float LAMP_HEAD = 0.44f, LAMP_HEAD_H = 0.12f, GARDEN_LAMP_STEM = 0.12f, BOLLARD_BAND = 0.25f, BOLLARD_BAND_H = 0.1f, BOLLARD_BAND_OUT = 0.01f;
constexpr float BENCH_SEAT = 0.4f, BENCH_SEAT_T = 0.06f, BENCH_BACK = 0.95f, BENCH_BACK_T = 0.06f;
constexpr float FASCIA_SHADE = 0.95f, FASCIA_H = 0.2f, AWN_SHADE = 0.8f, HALF = 0.5f, GRILLE_T = 0.02f;
constexpr uint32_t BAND_LIGHT = 0xffe8c8, LAMP_WARM = 0xf1ebdd;
constexpr float FAR_COVER_RANKS = 16.f; // the far level: the covers of a lot stand its allowed error apart over this many, one on another

struct LotEmit {
	Shell &shell, &parts;
	const Placed &pl;
	float floor_y;
	float ground(float x, float y) const { return pl.ground(pl.lot.at(x, y, 0)) - floor_y; }
	Lin paint(const LotElement &e, const char *fallback) const {
		return e.paint >= 0 ? lin_of(pl.b->paints[size_t(e.paint)].rgb) : paint_lin(pl.data->color(fallback));
	}
	int8_t fin(const LotElement &e) const { return e.paint >= 0 ? pl.b->paints[size_t(e.paint)].finish : FIN_CONCRETE; }
	// A box of the lot's frame held to the lot's own land (its plan's width either side of the front's middle, its depth back): what
	// stands out of a boundary piece's footprint (a coping's lip, a plinth, a gate's posts) is laid on the lot's side of its lines, never on
	// the footway nor in the neighbour's (render/buildings/building_renderer_probe.cpp lot_past_lot). A body with no lot known keeps it.
	void on_land(float &x0, float &x1, float &y0, float &y1) const {
		const float W = pl.b->plan.W, D = pl.b->plan.D;
		if (W <= 0 || D <= 0) return;
		x0 = std::max(x0, -W * HALF);
		x1 = std::min(x1, W * HALF);
		y0 = std::max(y0, 0.f);
		y1 = std::min(y1, D);
	}
	void cover(const CoverPiece &c);
	void wall(const LotElement &e, float x0, float x1, float y0, float y1);
	void fence(const LotElement &e, float x0, float x1, float y0, float y1, bool light);
	void gate(const LotElement &e);
	void prop(const LotElement &e);
	void hung(const LotElement &e);
	void element(const LotElement &e);
};

// A piece of a cover as lot_covers lays it: a lawn in the terrain's grass, any other in its paint. At the far level (pl.lean) a lawn
// is left to the land's grass under it, which it is, and a paved cover stands a share of the level's allowed error over the one laid
// before it: covers lie millimetres apart (tuning lot.cover_step_m), which the depth buffer does not tell apart kilometres away
// (Godot's manual, 3D rendering limitations: "move the Z-fighting objects further apart without the difference being visible").
void LotEmit::cover(const CoverPiece &c) {
	const LotElement &e = pl.b->elements[size_t(c.element)];
	const bool lawn = e.paint >= 0 && pl.b->paints[size_t(e.paint)].finish == FIN_NONE;
	if (!pl.lean) {
		Shell &to = lawn && shell.lawns ? *shell.lawns : shell;
		to.poly(c.p, c.count, pl.lot.e2, paint(e, "paving"), fin(e), lawn ? SK_LAWN : SK_GROUND);
		return;
	}
	if (lawn || std::min(e.w, e.d) < shell.box_least) return; // nor a cover narrower than the least width still drawn where this level begins
	Vector3 raised[COVER_POLY_MOST];
	const Vector3 up = pl.lot.e2 * (float(c.rank) * shell.quad_least / FAR_COVER_RANKS);
	for (int i = 0; i < c.count; i++) raised[i] = c.p[i] + up;
	shell.poly(raised, c.count, pl.lot.e2, paint(e, "paving"), fin(e), SK_GROUND);
}

void LotEmit::wall(const LotElement &e, float x0, float x1, float y0, float y1) {
	float g = ground((x0 + x1) / 2, (y0 + y1) / 2);
	float lo = std::min(ground(x0, y0), ground(x1, y1)) - FOOT, top = g + e.h;
	Lin lin = paint(e, "concrete");
	int8_t f = fin(e);
	shell.grime_floor = floor_y + g;
	on_land(x0, x1, y0, y1);
	if (pl.coarse) {
		shell.box(pl.lot, x0, x1, y0, y1, lo, top, lin, f, SKIP_E2_NEG);
		return;
	}
	shell.box(pl.lot, x0, x1, y0, y1, g + SPLASH, top - COPING_H, lin, f, SKIP_E2_NEG | SKIP_E2_POS);
	// The plinth and the coping proud of both faces of the wall's thickness, on the lot's own land: on a line the lot shares with a
	// neighbour or the street, the lip stands inward alone (the neighbour's wall on the line makes the pair one wall proud both ways).
	const float o = COPING_OUT, across_x = x1 - x0 < y1 - y0 ? o : 0, across_y = x1 - x0 >= y1 - y0 ? o : 0;
	float px0 = x0 - across_x, px1 = x1 + across_x, py0 = y0 - across_y, py1 = y1 + across_y;
	on_land(px0, px1, py0, py1);
	shell.box(pl.lot, px0, px1, py0, py1, lo, g + SPLASH, lin * SPLASH_SHADE, f, SKIP_E2_NEG);
	shell.box(pl.lot, px0, px1, py0, py1, top - COPING_H, top, paint_lin(pl.data->color("wall_coping")) * COPING_SHADE, FIN_STONE, SKIP_E2_NEG);
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
		float bx0 = along_x ? a0 : cx - t / 2, bx1 = along_x ? a1 : cx + t / 2, by0 = along_x ? cy - t / 2 : a0, by1 = along_x ? cy + t / 2 : a1;
		on_land(bx0, bx1, by0, by1);
		parts.box(pl.lot, bx0, bx1, by0, by1, z0, z1, m, FIN_METAL);
	};
	float s0 = along_x ? x0 : y0;
	bar(s0, s0 + len, top - COPING_H, top, RAIL_T);
	bar(s0, s0 + len, base + (top - base) * RAIL_LOW, base + (top - base) * RAIL_LOW + LOW_RAIL_H, RAIL_T);
	if (pl.coarse && !light) return; // a fence's posts stand far apart and go with the near level; a railing's close ones read together
	float every = light ? LIGHT_EVERY : POST_EVERY, pw = light ? LIGHT_POST : POST;
	for (float a = s0; a <= s0 + len + EPS; a += every) bar(std::min(a, s0 + len - pw), std::min(a, s0 + len - pw) + pw, base, top + POST_OVER, pw);
}

// A gate stands open while its lot is lived in, as the reference's gates stand: its posts proud of the run either side, and its leaves hung on
// them swung into the lot along the opening's sides, so a car drives and a person walks through with nothing of the gate in the way: as many
// as the rules hang in its opening (data.h LotRules::gate_leaves), each its share of the opening long; a leaf is its bottom and top rails, its
// stiles at the hinge and at the free end, and its bars between the rails.
void LotEmit::gate(const LotElement &e) {
	float x0 = e.x - e.w / 2, x1 = e.x + e.w / 2, g = ground(e.x, e.y);
	Lin m = paint(e, "gate");
	// Every piece of the gate on the lot's own land.
	const auto piece = [&](float a0, float a1, float b0, float b1, float z0, float z1) {
		on_land(a0, a1, b0, b1);
		parts.box(pl.lot, a0, a1, b0, b1, z0, z1, m, FIN_METAL);
	};
	const float bar = pl.look->fence_bar_m;
	for (float x : { x0 - GATE_POST, x1 }) piece(x, x + GATE_POST, e.y - GATE_POST / 2, e.y + GATE_POST / 2, g - FOOT, g + e.h + GATE_RAIL);
	const int leaves = pl.data->lots.gate_leaves(e.w);
	const float reach = e.w / float(leaves); // a leaf's length
	const float in = e.facing == FACE_BACK ? -1.f : 1.f; // the way into the lot
	const float root = e.y + in * e.d * HALF; // the leaves hang from the run's inner face
	const auto leaf = [&](float lx0) {
		// A piece of the leaf from s0 to s1 along it (from its hinge), at heights z0..z1.
		const auto along = [&](float s0, float s1, float z0, float z1) {
			const float a = root + in * s0, b = root + in * s1;
			piece(lx0, lx0 + bar, std::min(a, b), std::max(a, b), z0, z1);
		};
		along(0, reach, g + GATE_RAIL, g + 2 * GATE_RAIL);
		along(0, reach, g + e.h - GATE_RAIL, g + e.h);
		along(0, bar, g + GATE_RAIL, g + e.h);
		along(reach - bar, reach, g + GATE_RAIL, g + e.h);
		for (float s = BAR_EVERY; s < reach - bar; s += BAR_EVERY) along(s, s + bar, g + 2 * GATE_RAIL, g + e.h - GATE_RAIL);
	};
	leaf(x0);
	if (leaves > 1) leaf(x1 - bar);
}

void LotEmit::prop(const LotElement &e) {
	float x0 = e.x - e.w / 2, x1 = e.x + e.w / 2, y0 = e.y - e.d / 2, y1 = e.y + e.d / 2, g = ground(e.x, e.y);
	Lin lin = paint(e, e.kind == EL_PARKING ? "parking_asphalt" : "concrete");
	int8_t f = fin(e);
	switch (e.kind) {
		case EL_PLANTER:
			shell.box(pl.lot, x0, x1, y0, y1, g - FOOT, g + e.h, lin, f, SKIP_E2_NEG | SKIP_E2_POS);
			shell.rect(pl.lot, x0 + PLANTER_SOIL_DROP_M, x1 - PLANTER_SOIL_DROP_M, y0 + PLANTER_SOIL_DROP_M, y1 - PLANTER_SOIL_DROP_M, g + e.h - PLANTER_SOIL_DROP_M,
					paint_lin(pl.data->color("planter_soil")), FIN_NONE); // the soil inside the rim, where what grows in it stands (types.h)
			break;
		case EL_BENCH: {
			// The seat, its back on the side away from where the bench looks (types.h bench_back), an end at each end of its length.
			const Footprint back = bench_back(e, BENCH_BACK_T);
			parts.box(pl.lot, x0, x1, y0, y1, g + BENCH_SEAT, g + BENCH_SEAT + BENCH_SEAT_T, lin, FIN_WOOD);
			parts.box(pl.lot, back.x0, back.x1, back.y0, back.y1, g + BENCH_SEAT, g + BENCH_BACK, lin, FIN_WOOD);
			if (looks_along_y(e)) {
				parts.box(pl.lot, x0, x0 + BENCH_BACK_T, y0, y1, g, g + BENCH_SEAT, lin, FIN_WOOD);
				parts.box(pl.lot, x1 - BENCH_BACK_T, x1, y0, y1, g, g + BENCH_SEAT, lin, FIN_WOOD);
			} else {
				parts.box(pl.lot, x0, x1, y0, y0 + BENCH_BACK_T, g, g + BENCH_SEAT, lin, FIN_WOOD);
				parts.box(pl.lot, x0, x1, y1 - BENCH_BACK_T, y1, g, g + BENCH_SEAT, lin, FIN_WOOD);
			}
			break;
		}
		case EL_BOLLARD:
			parts.box(pl.lot, x0, x1, y0, y1, g, g + e.h, lin, FIN_METAL);
			parts.box(pl.lot, x0 - BOLLARD_BAND_OUT, x1 + BOLLARD_BAND_OUT, y0 - BOLLARD_BAND_OUT, y1 + BOLLARD_BAND_OUT, g + e.h - BOLLARD_BAND, g + e.h - BOLLARD_BAND + BOLLARD_BAND_H, lin_of(BAND_LIGHT), FIN_NONE);
			break;
		case EL_LAMP: {
			if (e.z > 0) { // on a wall: the lamp's head and its lit glass under it
				shell.box(pl.lot, x0, x1, y0, y1, e.z, e.z + e.h, lin, FIN_METAL);
				shell.quad(pl.lot.at(x0, y0, e.z - GRILLE_T), pl.lot.at(x1, y0, e.z - GRILLE_T), pl.lot.at(x1, y1, e.z - GRILLE_T), pl.lot.at(x0, y1, e.z - GRILLE_T),
						-pl.lot.e2, lin_of(LAMP_WARM), FIN_NONE, SK_LAMP);
				break;
			}
			parts.box(pl.lot, e.x - GARDEN_LAMP_STEM / 2, e.x + GARDEN_LAMP_STEM / 2, e.y - GARDEN_LAMP_STEM / 2, e.y + GARDEN_LAMP_STEM / 2, g, g + e.h, lin, FIN_METAL);
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
	// the far level: a run on ground level within what that level may be off is one piece
	if (pl.lean && std::fabs(ground(x0, y0) - ground(x1, y1)) < shell.quad_least) pieces = 1;
	if (pl.coarse && e.kind != EL_SURFACE && e.kind != EL_WALL && e.kind != EL_HEDGE && e.kind != EL_FENCE && e.kind != EL_RAILING && e.kind != EL_POOL) return;
	if (!pl.coarse && e.kind == EL_HEDGE) return; // near, a hedge is the gardens' plants (render/trees/garden_place.h)
	switch (e.kind) {
		case EL_SURFACE: break; // the covers are laid together, on one ground (emit_lot)
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
	local.angle = 0; // in its own frame the piece lies straight: who reads its outline there (element_shape.h) reads it unturned
	LotEmit{ shell, parts, at, pl.lot.o.y }.element(local);
}

// The lawn of a lot that is no rectangle (Body::lot_ring), laid to the lot's own outline on its ground, under every cover the plan
// laid on its rectangle: the land between the plan and the lot's lines is the lot's garden, not bare ground. In the terrain's grass,
// as the plan's lawns are; the far level leaves it to the land's own grass.
void outline_lawn(Shell &lawns, const Placed &pl, const LotEmit &le) {
	const std::vector<Point2> &ring = pl.b->lot_ring;
	std::vector<double> x, y;
	for (const Point2 &p : ring) {
		x.push_back(p.x);
		y.push_back(p.y);
	}
	const std::vector<int> tris = ring_triangles(x, y);
	for (size_t t = 0; t + 2 < tris.size(); t += 3) {
		Vector3 corner[3];
		for (int k = 0; k < 3; k++) {
			const Point2 &p = ring[size_t(tris[t + size_t(k)])];
			corner[k] = pl.lot.at(p.x, p.y, le.ground(p.x, p.y) + pl.look->lot_plate);
		}
		lawns.poly(corner, 3, pl.lot.e2, paint_lin(pl.data->color("lawn")), FIN_NONE, SK_LAWN);
	}
}
} // namespace

void emit_lot(Shell &shell, Shell &parts, const Placed &pl) {
	LotEmit le{ shell, parts, pl, pl.lot.o.y };
	const Body &b = *pl.b;
	if (!b.lot_ring.empty() && !pl.lean && shell.lawns) outline_lawn(*shell.lawns, pl, le);
	lot_covers(pl, [&le](const CoverPiece &c) { le.cover(c); });
	for (const LotElement &e : b.elements) {
		if (cut_away(b, e)) continue; // cut open: what hangs above the floor seen is gone
		if (e.angle != 0) turned_element(shell, parts, pl, e);
		else le.element(e);
	}
}

} // namespace city::b40
