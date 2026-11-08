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
constexpr float HALF = 0.5f;
constexpr int THEMES = 5; // the lived yard's themes, the cases of yard_life's switch

void table(LotSpace &s, float x, float y) {
	const LotYardShape &K = s.data.shapes.yard;
	const LotPartShape &P = s.data.shapes.parts;
	const int wood = s.paint("bench");
	if (!s.put(EL_TABLE, Rect::around(x, y, K.table, K.table), K.table_h, wood)) return;
	s.put(EL_BENCH, Rect::around(x, y - K.seat_at, K.seat_w, K.seat_d), P.bench_h, wood, FACE_BACK);
	s.put(EL_BENCH, Rect::around(x, y + K.seat_at, K.seat_w, K.seat_d), P.bench_h, wood, FACE_STREET);
}

// The lived part of a deep yard, one of the themes, its x mirrored at random.
void yard_life(LotSpace &s, const Rect &a) {
	const LotYardShape &K = s.data.shapes.yard;
	const LotPartShape &P = s.data.shapes.parts;
	const bool flip = s.r.f() < HALF;
	auto fx = [&](float x) { return flip ? a.x0 + a.x1 - x : x; };
	const int tree = s.paint("hedge"), shed = s.paint("shed");
	switch (s.r.irange(0, THEMES - 1)) {
		case 0: // an orchard
			for (float y = a.y0 + K.fruit * HALF; y < a.y1 - K.fruit * HALF; y += K.orchard_every)
				for (float x = a.x0 + K.fruit * HALF; x < a.x1 - K.fruit * HALF; x += K.orchard_every) s.put(EL_TREE, Rect::around(x, y, K.fruit, K.fruit), K.fruit_h, tree);
			break;
		case 1: { // a kitchen garden in raised beds, a coop
			const int bed = s.paint("deck");
			for (int i = 0; i < K.box_cols; i++)
				for (int j = 0; j < K.box_rows; j++)
					s.put(EL_PLANTER, Rect::around(fx(a.x0 + K.box_in + K.plot_w * HALF + float(i) * K.box_x), a.y0 + K.box_in + float(j) * K.box_y, K.plot_w, K.box_d), K.box_h, bed);
			s.put(EL_CAGE, Rect::around(fx(a.x1 - K.coop_in - K.coop_w * HALF), a.y1 - K.coop_in - K.coop_d * HALF, K.coop_w, K.coop_d), K.coop_h, s.paint("fence"));
			break;
		}
		case 2: { // a play corner
			Rect sand = Rect::around(fx(a.x0 + K.sand), a.cy(), K.sand, K.sand);
			s.cover(sand, "sand", 2);
			s.put(EL_SWING, Rect::around(sand.cx(), sand.cy(), P.swing_w, P.swing_d), P.swing_h, s.paint("play_blue"));
			table(s, fx(a.x1 - K.table * 2), a.cy());
			s.put(EL_SHED, Rect::around(fx(a.x1 - K.dog), a.y1 - K.dog, K.dog, K.dog), K.dog_h, shed);
			break;
		}
		case 3: { // a workshop shed, its pallets, a table by it
			Rect sh = Rect::around(fx(a.x1 - K.shed_w * HALF), a.y1 - K.shed_d * HALF, K.shed_w, K.shed_d);
			s.put(EL_SHED, sh, K.shed_h, shed);
			s.put(EL_PALLET, Rect::around(fx(a.x0 + P.pallet), a.y1 - P.pallet, P.pallet, P.pallet), P.pallet_h, shed);
			table(s, fx(a.x0 + K.table * 3), a.y0 + K.table * 2);
			break;
		}
		default: { // an edicula with its deck, a table, trees
			Rect ed = Rect::around(a.cx(), a.y1 - K.edicula_d * HALF, K.edicula_w, K.edicula_d);
			if (s.put(EL_SHED, ed, K.edicula_h, s.paint("edicula"))) s.cover({ ed.x0, ed.y0 - K.deck_d, ed.x1, ed.y0 }, "deck", 2);
			table(s, fx(a.x0 + K.table * 2), a.y0 + K.table * 2);
			for (float x : { a.x0 + K.fruit * HALF, a.x1 - K.fruit * HALF }) s.put(EL_TREE, Rect::around(x, a.y0 + K.fruit * HALF, K.fruit, K.fruit), K.fruit_h, tree);
			break;
		}
	}
}

