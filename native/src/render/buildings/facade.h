// A 4.0 body placed in the world and the emitters that draw it (4.0 r40/render/buildings): the placement (lot basis,
// floor height, building shade, ground sampler), the volume sides as facade frames, the paints of a bay, and the entry
// points of each emitter file.
#pragma once

#include "render/buildings/building_look.h"
#include "sim/buildings/b40/data.h"
#include "render/buildings/shell.h"

#include <functional>
#include <vector>

namespace city::b40 {

struct Placed {
	const Data *data = nullptr;
	const BuildingLook *look = nullptr; // sizes of the thin drawn pieces (fence bars, part frames, air handlers)
	const Body *b = nullptr;
	Basis lot; // o: middle of the lot front at floor height; e0 lot x, e1 lot y (away from the street), e2 up
	uint32_t id = 0; // the record seed (bay hashes, roof plant, flat roof pick)
	float shade = 1; // 4.0 building shade, 0.93..1.05 on walls
	float ground_lo = 0; // lowest ground under the ground volumes, relative to the floor (<= 0)
	std::function<float(const godot::Vector3 &)> ground; // terrain height under a world point
	bool coarse = false; // mid level: emit_far draws walls, slabs and parapets; roofs/lot add only the big pieces
	bool lean = false; // far level: one wall band per side, no coping or machine rooms (a third of the far memory)
};

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
// Moulding (bands, cornices, coping) paint: the trim, or plaster wall x 1.1 when the trim is metal (4.0 buildingMesh.ts).
Paint moulding_of(const Placed &pl);
// A two-tier band (4.0 roofMesh.ts band): lower tier 0.45 out from -h/2 to -0.1h, upper tier from -0.1h to +h/2;
// `flush`: one strip.
void band(Shell &s, const Side &sd, float z, float out, float h, const Lin &lin, int8_t fin, bool flush);
uint32_t bay_hash(int vol, int level, int side, int index, float bx, float by);

// Emitters. `parts` takes the small repeated pieces (frames, railings, louvres, AC units...) drawn only near.
void emit_facades(Shell &shell, Shell &parts, const Placed &pl);
void emit_roofs(Shell &shell, Shell &parts, const Placed &pl);
void emit_pitched(Shell &shell, const Placed &pl, const Volume &v, float eaves);
void emit_roof_plant(Shell &shell, Shell &parts, const Placed &pl, const Volume &v, int vol, float roof_z);
void emit_lot(Shell &shell, Shell &parts, const Placed &pl);
// The lot's furniture and ground marks (lot_props.cpp); false for a kind it does not draw. `g`: ground over the floor.
bool emit_lot_prop(Shell &shell, Shell &parts, const Placed &pl, const LotElement &e, float g, const Lin &lin);
// A used roof terrace (roof_terrace.cpp) on volume `vol` whose deck is at height z.
void emit_roof_terrace(Shell &shell, Shell &parts, const Placed &pl, const Volume &v, int vol, float z);
// Far walls with painted windows; `roofs`: also slabs, parapets and machine rooms (the mid level lays emit_roofs).
void emit_far(Shell &shell, const Placed &pl, bool roofs);

} // namespace city::b40
