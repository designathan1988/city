// The ground of every zone (lawn, paving, drive surfaces) and the ways kept clear before anything stands on the lot:
// each drive from its gate to the back of its strip, a car park's aisle across the yard, a works' visitors' aisle and a
// loading yard's turning ground, and the people's way from the front gate to the door.
#include "sim/buildings/b40/lot_layout.h"

#include <algorithm>

namespace city::b40 {

namespace {
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
	const LotGroundShape &K = s.data.shapes.ground;
	const LotPartShape &P = s.data.shapes.parts;
	const bool house = s.b.kind == KIND_HOUSE;
	const char *drive = house || s.b.kind == KIND_WORKS ? "concrete" : "asphalt";
	const char *walk = house ? "path" : "stone_path";
	if (const char *c = front_cover(s.plan.front); c && s.F > 0) s.cover(s.front(), c);
	for (const Strip &st : s.strips) {
		if (!st.drive.empty()) {
			s.cover(st.drive, drive, 1);
			s.keep_clear({ st.drive.x0 + P.way_side, 0, st.drive.x1 - P.way_side, s.B });
		}
		if (!st.walk.empty()) s.cover(st.walk, walk, 1);
		if (st.use == SIDE_GARDEN) s.cover(st.r, "lawn");
	}
	if (const char *c = back_cover(s.plan.back); c && s.B < s.D) s.cover(s.yard(), c);
	// The building's envelope under and beside the building: lawn round a house, paving round the others.
	Rect env{ s.plan.x0 + s.L, s.plan.y0, s.plan.x1 + s.L, s.plan.y1 };
	s.cover(env, house || s.b.kind == KIND_FLATS || s.b.kind == KIND_TOWER ? "lawn" : "paving");
	if (s.F > 0) {
		float g = s.rules.person_gate[house ? 0 : 1] * HALF + K.door_way_pad;
		s.keep_clear({ s.door_x - g, 0, s.door_x + g, std::max(s.F, K.door_way_min) });
	}
	const LotRules &l = s.rules;
	if (s.plan.back == BACK_PARKING) {
		float y0 = s.plan.rows >= 2 ? s.B + l.stall_d : s.B;
		s.keep_clear({ s.L, y0, s.R, s.D - l.stall_d });
	}
	if (s.plan.front == FRONT_APRON && s.F >= l.row_depth(1)) s.keep_clear({ s.L, 0, s.R, s.F - l.stall_d });
	if (s.plan.back == BACK_LOADING) {
		float docks_end = std::max(s.B, s.loading_y) + std::min(s.D - std::max(s.B, s.loading_y), l.lorry_l + l.dock_margin);
		s.keep_clear({ s.L + K.edge_keep, docks_end, s.R - K.edge_keep, s.D - K.edge_keep });
	}
}

} // namespace city::b40
