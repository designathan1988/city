// Open lots as lot elements (lot frame: x from the lot's middle,
// y from the street). Park: a lawn behind a clipped hedge, a cross of paths with a ring walk, a fountain on a paved
// round-point, a sand playground (swings, climbing frame, see-saw) and benches, lamps and bins along the paths.
// Square: paving with lawn borders, a fountain, benches facing it, a bandstand on big squares and lamps on the axes.
// Sports court: a fenced, painted court on a lawn with goals. Trees are stood by the tree renderer where
// open_lot_tree_ok allows (the same layout numbers).
#include "sim/buildings/b40/civic.h"

#include "sim/buildings/building_store.h"

#include <algorithm>
#include <cmath>

namespace city::b40 {

namespace {
constexpr float HALF = 0.5f;

struct Lay {
	const Data &data;
	Body &b;
	float W, D;
	const OpenLotShape &K = data.shapes.open;
	const LotPartShape &P = data.shapes.parts;
	int paint(const char *n) { return b.add_paint(data.color(n)); }
	void surface(float x0, float y0, float x1, float y1, int p, float lift = 1) { put(b, EL_SURFACE, x0, y0, x1, y1, P.cover_h * lift, p); }
	void fountain(float cx, float cy, bool plaza);
	void benches_along_x(float y, float x0, float x1, int bench, int lamp, int bin);
	void benches_along_y(float x, float y0, float y1, int bench, int lamp, int bin);
	void playground(float cx, float cy);
	void hedges(float gap);
};

void Lay::fountain(float cx, float cy, bool plaza) {
	int stone = paint("pool_coping");
	if (plaza) surface(cx - K.pool_plaza / 2, cy - K.pool_plaza / 2, cx + K.pool_plaza / 2, cy + K.pool_plaza / 2, paint("square_paving"), 2);
	surface(cx - K.basin / 2, cy - K.basin / 2, cx + K.basin / 2, cy + K.basin / 2, paint("fountain_water"), K.rim_h / P.cover_h * HALF);
	for (float s : { -1.f, 1.f }) {
		put(b, EL_WALL, cx - K.basin / 2, cy + s * K.basin / 2 - K.rim_t / 2, cx + K.basin / 2, cy + s * K.basin / 2 + K.rim_t / 2, K.rim_h, stone);
		put(b, EL_WALL, cx + s * K.basin / 2 - K.rim_t / 2, cy - K.basin / 2, cx + s * K.basin / 2 + K.rim_t / 2, cy + K.basin / 2, K.rim_h, stone);
	}
	put_at(b, EL_PILLAR, cx, cy, K.spout, K.spout, K.spout_h, stone);
	put_at(b, EL_CANOPY, cx, cy, K.bowl, K.bowl, K.bowl_h, stone, K.spout_h);
}

void Lay::benches_along_x(float y, float x0, float x1, int bench, int lamp, int bin) {
	for (float x = x0 + K.bench_every / 2; x < x1 - K.bench_w; x += K.bench_every) {
		put_at(b, EL_BENCH, x, y - K.path_w / 2 - K.bench_d, K.bench_w, K.bench_d, P.bench_h, bench);
		if (std::fmod(x - x0, K.lamp_every) < K.bench_every) put_at(b, EL_LAMP, x + K.bench_w, y + K.path_w / 2 + K.lamp, K.lamp, K.lamp, K.lamp_h, lamp);
		else put_at(b, EL_BIN, x + K.bench_w, y - K.path_w / 2 - K.bin, K.bin, K.bin, K.bin_h, bin);
	}
}

void Lay::benches_along_y(float x, float y0, float y1, int bench, int lamp, int bin) {
	for (float y = y0 + K.bench_every / 2; y < y1 - K.bench_w; y += K.bench_every) {
		put_at(b, EL_BENCH, x - K.path_w / 2 - K.bench_d, y, K.bench_d, K.bench_w, P.bench_h, bench);
		if (std::fmod(y - y0, K.lamp_every) < K.bench_every) put_at(b, EL_LAMP, x + K.path_w / 2 + K.lamp, y + K.bench_w, K.lamp, K.lamp, K.lamp_h, lamp);
		else put_at(b, EL_BIN, x - K.path_w / 2 - K.bin, y + K.bench_w, K.bin, K.bin, K.bin_h, bin);
	}
}

void Lay::playground(float cx, float cy) {
	surface(cx - K.play / 2, cy - K.play / 2, cx + K.play / 2, cy + K.play / 2, paint("sand"), 2);
	int red = paint("play_red"), yellow = paint("play_yellow"), blue = paint("play_blue"), dark = paint("plant_dark");
	float sx = cx - K.play / 4, sy = cy - K.play / 4; // swings
	for (float s : { -1.f, 1.f }) put_at(b, EL_PILLAR, sx + s * K.swing_w / 2, sy, K.post * 2, K.post * 2, K.swing_h, red);
	put_at(b, EL_CANOPY, sx, sy, K.swing_w + K.post * 2, K.post * 2, K.post * 2, red, K.swing_h);
	for (float s : { -1.f, 1.f }) {
		put_at(b, EL_CANOPY, sx + s * K.swing_w / 4, sy, K.chain, K.chain, K.swing_h - K.seat_z, dark, K.seat_z);
		put_at(b, EL_CANOPY, sx + s * K.swing_w / 4, sy, K.seat, K.seat * HALF, K.post, dark, K.seat_z);
	}
	float fx = cx + K.play / 4, fy = cy + K.play / 4; // climbing frame
	for (float a : { -1.f, 1.f })
		for (float c : { -1.f, 1.f }) put_at(b, EL_PILLAR, fx + a * K.frame / 2, fy + c * K.frame / 2, K.post, K.post, K.frame_h * 2, blue);
	put_at(b, EL_CANOPY, fx, fy, K.frame + K.post, K.frame + K.post, K.post * 2, yellow, K.frame_h);
	put_at(b, EL_CANOPY, fx, fy, K.frame + K.post, K.frame + K.post, K.post * 2, red, K.frame_h * 2);
	put_at(b, EL_PILLAR, cx + K.play / 4, cy - K.play / 4, K.post * 2, K.post * 2, K.seesaw_z, dark); // see-saw
	put_at(b, EL_CANOPY, cx + K.play / 4, cy - K.play / 4, K.seesaw_l, K.seesaw_w, K.post, yellow, K.seesaw_z);
}

void Lay::hedges(float gap) {
	int h = paint("hedge");
	float x0 = -W / 2 + K.lawn_clear, x1 = W / 2 - K.lawn_clear, y0 = K.lawn_clear, y1 = D - K.lawn_clear;
	for (float y : { y0, y1 }) {
		put(b, EL_HEDGE, x0, y, -gap / 2, y + K.hedge_t, K.hedge_h, h);
		put(b, EL_HEDGE, gap / 2, y, x1, y + K.hedge_t, K.hedge_h, h);
	}
	for (float x : { x0, x1 - K.hedge_t }) {
		put(b, EL_HEDGE, x, y0, x + K.hedge_t, D / 2 - gap / 2, K.hedge_h, h);
		put(b, EL_HEDGE, x, D / 2 + gap / 2, x + K.hedge_t, y1, K.hedge_h, h);
	}
}

float ring_of(const OpenLotShape &K, float W, float D) { return std::min(W, D) * K.ring_share; }
bool has_ring(const OpenLotShape &K, float W, float D) { return std::min(W, D) >= K.ring_min; }
bool fountain_fits(const OpenLotShape &K, float W, float D) { return std::min(W, D) >= K.fountain_min; }
bool play_fits(const OpenLotShape &K, float W, float D) { return std::min(W, D) >= K.play_min; }
// Playground centre: in the back-right quarter, between the cross and the ring walk.
float play_x(const OpenLotShape &K, float W, float D) { return (K.path_w / 2 + W / 2 - ring_of(K, W, D)) * HALF; }
float play_y(const OpenLotShape &K, float W, float D) { return (D / 2 + K.path_w / 2 + D - ring_of(K, W, D)) * HALF; }

void park(Lay &L) {
	const OpenLotShape &K = L.K;
	const float W = L.W, D = L.D, rg = ring_of(K, W, D);
	L.surface(-W / 2 + K.lawn_clear, K.lawn_clear, W / 2 - K.lawn_clear, D - K.lawn_clear, L.paint("square_lawn"));
	int path = L.paint("path");
	L.surface(-W / 2 + K.lawn_clear, D / 2 - K.path_w / 2, W / 2 - K.lawn_clear, D / 2 + K.path_w / 2, path, 2);
	L.surface(-K.path_w / 2, K.lawn_clear, K.path_w / 2, D - K.lawn_clear, path, 2);
	float rx0 = -W / 2 + rg, rx1 = W / 2 - rg, ry0 = rg, ry1 = D - rg;
	if (has_ring(K, W, D)) {
		for (float y : { ry0, ry1 }) L.surface(rx0, y - K.ring_w / 2, rx1, y + K.ring_w / 2, path, 2);
		for (float x : { rx0, rx1 }) L.surface(x - K.ring_w / 2, ry0, x + K.ring_w / 2, ry1, path, 2);
	}
	int bench = L.paint("bench"), lamp = L.paint("lamp_post"), bin = L.paint("bin");
	L.benches_along_x(D / 2, -W / 2 + K.lawn_clear, -K.pool_plaza / 2, bench, lamp, bin);
	L.benches_along_x(D / 2, K.pool_plaza / 2, W / 2 - K.lawn_clear, bench, lamp, bin);
	L.benches_along_y(0, K.lawn_clear, D / 2 - K.pool_plaza / 2, bench, lamp, bin);
	L.benches_along_y(0, D / 2 + K.pool_plaza / 2, D - K.lawn_clear, bench, lamp, bin);
	if (fountain_fits(K, W, D)) L.fountain(0, D / 2, true);
	if (play_fits(K, W, D)) L.playground(play_x(K, W, D), play_y(K, W, D));
	L.hedges(K.path_w + K.ring_w);
}

void square(Lay &L) {
	const OpenLotShape &K = L.K;
	const LotPartShape &P = L.P;
	const float W = L.W, D = L.D;
	L.surface(-W / 2 + K.lawn_clear, K.lawn_clear, W / 2 - K.lawn_clear, D - K.lawn_clear, L.paint("square_paving"));
	int lawn = L.paint("square_lawn");
	for (float s : { -1.f, 1.f }) L.surface(s > 0 ? W / 2 - K.lawn_clear - K.lot_edge : -W / 2 + K.lawn_clear, K.lawn_clear, s > 0 ? W / 2 - K.lawn_clear : -W / 2 + K.lawn_clear + K.lot_edge, D - K.lawn_clear, lawn, 2);
	if (fountain_fits(K, W, D)) L.fountain(0, D / 2, false);
	int bench = L.paint("bench"), lamp = L.paint("lamp_post");
	float rb = K.basin / 2 + K.path_w;
	for (float s : { -1.f, 1.f }) {
		put_at(L.b, EL_BENCH, s * rb, D / 2, K.bench_d, K.bench_w, P.bench_h, bench);
		put_at(L.b, EL_BENCH, 0, D / 2 + s * rb, K.bench_w, K.bench_d, P.bench_h, bench);
		for (float t : { -1.f, 1.f }) put_at(L.b, EL_LAMP, s * rb, D / 2 + t * rb, K.lamp, K.lamp, K.lamp_h, lamp);
	}
	if (std::min(W, D) < K.band_min) return;
	float bx = 0, by = D - K.lawn_clear - K.lot_edge - K.band; // bandstand (coreto) at the back
	int col = L.paint("column"), roof = L.paint("edicula_roof");
	put_at(L.b, EL_SLAB, bx, by, K.band, K.band, K.band_deck, L.paint("pool_coping"));
	for (float a : { -1.f, 1.f })
		for (float c : { -1.f, 1.f }) put_at(L.b, EL_PILLAR, bx + a * (K.band / 2 - K.band_col), by + c * (K.band / 2 - K.band_col), K.band_col, K.band_col, K.band_h + K.band_deck, col);
	for (int k = 0; k < 3; k++) {
		float s = K.band + K.band_col * 2 - float(k) * K.band / 3;
		put_at(L.b, EL_CANOPY, bx, by, s, s, K.band_roof, roof, K.band_deck + K.band_h + K.band_roof * float(k));
	}
}

void court(Lay &L) {
	const OpenLotShape &K = L.K;
	const LotPartShape &P = L.P;
	const float W = L.W, D = L.D;
	L.surface(-W / 2 + K.lawn_clear, K.lawn_clear, W / 2 - K.lawn_clear, D - K.lawn_clear, L.paint("square_lawn"));
	float cw = std::min(P.court_w, W - 2 * K.court_run), cd = std::min(P.court_d, D - 2 * K.court_run), cy = D / 2;
	if (cw <= 0 || cd <= 0) return;
	L.surface(-cw / 2 - K.court_run / 2, cy - cd / 2 - K.court_run / 2, cw / 2 + K.court_run / 2, cy + cd / 2 + K.court_run / 2, L.paint("court_run"), 2);
	L.surface(-cw / 2, cy - cd / 2, cw / 2, cy + cd / 2, L.paint("court"), 3);
	int line = L.paint("court_line"), fence = L.paint("fence"), white = L.paint("sign_white");
	put_at(L.b, EL_SURFACE, 0, cy, cw, P.court_line, P.cover_h * K.lift_line, line);
	put_at(L.b, EL_SURFACE, 0, cy, K.circle, K.circle, P.cover_h * K.lift_line, line);
	put_at(L.b, EL_SURFACE, 0, cy, K.circle - P.court_line * 2, K.circle - P.court_line * 2, P.cover_h * K.lift_inner, L.paint("court"));
	float fx = cw / 2 + K.court_run / 2, fy = cd / 2 + K.court_run / 2;
	for (float s : { -1.f, 1.f }) {
		put_at(L.b, EL_SURFACE, s * (cw - P.court_line) / 2, cy, P.court_line, cd, P.cover_h * K.lift_line, line);
		put_at(L.b, EL_SURFACE, 0, cy + s * (cd - P.court_line) / 2, cw, P.court_line, P.cover_h * K.lift_line, line);
		put(L.b, EL_FENCE, -fx, cy + s * fy, fx, cy + s * fy + P.court_line, K.fence_h, fence);
		put(L.b, EL_FENCE, s * fx, cy - fy, s * fx + P.court_line, cy + fy, K.fence_h, fence);
		float gy = cy + s * (cd / 2 - K.goal_d / 2);
		for (float t : { -1.f, 1.f }) put_at(L.b, EL_PILLAR, t * K.goal_w / 2, gy, K.post, K.post, K.goal_h, white);
		put_at(L.b, EL_CANOPY, 0, gy, K.goal_w + K.post, K.post, K.post, white, K.goal_h);
	}
}
} // namespace

void open_lot_body(const Data &data, Body &b, int fn, float W, float D) {
	b = Body{};
	b.plan.W = W;
	b.plan.D = D;
	b.wall = b.add_paint(data.color("square_paving"));
	b.trim = b.roof = b.wall;
	Lay L{ data, b, W, D };
	if (fn == FN_PARK) park(L);
	else if (fn == FN_SPORTS_COURT) court(L);
	else square(L);
}

bool open_lot_tree_ok(const Data &data, int fn, float W, float D, float x, float y) {
	const OpenLotShape &K = data.shapes.open;
	const LotPartShape &P = data.shapes.parts;
	if (fn == FN_SQUARE) return std::fabs(x) > W / 2 - K.lawn_clear - K.lot_edge && std::fabs(x) < W / 2 - K.lawn_clear;
	if (fn == FN_SPORTS_COURT)
		return std::fabs(x) > std::min(P.court_w, W) / 2 + K.court_run + K.tree_clear || std::fabs(y - D / 2) > std::min(P.court_d, D) / 2 + K.court_run + K.tree_clear;
	float rg = ring_of(K, W, D), c = K.tree_clear;
	if (std::fabs(y - D / 2) < K.path_w / 2 + c || std::fabs(x) < K.path_w / 2 + c) return false;
	if (x < -W / 2 + K.lawn_clear + K.hedge_t + c || x > W / 2 - K.lawn_clear - K.hedge_t - c || y < K.lawn_clear + K.hedge_t + c || y > D - K.lawn_clear - K.hedge_t - c) return false;
	bool on_ring_x = std::fabs(std::fabs(x) - (W / 2 - rg)) < K.ring_w / 2 + c, on_ring_y = std::fabs(y - rg) < K.ring_w / 2 + c || std::fabs(y - (D - rg)) < K.ring_w / 2 + c;
	if (has_ring(K, W, D) && (on_ring_x || on_ring_y)) return false;
	if (play_fits(K, W, D) && std::fabs(x - play_x(K, W, D)) < K.play / 2 + c && std::fabs(y - play_y(K, W, D)) < K.play / 2 + c) return false;
	return true;
}

} // namespace city::b40
