// The lot's furniture and its marks on the ground (sim/buildings/b40/lot_*.cpp lays them): car bays (their painted
// lines), lorry docks (the dock plate and the wall bumpers), drains, shrubs, flower beds, a pool with its coping,
// loungers, garden tables, swings, sheds and edicula (walls under an overhanging roof), clothes lines, pallets and caged
// stores. Trees are drawn by the trees' renderer from the same elements. Heights over the building's floor.
#include "render/buildings/facade.h"

#include <cmath>

namespace city::b40 {

namespace {
constexpr float LINE_LIFT = 0.09f; // over the ground covers (lot_space.cpp lays them up to 0.10 m)
constexpr float BUMPER = 0.3f, BUMPER_T = 0.15f, BUMPER_LO = 0.9f, BUMPER_HI = 1.3f, BUMPER_IN = 0.4f;
// A shrub: an eight-sided mass, narrower at its foot and its crown than at its waist, flat shaded like the trees.
constexpr int BUSH_SIDES = 8;
constexpr float BUSH_FOOT = 0.75f, BUSH_WAIST_Z = 0.45f, BUSH_CROWN = 0.55f, BUSH_TOP = 1.05f, SHADE_SPAN = 0.2f, SHADE_LO = 0.9f;
constexpr float TAU = 6.2831853f;
constexpr float SOIL_H = 0.12f, BLOOM = 0.28f, BLOOM_H = 0.14f, BLOOM_EVERY = 0.4f, BLOOM_IN = 0.2f;
constexpr float WATER_LIFT = 0.02f, COPING_W = 0.3f, COPING_OVER = 0.07f; // water over the covers, coping over the water
constexpr float LOUNGER_SEAT = 0.3f, LOUNGER_T = 0.07f, LOUNGER_HEAD = 0.5f, LOUNGER_BACK = 0.65f;
constexpr float TABLE_TOP = 0.05f, LEG = 0.1f, SWING_POST = 0.08f, SWING_SEAT_Z = 0.45f, SWING_SEAT = 0.5f, SWING_SEAT_D = 0.25f;
constexpr float SWING_SEAT_T = 0.05f, ROPE = 0.02f, ROPE_X = 0.22f;
constexpr float SHED_ROOF = 0.15f, SHED_EAVE = 0.3f, EDICULA_AREA = 12.f, DOOR_W = 0.9f, DOOR_H = 2.f, DOOR_OUT = 0.02f;
constexpr float LINE_POST = 0.06f, WIRE = 0.02f, WIRE_DOWN = 0.1f, CLOTH = 0.6f, CLOTH_H = 0.5f, CLOTH_AT = 0.3f;
constexpr float PALLET_T = 0.14f, CAGE_POST = 0.06f, CAGE_RAIL = 0.05f, CAGE_BAR = 0.02f, CAGE_BAR_EVERY = 0.15f;
constexpr float CYLINDER = 0.32f, CYLINDER_H = 0.9f, CYLINDER_EVERY = 0.5f, HALF = 0.5f;
constexpr uint32_t BLUE_GAS = 0x3a6fb0, BUMPER_BLACK = 0x1c1d1f, DOOR_DARK = 0x4a3a2c;
constexpr uint32_t HASH_X = 0x9e3779b1u, HASH_Y = 0x85ebca6bu, HASH_BITS = 0xffff;
constexpr float HASH_Q = 100.f; // centimetres: a shrub's tint follows its place

float jitter(const LotElement &e) {
	uint32_t h = uint32_t(int32_t(e.x * HASH_Q)) * HASH_X ^ uint32_t(int32_t(e.y * HASH_Q)) * HASH_Y;
	return SHADE_LO + SHADE_SPAN * float(h & HASH_BITS) / float(HASH_BITS);
}

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
	void mark(float a0, float a1, float b0, float b1, const Lin &l) const { shell.rect(pl.lot, a0, a1, b0, b1, g + LINE_LIFT, l, FIN_NONE); }

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
		for (float x : { x0() + BUMPER_IN, x1() - BUMPER_IN - BUMPER })
			parts.box(pl.lot, x, x + BUMPER, y0(), y0() + BUMPER_T, g + BUMPER_LO, g + BUMPER_HI, lin_of(BUMPER_BLACK), FIN_NONE);
	}
	void shrub() const {
		const Lin l = lin * jitter(e);
		const float rings[3][2] = { { BUSH_FOOT, 0 }, { 1, BUSH_WAIST_Z }, { BUSH_CROWN, 1 } }; // radius share, height share
		auto at = [&](int ring, int k) {
			float a = TAU * float(k) / float(BUSH_SIDES);
			return pl.lot.at(e.x + std::cos(a) * e.w * HALF * rings[ring][0], e.y + std::sin(a) * e.d * HALF * rings[ring][0], g + e.h * rings[ring][1]);
		};
		const godot::Vector3 top = pl.lot.at(e.x, e.y, g + e.h * BUSH_TOP), mid = pl.lot.at(e.x, e.y, g + e.h * BUSH_WAIST_Z);
		for (int k = 0; k < BUSH_SIDES; k++) {
			int n = (k + 1) % BUSH_SIDES;
			for (int ring = 0; ring < 2; ring++) {
				godot::Vector3 out = (at(ring, k) + at(ring + 1, n)) * HALF - mid;
				shell.quad(at(ring, k), at(ring, n), at(ring + 1, n), at(ring + 1, k), out, l, FIN_NONE);
			}
			shell.tri(at(2, k), at(2, n), top, (at(2, k) + at(2, n) + top) / 3 - mid, l * (1 + SHADE_SPAN * HALF), FIN_NONE);
		}
	}
	void flowers() const {
		shell.box(pl.lot, x0(), x1(), y0(), y1(), g, g + SOIL_H, named("planter_soil"), FIN_NONE, SKIP_E2_NEG);
		const Lin bloom[3] = { lin, named("flowers_alt"), named("hedge") };
		int k = 0;
		for (float y = y0() + BLOOM_IN; y + BLOOM <= y1() - BLOOM_IN + CAGE_BAR; y += BLOOM_EVERY)
			for (float x = x0() + BLOOM_IN; x + BLOOM <= x1() - BLOOM_IN + CAGE_BAR; x += BLOOM_EVERY, k++)
				parts.box(pl.lot, x, x + BLOOM, y, y + BLOOM, g + SOIL_H, g + SOIL_H + BLOOM_H, bloom[k % 3], FIN_NONE);
	}
	// The water just over the ground covers, the stone coping round it higher still: a pool read from above.
	void pool() const {
		const float water = g + LINE_LIFT + WATER_LIFT, top = water + COPING_OVER;
		shell.rect(pl.lot, x0(), x1(), y0(), y1(), water, lin, FIN_NONE);
		const Lin coping = named("pool_coping");
		const float c = COPING_W;
		shell.box(pl.lot, x0() - c, x1() + c, y0() - c, y0(), g, top, coping, FIN_STONE, SKIP_E2_NEG);
		shell.box(pl.lot, x0() - c, x1() + c, y1(), y1() + c, g, top, coping, FIN_STONE, SKIP_E2_NEG);
		shell.box(pl.lot, x0() - c, x0(), y0(), y1(), g, top, coping, FIN_STONE, SKIP_E2_NEG);
		shell.box(pl.lot, x1(), x1() + c, y0(), y1(), g, top, coping, FIN_STONE, SKIP_E2_NEG);
	}
	void lounger() const {
		parts.box(pl.lot, x0(), x1(), y0(), y1(), g + LOUNGER_SEAT - LOUNGER_T, g + LOUNGER_SEAT, lin, FIN_WOOD);
		parts.box(pl.lot, x0(), x1(), y1() - LOUNGER_HEAD, y1(), g + LOUNGER_SEAT, g + LOUNGER_BACK, lin, FIN_WOOD);
	}
	void table() const {
		parts.box(pl.lot, x0(), x1(), y0(), y1(), g + e.h - TABLE_TOP, g + e.h, lin, FIN_WOOD);
		parts.box(pl.lot, e.x - LEG * HALF, e.x + LEG * HALF, e.y - LEG * HALF, e.y + LEG * HALF, g, g + e.h - TABLE_TOP, lin * HALF, FIN_METAL);
	}
	void swing() const {
		for (float x : { x0(), x1() - SWING_POST })
			for (float y : { y0(), y1() - SWING_POST }) parts.box(pl.lot, x, x + SWING_POST, y, y + SWING_POST, g, g + e.h, lin, FIN_METAL);
		parts.box(pl.lot, x0(), x1(), e.y - SWING_POST * HALF, e.y + SWING_POST * HALF, g + e.h - SWING_POST, g + e.h, lin, FIN_METAL);
		for (float dx : { -ROPE_X, ROPE_X })
			parts.box(pl.lot, e.x + dx, e.x + dx + ROPE, e.y, e.y + ROPE, g + SWING_SEAT_Z, g + e.h - SWING_POST, lin, FIN_METAL);
		parts.box(pl.lot, e.x - SWING_SEAT * HALF, e.x + SWING_SEAT * HALF, e.y - SWING_SEAT_D * HALF, e.y + SWING_SEAT_D * HALF, g + SWING_SEAT_Z - SWING_SEAT_T,
				g + SWING_SEAT_Z, named("bench"), FIN_WOOD);
	}
	void shed() const {
		shell.box(pl.lot, x0(), x1(), y0(), y1(), g, g + e.h, lin, pl.b->paints[size_t(e.paint)].finish, SKIP_E2_NEG);
		const bool big = e.w * e.d >= EDICULA_AREA;
		Lin roof = named(big ? "edicula_roof" : "shed_roof");
		shell.box(pl.lot, x0() - SHED_EAVE, x1() + SHED_EAVE, y0() - SHED_EAVE, y1() + SHED_EAVE, g + e.h, g + e.h + SHED_ROOF, roof, big ? FIN_TILE : FIN_METAL);
		const float dw = std::fmin(DOOR_W, e.w * HALF);
		Basis front{ pl.lot.o, pl.lot.e0, pl.lot.e2, -pl.lot.e1 };
		shell.rect(front, e.x - dw * HALF, e.x + dw * HALF, g, g + std::fmin(DOOR_H, e.h), -y0() + DOOR_OUT, lin_of(DOOR_DARK), FIN_WOOD);
	}
	void line() const {
		for (float x : { x0(), x1() - LINE_POST })
			parts.box(pl.lot, x, x + LINE_POST, e.y - LINE_POST * HALF, e.y + LINE_POST * HALF, g, g + e.h, lin, FIN_METAL);
		const float wire = g + e.h - WIRE_DOWN;
		parts.box(pl.lot, x0(), x1(), e.y - WIRE * HALF, e.y + WIRE * HALF, wire - WIRE, wire, lin, FIN_METAL);
		const Lin cloth[2] = { named("flowers"), named("canopy") };
		for (int k = 0; k < 2; k++) { // a cloth folds over the wire: its top half a wire above the wire's (no shared face)
			float a = x0() + (x1() - x0()) * (CLOTH_AT + CLOTH_AT * float(k));
			parts.box(pl.lot, a, a + CLOTH, e.y - WIRE, e.y + WIRE, wire - CLOTH_H, wire + WIRE * HALF, cloth[k], FIN_NONE);
		}
	}
	void pallet() const {
		parts.box(pl.lot, x0(), x1(), y0(), y1(), g, g + PALLET_T, lin, FIN_WOOD);
		parts.box(pl.lot, x0(), x1(), y0(), y1(), g + PALLET_T, g + 2 * PALLET_T, lin * (1 - SHADE_SPAN * HALF), FIN_WOOD);
	}
	void cage() const {
		for (float x : { x0(), x1() - CAGE_POST })
			for (float y : { y0(), y1() - CAGE_POST }) parts.box(pl.lot, x, x + CAGE_POST, y, y + CAGE_POST, g, g + e.h, lin, FIN_METAL);
		parts.box(pl.lot, x0(), x1(), y0(), y1(), g + e.h - CAGE_RAIL, g + e.h, lin, FIN_METAL);
		for (float x = x0() + CAGE_BAR_EVERY; x < x1() - CAGE_POST; x += CAGE_BAR_EVERY)
			parts.box(pl.lot, x, x + CAGE_BAR, y0(), y0() + CAGE_BAR, g, g + e.h, lin, FIN_METAL);
		for (float x = x0() + CYLINDER; x + CYLINDER < x1(); x += CYLINDER_EVERY)
			parts.box(pl.lot, x, x + CYLINDER, e.y - CYLINDER * HALF, e.y + CYLINDER * HALF, g, g + CYLINDER_H, lin_of(BLUE_GAS), FIN_METAL);
	}
};
} // namespace

bool emit_lot_prop(Shell &shell, Shell &parts, const Placed &pl, const LotElement &e, float g, const Lin &lin) {
	PropEmit p{ shell, parts, pl, e, g, lin };
	switch (e.kind) {
		case EL_STALL: p.stall(); return true;
		case EL_DOCK: p.dock(); return true;
		case EL_DRAIN: p.mark(p.x0(), p.x1(), p.y0(), p.y1(), lin); return true;
		case EL_TREE: return true; // the trees' renderer draws it
		case EL_SHRUB: p.shrub(); return true;
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
