// The lot round a building, laid after its massing (internal to sim/buildings/b40). LotSpace knows the lot in its own
// frame (x across the frontage from its middle, y from the street) - the ground volumes, the doors, the side strips -
// and keeps the ways clear: the car's way from its gate to its bay or to the car park's aisle, and the people's way from
// their gate to the door. Each zone has its own file and lays its parts through `put`, which refuses a solid part that
// would stand in a way, on the building, on another part or outside the lot, so the cars and people that use the lot
// always find their way (the simulation reads the bays, docks and gates back from the elements).
#pragma once

#include "sim/buildings/b40/cover_layers.h"
#include "sim/buildings/b40/gen.h"

#include <vector>

namespace city::b40 {

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
// kind, paint and heights, each edge's band (its thickness, held from every other part) and the front's stretches its gates take.
struct BoundaryPlan {
	BoundaryStyle front;
	Element side = EL_WALL;
	int front_paint = -1, side_paint = -1;
	float front_t = 0, side_t = 0; // the bands: the front's, and the sides' and the back's
	float party = 0, low = 0; // the sides' and the back's height, and the sides' beside the front zone
	std::vector<std::pair<float, float>> gates; // the front's stretches the gates take, with the gap a run keeps from them
	int pieces = 0; // the most pieces the runs can take: kept from the lot's budget until they are laid
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
	bool back_door = false;
	float back_door_x = 0;
	std::vector<float> loading_x; // loading doors on the back faces, and the wall they stand in
	float loading_y = 0;
	BoundaryPlan bounds; // open_boundary's plan, laid by lay_boundary

	// The lot's budget of parts is spent, the boundary's pieces kept for it until they are laid.
	bool full() const { return int(b.elements.size()) + bounds.pieces >= rules.max_elements; }
	int paint(const char *name);
	int paint(const Paint &p) { return b.add_paint(p); }
	// Ground cover over `r`, drawn under the parts, on its layer of the one cover scale (cover_layers.h).
	void cover(const Rect &r, const char *name, CoverLayer layer = COVER_GROUND);
	// A part standing on the lot: refused (false) when it meets the building, a way, a solid part or the lot's edge.
	bool put(Element k, const Rect &r, float h, int paint, int facing = FACE_STREET, float z = 0);
	// A part flat on the ground (a bay, a dock, a drain): it may lie in a way, never on the building; a bay or a dock
	// keeps its own ground clear (a car or a lorry stands there).
	bool flat(Element k, const Rect &r, float h, int paint, int facing = FACE_STREET);
	// A part laid where asked, without the tests (the boundary and its gates, a carport's posts, a tree in its planter).
	void edge(Element k, const Rect &r, float h, int paint, float base = 0, float z = 0);
	void keep_clear(const Rect &r) { ways_.push_back(r); }
	// Room kept for a part laid later (the boundary's bands): nothing is put there meanwhile.
	void hold(const Rect &r) { solid_.push_back(r); }
	// The lot's land inside its boundary's bands: where a structure that is not the boundary (a carport) stands.
	Rect inner() const { return { L + bounds.side_t, bounds.front_t, R - bounds.side_t, D - bounds.side_t }; }
	bool clear(const Rect &r) const; // nothing solid, no way, no building there, inside the lot
	bool on_building(const Rect &r) const;
	// The deepest built edge over the x span [x0, x1] (F when nothing is built there).
	float built_back(float x0, float x1) const;
	Rect yard() const { return { L, B, R, D }; }
	Rect front() const { return { L, 0, R, F }; }

private:
	std::vector<Rect> ways_, solid_;
	std::vector<std::pair<const char *, int>> paints_;
	void add(Element k, const Rect &r, float h, int paint, int facing, float z, float base);
};

// A narrow house's car pad in its front garden (lot_cars.cpp); empty for every other lot.
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
