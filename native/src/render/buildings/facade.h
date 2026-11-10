// A body placed in the world and the emitters that draw it: the placement (lot basis,
// floor height, building shade, ground sampler), the volume sides as facade frames, the paints of a bay, and the entry
// points of each emitter file.
#pragma once

#include "render/buildings/building_look.h"
#include "sim/buildings/b40/data.h"
#include "render/buildings/shell.h"

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

namespace city::b40 {

struct PaneRooms;

// What keeps a placement's bodies and rooms alive while it is drawn (the body cache is shared: building_bodies.h).
struct PlacedHold {
	std::shared_ptr<const Body> whole, cut;
	std::shared_ptr<PaneRooms> rooms;
};

struct Placed {
	const Data *data = nullptr;
	const BuildingLook *look = nullptr; // sizes of the thin drawn pieces (fence bars, part frames, air handlers)
	const Body *b = nullptr;
	const Body *whole = nullptr; // the body of the record: `b` is the cut one while a building is opened (Interior layer)
	uint8_t fn = 0; // the record's function (BuildingFn): what its floors are for
	PaneRooms *rooms = nullptr; // the rooms behind its panes (none for a cut body: its panes are clear glass)
	Basis lot; // o: middle of the lot front at floor height; e0 lot x, e1 lot y (away from the street), e2 up
	uint32_t id = 0; // the record seed (bay hashes, roof plant, flat roof pick)
	float shade = 1; // a building's own shade on its walls
	float ground_lo = 0; // lowest ground under the ground volumes, relative to the floor (<= 0)
	std::function<float(const godot::Vector3 &)> ground; // terrain height under a world point
	bool coarse = false; // mid level: emit_far draws walls, slabs and parapets; roofs/lot add only the big pieces
	bool lean = false; // far level: what its nearest distance shows (far_mass.cpp): no coping, roof planes alone, a box a roof piece
	PlacedHold hold; // set by BuildingBodies::place: `b`, `whole` and `rooms` point into it
};

// The frame a block is drawn in: the lot's, raised by the block's lift. What a block draws, it draws through the frame, so a split level needs nothing else of the emitters.
inline const Placed &block_frame(const Placed &pl, const Volume &v, Placed &raised) {
	if (v.lift == 0) return pl;
	raised = pl;
	raised.lot.o += pl.lot.e2 * v.lift;
	raised.ground_lo = std::min(pl.ground_lo - v.lift, 0.f);
	return raised;
}

// One face of a volume at a height: the frame (e0 along the face left to right seen from outside, e1 up, e2 out),
// its length and its plan endpoints (lot frame).
struct Side {
	Basis f;
	float length = 0;
	int side = 0;
	Point2 p0, p1, n;
};

std::vector<Side> sides_of(const Placed &pl, const Volume &v, float z);
// True when the plan point (lot frame) lies inside another closed volume standing at `level`.
bool covered(const Body &b, const Volume &self, int level, Point2 p);
Lin wall_lin(const Placed &pl, const Volume &v, int storey, int side);
Lin trim_lin(const Placed &pl);
Lin paint_lin(const Paint &p);
// Moulding (bands, cornices, coping) paint: the trim, or plaster wall x 1.1 when the trim is metal.
Paint moulding_of(const Placed &pl);
Paint flat_roof_of(const Placed &pl, const Volume &v); // a flat roof's slab
// A two-tier band: lower tier 0.45 out from -h/2 to -0.1h, upper tier from -0.1h to +h/2;
// `flush`: one strip.
void band(Shell &s, const Side &sd, float z, float out, float h, const Lin &lin, int8_t fin, bool flush);
uint32_t bay_hash(int vol, int level, int side, int index, float bx, float by);

// Emitters. `parts` takes the small repeated pieces (frames, railings, louvres, AC units...) drawn only near.
void emit_facades(Shell &shell, Shell &parts, const Placed &pl);
void emit_roofs(Shell &shell, Shell &parts, const Placed &pl);
// What the player set on a block's roof (roof_gear.cpp).
void emit_roof_gear(Shell &shell, const Placed &pl, const Volume &v, float eaves);
void emit_pitched(Shell &shell, const Placed &pl, const Volume &v, float eaves);
void top_face(Shell &s, const Placed &pl, const Volume &v, float z, const Lin &lin, int8_t fin); // a block's flat top at `z`: a fan over its plan
void emit_roof_plant(Shell &shell, Shell &parts, const Placed &pl, const Volume &v, int vol, float roof_z);
// A round piece on a roof, standing on (cx, cy) of the frame `b`; `capped`: its top drawn, in `top`.
void cylinder(Shell &s, const Basis &b, float cx, float cy, float r, float z0, float z1, const Lin &side, const Lin &top, int8_t fin, bool capped = true);
void emit_lot(Shell &shell, Shell &parts, const Placed &pl);
// The lot's furniture and ground marks (lot_props.cpp); false for a kind it does not draw. `g`: ground over the floor.
// A part only the player sets (lot_parts.cpp); false when `e` is not one.
bool emit_built_part(Shell &shell, const Placed &pl, const LotElement &e, float g, const Lin &lin, int8_t fin);
bool emit_lot_prop(Shell &shell, Shell &parts, const Placed &pl, const LotElement &e, float g, const Lin &lin);
// A used roof terrace (roof_terrace.cpp) on volume `vol` whose deck is at height z.
void emit_roof_terrace(Shell &shell, Shell &parts, const Placed &pl, const Volume &v, int vol, float z);
// Far walls with painted windows; `roofs`: also slabs, parapets and machine rooms (the mid level lays emit_roofs).
void emit_far(Shell &shell, const Placed &pl, bool roofs);
// What a body throws in the sun from afar (far_casts.h): each block's sides as one face from its foot to its parapet, and its roof.
void emit_cast(Shell &shell, const Placed &pl);
// The mid level's openings alone, as emit_far lays them into a shell that keeps them (Shell::keep_bays): a level with its own reach.
void emit_mid_bays(Shell &shell, const Placed &pl);
// The mid level's relief, from the same bays the near level draws (facade_emit.cpp).
void emit_relief(Shell &shell, Shell &parts, const Placed &pl);
float relief_push(const Volume &v, int side, int bay, int k); // how far a bay is pushed out of its wall

} // namespace city::b40
