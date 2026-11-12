// The lot round a building, laid after its massing (internal to sim/buildings/b40). LotSpace knows the lot in its own
// frame (x across the frontage from its middle, y from the street) - the ground volumes, the doors, the side strips -
// and keeps the ways clear: the car's way from its gate to its bay or to the car park's aisle, and the people's way from
// their gate to the door. Each zone has its own file and lays its parts through `put`, which refuses a solid part that
// would stand in a way, on the building, on another part or outside the lot, so the cars and people that use the lot
// always find their way (the simulation reads the bays, docks and gates back from the elements).
#pragma once

#include "sim/buildings/b40/body_air.h"
#include "sim/buildings/b40/cover_layers.h"
#include "sim/buildings/b40/gen.h"

#include <algorithm>
#include <vector>

namespace city::b40 {

constexpr float BOUNDARY_WALL_T = 0.2f, FENCE_THICK = 0.12f, HEDGE_THICK = 0.7f; // thickness of a wall, a fence or railing, a hedge
constexpr float PART_GAP = 0.05f; // two solid parts never touch
constexpr float PLANTER_SHRUB_H = 0.9f; // a shrub in a court's or a forecourt's planter stands this tall over its soil
// The least room a tree and a shrub are planted in: a narrower one is left unplanted (LotSpace::plant; a car park's planter).
constexpr float TREE_ROOM_LEAST = 1.4f, SHRUB_ROOM_LEAST = 0.6f;

// How near a lot's edge a ground block's face closes it (the block's wall is the boundary along it): the rules' reach, and never less
// than the room the thickest run needs to stand before the block, so no run is ever laid into a wall. The one owner: the boundary
// (lot_boundary.cpp), a garage's place behind the front run (lot_cars.cpp) and the probe lot_bounds ask it.
inline float block_reach(const LotRules &l) { return std::max(l.block_closes, HEDGE_THICK + PART_GAP); }

struct Rect {
	float x0 = 0, y0 = 0, x1 = 0, y1 = 0;
	float w() const { return x1 - x0; }
	float d() const { return y1 - y0; }
	float cx() const { return (x0 + x1) / 2; }
	float cy() const { return (y0 + y1) / 2; }
	bool empty() const { return w() <= EPS || d() <= EPS; }
	bool meets(const Rect &o) const { return x0 < o.x1 && o.x0 < x1 && y0 < o.y1 && o.y0 < y1; }
	Rect grown(float g) const { return { x0 - g, y0 - g, x1 + g, y1 + g }; }
	static Rect around(float x, float y, float w, float d) { return { x - w / 2, y - d / 2, x + w / 2, y + d / 2 }; }
};

// The lot's boundary as open_boundary plans it and lay_boundary, last, lays it: the front's style and paint, the sides' and the back's
// kind, paint and heights, each edge's band (its thickness, held from every other part) and the front's stretches its gates take. A side
// a street runs along (LotEdges) is closed as the front is: its style, its paint, its band.
struct BoundaryPlan {
	BoundaryStyle front;
	Element side = EL_WALL;
	int front_paint = -1, side_paint = -1;
	float front_t = 0, side_t = 0; // the bands: the front's, and the sides' and the back's
	float party = 0, low = 0; // the sides' and the back's height, and the sides' beside the front zone
	bool street[2] = { false, false }; // the left side, the right side: a street runs along it
	std::vector<std::pair<float, float>> gates; // the front's stretches the gates take, with the gap a run keeps from them
	int pieces = 0; // the most pieces the runs can take: kept from the lot's budget until they are laid
	float band(int side) const { return street[side] ? front_t : side_t; } // a side's band: 0 left, 1 right
};

// A side strip of the lot (between the front zone and the back zone) and what it is used for.
struct Strip {
	SideUse use = SIDE_ATTACHED;
	Rect r; // the whole strip
	Rect drive; // the car's part (empty when none)
	Rect walk; // the people's part (empty when none)
	bool left = true;
};

class LotSpace {
public:
	LotSpace(const Data &data, Body &b, Xoshiro128 &r);

