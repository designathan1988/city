// The lot's furniture and its marks on the ground (sim/buildings/b40/lot_*.cpp lays them): car bays (their painted
// lines), lorry docks (the dock plate and the wall bumpers), drains, shrubs, flower beds, a pool with its coping,
// loungers, garden tables, swings, sheds and edicula (walls under an overhanging roof), clothes lines, pallets and caged
// stores. Trees are drawn by the trees' renderer from the same elements. Heights over the building's floor.
#include "render/buildings/facade.h"

#include <cmath>

namespace city::b40 {

namespace {
constexpr float HALF = 0.5f;
constexpr uint32_t BLUE_GAS = 0x3a6fb0, BUMPER_BLACK = 0x1c1d1f, DOOR_DARK = 0x4a3a2c; // gas cylinders, bay bumpers, a shed's door (the source game's)

struct PropEmit {
	Shell &shell, &parts;
	const Placed &pl;
	const LotElement &e;
	float g;
	Lin lin;
	float x0() const { return e.x - e.w * HALF; }
	float x1() const { return e.x + e.w * HALF; }
	float y0() const { return e.y - e.d * HALF; }
	float y1() const { return e.y + e.d * HALF; }
	Lin named(const char *n) const { return paint_lin(pl.data->color(n)); }
	void mark(float a0, float a1, float b0, float b1, const Lin &l) const { shell.rect(pl.lot, a0, a1, b0, b1, g + K.line_lift, l, FIN_NONE); }

