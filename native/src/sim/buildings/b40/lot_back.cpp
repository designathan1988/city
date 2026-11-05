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
constexpr float BAY_OFF_WALL = 0.25f, ROW_FROM = 0.3f, DOOR_GAP = 0.9f, PLANTER_MIN = 1.4f, PLANTER_D = 1.6f, PLANTER_H = 0.5f;
constexpr float CROWN_TOP = 5.f, LAMP = 0.3f, DRAIN = 0.5f, DRAIN_H = 0.02f, WALK_W = 1.4f;
constexpr float BIN_W = 1.4f, BIN_D = 0.7f, BIN_H = 1.1f, SKIP_W = 2.2f, SKIP_D = 1.4f, SKIP_H = 1.3f, BIN_X = 2.2f, SKIP_X = 4.f;
constexpr float BIN_Y = 0.6f, SKIP_Y = 0.9f, PALLET = 1.1f, PALLET_H = 0.28f, PALLET_X = 3.f, PALLET_Y = 1.f;
constexpr float CAGE_W = 1.6f, CAGE_D = 0.9f, CAGE_H = 1.4f, CAGE_IN = 1.2f, AC_W = 0.8f, AC_D = 0.35f, AC_H = 0.6f, AC_Z = 2.6f;
constexpr float AC_EVERY = 2.4f, SERVICE_LAMP_H = 3.2f, SERVICE_LAMP_X = 1.6f, SERVICE_LAMP_Y = 0.4f, CAGE_MIN_W = 8.f;
constexpr float SQUARE_W = 6.f, SQUARE_D = 4.f, SQUARE_FROM = 2.f, BENCH_W = 1.6f, BENCH_D = 0.5f, BENCH_H = 0.45f;
constexpr float BENCH_X = 1.5f, PLAY = 4.f, PLAY_IN = 0.5f, PLAY_MIN_D = 8.f, PLAY_MIN_W = 12.f, SWING_W = 2.3f, SWING_D = 1.2f;
constexpr float SWING_H = 2.2f, COURT_TREE = 3.f, COURT_TREE_IN = 2.6f, COURT_TREE_BACK = 2.8f, COURT_BIN_IN = 1.2f;
constexpr float DOCK_EDGE = 0.3f, DOCK_ON_DOOR = 0.6f, DOCK_APART = 0.5f, BOLLARD = 0.25f, BOLLARD_H = 1.f, BOLLARD_OUT = 0.5f;
constexpr float YARD_LAMP_H = 6.f, YARD_LAMP_IN = 0.3f, YARD_BACK = 0.5f, HALF = 0.5f;
// Car park lights: lamps on the building's back wall over the bays and the aisle, out of the cars' way.
constexpr float WALL_LAMP_EVERY = 8.f, WALL_LAMP_Z = 3.f, WALL_LAMP_OUT = 0.4f, WALL_LAMP_W = 0.3f, WALL_LAMP_H = 0.15f;

// Free x stretches of a row from a to b less the cuts: each laid with bays, its leftover planted.
void row(LotSpace &s, float y0, float y1, int facing, std::vector<std::pair<float, float>> cuts) {
	const LotRules &l = s.rules;
	const int line = s.paint("stall_line"), planter = s.paint("planter"), tree = s.paint("hedge");
	std::sort(cuts.begin(), cuts.end());
	cuts.push_back({ s.R - ROW_FROM, s.R });
	float at = s.L + ROW_FROM;
	for (const auto &c : cuts) {
		float a = at, b = std::min(c.first, s.R - ROW_FROM);
		at = std::max(at, c.second);
		if (b <= a) continue; // a cut ending past the row's end leaves nothing (never a planter in a drive's mouth)
		int n = int(std::floor((b - a) / l.stall_w));
		float used = float(n) * l.stall_w, x0 = a + (b - a - used) * HALF;
		for (int k = 0; k < n; k++) s.flat(EL_STALL, { x0 + float(k) * l.stall_w, y0, x0 + float(k + 1) * l.stall_w, y1 }, 0, line, facing);
		float spare = (b - a - used) * HALF;
		if (spare < PLANTER_MIN * HALF) continue;
		for (float x : { a + spare * HALF, b - spare * HALF })
			if (s.put(EL_PLANTER, Rect::around(x, (y0 + y1) * HALF, std::min(spare, PLANTER_D), PLANTER_D), PLANTER_H, planter))
				s.edge(EL_TREE, Rect::around(x, (y0 + y1) * HALF, PLANTER_D, PLANTER_D), CROWN_TOP, tree);
	}
}