	const Data &data;
	Body &b;
	Xoshiro128 &r;
	const LotRules &rules;
	const LotPlan &plan;
	float L = 0, R = 0, D = 0; // lot edges: left and right x, depth
	float F = 0, B = 0; // the front zone ends at F, the back zone starts at B
	Strip strips[2]; // left, right
	std::vector<Rect> built; // ground volumes, footprints (the building's own first, then the garages laid later)
	size_t own_parts = 0; // how many of `built` are the building's own volumes
	Rect built_box; // their bounds (empty when the lot has no building)
	float door_x = 0; // the front door
	float door_wall = 0; // the front wall the door stands in (y)
	// The people's gate on the front: in line with the door, else (the door's gate would open on a crosswalk's landing or a signal post's
	// place: LotEdges) the nearest place along the front where it opens off them, clear of the side bands and the drives; then the way
	// bends along the middle of the front zone (people_way; sim/buildings/lot_doors.h reads the same bend for the walkers).
	float gate_x = 0;
	bool back_door = false;
	float back_door_x = 0;
	std::vector<float> loading_x; // loading doors on the back faces, and the wall they stand in
	float loading_y = 0;
	BoundaryPlan bounds; // open_boundary's plan, laid by lay_boundary

	// The lot's budget of parts is spent, the boundary's pieces kept for it until they are laid. A planter and what grows in it are
	// one part (planter): the plant's element is not counted again.
	bool full() const { return int(b.elements.size()) - grown_ + bounds.pieces >= rules.max_elements; }
	int paint(const char *name);
	int paint(const Paint &p) { return b.add_paint(p); }
	// Ground cover over `r`, drawn under the parts, on its layer of the one cover scale (cover_layers.h).
	void cover(const Rect &r, const char *name, CoverLayer layer = COVER_GROUND);
	// The lot's own ground, from edge to edge: laid first, every other cover over it.
	void cover_lot(const char *name);
	// Every band too thin to plant between a hard cover and what faces it (the lot's edge, a ground volume, another hard cover) laid
	// with a piece of that cover, stretch by stretch of the side they share, so no such band shows between two surfaces that meet.
	// Run once every zone has laid its covers.
	void close_slivers();
	// A part standing on the lot: refused (false) when it meets the building, a way, a solid part or the lot's edge, or stands as tall
	// as something the building holds over it there (its eaves, a balcony, an upper block: body_air.h).
	bool put(Element k, const Rect &r, float h, int paint, int facing = FACE_STREET, float z = 0);
	// A post `side` across and `h` tall (a lamp) at (x, y), or at the first clear place from there along (dx, dy), a step at a time
	// and no farther than the deepest thing a building holds over its foot: out from under a canopy, an awning or a balcony, never
	// left out because the place asked lies under one. False where no place that near is clear.
	bool post(Element k, float x, float y, float side, float h, int paint, float dx, float dy);
	// A tree or a shrub about (x, y), where a room `wide` across stands on the ground: in that room, or, under something the building
	// holds over it, in the largest square room about (x, y) that keeps under it, never narrower than `least`: `tall` in the room
	// asked and in the same proportion in a narrower one (a side garden's tree beside a house's eaves takes what they leave). False
	// where the room's ground is not free, or not even the least room keeps under what hangs there.
	bool plant(Element k, float x, float y, float wide, float tall, int paint, float least);
	// A planter with what grows in it, laid as one, so none stands bare: the box put (refused as any part is: then nothing is laid and
	// it answers false), and the plant `grows` in `room` (its element: a tree's crown may be wider than its planter), `tall` over the
	// planter's soil, on which it stands (its element's z: types.h PLANTER_SOIL_DROP_M under the rim). Under something the building
	// holds over the planter the plant is narrower and lower in proportion (a tree never in less than its least room), and where not
	// even that keeps under it, a shrub as low as what hangs there.
	bool planter(const Rect &box, float h, int paint, Element grows, const Rect &room, float tall, int plant_paint);
	// A part flat on the ground (a bay, a dock, a drain): it may lie in a way, never on the building; a bay or a dock
	// keeps its own ground clear (a car or a lorry stands there).
	bool flat(Element k, const Rect &r, float h, int paint, int facing = FACE_STREET);
	// A part laid where asked, without the tests (the boundary and its gates, a carport's posts, a tree in its planter).
	void edge(Element k, const Rect &r, float h, int paint, float base = 0, float z = 0);
	void keep_clear(const Rect &r) { ways_.push_back(r); }
	// Room kept for a part laid later (the boundary's bands): nothing is put there meanwhile.
	void hold(const Rect &r) { solid_.push_back(r); }
	// The lot's land inside its boundary's bands: where a structure that is not the boundary (a carport) stands.
	Rect inner() const { return { L + bounds.band(0), bounds.front_t, R - bounds.band(1), D - bounds.side_t }; }
	// Nothing solid, no way, no building there, inside the lot (ground_free), and nothing of the building hanging lower than `top`
	// over it (open_above).
	bool clear(const Rect &r, float top) const { return ground_free(r) && open_above(r, top); }
	bool ground_free(const Rect &r) const;
	bool open_above(const Rect &r, float top) const;
	// A block added to the body while the lot is laid (a garage): its footprint is the building's, and what it holds over the ground
	// (its roof's edge) the air's. A piece hung over the lot (a carport's slab on its posts): laid, and the air's too.
	void block_added(const Rect &footprint);
	void hang(const LotElement &piece);
	bool on_building(const Rect &r) const;
	// The deepest built edge over the x span [x0, x1] (F when nothing is built there).
	float built_back(float x0, float x1) const;
	// The people's way from their gate to the door, `half` either side of its line, reaching `depth` in at the door: one strip straight in
	// when the gate stands in line with the door, else three that meet without overlapping, in from the gate to the middle of the front
	// zone (half the door's wall), along it, and in to the door.
	std::vector<Rect> people_way(float half, float depth) const;
	Rect yard() const { return { L, B, R, D }; }
	Rect front() const { return { L, 0, R, F }; }

private:
	std::vector<Rect> ways_, solid_;
	std::vector<Overhang> air_; // what the building holds over the ground past its walls (body_air.h), as it stood when the lot was begun
	std::vector<std::pair<const char *, int>> paints_;
	// A cover laid: its element, whether it is hard (anything but lawn) and whether it is the lot's own ground.
	struct Laid {
		size_t element = 0;
		bool hard = false, whole = false;
	};
	std::vector<Laid> covers_;
	int grown_ = 0; // the plants laid with their planters
	void add(Element k, const Rect &r, float h, int paint, int facing, float z, float base);
	float people_gate_x() const; // gate_x
};

// A narrow house's car pad in its front garden (lot_cars.cpp), its gate off the front's shut stretches (LotEdges); empty for every
// other lot, and where neither side has room for one off them.
Rect car_pad(const LotSpace &s);

// The zones, in the order lay_out_lot runs them (each file's own rules).
void lay_ground(LotSpace &s); // lot_ground.cpp: covers of every zone, and the ways kept clear
void open_boundary(LotSpace &s); // lot_boundary.cpp: the boundary's style, its gates, and every edge's band held from the parts
void lay_cars(LotSpace &s); // lot_cars.cpp: house garages, carports and car pads; drives, alleys and side gardens
void lay_front(LotSpace &s); // lot_front.cpp: front garden, forecourt, plaza and works apron
void lay_back(LotSpace &s); // lot_back.cpp: car park, service yard, residents' court and loading yard
void lay_yard(LotSpace &s); // lot_yard.cpp: a house's back yard and what is lived in it; the courts the building leaves
void lay_boundary(LotSpace &s); // lot_boundary.cpp, last: every edge closed by its runs, its gates or the blocks standing on it

} // namespace city::b40
