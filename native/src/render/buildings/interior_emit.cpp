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

// The floor of each block that reaches it.
void floors(Shell &shell, const Placed &pl, const InteriorLook &look, int cut, FloorFinish finish) {
	const InteriorEmitLook &K = pl.look->shapes.interior_emit;
	const Paint &p = look.paint(finish == FLOOR_WOOD ? "floor_wood" : finish == FLOOR_STONE ? "floor_stone" : "floor_porcelain");
	const Lin lin = paint_lin(p);
	for (const Volume &v : pl.b->volumes) {
		if (!(v.base <= cut && v.top() > cut)) continue;
		const float z = pl.b->block_z(v, cut) + K.floor_lift;
		for (const Ring &t : plan_triangles(ring_of(v)))
			shell.tri(pl.lot.at(t[0].x, t[0].y, z), pl.lot.at(t[1].x, t[1].y, z), pl.lot.at(t[2].x, t[2].y, z), pl.lot.e2, lin, p.finish);
	}
}

// A lift shaft: four walls (the front one with a doorway) from the floor up under the next one, and the car at this floor.
void cores(Shell &shell, const Placed &pl, const InteriorLook &look, int cut) {
	const InteriorEmitLook &K = pl.look->shapes.interior_emit;
	const Body &b = *pl.b;
	const Paint &shaft = look.paint("shaft"), &cab = look.paint("cab"), &step = look.paint("step");
	const Lin sl = paint_lin(shaft), cl = paint_lin(cab), tl = paint_lin(step);
	for (const Core &c : b.cores) {
		if (cut > c.to) continue;
		const float lift = lift_at(*pl.whole, cut, c.x + b40::core_width(c) / 2, c.y + b40::CORE_SIDE_M / 2);
		const float z = lift + b.level_z(cut), top = lift + b.level_z(cut + 1) - K.shaft_clear;
		const float x0 = c.x, y0 = c.y, x1 = c.x + b40::core_width(c), y1 = c.y + b40::CORE_SIDE_M;
		shell.box(pl.lot, x0, x1, y1 - K.shaft_wall, y1, z, top, sl, shaft.finish, SKIP_E2_NEG);
		shell.box(pl.lot, x0, x0 + K.shaft_wall, y0, y1 - K.shaft_wall, z, top, sl, shaft.finish, SKIP_E2_NEG);
		shell.box(pl.lot, x1 - K.shaft_wall, x1, y0, y1 - K.shaft_wall, z, top, sl, shaft.finish, SKIP_E2_NEG);
		const float door = std::min(b40::core_width(c) * K.lift_door_share, K.lift_door_w), mid = (x0 + x1) / 2;
		shell.box(pl.lot, x0 + K.shaft_wall, mid - door / 2, y0, y0 + K.shaft_wall, z, top, sl, shaft.finish, SKIP_E2_NEG);
		shell.box(pl.lot, mid + door / 2, x1 - K.shaft_wall, y0, y0 + K.shaft_wall, z, top, sl, shaft.finish, SKIP_E2_NEG);
		shell.box(pl.lot, mid - door / 2, mid + door / 2, y0, y0 + K.shaft_wall, z + K.door_h, top, sl, shaft.finish, SKIP_E2_NEG);
		if (c.kind != b40::CORE_STAIR)
			shell.box(pl.lot, x0 + K.shaft_wall + K.cab_gap, x1 - K.shaft_wall - K.cab_gap, y0 + K.shaft_wall + K.cab_gap, y1 - K.shaft_wall - K.cab_gap, z + K.cab_lift,
					z + K.cab_h, cl, cab.finish, SKIP_E2_NEG);
		if (c.kind == b40::CORE_LIFT) continue;
		const float run = (x1 - x0 - 2 * K.shaft_wall) / float(K.flight_steps), rise = b.level_h(cut) / float(K.flight_steps), back = (y0 + y1) / 2; // along the back of the shaft
		for (int k = 0; k < K.flight_steps; k++) {
			const float sx = x0 + K.shaft_wall + run * float(k);
			shell.box(pl.lot, sx, sx + run, back, y1 - K.shaft_wall, z, z + rise * float(k + 1), tl, step.finish, SKIP_E2_NEG);
		}
	}
}

// The flights between blocks at floors of their own, step on step from the lower floor.
void flights(Shell &shell, const Placed &pl, const InteriorLook &look, int cut) {
	const Paint &step = look.paint("step");
	const Lin lin = paint_lin(step);
	for (const LevelFlight &f : level_flights(*pl.data, *pl.whole, cut)) {
		const float run = std::hypot(f.top.x - f.foot.x, f.top.y - f.foot.y), ux = (f.top.x - f.foot.x) / run, uy = (f.top.y - f.foot.y) / run;
		const float tread = run / float(f.risers), rise = (f.z1 - f.z0) / float(f.risers), half = f.width / 2;
		for (int k = 0; k < f.risers; k++) {
			const V2 a{ f.foot.x + ux * tread * float(k), f.foot.y + uy * tread * float(k) }, c{ f.foot.x + ux * tread * float(k + 1), f.foot.y + uy * tread * float(k + 1) };
			shell.box(pl.lot, std::min(a.x, c.x) - std::fabs(uy) * half, std::max(a.x, c.x) + std::fabs(uy) * half, std::min(a.y, c.y) - std::fabs(ux) * half,
					std::max(a.y, c.y) + std::fabs(ux) * half, f.z0, f.z0 + rise * float(k + 1), lin, step.finish, SKIP_E2_NEG);
		}
	}
}

// The walls of the plan, with their doorways (they come from the interior's own plan): the ones that face the camera come down.
void partitions(Shell &shell, const Placed &pl, const InteriorLook &look, int cut, const FloorInterior &floor) {
	const InteriorEmitLook &K = pl.look->shapes.interior_emit;
	const Body &b = *pl.b;
	const Paint &p = look.paint("partition");
	const Lin lin = paint_lin(p);
	const float rise = std::min(b.level_h(cut) - K.wall_clear, K.wall_max);
	for (const Partition &w : floor.partitions) {
		const float z = lift_at(*pl.whole, cut, (w.x0 + w.x1) / 2, (w.y0 + w.y1) / 2) + b.level_z(cut);
		const float len = std::max(std::hypot(w.x1 - w.x0, w.y1 - w.y0), EPS);
		// The wall's normal against the camera's line: near 1 means it faces the camera.
		const float facing = std::abs(-(w.y1 - w.y0) / len * b.cut_view.x + (w.x1 - w.x0) / len * b.cut_view.y);
		const float top = z + (facing > K.lower_dot ? CUT_WALL_LOW_M : rise);
		shell.box(pl.lot, std::min(w.x0, w.x1) - K.wall_half, std::max(w.x0, w.x1) + K.wall_half, std::min(w.y0, w.y1) - K.wall_half,
				std::max(w.y0, w.y1) + K.wall_half, z, top, lin, p.finish, SKIP_E2_NEG);
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
	floors(shell, pl, look, cut, floor_finish(*pl.data, inside.purpose(), cut, pl.id));
	cores(shell, pl, look, cut);
	partitions(shell, pl, look, cut, floor);
	flights(shell, pl, look, cut);
	for (const Furniture &f : floor.furniture) emit_furniture(shell, parts, pl, f, lift_at(*pl.whole, cut, f.x, f.y) + b.level_z(cut));
}

} // namespace city::b40