void house_yard(LotSpace &s) {
	const LotYardShape &K = s.data.shapes.yard;
	const LotPartShape &P = s.data.shapes.parts;
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
		for (float x = t.x0 + K.pot_in; x < t.x1 - K.pot_in; x += K.pot_every) s.put(EL_PLANTER, Rect::around(x, t.y1 - K.pot_in, K.pot, K.pot), K.pot_h, pot);
		if (t.w() > widest.w()) widest = t;
	}
	s.put(EL_BENCH, Rect::around(widest.cx() + K.bench_x, widest.cy(), K.bench_w, K.bench_d), P.bench_h, s.paint("bench"), FACE_STREET);
	const float lawn = std::min(s.D, hb.y1 + l.terrace), depth = s.D - lawn;
	const float mid = (s.L + s.R) * HALF;
	if (s.r.f() < l.pool_share && depth >= l.pool_d + K.pool_gap + K.pool_room && s.R - s.L >= l.pool_w + 2 * K.pool_side) {
		Rect pool = Rect::around(mid, lawn + K.pool_gap + l.pool_d * HALF, l.pool_w, l.pool_d);
		if (s.put(EL_POOL, pool, l.pool_depth, s.paint("pool"))) {
			s.cover(pool.grown(K.deck), "pool_coping", 2);
			const int lounger = s.paint("bench");
			for (int k = 0; k < 2; k++) {
				float y = pool.y0 + K.lounger_l * HALF + float(k) * (K.lounger_l + K.lounger_gap);
				s.put(EL_LOUNGER, Rect::around(pool.x1 + K.lounger_out, y, K.lounger_w, K.lounger_l), K.lounger_h, lounger, FACE_LEFT);
			}
		}
	}
	for (const Strip &st : s.strips) {
		if (st.drive.empty()) continue;
		// The drive ends in a paved service corner: bins, a drain (where no garage stands at its end).
		Rect corner{ st.drive.x0, s.B, st.drive.x0 + K.corner, s.B + K.corner };
		if (s.put(EL_BIN, Rect::around(corner.cx(), corner.y1 - K.bin_d, K.bin_w, K.bin_d), K.bin_h, s.paint("bin"))) {
			s.cover(corner, "path", 2);
			s.flat(EL_DRAIN, Rect::around(corner.cx(), corner.y0 + P.drain, P.drain, P.drain), P.drain_h, s.paint("grille"));
		}
	}
	const int flowers = s.paint("flowers"), tree = s.paint("hedge");
	for (float x = s.L + K.bed_from; x < s.R - K.bed_from; x += K.bed_every) s.put(EL_FLOWERS, Rect::around(x, s.D - K.bed_back, K.bed_w, K.bed_d), K.bed_h, flowers);
	for (float x : { s.L + K.corner_tree_in, s.R - K.corner_tree_in }) s.put(EL_TREE, Rect::around(x, s.D - K.corner_tree_back, K.corner_tree, K.corner_tree), K.tree_h, tree);
	for (float x : { s.L + K.shrub_in, s.R - K.shrub_in })
		for (float y = lawn + K.shrub_in; y < s.D - K.shrub_to; y += K.shrub_every) s.put(EL_SHRUB, Rect::around(x, y, K.bush, K.bush), K.bush_top, tree);
	if (depth >= K.line_min_d && s.r.f() < K.clothes_share) {
		float x0 = s.L + (s.R - s.L) * K.line_from, x1 = x0 + (s.R - s.L) * K.line_span, y = lawn + depth * HALF;
		s.put(EL_LINE, { x0, y - K.line_d * HALF, x1, y + K.line_d * HALF }, K.line_h, s.paint("lamp_post"));
	}
	if (depth >= l.yard_life_depth) yard_life(s, { s.L + K.life_in, lawn + depth * K.life_from, s.R - K.life_in, s.D - K.bed_back - K.bed_d });
}

