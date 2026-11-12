// The lot's ground, whole, then the ground of every zone over it (lawn, paving, drive surfaces), and the ways kept clear before anything stands on the lot:
// each drive from its gate to the back of its strip, a car park's aisle across the yard, a works' visitors' aisle and a
// loading yard's turning ground, and the people's way from the front gate to the door (LotSpace::people_way: straight, or bent along the
// front zone where the gate stands off the door's line).
#include "sim/buildings/b40/lot_layout.h"

#include <algorithm>

namespace city::b40 {

namespace {
constexpr float WAY_SIDE = 0.3f; // a car's way is its drive less this each side (a door opened, a wing mirror)
constexpr float DOOR_WAY_PAD = 0.5f; // the people's way: their gate and this each side
constexpr float DOOR_WAY_MIN = 1.f; // the people's way reaches at least this far in from the street
constexpr float EDGE_KEEP = 0.5f; // a loading yard's turning ground stops short of the boundary
constexpr float HALF = 0.5f;

const char *front_cover(FrontUse u) {
	switch (u) {
		case FRONT_GARDEN: case FRONT_CARPAD: return "lawn";
		case FRONT_FORECOURT: return "pavers";
		case FRONT_PLAZA: return "paving";
		case FRONT_APRON: return "asphalt";
		default: return nullptr;
	}
}

const char *back_cover(BackUse u) {
	switch (u) {
		case BACK_YARD: case BACK_COURTYARD: return "lawn";
		case BACK_PARKING: return "asphalt";
		case BACK_SERVICE: case BACK_LOADING: return "concrete";
		default: return nullptr;
	}
}
} // namespace

void lay_ground(LotSpace &s) {
	const bool house = s.b.kind == KIND_HOUSE;
	const char *drive = house || s.b.kind == KIND_WORKS ? "concrete" : "asphalt";
	const char *walk = house ? "path" : "stone_path";
	// The lot's ground, whole, before any zone lays its own over it: lawn round a house, flats and a tower, paving round the others. No
	// part of a lot is left to whatever lies under it.
	s.cover_lot(house || s.b.kind == KIND_FLATS || s.b.kind == KIND_TOWER ? "lawn" : "paving");
	if (const char *c = front_cover(s.plan.front); c && s.F > 0) s.cover(s.front(), c);
	for (const Strip &st : s.strips) {
		if (!st.drive.empty()) {
			s.cover(st.drive, drive, COVER_WAY);
			s.keep_clear({ st.drive.x0 + WAY_SIDE, 0, st.drive.x1 - WAY_SIDE, s.B });
		}
		if (!st.walk.empty()) s.cover(st.walk, walk, COVER_WAY);
		if (st.use == SIDE_GARDEN) s.cover(st.r, "lawn");
	}
	if (const char *c = back_cover(s.plan.back); c && s.B < s.D) s.cover(s.yard(), c);
	if (s.F > 0) {
		float g = s.rules.person_gate[house ? 0 : 1] * HALF + DOOR_WAY_PAD;
		for (const Rect &r : s.people_way(g, std::max(s.F, DOOR_WAY_MIN))) s.keep_clear(r);
	}
	const LotRules &l = s.rules;
	if (s.plan.back == BACK_PARKING) {
		float y0 = s.plan.rows >= 2 ? s.B + l.stall_d : s.B;
		s.keep_clear({ s.L, y0, s.R, s.D - l.stall_d });
	}
	if (s.plan.front == FRONT_APRON && s.F >= l.row_depth(1)) s.keep_clear({ s.L, 0, s.R, s.F - l.stall_d });
	if (s.plan.back == BACK_LOADING) {
		float docks_end = std::max(s.B, s.loading_y) + std::min(s.D - std::max(s.B, s.loading_y), l.lorry_l + l.dock_margin);
		s.keep_clear({ s.L + EDGE_KEEP, docks_end, s.R - EDGE_KEEP, s.D - EDGE_KEEP });
	}
}

} // namespace city::b40
