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
constexpr float LAWN_CLEAR = 0.5f, SURFACE_H = 0.06f, PATH_W = 3.f, RING_SHARE = 0.2f, RING_W = 2.2f, TREE_CLEAR = 1.6f, RING_MIN = 34.f;
constexpr float BASIN = 5.f, POOL_PLAZA = 11.f, RIM_H = 0.5f, RIM_T = 0.35f, SPOUT = 0.7f, SPOUT_H = 1.8f, BOWL = 1.6f, BOWL_H = 0.25f;
constexpr float FOUNTAIN_MIN = 22.f, PLAY = 10.f, PLAY_MIN = 46.f, HEDGE_H = 1.1f, HEDGE_T = 0.8f;
constexpr float BENCH_W = 1.8f, BENCH_D = 0.55f, BENCH_H = 0.45f, BENCH_EVERY = 9.f, LAMP = 0.2f, LAMP_H = 4.2f, LAMP_EVERY = 14.f;
constexpr float BIN = 0.5f, BIN_H = 0.9f, POST = 0.12f, SWING_H = 2.4f, SWING_W = 3.6f, FRAME = 2.f, FRAME_H = 1.6f, SEAT = 0.5f;
constexpr float SEAT_Z = 0.5f, SEESAW_L = 3.6f, SEESAW_W = 0.3f, SEESAW_Z = 0.45f, CHAIN = 0.03f;
constexpr float LOT_EDGE = 5.f, BAND_MIN = 40.f, BAND = 7.f, BAND_COL = 0.3f, BAND_H = 3.f, BAND_ROOF = 0.4f, BAND_DECK = 0.6f;
constexpr float COURT_W = 15.f, COURT_D = 28.f, COURT_RUN = 2.f, LINE = 0.12f, FENCE_H = 3.f, GOAL_W = 3.f, GOAL_H = 2.f;
constexpr float GOAL_D = 0.8f, CIRCLE = 3.f, HALF = 0.5f, LIFT_LINE = 4.f, LIFT_INNER = 5.f; // layer lifts (x SURFACE_H) of painted lines

struct Lay {
	const Data &data;
	Body &b;
	float W, D;
	int paint(const char *n) { return b.add_paint(data.color(n)); }
	void surface(float x0, float y0, float x1, float y1, int p, float lift = 1) { put(b, EL_SURFACE, x0, y0, x1, y1, SURFACE_H * lift, p); }
	void fountain(float cx, float cy, bool plaza);
	void benches_along_x(float y, float x0, float x1, int bench, int lamp, int bin);
	void benches_along_y(float x, float y0, float y1, int bench, int lamp, int bin);
	void playground(float cx, float cy);
	void hedges(float gap);
};

void Lay::fountain(float cx, float cy, bool plaza) {
	int stone = paint("pool_coping");
	if (plaza) surface(cx - POOL_PLAZA / 2, cy - POOL_PLAZA / 2, cx + POOL_PLAZA / 2, cy + POOL_PLAZA / 2, paint("square_paving"), 2);
	surface(cx - BASIN / 2, cy - BASIN / 2, cx + BASIN / 2, cy + BASIN / 2, paint("fountain_water"), RIM_H / SURFACE_H * HALF);
	for (float s : { -1.f, 1.f }) {
		put(b, EL_WALL, cx - BASIN / 2, cy + s * BASIN / 2 - RIM_T / 2, cx + BASIN / 2, cy + s * BASIN / 2 + RIM_T / 2, RIM_H, stone);
		put(b, EL_WALL, cx + s * BASIN / 2 - RIM_T / 2, cy - BASIN / 2, cx + s * BASIN / 2 + RIM_T / 2, cy + BASIN / 2, RIM_H, stone);
	}
	put_at(b, EL_PILLAR, cx, cy, SPOUT, SPOUT, SPOUT_H, stone);
	put_at(b, EL_CANOPY, cx, cy, BOWL, BOWL, BOWL_H, stone, SPOUT_H);
}

void Lay::benches_along_x(float y, float x0, float x1, int bench, int lamp, int bin) {
	for (float x = x0 + BENCH_EVERY / 2; x < x1 - BENCH_W; x += BENCH_EVERY) {
		put_at(b, EL_BENCH, x, y - PATH_W / 2 - BENCH_D, BENCH_W, BENCH_D, BENCH_H, bench);
		if (std::fmod(x - x0, LAMP_EVERY) < BENCH_EVERY) put_at(b, EL_LAMP, x + BENCH_W, y + PATH_W / 2 + LAMP, LAMP, LAMP, LAMP_H, lamp);
		else put_at(b, EL_BIN, x + BENCH_W, y - PATH_W / 2 - BIN, BIN, BIN, BIN_H, bin);
	}
}