void car_park(LotSpace &s) {
	const LotRules &l = s.rules;
	row(s, s.D - BAY_OFF_WALL - l.stall_d, s.D - BAY_OFF_WALL, FACE_STREET, {});
	float aisle0 = s.B;
	if (s.plan.rows >= 2) {
		std::vector<std::pair<float, float>> cuts;
		for (const Strip &st : s.strips)
			if (!st.drive.empty()) cuts.push_back({ st.drive.x0, st.drive.x1 });
		if (s.back_door) cuts.push_back({ s.back_door_x - DOOR_GAP, s.back_door_x + DOOR_GAP });
		row(s, s.B, s.B + l.stall_d, FACE_BACK, cuts);
		aisle0 = s.B + l.stall_d;
	}
	const float aisle_y = (aisle0 + s.D - BAY_OFF_WALL - l.stall_d) * HALF;
	s.flat(EL_DRAIN, Rect::around((s.L + s.R) * HALF, aisle_y, DRAIN, DRAIN), DRAIN_H, s.paint("grille"));
	const int post = s.paint("lamp_post");
	if (!s.built_box.empty())
		for (float x = s.built_box.x0 + WALL_LAMP_EVERY * HALF; x < s.built_box.x1; x += WALL_LAMP_EVERY) {
			float wall = s.built_back(x - WALL_LAMP_W, x + WALL_LAMP_W);
			s.edge(EL_LAMP, { x - WALL_LAMP_W * HALF, wall, x + WALL_LAMP_W * HALF, wall + WALL_LAMP_OUT }, WALL_LAMP_H, post, 0, WALL_LAMP_Z);
		}
	if (s.back_door) s.cover({ s.back_door_x - WALK_W * HALF, s.B, s.back_door_x + WALK_W * HALF, aisle0 + 1 }, "pavers", 2);
}

void service(LotSpace &s) {
	const float bd = s.back_door ? s.back_door_x : s.built_box.cx();
	s.put(EL_BIN, Rect::around(std::min(s.R - BIN_W, bd + BIN_X), s.B + BIN_Y, BIN_W, BIN_D), BIN_H, s.paint("bin"));
	s.put(EL_BIN, Rect::around(std::min(s.R - SKIP_W, bd + SKIP_X), s.B + SKIP_Y + BIN_D, SKIP_W, SKIP_D), SKIP_H, s.paint("skip"));
	s.put(EL_PALLET, Rect::around(std::max(s.L + PALLET, bd - PALLET_X), s.B + PALLET_Y, PALLET, PALLET), PALLET_H, s.paint("shed"));
	if (s.R - s.L >= CAGE_MIN_W) s.put(EL_CAGE, Rect::around(s.L + CAGE_IN, s.D - CAGE_D, CAGE_W, CAGE_D), CAGE_H, s.paint("fence"));
	const int ac = s.paint("wall_ac");
	if (!s.built_box.empty())
		for (float x = s.built_box.x0 + AC_EVERY * HALF; x < s.built_box.x1 - AC_W; x += AC_EVERY) {
			if (s.back_door && std::abs(x - s.back_door_x) < AC_EVERY * HALF) continue;
			float wall = s.built_back(x - AC_W * HALF, x + AC_W * HALF);
			s.put(EL_AC, { x - AC_W * HALF, wall, x + AC_W * HALF, wall + AC_D }, AC_H, ac, FACE_BACK, AC_Z);
		}
	s.put(EL_LAMP, Rect::around(std::max(s.L + 1, bd - SERVICE_LAMP_X), s.B + SERVICE_LAMP_Y + LAMP, LAMP, LAMP), SERVICE_LAMP_H, s.paint("lamp_post"));
	s.flat(EL_DRAIN, Rect::around(bd, (s.B + s.D) * HALF, DRAIN, DRAIN), DRAIN_H, s.paint("grille"));
}