	void stall() const {
		const float t = pl.data->lots.stall_line * HALF;
		const bool along_y = e.facing == FACE_STREET || e.facing == FACE_BACK;
		if (along_y) {
			mark(x0() - t, x0() + t, y0(), y1(), lin);
			mark(x1() - t, x1() + t, y0(), y1(), lin);
			float closed = e.facing == FACE_STREET ? y1() : y0();
			mark(x0(), x1(), closed - t, closed + t, lin);
		} else {
			mark(x0(), x1(), y0() - t, y0() + t, lin);
			mark(x0(), x1(), y1() - t, y1() + t, lin);
			float closed = e.facing == FACE_RIGHT ? x0() : x1();
			mark(closed - t, closed + t, y0(), y1(), lin);
		}
	}
	void dock() const {
		mark(x0(), x1(), y0(), y1(), lin);
		for (float x : { x0() + K.bumper_in, x1() - K.bumper_in - K.bumper })
			parts.box(pl.lot, x, x + K.bumper, y0(), y0() + K.bumper_t, g + K.bumper_lo, g + K.bumper_hi, lin_of(BUMPER_BLACK), FIN_NONE);
	}
	// The bed's kerb and its soil; the flowers are the gardens' plants (render/trees/garden_place.h).
	void flowers() const {
		shell.box(pl.lot, x0(), x1(), y0(), y1(), g, g + K.bed_kerb_h, named("planter"), FIN_NONE, SKIP_E2_NEG);
		shell.box(pl.lot, x0() + K.bed_kerb_w, x1() - K.bed_kerb_w, y0() + K.bed_kerb_w, y1() - K.bed_kerb_w, g + K.bed_kerb_h, g + K.bed_soil_h, named("planter_soil"), FIN_NONE, SKIP_E2_NEG);
	}
	// The water just over the ground covers, the stone coping round it higher still: a pool read from above.
	void pool() const {
		const float water = g + K.line_lift + K.water_lift, top = water + K.coping_over;
		shell.rect(pl.lot, x0(), x1(), y0(), y1(), water, lin, FIN_NONE);
		const Lin coping = named("pool_coping");
		const float c = K.coping_w;
		shell.box(pl.lot, x0() - c, x1() + c, y0() - c, y0(), g, top, coping, FIN_STONE, SKIP_E2_NEG);
		shell.box(pl.lot, x0() - c, x1() + c, y1(), y1() + c, g, top, coping, FIN_STONE, SKIP_E2_NEG);
		shell.box(pl.lot, x0() - c, x0(), y0(), y1(), g, top, coping, FIN_STONE, SKIP_E2_NEG);
		shell.box(pl.lot, x1(), x1() + c, y0(), y1(), g, top, coping, FIN_STONE, SKIP_E2_NEG);
	}
	void lounger() const {
		parts.box(pl.lot, x0(), x1(), y0(), y1(), g + K.lounger_seat - K.lounger_t, g + K.lounger_seat, lin, FIN_WOOD);
		parts.box(pl.lot, x0(), x1(), y1() - K.lounger_head, y1(), g + K.lounger_seat, g + K.lounger_back, lin, FIN_WOOD);
	}
	void table() const {
		parts.box(pl.lot, x0(), x1(), y0(), y1(), g + e.h - K.table_top, g + e.h, lin, FIN_WOOD);
		parts.box(pl.lot, e.x - K.leg * HALF, e.x + K.leg * HALF, e.y - K.leg * HALF, e.y + K.leg * HALF, g, g + e.h - K.table_top, lin * HALF, FIN_METAL);
	}
	void swing() const {
		for (float x : { x0(), x1() - K.swing_post })
			for (float y : { y0(), y1() - K.swing_post }) parts.box(pl.lot, x, x + K.swing_post, y, y + K.swing_post, g, g + e.h, lin, FIN_METAL);
		parts.box(pl.lot, x0(), x1(), e.y - K.swing_post * HALF, e.y + K.swing_post * HALF, g + e.h - K.swing_post, g + e.h, lin, FIN_METAL);
		for (float dx : { -K.rope_x, K.rope_x })
			parts.box(pl.lot, e.x + dx, e.x + dx + K.rope, e.y, e.y + K.rope, g + K.swing_seat_z, g + e.h - K.swing_post, lin, FIN_METAL);
		parts.box(pl.lot, e.x - K.swing_seat * HALF, e.x + K.swing_seat * HALF, e.y - K.swing_seat_d * HALF, e.y + K.swing_seat_d * HALF, g + K.swing_seat_z - K.swing_seat_t,
				g + K.swing_seat_z, named("bench"), FIN_WOOD);
	}
	void shed() const {
		shell.box(pl.lot, x0(), x1(), y0(), y1(), g, g + e.h, lin, pl.b->paints[size_t(e.paint)].finish, SKIP_E2_NEG);
		const bool big = e.w * e.d >= K.edicula_area;
		Lin roof = named(big ? "edicula_roof" : "shed_roof");
		shell.box(pl.lot, x0() - K.shed_eave, x1() + K.shed_eave, y0() - K.shed_eave, y1() + K.shed_eave, g + e.h, g + e.h + K.shed_roof, roof, big ? FIN_TILE : FIN_METAL);
		const float dw = std::fmin(K.door_w, e.w * HALF);
		Basis front{ pl.lot.o, pl.lot.e0, pl.lot.e2, -pl.lot.e1 };
		shell.rect(front, e.x - dw * HALF, e.x + dw * HALF, g, g + std::fmin(K.door_h, e.h), -y0() + K.door_out, lin_of(DOOR_DARK), FIN_WOOD);
	}
	void line() const {
		for (float x : { x0(), x1() - K.line_post })
			parts.box(pl.lot, x, x + K.line_post, e.y - K.line_post * HALF, e.y + K.line_post * HALF, g, g + e.h, lin, FIN_METAL);
		const float wire = g + e.h - K.wire_down;
		parts.box(pl.lot, x0(), x1(), e.y - K.wire * HALF, e.y + K.wire * HALF, wire - K.wire, wire, lin, FIN_METAL);
		const Lin cloth[2] = { named("flowers"), named("canopy") };
		for (int k = 0; k < 2; k++) { // a cloth folds over the wire: its top half a wire above the wire's (no shared face)
			float a = x0() + (x1() - x0()) * (K.cloth_at + K.cloth_at * float(k));
			parts.box(pl.lot, a, a + K.cloth, e.y - K.wire, e.y + K.wire, wire - K.cloth_h, wire + K.wire * HALF, cloth[k], FIN_NONE);
		}
	}
	void pallet() const {
		parts.box(pl.lot, x0(), x1(), y0(), y1(), g, g + K.pallet_t, lin, FIN_WOOD);
		parts.box(pl.lot, x0(), x1(), y0(), y1(), g + K.pallet_t, g + 2 * K.pallet_t, lin * (1 - K.shade_span * HALF), FIN_WOOD);
	}
	void cage() const {
		for (float x : { x0(), x1() - K.cage_post })
			for (float y : { y0(), y1() - K.cage_post }) parts.box(pl.lot, x, x + K.cage_post, y, y + K.cage_post, g, g + e.h, lin, FIN_METAL);
		parts.box(pl.lot, x0(), x1(), y0(), y1(), g + e.h - K.cage_rail, g + e.h, lin, FIN_METAL);
		for (float x = x0() + K.cage_bar_every; x < x1() - K.cage_post; x += K.cage_bar_every)
			parts.box(pl.lot, x, x + K.cage_bar, y0(), y0() + K.cage_bar, g, g + e.h, lin, FIN_METAL);
		for (float x = x0() + K.cylinder; x + K.cylinder < x1(); x += K.cylinder_every)
			parts.box(pl.lot, x, x + K.cylinder, e.y - K.cylinder * HALF, e.y + K.cylinder * HALF, g, g + K.cylinder_h, lin_of(BLUE_GAS), FIN_METAL);
	}
	const LotPropsLook &K = pl.look->shapes.lot_props; // last: it reads pl
};
} // namespace

bool emit_lot_prop(Shell &shell, Shell &parts, const Placed &pl, const LotElement &e, float g, const Lin &lin) {
	PropEmit p{ shell, parts, pl, e, g, lin };
	switch (e.kind) {
		case EL_STALL: p.stall(); return true;
		case EL_DOCK: p.dock(); return true;
		case EL_DRAIN: p.mark(p.x0(), p.x1(), p.y0(), p.y1(), lin); return true;
		case EL_TREE: return true; // the gardens' plants (render/trees/garden_place.h)
		case EL_SHRUB: return true; // the gardens' plants
		case EL_FLOWERS: p.flowers(); return true;
		case EL_POOL: p.pool(); return true;
		case EL_LOUNGER: p.lounger(); return true;
		case EL_TABLE: p.table(); return true;
		case EL_SWING: p.swing(); return true;
		case EL_SHED: p.shed(); return true;
		case EL_LINE: p.line(); return true;
		case EL_PALLET: p.pallet(); return true;
		case EL_CAGE: p.cage(); return true;
		default: return false;
	}
}

} // namespace city::b40