void Lay::benches_along_y(float x, float y0, float y1, int bench, int lamp, int bin) {
	for (float y = y0 + BENCH_EVERY / 2; y < y1 - BENCH_W; y += BENCH_EVERY) {
		put_at(b, EL_BENCH, x - PATH_W / 2 - BENCH_D, y, BENCH_D, BENCH_W, BENCH_H, bench);
		if (std::fmod(y - y0, LAMP_EVERY) < BENCH_EVERY) put_at(b, EL_LAMP, x + PATH_W / 2 + LAMP, y + BENCH_W, LAMP, LAMP, LAMP_H, lamp);
		else put_at(b, EL_BIN, x - PATH_W / 2 - BIN, y + BENCH_W, BIN, BIN, BIN_H, bin);
	}
}

void Lay::playground(float cx, float cy) {
	surface(cx - PLAY / 2, cy - PLAY / 2, cx + PLAY / 2, cy + PLAY / 2, paint("sand"), 2);
	int red = paint("play_red"), yellow = paint("play_yellow"), blue = paint("play_blue"), dark = paint("plant_dark");
	float sx = cx - PLAY / 4, sy = cy - PLAY / 4; // swings
	for (float s : { -1.f, 1.f }) put_at(b, EL_PILLAR, sx + s * SWING_W / 2, sy, POST * 2, POST * 2, SWING_H, red);
	put_at(b, EL_CANOPY, sx, sy, SWING_W + POST * 2, POST * 2, POST * 2, red, SWING_H);
	for (float s : { -1.f, 1.f }) {
		put_at(b, EL_CANOPY, sx + s * SWING_W / 4, sy, CHAIN, CHAIN, SWING_H - SEAT_Z, dark, SEAT_Z);
		put_at(b, EL_CANOPY, sx + s * SWING_W / 4, sy, SEAT, SEAT * HALF, POST, dark, SEAT_Z);
	}
	float fx = cx + PLAY / 4, fy = cy + PLAY / 4; // climbing frame
	for (float a : { -1.f, 1.f })
		for (float c : { -1.f, 1.f }) put_at(b, EL_PILLAR, fx + a * FRAME / 2, fy + c * FRAME / 2, POST, POST, FRAME_H * 2, blue);
	put_at(b, EL_CANOPY, fx, fy, FRAME + POST, FRAME + POST, POST * 2, yellow, FRAME_H);
	put_at(b, EL_CANOPY, fx, fy, FRAME + POST, FRAME + POST, POST * 2, red, FRAME_H * 2);
	put_at(b, EL_PILLAR, cx + PLAY / 4, cy - PLAY / 4, POST * 2, POST * 2, SEESAW_Z, dark); // see-saw
	put_at(b, EL_CANOPY, cx + PLAY / 4, cy - PLAY / 4, SEESAW_L, SEESAW_W, POST, yellow, SEESAW_Z);
}

void Lay::hedges(float gap) {
	int h = paint("hedge");
	float x0 = -W / 2 + LAWN_CLEAR, x1 = W / 2 - LAWN_CLEAR, y0 = LAWN_CLEAR, y1 = D - LAWN_CLEAR;
	for (float y : { y0, y1 }) {
		put(b, EL_HEDGE, x0, y, -gap / 2, y + HEDGE_T, HEDGE_H, h);
		put(b, EL_HEDGE, gap / 2, y, x1, y + HEDGE_T, HEDGE_H, h);
	}
	for (float x : { x0, x1 - HEDGE_T }) {
		put(b, EL_HEDGE, x, y0, x + HEDGE_T, D / 2 - gap / 2, HEDGE_H, h);
		put(b, EL_HEDGE, x, D / 2 + gap / 2, x + HEDGE_T, y1, HEDGE_H, h);
	}
}

float ring_of(float W, float D) { return std::min(W, D) * RING_SHARE; }
bool has_ring(float W, float D) { return std::min(W, D) >= RING_MIN; }
bool fountain_fits(float W, float D) { return std::min(W, D) >= FOUNTAIN_MIN; }
bool play_fits(float W, float D) { return std::min(W, D) >= PLAY_MIN; }
// Playground centre: in the back-right quarter, between the cross and the ring walk.
float play_x(float W, float D) { return (PATH_W / 2 + W / 2 - ring_of(W, D)) * HALF; }
float play_y(float W, float D) { return (D / 2 + PATH_W / 2 + D - ring_of(W, D)) * HALF; }

