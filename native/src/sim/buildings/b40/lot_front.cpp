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
constexpr float HALF = 0.5f;

void garden(LotSpace &s) {
	const LotFrontShape &K = s.data.shapes.front;
	const bool house = s.b.kind == KIND_HOUSE;
	const char *walk = house ? "house_path" : "pavers";
	s.cover({ s.door_x - K.path_w * HALF, 0, s.door_x + K.path_w * HALF, s.F }, walk, 2);
	for (const Strip &st : s.strips) {
		if (st.walk.empty() || !st.drive.empty()) continue;
		float a = st.walk.x0 < s.door_x ? st.walk.x0 : s.door_x + K.path_w * HALF;
		float b = st.walk.x0 < s.door_x ? s.door_x - K.path_w * HALF : st.walk.x1;
		s.cover({ a, s.F - K.link_d, b, s.F }, walk, 2);
	}
	const int shrub = s.paint("hedge"), flowers = s.paint("flowers");
	for (float x = s.L + K.bed_in; x < s.R - K.bed_in * HALF; x += K.bed_every) {
		bool sh = s.r.f() < K.shrub_share;
		s.put(sh ? EL_SHRUB : EL_FLOWERS, Rect::around(x, K.bed_in, K.bed, K.bed), K.bed_h, sh ? shrub : flowers);
	}
	if (s.F >= K.tree_min_f && !s.built_box.empty())
		for (float x = s.built_box.x0 + K.tree_from; x < s.built_box.x1 - 1; x += K.tree_every)
			if (s.put(EL_TREE, Rect::around(x, s.F - K.tree_in, K.front_crown, K.front_crown), K.front_crown_top, shrub)) break;
	if (!s.built_box.empty())
		for (float x = s.built_box.x0 + 1; x < s.built_box.x1 - K.window_bed_d; x += K.window_bed_every)
			s.put(EL_FLOWERS, Rect::around(x, s.F - K.window_bed_out, K.window_bed_w, K.window_bed_d), K.window_bed_h, flowers);
}

void forecourt(LotSpace &s) {
	const LotFrontShape &K = s.data.shapes.front;
	const LotPartShape &P = s.data.shapes.parts;
	const int bench = s.paint("bench"), planter = s.paint("planter");
	for (float x = s.L + K.pair_every * HALF; x < s.R - K.pair_gap; x += K.pair_every) {
		if (s.r.f() < K.pair_share && s.F >= K.pair_min_f) {
			float y = s.F * K.pair_at;
			s.put(EL_BENCH, Rect::around(x, y, K.bench_w, K.bench_d), P.bench_h, bench, FACE_BACK);
			s.put(EL_BENCH, Rect::around(x, y + K.pair_gap, K.bench_w, K.bench_d), P.bench_h, bench, FACE_STREET);
		} else {
			s.put(EL_PLANTER, Rect::around(x, s.F * HALF, K.planter, K.planter), K.planter_h, planter);
		}
	}
	const int bollard = s.paint("lamp_post");
	for (float x = s.L + K.bollard_every * HALF; x < s.R - K.bollard; x += K.bollard_every)
		s.put(EL_BOLLARD, Rect::around(x, K.bollard_at, K.bollard, K.bollard), K.bollard_h, bollard);
	s.put(EL_BIN, Rect::around(s.R - K.bin_in, s.F - K.bin_out, K.bin_w, K.bin_d), K.bin_h, s.paint("bin"));
}

void plaza(LotSpace &s) {
	const LotFrontShape &K = s.data.shapes.front;
	const LotPartShape &P = s.data.shapes.parts;
	const int planter = s.paint("planter"), tree = s.paint("hedge"), bench = s.paint("bench");
	for (float x = s.L + K.plaza_from; x < s.R - K.plaza_from; x += K.plaza_every) {
		float y = s.F * K.plaza_at;
		if (!s.put(EL_PLANTER, Rect::around(x, y, K.plaza_planter, K.plaza_planter), K.planter_h, planter)) continue;
		s.edge(EL_TREE, Rect::around(x, y, K.plaza_tree, K.plaza_tree), K.plaza_tree_h, tree);
		s.put(EL_BENCH, Rect::around(x, y + K.plaza_planter * HALF + K.plaza_bench_d, K.plaza_bench, K.plaza_bench_d), P.bench_h, bench, FACE_STREET);
	}
	const int post = s.paint("lamp_post");
	for (float x : { s.L + K.lamp_in, s.R - K.lamp_in }) s.put(EL_LAMP, Rect::around(x, s.F - K.lamp_back, P.lamp, P.lamp), K.lamp_h, post);
	for (float x = s.L + K.plaza_bollard_every * HALF; x < s.R - K.bollard; x += K.plaza_bollard_every)
		s.put(EL_BOLLARD, Rect::around(x, K.bollard_at, K.bollard, K.bollard), K.bollard_h, post);
}

void apron(LotSpace &s) {
	const LotFrontShape &K = s.data.shapes.front;
	const LotPartShape &P = s.data.shapes.parts;
	const LotRules &l = s.rules;
	s.cover({ s.door_x - K.apron_path * HALF, 0, s.door_x + K.apron_path * HALF, s.F }, "path", 2);
	if (s.F >= l.row_depth(1) && !s.built_box.empty()) {
		const int line = s.paint("stall_line");
		for (float x = s.built_box.x0 + K.visitor_from; x + l.stall_w <= s.built_box.x1; x += l.stall_w) {
			if (std::abs(x + l.stall_w * HALF - s.door_x) < K.visitor_door) continue;
			s.flat(EL_STALL, { x, s.F - l.stall_d, x + l.stall_w, s.F }, 0, line, FACE_STREET);
		}
	}
	s.put(EL_LAMP, Rect::around(s.door_x + K.apron_lamp_x, s.F - K.apron_lamp_in, P.lamp, P.lamp), K.apron_lamp_h, s.paint("lamp_post"));
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
