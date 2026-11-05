// The front zone, between the street boundary and the building. A garden: the walk from the gate to the door, beds of
// shrubs and flowers along the boundary, a tree before the house, flowers under the windows. A forecourt: pairs of
// benches or planters, bollards at the kerb, a bin. A plaza: trees in big planters with a bench each, lamps, bollards.
// A works' apron: the walk to the office and, on a deep one, the visitors' bays along the building. Nothing stands on
// the ways kept clear (LotSpace::put refuses it), so the door and the drives stay open.
#include "sim/buildings/b40/lot_layout.h"

#include <algorithm>
#include <cmath>

namespace city::b40 {

namespace {
constexpr float PATH_W = 1.4f, LINK_D = 1.2f; // the walk to the door; the walks along the front to the side paths
constexpr float BED_IN = 1.2f, BED_EVERY = 2.6f, BED = 1.2f, BED_H = 0.8f, SHRUB_SHARE = 0.5f; // beds along the boundary
constexpr float WINDOW_BED_W = 1.6f, WINDOW_BED_D = 0.8f, WINDOW_BED_H = 0.35f, WINDOW_BED_EVERY = 2.f, WINDOW_BED_OUT = 0.6f;
constexpr float FRONT_CROWN = 2.6f, FRONT_CROWN_TOP = 4.5f, TREE_MIN_F = 4.f, TREE_IN = 2.2f, TREE_EVERY = 3.5f, TREE_FROM = 1.5f;
constexpr float PAIR_EVERY = 3.6f, PAIR_SHARE = 0.7f, PAIR_MIN_F = 3.f, PAIR_AT = 0.35f, PAIR_GAP = 1.3f;
constexpr float BENCH_W = 1.3f, BENCH_D = 0.45f, BENCH_H = 0.45f, PLANTER = 1.f, PLANTER_H = 0.6f;
constexpr float BOLLARD = 0.2f, BOLLARD_H = 0.9f, BOLLARD_AT = 0.35f, BOLLARD_EVERY = 1.8f, PLAZA_BOLLARD_EVERY = 2.f;
constexpr float BIN_W = 0.7f, BIN_D = 0.6f, BIN_H = 1.f, BIN_IN = 1.2f, BIN_OUT = 0.6f;
constexpr float PLAZA_EVERY = 6.f, PLAZA_FROM = 2.5f, PLAZA_PLANTER = 2.f, PLAZA_TREE = 2.5f, PLAZA_TREE_H = 5.f;
constexpr float PLAZA_AT = 0.45f, PLAZA_BENCH = 1.6f, PLAZA_BENCH_D = 0.5f, LAMP = 0.3f, LAMP_H = 4.5f, LAMP_IN = 1.f;
constexpr float LAMP_BACK = 0.8f, APRON_PATH = 1.6f, APRON_LAMP_H = 5.f, APRON_LAMP_X = 1.6f, APRON_LAMP_IN = 0.6f;
constexpr float VISITOR_FROM = 0.3f, VISITOR_DOOR = 2.2f, HALF = 0.5f;

void garden(LotSpace &s) {
	const bool house = s.b.kind == KIND_HOUSE;
	const char *walk = house ? "house_path" : "pavers";
	s.cover({ s.door_x - PATH_W * HALF, 0, s.door_x + PATH_W * HALF, s.F }, walk, 2);
	for (const Strip &st : s.strips) {
		if (st.walk.empty() || !st.drive.empty()) continue;
		float a = st.walk.x0 < s.door_x ? st.walk.x0 : s.door_x + PATH_W * HALF;
		float b = st.walk.x0 < s.door_x ? s.door_x - PATH_W * HALF : st.walk.x1;
		s.cover({ a, s.F - LINK_D, b, s.F }, walk, 2);
	}
	const int shrub = s.paint("hedge"), flowers = s.paint("flowers");
	for (float x = s.L + BED_IN; x < s.R - BED_IN * HALF; x += BED_EVERY) {
		bool sh = s.r.f() < SHRUB_SHARE;
		s.put(sh ? EL_SHRUB : EL_FLOWERS, Rect::around(x, BED_IN, BED, BED), BED_H, sh ? shrub : flowers);
	}
	if (s.F >= TREE_MIN_F && !s.built_box.empty())
		for (float x = s.built_box.x0 + TREE_FROM; x < s.built_box.x1 - 1; x += TREE_EVERY)
			if (s.put(EL_TREE, Rect::around(x, s.F - TREE_IN, FRONT_CROWN, FRONT_CROWN), FRONT_CROWN_TOP, shrub)) break;
	if (!s.built_box.empty())
		for (float x = s.built_box.x0 + 1; x < s.built_box.x1 - WINDOW_BED_D; x += WINDOW_BED_EVERY)
			s.put(EL_FLOWERS, Rect::around(x, s.F - WINDOW_BED_OUT, WINDOW_BED_W, WINDOW_BED_D), WINDOW_BED_H, flowers);
}

void forecourt(LotSpace &s) {
	const int bench = s.paint("bench"), planter = s.paint("planter");
	for (float x = s.L + PAIR_EVERY * HALF; x < s.R - PAIR_GAP; x += PAIR_EVERY) {
		if (s.r.f() < PAIR_SHARE && s.F >= PAIR_MIN_F) {
			float y = s.F * PAIR_AT;
			s.put(EL_BENCH, Rect::around(x, y, BENCH_W, BENCH_D), BENCH_H, bench, FACE_BACK);
			s.put(EL_BENCH, Rect::around(x, y + PAIR_GAP, BENCH_W, BENCH_D), BENCH_H, bench, FACE_STREET);
		} else {
			s.put(EL_PLANTER, Rect::around(x, s.F * HALF, PLANTER, PLANTER), PLANTER_H, planter);
		}
	}
	const int bollard = s.paint("lamp_post");
	for (float x = s.L + BOLLARD_EVERY * HALF; x < s.R - BOLLARD; x += BOLLARD_EVERY)
		s.put(EL_BOLLARD, Rect::around(x, BOLLARD_AT, BOLLARD, BOLLARD), BOLLARD_H, bollard);
	s.put(EL_BIN, Rect::around(s.R - BIN_IN, s.F - BIN_OUT, BIN_W, BIN_D), BIN_H, s.paint("bin"));
}

void plaza(LotSpace &s) {
	const int planter = s.paint("planter"), tree = s.paint("hedge"), bench = s.paint("bench");
	for (float x = s.L + PLAZA_FROM; x < s.R - PLAZA_FROM; x += PLAZA_EVERY) {
		float y = s.F * PLAZA_AT;
		if (!s.put(EL_PLANTER, Rect::around(x, y, PLAZA_PLANTER, PLAZA_PLANTER), PLANTER_H, planter)) continue;
		s.edge(EL_TREE, Rect::around(x, y, PLAZA_TREE, PLAZA_TREE), PLAZA_TREE_H, tree);
		s.put(EL_BENCH, Rect::around(x, y + PLAZA_PLANTER * HALF + PLAZA_BENCH_D, PLAZA_BENCH, PLAZA_BENCH_D), BENCH_H, bench, FACE_STREET);
	}
	const int post = s.paint("lamp_post");
	for (float x : { s.L + LAMP_IN, s.R - LAMP_IN }) s.put(EL_LAMP, Rect::around(x, s.F - LAMP_BACK, LAMP, LAMP), LAMP_H, post);
	for (float x = s.L + PLAZA_BOLLARD_EVERY * HALF; x < s.R - BOLLARD; x += PLAZA_BOLLARD_EVERY)
		s.put(EL_BOLLARD, Rect::around(x, BOLLARD_AT, BOLLARD, BOLLARD), BOLLARD_H, post);
}

void apron(LotSpace &s) {
	const LotRules &l = s.rules;
	s.cover({ s.door_x - APRON_PATH * HALF, 0, s.door_x + APRON_PATH * HALF, s.F }, "path", 2);
	if (s.F >= l.row_depth(1) && !s.built_box.empty()) {
		const int line = s.paint("stall_line");
		for (float x = s.built_box.x0 + VISITOR_FROM; x + l.stall_w <= s.built_box.x1; x += l.stall_w) {
			if (std::abs(x + l.stall_w * HALF - s.door_x) < VISITOR_DOOR) continue;
			s.flat(EL_STALL, { x, s.F - l.stall_d, x + l.stall_w, s.F }, 0, line, FACE_STREET);
		}
	}
	s.put(EL_LAMP, Rect::around(s.door_x + APRON_LAMP_X, s.F - APRON_LAMP_IN, LAMP, LAMP), APRON_LAMP_H, s.paint("lamp_post"));
}
} // namespace

void lay_front(LotSpace &s) {
	if (s.F <= 0) return;
	switch (s.plan.front) {
		case FRONT_GARDEN: case FRONT_CARPAD: garden(s); break;
		case FRONT_FORECOURT: forecourt(s); break;
		case FRONT_PLAZA: plaza(s); break;
		case FRONT_APRON: apron(s); break;
		default: break;
	}
}

} // namespace city::b40