void park(Lay &L) {
	const float W = L.W, D = L.D, rg = ring_of(W, D);
	L.surface(-W / 2 + LAWN_CLEAR, LAWN_CLEAR, W / 2 - LAWN_CLEAR, D - LAWN_CLEAR, L.paint("square_lawn"));
	int path = L.paint("path");
	L.surface(-W / 2 + LAWN_CLEAR, D / 2 - PATH_W / 2, W / 2 - LAWN_CLEAR, D / 2 + PATH_W / 2, path, 2);
	L.surface(-PATH_W / 2, LAWN_CLEAR, PATH_W / 2, D - LAWN_CLEAR, path, 2);
	float rx0 = -W / 2 + rg, rx1 = W / 2 - rg, ry0 = rg, ry1 = D - rg;
	if (has_ring(W, D)) {
		for (float y : { ry0, ry1 }) L.surface(rx0, y - RING_W / 2, rx1, y + RING_W / 2, path, 2);
		for (float x : { rx0, rx1 }) L.surface(x - RING_W / 2, ry0, x + RING_W / 2, ry1, path, 2);
	}
	int bench = L.paint("bench"), lamp = L.paint("lamp_post"), bin = L.paint("bin");
	L.benches_along_x(D / 2, -W / 2 + LAWN_CLEAR, -POOL_PLAZA / 2, bench, lamp, bin);
	L.benches_along_x(D / 2, POOL_PLAZA / 2, W / 2 - LAWN_CLEAR, bench, lamp, bin);
	L.benches_along_y(0, LAWN_CLEAR, D / 2 - POOL_PLAZA / 2, bench, lamp, bin);
	L.benches_along_y(0, D / 2 + POOL_PLAZA / 2, D - LAWN_CLEAR, bench, lamp, bin);
	if (fountain_fits(W, D)) L.fountain(0, D / 2, true);
	if (play_fits(W, D)) L.playground(play_x(W, D), play_y(W, D));
	L.hedges(PATH_W + RING_W);
}

void square(Lay &L) {
	const float W = L.W, D = L.D;
	L.surface(-W / 2 + LAWN_CLEAR, LAWN_CLEAR, W / 2 - LAWN_CLEAR, D - LAWN_CLEAR, L.paint("square_paving"));
	int lawn = L.paint("square_lawn");
	for (float s : { -1.f, 1.f }) L.surface(s > 0 ? W / 2 - LAWN_CLEAR - LOT_EDGE : -W / 2 + LAWN_CLEAR, LAWN_CLEAR, s > 0 ? W / 2 - LAWN_CLEAR : -W / 2 + LAWN_CLEAR + LOT_EDGE, D - LAWN_CLEAR, lawn, 2);
	if (fountain_fits(W, D)) L.fountain(0, D / 2, false);
	int bench = L.paint("bench"), lamp = L.paint("lamp_post");
	float rb = BASIN / 2 + PATH_W;
	for (float s : { -1.f, 1.f }) {
		put_at(L.b, EL_BENCH, s * rb, D / 2, BENCH_D, BENCH_W, BENCH_H, bench);
		put_at(L.b, EL_BENCH, 0, D / 2 + s * rb, BENCH_W, BENCH_D, BENCH_H, bench);
		for (float t : { -1.f, 1.f }) put_at(L.b, EL_LAMP, s * rb, D / 2 + t * rb, LAMP, LAMP, LAMP_H, lamp);
	}
	if (std::min(W, D) < BAND_MIN) return;
	float bx = 0, by = D - LAWN_CLEAR - LOT_EDGE - BAND; // bandstand (coreto) at the back
	int col = L.paint("column"), roof = L.paint("edicula_roof");
	put_at(L.b, EL_SLAB, bx, by, BAND, BAND, BAND_DECK, L.paint("pool_coping"));
	for (float a : { -1.f, 1.f })
		for (float c : { -1.f, 1.f }) put_at(L.b, EL_PILLAR, bx + a * (BAND / 2 - BAND_COL), by + c * (BAND / 2 - BAND_COL), BAND_COL, BAND_COL, BAND_H + BAND_DECK, col);
	for (int k = 0; k < 3; k++) {
		float s = BAND + BAND_COL * 2 - float(k) * BAND / 3;
		put_at(L.b, EL_CANOPY, bx, by, s, s, BAND_ROOF, roof, BAND_DECK + BAND_H + BAND_ROOF * float(k));
	}
}

