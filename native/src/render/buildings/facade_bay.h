// One bay of one storey (4.0 facadeEmit.ts emitBay + meshCore.ts openingOf): the opening of its component, the wall
// round it with reveals, sill and head, the pane, and its parts (frames, sills, doors, balconies, railings, shutters,
// louvres, brises, awnings, AC units) placed with the 4.0 `put` convention: a along the bay, h above the storey
// floor, depth INTO the wall (negative = out in front), sizes (along, up, out).
#pragma once

#include "render/buildings/facade.h"

namespace city::b40 {

struct BayCtx {
	Basis f; // origin at the bay's left end on the storey floor, in the wall plane; e2 out of the wall
	float W = 3, H = 3;
	uint8_t comp = BAY_WALL;
	int level = 0;
	uint32_t hash = 0;
	Lin wall, trim, mould;
	int8_t wall_fin = FIN_PLASTER, trim_fin = FIN_PLASTER;
	const FaceGeometry *geo = nullptr;
	bool open_left = false, open_right = false; // the neighbouring bay is also a gourmet balcony
	Use use = USE_RESIDENTIAL;
	bool plain = false; // partly covered by another block: a blank wall
};

struct Opening {
	bool has = false;
	float w = 0, h0 = 0, h1 = 0, depth = 0, a0 = 0, a1 = 0;
};

Opening opening_of(const Data &data, const BayCtx &c);
void emit_bay(Shell &shell, Shell &parts, const Placed &pl, const BayCtx &c);
void emit_bay_parts(Shell &shell, Shell &parts, const Placed &pl, const BayCtx &c, const Opening &o);
void emit_bay_props(Shell &parts, const Placed &pl, const BayCtx &c, const Opening &o);

// Part models (part_models.cpp), each placed by its centre (a, h, depth) and size (sx, sy, sz) in the bay frame.
struct Part {
	const Basis *f;
	float a, h, depth, sx, sy, sz;
	const Data *data = nullptr; // part model sizes (buildings.json "parts")
};
void part_box(Shell &s, const Part &p, const Lin &lin, int8_t fin);
void part_frame(Shell &s, const Part &p, const Lin &lin, int8_t fin);
void part_railing(Shell &s, const Part &p, const Lin &lin, int8_t fin);
void part_door(Shell &s, const Part &p, const Lin &lin, int8_t fin);
void part_louvre(Shell &s, const Part &p, const Lin &lin, int8_t fin);
void part_brise(Shell &s, const Part &p, const Lin &lin, int8_t fin);
void part_awning(Shell &s, const Part &p, const Lin &lin, int8_t fin);
void part_ac(Shell &s, const Part &p, const Lin &lin, const Lin &dark);
void part_lamp(Shell &s, const Part &p);

} // namespace city::b40
