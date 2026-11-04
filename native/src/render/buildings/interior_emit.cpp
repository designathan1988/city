// The inside of a building cut open at a floor: the floor of every block that reaches it, the cores (a lift shaft with its
// door and its car standing at the floor), the walls of the plan with their doorways and the furniture. The walls facing the
// camera come down as the outside ones do (interior_cut.cpp).
#include "render/buildings/interior_emit.h"

#include "sim/buildings/b40/plan_area.h"
#include "sim/buildings/interior/cut.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city::b40 {

namespace {
constexpr float FLOOR_LIFT = 0.04f; // the floor over the storey's slab: it never fights the slab's face
constexpr float WALL_HALF = 0.06f; // an inner wall is 12 cm thick
constexpr float WALL_CLEAR = 0.4f, WALL_MAX = 2.7f; // an inner wall stands the storey's height less this, at most this high
constexpr float LOWER_DOT = 0.6f; // a wall whose normal is this close to the camera's line comes down
constexpr float SHAFT_WALL = 0.2f, SHAFT_CLEAR = 0.3f; // the shaft's walls, and how far under the next floor it stops
constexpr float LIFT_DOOR_W = 1.2f, DOOR_SHARE = 0.5f, DOOR_H = 2.2f; // the shaft's door at the floor
constexpr float CAB_GAP = 0.1f, CAB_LIFT = 0.05f, CAB_H = 2.3f; // the car in the shaft

Lin lin_of_paint(const Paint &p) { return paint_lin(p); }

// The floor of each block that reaches it.
void floors(Shell &shell, const Placed &pl, const InteriorLook &look, int cut, FloorFinish finish) {
	const Paint &p = look.paint(finish == FLOOR_WOOD ? "floor_wood" : finish == FLOOR_STONE ? "floor_stone" : "floor_porcelain");
	const Lin lin = lin_of_paint(p);
	const float z = pl.b->level_z(cut) + FLOOR_LIFT;
	for (const Volume &v : pl.b->volumes) {
		if (!(v.base <= cut && v.top() > cut)) continue;
		for (const Ring &t : plan_triangles(ring_of(v)))
			shell.tri(pl.lot.at(t[0].x, t[0].y, z), pl.lot.at(t[1].x, t[1].y, z), pl.lot.at(t[2].x, t[2].y, z), pl.lot.e2, lin, p.finish);
	}
}

// A lift shaft: four walls (the front one with a doorway) from the floor up under the next one, and the car at this floor.
void cores(Shell &shell, const Placed &pl, const InteriorLook &look, int cut) {
	const Body &b = *pl.b;
	const Paint &shaft = look.paint("shaft"), &cab = look.paint("cab");
	const Lin sl = lin_of_paint(shaft), cl = lin_of_paint(cab);
	const float z = b.level_z(cut), top = b.level_z(cut + 1) - SHAFT_CLEAR;
	for (const Core &c : b.cores) {
		if (cut > c.to) continue;
		const float x0 = c.x, y0 = c.y, x1 = c.x + b40::CORE_SIDE_M, y1 = c.y + b40::CORE_SIDE_M;
		shell.box(pl.lot, x0, x1, y1 - SHAFT_WALL, y1, z, top, sl, shaft.finish, SKIP_E2_NEG);
		shell.box(pl.lot, x0, x0 + SHAFT_WALL, y0, y1 - SHAFT_WALL, z, top, sl, shaft.finish, SKIP_E2_NEG);
		shell.box(pl.lot, x1 - SHAFT_WALL, x1, y0, y1 - SHAFT_WALL, z, top, sl, shaft.finish, SKIP_E2_NEG);
		const float door = std::min(b40::CORE_SIDE_M * DOOR_SHARE, LIFT_DOOR_W), mid = (x0 + x1) / 2;
		shell.box(pl.lot, x0 + SHAFT_WALL, mid - door / 2, y0, y0 + SHAFT_WALL, z, top, sl, shaft.finish, SKIP_E2_NEG);
		shell.box(pl.lot, mid + door / 2, x1 - SHAFT_WALL, y0, y0 + SHAFT_WALL, z, top, sl, shaft.finish, SKIP_E2_NEG);
		shell.box(pl.lot, mid - door / 2, mid + door / 2, y0, y0 + SHAFT_WALL, z + DOOR_H, top, sl, shaft.finish, SKIP_E2_NEG);
		shell.box(pl.lot, x0 + SHAFT_WALL + CAB_GAP, x1 - SHAFT_WALL - CAB_GAP, y0 + SHAFT_WALL + CAB_GAP, y1 - SHAFT_WALL - CAB_GAP, z + CAB_LIFT,
				z + CAB_H, cl, cab.finish, SKIP_E2_NEG);
	}
}

// The walls of the plan, with their doorways (they come from the interior's own plan): the ones that face the camera come down.
void partitions(Shell &shell, const Placed &pl, const InteriorLook &look, int cut, const FloorInterior &floor) {
	const Body &b = *pl.b;
	const Paint &p = look.paint("partition");
	const Lin lin = lin_of_paint(p);
	const float z = b.level_z(cut), rise = std::min(b.level_h(cut) - WALL_CLEAR, WALL_MAX);
	for (const Partition &w : floor.partitions) {
		const float len = std::max(std::hypot(w.x1 - w.x0, w.y1 - w.y0), EPS);
		// The wall's normal against the camera's line: near 1 means it faces the camera.
		const float facing = std::abs(-(w.y1 - w.y0) / len * b.cut_view.x + (w.x1 - w.x0) / len * b.cut_view.y);
		const float top = z + (facing > LOWER_DOT ? CUT_WALL_LOW_M : rise);
		shell.box(pl.lot, std::min(w.x0, w.x1) - WALL_HALF, std::max(w.x0, w.x1) + WALL_HALF, std::min(w.y0, w.y1) - WALL_HALF,
				std::max(w.y0, w.y1) + WALL_HALF, z, top, lin, p.finish, SKIP_E2_NEG);
	}
}
} // namespace

void emit_interior(Shell &shell, Shell &parts, const Placed &pl) {
	const Body &b = *pl.b;
	const int cut = b.cut_level;
	if (cut < 0 || !pl.whole) return;
	Interior inside(*pl.data, *pl.whole, purpose_of(*pl.whole, pl.fn, pl.id));
	const InteriorLook &look = pl.look->interior;
	const FloorInterior &floor = inside.at(cut);
	floors(shell, pl, look, cut, floor_finish(inside.purpose(), cut, pl.id));
	cores(shell, pl, look, cut);
	partitions(shell, pl, look, cut, floor);
	const float z = b.level_z(cut);
	for (const Furniture &f : floor.furniture) emit_furniture(shell, parts, pl, f, z);
}

} // namespace city::b40