void free_court(LotSpace &s, const Rect &c) {
	const LotYardShape &K = s.data.shapes.yard;
	const LotPartShape &P = s.data.shapes.parts;
	if (s.b.kind == KIND_WORKS) {
		s.cover(c, "asphalt", 2);
		const LotRules &l = s.rules;
		if (c.d() >= l.stall_d)
			for (float x = c.x0; x + l.stall_w <= c.x1; x += l.stall_w)
				s.flat(EL_STALL, { x, c.y0, x + l.stall_w, c.y0 + l.stall_d }, 0, s.paint("stall_line"), FACE_BACK);
		return;
	}
	if (c.w() <= K.small_w && c.d() <= K.small_d) {
		s.cover(c, "tiles", 2);
		s.put(EL_PLANTER, Rect::around(c.x1 - K.planter, c.y1 - K.planter, K.planter, K.planter), K.planter_h, s.paint("planter"));
		s.put(EL_BENCH, Rect::around(c.cx(), c.y1 - K.bench_d, K.bench_w, K.bench_d), P.bench_h, s.paint("bench"), FACE_STREET);
		return;
	}
	Rect sq = Rect::around(c.cx(), c.cy(), std::min(c.w() - 2 * K.square_in, K.square_max), std::min(c.d() - 2 * K.square_in, K.square_max));
	s.cover(sq, "square_paving", 2);
	for (float dy : { -1.f, 1.f }) s.put(EL_BENCH, Rect::around(sq.cx(), sq.cy() + dy * (sq.d() * HALF - K.bench_d), K.bench_w, K.bench_d), P.bench_h, s.paint("bench"), dy < 0 ? FACE_BACK : FACE_STREET);
	const int tree = s.paint("hedge");
	s.put(EL_TREE, Rect::around(c.x0 + K.free_tree_in, c.y0 + K.free_tree_in, K.corner_tree, K.corner_tree), K.tree_h, tree);
	s.put(EL_TREE, Rect::around(c.x1 - K.free_tree_in, c.y1 - K.free_tree_in, K.corner_tree, K.corner_tree), K.tree_h, tree);
	s.put(EL_FLOWERS, Rect::around(c.x1 - K.free_flowers, c.y0 + K.free_flowers, K.free_flowers, K.free_flowers), K.flowers_h, s.paint("flowers"));
}

// The envelope less the ground volumes, in bands between the volumes' y edges (a house's band behind it is its yard).
void free_courts(LotSpace &s) {
	const LotYardShape &K = s.data.shapes.yard;
	if (s.built_box.empty()) return;
	const Rect env{ s.plan.x0 + s.L, s.plan.y0, s.plan.x1 + s.L, s.b.kind == KIND_HOUSE ? s.built_box.y1 : s.plan.y1 };
	std::vector<float> ys{ env.y0, env.y1 };
	for (const Rect &f : s.built) ys.insert(ys.end(), { std::clamp(f.y0, env.y0, env.y1), std::clamp(f.y1, env.y0, env.y1) });
	std::sort(ys.begin(), ys.end());
	ys.erase(std::unique(ys.begin(), ys.end()), ys.end());
	for (size_t j = 0; j + 1 < ys.size(); j++) {
		float y0 = ys[j], y1 = ys[j + 1];
		if (y1 - y0 < K.free_min) continue;
		std::vector<std::pair<float, float>> cut;
		for (const Rect &f : s.built)
			if (f.y0 < y1 && y0 < f.y1) cut.push_back({ f.x0, f.x1 });
		std::sort(cut.begin(), cut.end());
		float at = env.x0;
		cut.push_back({ env.x1, env.x1 });
		for (const auto &c : cut) {
			if (c.first - at >= K.free_min) free_court(s, { at, y0, std::min(c.first, env.x1), y1 });
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
