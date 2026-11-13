// One bay of one storey: the opening of its component, the wall
// round it with reveals, sill and head, the pane, and its parts (frames, sills, doors, balconies, railings, shutters,
// louvres, brises, awnings, AC units) placed in the bay frame: a along the bay, h above the storey
// floor, depth INTO the wall (negative = out in front), sizes (along, up, out).
#pragma once

#include "render/buildings/facade.h"
#include "sim/buildings/interior/interior.h"

#include <functional>

namespace city::b40 {

// A part in the trim's colour (frame, fin, brise) is paint over no finish, whatever the trim's own finish: a metal finish has no diffuse,
// and a thin part would read as the sky it reflects.
constexpr int8_t PART_PAINT = FIN_NONE;

struct BayCtx {
	Basis f; // origin at the bay's left end on the storey floor, in the wall plane; e2 out of the wall
	float W = 3, H = 3;
	uint8_t comp = BAY_WALL;
	int level = 0;
	uint32_t hash = 0;
	Lin wall, trim, mould;
	float wall_before = 0, wall_after = 0; // how far the wall runs on past this bay, each way along its side
	int8_t wall_fin = FIN_PLASTER, trim_fin = FIN_PLASTER, mould_fin = FIN_PLASTER; // trim_fin: the reveals and fascias, faces of the shell
	const FaceGeometry *geo = nullptr;
	bool open_left = false, open_right = false; // the neighbouring bay is also a gourmet balcony
	Use use = USE_RESIDENTIAL;
	bool plain = false; // partly covered by another block: a blank wall
	int vol = 0;
	Point2 plan_at, plan_along, plan_out; // the bay's left end on the plan (lot frame), along the facade, out of it
};

struct Opening {
	bool has = false;
	float w = 0, h0 = 0, h1 = 0, depth = 0, a0 = 0, a1 = 0;
};

// Bay `i` of a side on the plan, for whatever level draws it (`c` has its width and level): the hash it picks by, its block, where
// it stands, and `plain` when another block covers a part of it. False when another block covers it whole: nothing of it is drawn.
bool bay_on_plan(BayCtx &c, const Body &b, const Volume &v, int vol, const Side &sd, int i);
Opening opening_of(const Data &data, const BayCtx &c);
// The four faces of an opening's reveal between wall depths `from` and `to` (<= 0, from < to): the jambs, the sill at `sill_h` and the head.
// The one owner of that shape: the bay's own (depth to the wall face) and the inside of a wall cut open (plaster to the pane's recess).
void emit_reveal(Shell &shell, const BayCtx &c, const Opening &o, float from, float to, const Lin &jamb, const Lin &sill, const Lin &head, int8_t fin,
		float sill_h);
// What the pane of an opening draws (pane_rooms.cpp): `frame` the thickness of its frame, 0 none.
PaneLook pane_look(const Placed &pl, const BayCtx &c, float a0, float a1, float h0, float h1, bool dark, float frame);
// What the pane over a0..a1 of a bay is, at any level (shell.h PaneSeed): its room among the level's `spaces` (interior.h spaces_at),
// the bay's byte, and its state by the building's `purpose`. The one rule of it: the near pane and the mid level's bay both ask here.
PaneSeed pane_seed(const Placed &pl, const BayCtx &c, float a0, float a1, Purpose purpose, const std::vector<Space> &spaces);
uint8_t pane_state(const BayCtx &c); // the glass and the shutter of it (facade_bay.cpp): PaneSeed's state without the floor
// How many of a component's panes take the darker glass: the far level's grid fades to the same share (far_window).
enum PaneDark { PANE_DARK_PICK, PANE_DARK_NONE, PANE_DARK_QUARTER, PANE_DARK_FIFTH };
PaneDark pane_dark(uint8_t comp);
// The wall of bays side by side with nothing cut in it, as one face: a flush pane stands on it, so a storey of windows is a few faces
// and a pane each, not four rectangles a bay. Short enough that the macro tone, written per vertex, still moves along it.
// It is made on its shell alone (the run opens with the first bay added), never as an aggregate with half of its fields given.
struct WallRun {
	explicit WallRun(Shell &on) : shell(on) {}
	Shell &shell;
	int last = -1; // the bay it reaches, -1: none open
	Basis f;
	float width = 0, height = 0, push = 0;
	Lin wall;
	int8_t fin = FIN_NONE;
	void add(const BayCtx &c, int bay, float at_push);
	void close();
};
void emit_bay(Shell &shell, Shell &parts, const Placed &pl, const BayCtx &c, WallRun &run, int bay, float push);
// Every bay of a placed body with its opening (the census of the bays, building_renderer_probe.cpp).
void for_each_bay(const Placed &pl, const std::function<void(const BayCtx &, const Opening &)> &at);
void emit_bay_parts(Shell &shell, Shell &parts, const Placed &pl, const BayCtx &c, const Opening &o);
void emit_bay_props(Shell &parts, const Placed &pl, const BayCtx &c, const Opening &o);
// What of a bay stands out of its wall, for the mid level: slabs, guards, shutters, fins and awnings. Boxes go to the instanced
// `parts`; what is no box (a railing's card, an awning's canvas) to `shell`.
void emit_bay_relief(Shell &shell, Shell &parts, const Placed &pl, const BayCtx &c, const Opening &o);

// Part models (part_models.cpp), each placed by its centre (a, h, depth) and size (sx, sy, sz) in the bay frame.
struct Part {
	const Basis *f;
	float a, h, depth, sx, sy, sz;
	const BuildingLook *look = nullptr; // part model sizes (buildings.json "parts")
};
void part_box(Shell &s, const Part &p, const Lin &lin, int8_t fin);
void part_railing(Shell &s, const Part &p, const Lin &lin, int8_t fin);
void part_door(Shell &s, const Part &p, const Lin &lin, int8_t fin);
void part_louvre(Shell &s, const Part &p, const Lin &lin, int8_t fin);
void part_brise(Shell &s, const Part &p, const Lin &lin, int8_t fin);
void part_awning(Shell &s, const Part &p, const Lin &lin, int8_t fin);
void part_lamp(Shell &s, const Part &p);

} // namespace city::b40