void court(LotSpace &s) {
	const float cx = s.built_box.empty() ? (s.L + s.R) * HALF : s.built_box.cx();
	const Rect sq{ cx - SQUARE_W * HALF, s.B + SQUARE_FROM, cx + SQUARE_W * HALF, std::min(s.D - YARD_BACK, s.B + SQUARE_FROM + SQUARE_D) };
	s.cover(sq, "square_paving", 2);
	if (s.back_door) s.cover({ s.back_door_x - WALK_W * HALF, s.B, s.back_door_x + WALK_W * HALF, sq.y0 }, "pavers", 2);
	const int bench = s.paint("bench");
	for (float dx : { -BENCH_X, BENCH_X }) s.put(EL_BENCH, Rect::around(cx + dx, sq.cy(), BENCH_W, BENCH_D), BENCH_H, bench, FACE_BACK);
	if (s.D - s.B >= PLAY_MIN_D && s.R - s.L >= PLAY_MIN_W) {
		Rect play{ s.R - PLAY - PLAY_IN * 2, s.D - PLAY - PLAY_IN, s.R - PLAY_IN * 2, s.D - PLAY_IN };
		s.cover(play, "sand", 2);
		s.put(EL_SWING, Rect::around(play.cx(), play.cy(), SWING_W, SWING_D), SWING_H, s.paint("play_blue"));
	}
	const int tree = s.paint("hedge");
	for (float x : { s.L + COURT_TREE_IN, s.R - COURT_TREE_IN }) s.put(EL_TREE, Rect::around(x, s.D - COURT_TREE_BACK, COURT_TREE, COURT_TREE), CROWN_TOP, tree);
	s.put(EL_BIN, Rect::around(s.L + COURT_BIN_IN, s.B + BIN_Y, BIN_W, BIN_D), BIN_H, s.paint("bin"));
}

void loading(LotSpace &s) {
	const LotRules &l = s.rules;
	const float dw = l.lorry_w + 2 * l.dock_margin, wall = std::max(s.B, s.loading_y);
	const float depth = std::min(l.lorry_l + l.dock_margin, s.D - wall - YARD_BACK);
	const int dock = s.paint("dock"), bollard = s.paint("lamp_post");
	auto lay = [&](float x) {
		if (!s.flat(EL_DOCK, { x - dw * HALF, wall, x + dw * HALF, wall + depth }, 0, dock, FACE_BACK)) return false;
		for (float side : { -1.f, 1.f })
			s.put(EL_BOLLARD, Rect::around(x + side * (dw * HALF + BOLLARD), wall + BOLLARD_OUT, BOLLARD, BOLLARD), BOLLARD_H, bollard);
		return true;
	};
	float last = -FAR_AWAY;
	for (float door : s.loading_x) {
		float x = std::clamp(door, s.L + DOCK_EDGE + dw * HALF, s.R - DOCK_EDGE - dw * HALF);
		if (std::abs(x - door) > DOCK_ON_DOOR || x - last < dw + DOCK_APART) continue;
		if (lay(x)) last = x;
	}
	if (last == -FAR_AWAY && !s.built_box.empty())
		for (float x = s.built_box.x0 + dw * HALF; x + dw * HALF <= s.built_box.x1; x += dw + DOCK_APART + 2 * BOLLARD) lay(x);
	const int post = s.paint("lamp_post");
	for (float x : { s.L + YARD_LAMP_IN, s.R - YARD_LAMP_IN }) s.put(EL_LAMP, Rect::around(x, (s.B + s.D) * HALF, LAMP, LAMP), YARD_LAMP_H, post);
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
