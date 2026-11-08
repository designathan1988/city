// The back zone of lots other than houses. A car park: rows of bays (lots.stall_w x stall_d) along the back boundary
// and, for two rows, along the building, the second row left open at the drive's mouth and at the back door; the ends
// too short for a bay planted (a tree in a planter); lamps; a drain in the aisle; the walk from the back door. A service
// yard: bins and a skip, pallets, a caged gas store, air conditioners on the back wall, a lamp, a drain. A residents'
// court: a paved square with benches, a sand play corner with a swing, trees, a bin. A loading yard: a dock at each
// loading door, as wide as the widest lorry and its margins and as deep as a lorry, with bollards at its mouth.
#include "sim/buildings/b40/lot_layout.h"

#include <algorithm>
#include <cmath>

namespace city::b40 {

namespace {
constexpr float HALF = 0.5f;
// Car park lights: lamps on the building's back wall over the bays and the aisle, out of the cars' way.

// Free x stretches of a row from a to b less the cuts: each laid with bays, its leftover planted.
void row(LotSpace &s, float y0, float y1, int facing, std::vector<std::pair<float, float>> cuts) {
	const LotBackShape &K = s.data.shapes.back;
	const LotRules &l = s.rules;
	const int line = s.paint("stall_line"), planter = s.paint("planter"), tree = s.paint("hedge");
	std::sort(cuts.begin(), cuts.end());
	cuts.push_back({ s.R - K.row_from, s.R });
	float at = s.L + K.row_from;
	for (const auto &c : cuts) {
		float a = at, b = std::min(c.first, s.R - K.row_from);
		at = std::max(at, c.second);
		if (b <= a) continue; // a cut ending past the row's end leaves nothing (never a planter in a drive's mouth)
		int n = int(std::floor((b - a) / l.stall_w));
		float used = float(n) * l.stall_w, x0 = a + (b - a - used) * HALF;
		for (int k = 0; k < n; k++) s.flat(EL_STALL, { x0 + float(k) * l.stall_w, y0, x0 + float(k + 1) * l.stall_w, y1 }, 0, line, facing);
		float spare = (b - a - used) * HALF;
		if (spare < K.planter_min * HALF) continue;
		for (float x : { a + spare * HALF, b - spare * HALF })
			if (s.put(EL_PLANTER, Rect::around(x, (y0 + y1) * HALF, std::min(spare, K.planter_d), K.planter_d), K.planter_h, planter))
				s.edge(EL_TREE, Rect::around(x, (y0 + y1) * HALF, K.planter_d, K.planter_d), K.crown_top, tree);
	}
}

void car_park(LotSpace &s) {
	const LotBackShape &K = s.data.shapes.back;
	const LotPartShape &P = s.data.shapes.parts;
	const LotRules &l = s.rules;
	row(s, s.D - K.bay_off_wall - l.stall_d, s.D - K.bay_off_wall, FACE_STREET, {});
	float aisle0 = s.B;
	if (s.plan.rows >= 2) {
		std::vector<std::pair<float, float>> cuts;
		for (const Strip &st : s.strips)
			if (!st.drive.empty()) cuts.push_back({ st.drive.x0, st.drive.x1 });
		if (s.back_door) cuts.push_back({ s.back_door_x - K.door_gap, s.back_door_x + K.door_gap });
		row(s, s.B, s.B + l.stall_d, FACE_BACK, cuts);
		aisle0 = s.B + l.stall_d;
	}
	const float aisle_y = (aisle0 + s.D - K.bay_off_wall - l.stall_d) * HALF;
	s.flat(EL_DRAIN, Rect::around((s.L + s.R) * HALF, aisle_y, P.drain, P.drain), P.drain_h, s.paint("grille"));
	const int post = s.paint("lamp_post");
	if (!s.built_box.empty())
		for (float x = s.built_box.x0 + K.wall_lamp_every * HALF; x < s.built_box.x1; x += K.wall_lamp_every) {
			float wall = s.built_back(x - K.wall_lamp_w, x + K.wall_lamp_w);
			s.edge(EL_LAMP, { x - K.wall_lamp_w * HALF, wall, x + K.wall_lamp_w * HALF, wall + K.wall_lamp_out }, K.wall_lamp_h, post, 0, K.wall_lamp_z);
		}
	if (s.back_door) s.cover({ s.back_door_x - K.walk_w * HALF, s.B, s.back_door_x + K.walk_w * HALF, aisle0 + 1 }, "pavers", 2);
}

void service(LotSpace &s) {
	const LotBackShape &K = s.data.shapes.back;
	const LotPartShape &P = s.data.shapes.parts;
	const float bd = s.back_door ? s.back_door_x : s.built_box.cx();
	s.put(EL_BIN, Rect::around(std::min(s.R - K.bin_w, bd + K.bin_x), s.B + K.bin_y, K.bin_w, K.bin_d), K.bin_h, s.paint("bin"));
	s.put(EL_BIN, Rect::around(std::min(s.R - K.skip_w, bd + K.skip_x), s.B + K.skip_y + K.bin_d, K.skip_w, K.skip_d), K.skip_h, s.paint("skip"));
	s.put(EL_PALLET, Rect::around(std::max(s.L + P.pallet, bd - K.pallet_x), s.B + K.pallet_y, P.pallet, P.pallet), P.pallet_h, s.paint("shed"));
	if (s.R - s.L >= K.cage_min_w) s.put(EL_CAGE, Rect::around(s.L + K.cage_in, s.D - K.cage_d, K.cage_w, K.cage_d), K.cage_h, s.paint("fence"));
	const int ac = s.paint("wall_ac");
	if (!s.built_box.empty())
		for (float x = s.built_box.x0 + K.ac_every * HALF; x < s.built_box.x1 - K.ac_w; x += K.ac_every) {
			if (s.back_door && std::abs(x - s.back_door_x) < K.ac_every * HALF) continue;
			float wall = s.built_back(x - K.ac_w * HALF, x + K.ac_w * HALF);
			s.put(EL_AC, { x - K.ac_w * HALF, wall, x + K.ac_w * HALF, wall + K.ac_d }, K.ac_h, ac, FACE_BACK, K.ac_z);
		}
	s.put(EL_LAMP, Rect::around(std::max(s.L + 1, bd - K.service_lamp_x), s.B + K.service_lamp_y + P.lamp, P.lamp, P.lamp), K.service_lamp_h, s.paint("lamp_post"));
	s.flat(EL_DRAIN, Rect::around(bd, (s.B + s.D) * HALF, P.drain, P.drain), P.drain_h, s.paint("grille"));
}

void court(LotSpace &s) {
	const LotBackShape &K = s.data.shapes.back;
	const LotPartShape &P = s.data.shapes.parts;
	const float cx = s.built_box.empty() ? (s.L + s.R) * HALF : s.built_box.cx();
	const Rect sq{ cx - K.square_w * HALF, s.B + K.square_from, cx + K.square_w * HALF, std::min(s.D - K.yard_back, s.B + K.square_from + K.square_d) };
	s.cover(sq, "square_paving", 2);
	if (s.back_door) s.cover({ s.back_door_x - K.walk_w * HALF, s.B, s.back_door_x + K.walk_w * HALF, sq.y0 }, "pavers", 2);
	const int bench = s.paint("bench");
	for (float dx : { -K.bench_x, K.bench_x }) s.put(EL_BENCH, Rect::around(cx + dx, sq.cy(), K.bench_w, K.bench_d), P.bench_h, bench, FACE_BACK);
	if (s.D - s.B >= K.play_min_d && s.R - s.L >= K.play_min_w) {
		Rect play{ s.R - K.play - K.play_in * 2, s.D - K.play - K.play_in, s.R - K.play_in * 2, s.D - K.play_in };
		s.cover(play, "sand", 2);
		s.put(EL_SWING, Rect::around(play.cx(), play.cy(), P.swing_w, P.swing_d), P.swing_h, s.paint("play_blue"));
	}
	const int tree = s.paint("hedge");
	for (float x : { s.L + K.court_tree_in, s.R - K.court_tree_in }) s.put(EL_TREE, Rect::around(x, s.D - K.court_tree_back, K.court_tree, K.court_tree), K.crown_top, tree);
	s.put(EL_BIN, Rect::around(s.L + K.court_bin_in, s.B + K.bin_y, K.bin_w, K.bin_d), K.bin_h, s.paint("bin"));
}

void loading(LotSpace &s) {
	const LotBackShape &K = s.data.shapes.back;
	const LotPartShape &P = s.data.shapes.parts;
	const LotRules &l = s.rules;
	const float dw = l.lorry_w + 2 * l.dock_margin, wall = std::max(s.B, s.loading_y);
	const float depth = std::min(l.lorry_l + l.dock_margin, s.D - wall - K.yard_back);
	const int dock = s.paint("dock"), bollard = s.paint("lamp_post");
	auto lay = [&](float x) {
		if (!s.flat(EL_DOCK, { x - dw * HALF, wall, x + dw * HALF, wall + depth }, 0, dock, FACE_BACK)) return false;
		for (float side : { -1.f, 1.f })
			s.put(EL_BOLLARD, Rect::around(x + side * (dw * HALF + K.bollard), wall + K.bollard_out, K.bollard, K.bollard), K.bollard_h, bollard);
		return true;
	};
	float last = -FAR_AWAY;
	for (float door : s.loading_x) {
		float x = std::clamp(door, s.L + K.dock_edge + dw * HALF, s.R - K.dock_edge - dw * HALF);
		if (std::abs(x - door) > K.dock_on_door || x - last < dw + K.dock_apart) continue;
		if (lay(x)) last = x;
	}
	if (last == -FAR_AWAY && !s.built_box.empty())
		for (float x = s.built_box.x0 + dw * HALF; x + dw * HALF <= s.built_box.x1; x += dw + K.dock_apart + 2 * K.bollard) lay(x);
	const int post = s.paint("lamp_post");
	for (float x : { s.L + K.yard_lamp_in, s.R - K.yard_lamp_in }) s.put(EL_LAMP, Rect::around(x, (s.B + s.D) * HALF, P.lamp, P.lamp), K.yard_lamp_h, post);
}
} // namespace

void lay_back(LotSpace &s) {
	switch (s.plan.back) {
		case BACK_PARKING: car_park(s); break;
		case BACK_SERVICE: service(s); break;
		case BACK_COURTYARD: court(s); break;
		case BACK_LOADING: loading(s); break;
		default: break;
	}
}

} // namespace city::b40