void court(Lay &L) {
	const float W = L.W, D = L.D;
	L.surface(-W / 2 + LAWN_CLEAR, LAWN_CLEAR, W / 2 - LAWN_CLEAR, D - LAWN_CLEAR, L.paint("square_lawn"));
	float cw = std::min(COURT_W, W - 2 * COURT_RUN), cd = std::min(COURT_D, D - 2 * COURT_RUN), cy = D / 2;
	if (cw <= 0 || cd <= 0) return;
	L.surface(-cw / 2 - COURT_RUN / 2, cy - cd / 2 - COURT_RUN / 2, cw / 2 + COURT_RUN / 2, cy + cd / 2 + COURT_RUN / 2, L.paint("court_run"), 2);
	L.surface(-cw / 2, cy - cd / 2, cw / 2, cy + cd / 2, L.paint("court"), 3);
	int line = L.paint("court_line"), fence = L.paint("fence"), white = L.paint("sign_white");
	put_at(L.b, EL_SURFACE, 0, cy, cw, LINE, SURFACE_H * LIFT_LINE, line);
	put_at(L.b, EL_SURFACE, 0, cy, CIRCLE, CIRCLE, SURFACE_H * LIFT_LINE, line);
	put_at(L.b, EL_SURFACE, 0, cy, CIRCLE - LINE * 2, CIRCLE - LINE * 2, SURFACE_H * LIFT_INNER, L.paint("court"));
	float fx = cw / 2 + COURT_RUN / 2, fy = cd / 2 + COURT_RUN / 2;
	for (float s : { -1.f, 1.f }) {
		put_at(L.b, EL_SURFACE, s * (cw - LINE) / 2, cy, LINE, cd, SURFACE_H * LIFT_LINE, line);
		put_at(L.b, EL_SURFACE, 0, cy + s * (cd - LINE) / 2, cw, LINE, SURFACE_H * LIFT_LINE, line);
		put(L.b, EL_FENCE, -fx, cy + s * fy, fx, cy + s * fy + LINE, FENCE_H, fence);
		put(L.b, EL_FENCE, s * fx, cy - fy, s * fx + LINE, cy + fy, FENCE_H, fence);
		float gy = cy + s * (cd / 2 - GOAL_D / 2);
		for (float t : { -1.f, 1.f }) put_at(L.b, EL_PILLAR, t * GOAL_W / 2, gy, POST, POST, GOAL_H, white);
		put_at(L.b, EL_CANOPY, 0, gy, GOAL_W + POST, POST, POST, white, GOAL_H);
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

bool open_lot_tree_ok(int fn, float W, float D, float x, float y) {
	if (fn == FN_SQUARE) return std::fabs(x) > W / 2 - LAWN_CLEAR - LOT_EDGE && std::fabs(x) < W / 2 - LAWN_CLEAR;
	if (fn == FN_SPORTS_COURT)
		return std::fabs(x) > std::min(COURT_W, W) / 2 + COURT_RUN + TREE_CLEAR || std::fabs(y - D / 2) > std::min(COURT_D, D) / 2 + COURT_RUN + TREE_CLEAR;
	float rg = ring_of(W, D), c = TREE_CLEAR;
	if (std::fabs(y - D / 2) < PATH_W / 2 + c || std::fabs(x) < PATH_W / 2 + c) return false;
	if (x < -W / 2 + LAWN_CLEAR + HEDGE_T + c || x > W / 2 - LAWN_CLEAR - HEDGE_T - c || y < LAWN_CLEAR + HEDGE_T + c || y > D - LAWN_CLEAR - HEDGE_T - c) return false;
	bool on_ring_x = std::fabs(std::fabs(x) - (W / 2 - rg)) < RING_W / 2 + c, on_ring_y = std::fabs(y - rg) < RING_W / 2 + c || std::fabs(y - (D - rg)) < RING_W / 2 + c;
	if (has_ring(W, D) && (on_ring_x || on_ring_y)) return false;
	if (play_fits(W, D) && std::fabs(x - play_x(W, D)) < PLAY / 2 + c && std::fabs(y - play_y(W, D)) < PLAY / 2 + c) return false;
	return true;
}

} // namespace city::b40
