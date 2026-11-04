// A house's back yard and the courts a building leaves free in its envelope. The yard: a tiled terrace on the house's
// own back (wherever the house ends, not where its zone starts), pots and a bench on it; a pool on part of the yards
// (lots.pool, pool_share) with its stone deck and two loungers; beds along the back wall, trees in the back corners,
// shrubs along the side walls, a clothes line; a yard deep enough (lots.yard_life_depth) is lived in - an orchard, a
// kitchen garden with a coop, a play corner, a workshop shed or an edicula with its deck - mirrored at random. The free
// courts (envelope less the ground volumes): a works' are paved with bays, a small one tiled with a planter and a bench,
// a large one a garden square with benches, trees and flowers.
#include "sim/buildings/b40/lot_layout.h"

#include <algorithm>
#include <cmath>

namespace city::b40 {

namespace {
constexpr float POOL_GAP = 1.f, POOL_SIDE = 1.5f, POOL_ROOM = 2.f, DECK = 0.6f;
constexpr float LOUNGER_W = 0.65f, LOUNGER_L = 1.8f, LOUNGER_H = 0.35f, LOUNGER_GAP = 1.f, LOUNGER_OUT = 1.f;
constexpr float BED_EVERY = 2.2f, BED_W = 1.6f, BED_D = 1.2f, BED_H = 0.4f, BED_BACK = 1.f, BED_FROM = 1.5f;
constexpr float CORNER_TREE = 3.f, TREE_H = 5.f, CORNER_TREE_IN = 2.6f, CORNER_TREE_BACK = 2.8f;
constexpr float BUSH = 1.3f, BUSH_TOP = 1.4f, SHRUB_IN = 1.5f, SHRUB_EVERY = 3.2f, SHRUB_TO = 3.f;
constexpr float CLOTHES_SHARE = 0.5f, LINE_MIN_D = 6.f, LINE_H = 1.9f, LINE_SPAN = 0.4f, LINE_FROM = 0.2f, LINE_D = 0.2f;
constexpr float BENCH_W = 1.4f, BENCH_D = 0.45f, BENCH_H = 0.45f, BENCH_X = 1.5f, POT = 0.5f, POT_H = 0.55f, POT_EVERY = 1.6f;
constexpr float POT_IN = 0.4f, CORNER = 3.f, BIN_W = 1.3f, BIN_D = 0.65f, BIN_H = 1.1f, DRAIN = 0.5f, DRAIN_H = 0.02f;
constexpr float LIFE_FROM = 0.45f, LIFE_IN = 0.5f, ORCHARD_EVERY = 4.f, FRUIT = 3.f, FRUIT_H = 4.2f;
constexpr float BOX_W = 1.15f, BOX_D = 0.85f, BOX_H = 0.35f, BOX_X = 1.5f, BOX_Y = 1.25f, BOX_IN = 0.8f;
constexpr int BOX_COLS = 4, BOX_ROWS = 3, THEMES = 5;
constexpr float COOP_W = 2.4f, COOP_D = 1.6f, COOP_H = 1.2f, COOP_IN = 0.3f, SWING_W = 2.3f, SWING_D = 1.2f, SWING_H = 2.2f;
constexpr float SAND = 3.5f, SHED_W = 3.f, SHED_D = 2.4f, SHED_H = 2.4f, EDICULA_W = 5.f, EDICULA_D = 3.6f, EDICULA_H = 2.6f;
constexpr float TABLE = 1.f, TABLE_H = 0.75f, SEAT_W = 1.1f, SEAT_D = 0.42f, SEAT_AT = 0.95f, PALLET = 1.1f, PALLET_H = 0.28f;
constexpr float DOG = 1.f, DOG_H = 0.8f, DECK_D = 1.6f, FREE_MIN = 2.f, SMALL_W = 8.f, SMALL_D = 6.f, PLANTER = 1.f, PLANTER_H = 0.6f;
constexpr float SQUARE_MAX = 6.f, SQUARE_IN = 1.f, FREE_TREE_IN = 1.5f, FREE_FLOWERS = 1.4f, FLOWERS_H = 0.4f, HALF = 0.5f;

void table(LotSpace &s, float x, float y) {
	const int wood = s.paint("bench");
	if (!s.put(EL_TABLE, Rect::around(x, y, TABLE, TABLE), TABLE_H, wood)) return;
	s.put(EL_BENCH, Rect::around(x, y - SEAT_AT, SEAT_W, SEAT_D), BENCH_H, wood, FACE_BACK);
	s.put(EL_BENCH, Rect::around(x, y + SEAT_AT, SEAT_W, SEAT_D), BENCH_H, wood, FACE_STREET);
}

// The lived part of a deep yard, one of the themes, its x mirrored at random.
void yard_life(LotSpace &s, const Rect &a) {
	const bool flip = s.r.f() < HALF;
	auto fx = [&](float x) { return flip ? a.x0 + a.x1 - x : x; };
	const int tree = s.paint("hedge"), shed = s.paint("shed");
	switch (s.r.irange(0, THEMES - 1)) {
		case 0: // an orchard
			for (float y = a.y0 + FRUIT * HALF; y < a.y1 - FRUIT * HALF; y += ORCHARD_EVERY)
				for (float x = a.x0 + FRUIT * HALF; x < a.x1 - FRUIT * HALF; x += ORCHARD_EVERY) s.put(EL_TREE, Rect::around(x, y, FRUIT, FRUIT), FRUIT_H, tree);
			break;
		case 1: { // a kitchen garden in raised beds, a coop
			const int bed = s.paint("deck");
			for (int i = 0; i < BOX_COLS; i++)
				for (int j = 0; j < BOX_ROWS; j++)
					s.put(EL_PLANTER, Rect::around(fx(a.x0 + BOX_IN + BOX_W * HALF + float(i) * BOX_X), a.y0 + BOX_IN + float(j) * BOX_Y, BOX_W, BOX_D), BOX_H, bed);
			s.put(EL_CAGE, Rect::around(fx(a.x1 - COOP_IN - COOP_W * HALF), a.y1 - COOP_IN - COOP_D * HALF, COOP_W, COOP_D), COOP_H, s.paint("fence"));
			break;
		}
		case 2: { // a play corner
			Rect sand = Rect::around(fx(a.x0 + SAND), a.cy(), SAND, SAND);
			s.cover(sand, "sand", 2);
			s.put(EL_SWING, Rect::around(sand.cx(), sand.cy(), SWING_W, SWING_D), SWING_H, s.paint("play_blue"));
			table(s, fx(a.x1 - TABLE * 2), a.cy());
			s.put(EL_SHED, Rect::around(fx(a.x1 - DOG), a.y1 - DOG, DOG, DOG), DOG_H, shed);
			break;
		}
		case 3: { // a workshop shed, its pallets, a table by it
			Rect sh = Rect::around(fx(a.x1 - SHED_W * HALF), a.y1 - SHED_D * HALF, SHED_W, SHED_D);
			s.put(EL_SHED, sh, SHED_H, shed);
			s.put(EL_PALLET, Rect::around(fx(a.x0 + PALLET), a.y1 - PALLET, PALLET, PALLET), PALLET_H, shed);
			table(s, fx(a.x0 + TABLE * 3), a.y0 + TABLE * 2);
			break;
		}
		default: { // an edicula with its deck, a table, trees
			Rect ed = Rect::around(a.cx(), a.y1 - EDICULA_D * HALF, EDICULA_W, EDICULA_D);
			if (s.put(EL_SHED, ed, EDICULA_H, s.paint("edicula"))) s.cover({ ed.x0, ed.y0 - DECK_D, ed.x1, ed.y0 }, "deck", 2);
			table(s, fx(a.x0 + TABLE * 2), a.y0 + TABLE * 2);
			for (float x : { a.x0 + FRUIT * HALF, a.x1 - FRUIT * HALF }) s.put(EL_TREE, Rect::around(x, a.y0 + FRUIT * HALF, FRUIT, FRUIT), FRUIT_H, tree);
			break;
		}
	}
}

void house_yard(LotSpace &s) {
	const LotRules &l = s.rules;
	const Rect &hb = s.built_box;
	if (hb.empty()) return;
	// A terrace behind every ground volume of the house, on its own back face; the lawn begins past the deepest one.
	Rect widest;
	const int pot = s.paint("planter");
	for (size_t i = 0; i < s.own_parts; i++) {
		const Rect &f = s.built[i];
		const Rect t{ f.x0, f.y1, f.x1, std::min(s.D, f.y1 + l.terrace) };
		s.cover(t, "tiles", 2);
		for (float x = t.x0 + POT_IN; x < t.x1 - POT_IN; x += POT_EVERY) s.put(EL_PLANTER, Rect::around(x, t.y1 - POT_IN, POT, POT), POT_H, pot);
		if (t.w() > widest.w()) widest = t;
	}
	s.put(EL_BENCH, Rect::around(widest.cx() + BENCH_X, widest.cy(), BENCH_W, BENCH_D), BENCH_H, s.paint("bench"), FACE_STREET);
	const float lawn = std::min(s.D, hb.y1 + l.terrace), depth = s.D - lawn;
	const float mid = (s.L + s.R) * HALF;
	if (s.r.f() < l.pool_share && depth >= l.pool_d + POOL_GAP + POOL_ROOM && s.R - s.L >= l.pool_w + 2 * POOL_SIDE) {
		Rect pool = Rect::around(mid, lawn + POOL_GAP + l.pool_d * HALF, l.pool_w, l.pool_d);
		if (s.put(EL_POOL, pool, l.pool_depth, s.paint("pool"))) {
			s.cover(pool.grown(DECK), "pool_coping", 2);
			const int lounger = s.paint("bench");
			for (int k = 0; k < 2; k++) {
				float y = pool.y0 + LOUNGER_L * HALF + float(k) * (LOUNGER_L + LOUNGER_GAP);
				s.put(EL_LOUNGER, Rect::around(pool.x1 + LOUNGER_OUT, y, LOUNGER_W, LOUNGER_L), LOUNGER_H, lounger, FACE_LEFT);
			}
		}
	}
	for (const Strip &st : s.strips) {
		if (st.drive.empty()) continue;
		// The drive ends in a paved service corner: bins, a drain (where no garage stands at its end).
		Rect corner{ st.drive.x0, s.B, st.drive.x0 + CORNER, s.B + CORNER };
		if (s.put(EL_BIN, Rect::around(corner.cx(), corner.y1 - BIN_D, BIN_W, BIN_D), BIN_H, s.paint("bin"))) {
			s.cover(corner, "path", 2);
			s.flat(EL_DRAIN, Rect::around(corner.cx(), corner.y0 + DRAIN, DRAIN, DRAIN), DRAIN_H, s.paint("grille"));
		}
	}
	const int flowers = s.paint("flowers"), tree = s.paint("hedge");
	for (float x = s.L + BED_FROM; x < s.R - BED_FROM; x += BED_EVERY) s.put(EL_FLOWERS, Rect::around(x, s.D - BED_BACK, BED_W, BED_D), BED_H, flowers);
	for (float x : { s.L + CORNER_TREE_IN, s.R - CORNER_TREE_IN }) s.put(EL_TREE, Rect::around(x, s.D - CORNER_TREE_BACK, CORNER_TREE, CORNER_TREE), TREE_H, tree);
	for (float x : { s.L + SHRUB_IN, s.R - SHRUB_IN })
		for (float y = lawn + SHRUB_IN; y < s.D - SHRUB_TO; y += SHRUB_EVERY) s.put(EL_SHRUB, Rect::around(x, y, BUSH, BUSH), BUSH_TOP, tree);
	if (depth >= LINE_MIN_D && s.r.f() < CLOTHES_SHARE) {
		float x0 = s.L + (s.R - s.L) * LINE_FROM, x1 = x0 + (s.R - s.L) * LINE_SPAN, y = lawn + depth * HALF;
		s.put(EL_LINE, { x0, y - LINE_D * HALF, x1, y + LINE_D * HALF }, LINE_H, s.paint("lamp_post"));
	}
	if (depth >= l.yard_life_depth) yard_life(s, { s.L + LIFE_IN, lawn + depth * LIFE_FROM, s.R - LIFE_IN, s.D - BED_BACK - BED_D });
}

void free_court(LotSpace &s, const Rect &c) {
	if (s.b.kind == KIND_WORKS) {
		s.cover(c, "asphalt", 2);
		const LotRules &l = s.rules;
		if (c.d() >= l.stall_d)
			for (float x = c.x0; x + l.stall_w <= c.x1; x += l.stall_w)
				s.flat(EL_STALL, { x, c.y0, x + l.stall_w, c.y0 + l.stall_d }, 0, s.paint("stall_line"), FACE_BACK);
		return;
	}
	if (c.w() <= SMALL_W && c.d() <= SMALL_D) {
		s.cover(c, "tiles", 2);
		s.put(EL_PLANTER, Rect::around(c.x1 - PLANTER, c.y1 - PLANTER, PLANTER, PLANTER), PLANTER_H, s.paint("planter"));
		s.put(EL_BENCH, Rect::around(c.cx(), c.y1 - BENCH_D, BENCH_W, BENCH_D), BENCH_H, s.paint("bench"), FACE_STREET);
		return;
	}
	Rect sq = Rect::around(c.cx(), c.cy(), std::min(c.w() - 2 * SQUARE_IN, SQUARE_MAX), std::min(c.d() - 2 * SQUARE_IN, SQUARE_MAX));
	s.cover(sq, "square_paving", 2);
	for (float dy : { -1.f, 1.f }) s.put(EL_BENCH, Rect::around(sq.cx(), sq.cy() + dy * (sq.d() * HALF - BENCH_D), BENCH_W, BENCH_D), BENCH_H, s.paint("bench"), dy < 0 ? FACE_BACK : FACE_STREET);
	const int tree = s.paint("hedge");
	s.put(EL_TREE, Rect::around(c.x0 + FREE_TREE_IN, c.y0 + FREE_TREE_IN, CORNER_TREE, CORNER_TREE), TREE_H, tree);
	s.put(EL_TREE, Rect::around(c.x1 - FREE_TREE_IN, c.y1 - FREE_TREE_IN, CORNER_TREE, CORNER_TREE), TREE_H, tree);
	s.put(EL_FLOWERS, Rect::around(c.x1 - FREE_FLOWERS, c.y0 + FREE_FLOWERS, FREE_FLOWERS, FREE_FLOWERS), FLOWERS_H, s.paint("flowers"));
}

// The envelope less the ground volumes, in bands between the volumes' y edges (a house's band behind it is its yard).
void free_courts(LotSpace &s) {
	if (s.built_box.empty()) return;
	const Rect env{ s.plan.x0 + s.L, s.plan.y0, s.plan.x1 + s.L, s.b.kind == KIND_HOUSE ? s.built_box.y1 : s.plan.y1 };
	std::vector<float> ys{ env.y0, env.y1 };
	for (const Rect &f : s.built) ys.insert(ys.end(), { std::clamp(f.y0, env.y0, env.y1), std::clamp(f.y1, env.y0, env.y1) });
	std::sort(ys.begin(), ys.end());
	ys.erase(std::unique(ys.begin(), ys.end()), ys.end());
	for (size_t j = 0; j + 1 < ys.size(); j++) {
		float y0 = ys[j], y1 = ys[j + 1];
		if (y1 - y0 < FREE_MIN) continue;
		std::vector<std::pair<float, float>> cut;
		for (const Rect &f : s.built)
			if (f.y0 < y1 && y0 < f.y1) cut.push_back({ f.x0, f.x1 });
		std::sort(cut.begin(), cut.end());
		float at = env.x0;
		cut.push_back({ env.x1, env.x1 });
		for (const auto &c : cut) {
			if (c.first - at >= FREE_MIN) free_court(s, { at, y0, std::min(c.first, env.x1), y1 });
			at = std::max(at, c.second);
		}
	}
}
} // namespace

void lay_yard(LotSpace &s) {
	if (s.plan.back == BACK_YARD) house_yard(s);
	free_courts(s);
}

} // namespace city::b40
