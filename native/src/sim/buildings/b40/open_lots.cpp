// Open lots as lot elements (lot frame: x from the lot's middle,
// y from the street). Park: a lawn behind a clipped hedge, a cross of paths with a ring walk, a fountain on a paved
// round-point, a sand playground (swings, climbing frame, see-saw) and benches, lamps and bins along the paths.
// Square: paving with lawn borders, a fountain, benches facing it, a bandstand on big squares and lamps on the axes.
// Sports court: a fenced, painted court on a lawn with goals. Each lays its grove last (Lay::grove): its trees are parts of the
// lot like the rest, each one's element the room of its crown.
#include "sim/buildings/b40/civic.h"

#include "sim/buildings/b40/bench.h"
#include "sim/buildings/b40/cover_layers.h"
#include "sim/buildings/building_store.h"

#include <algorithm>
#include <cmath>

namespace city::b40 {

namespace {
constexpr float LAWN_CLEAR = 0.5f, PATH_W = 3.f, RING_SHARE = 0.2f, RING_W = 2.2f, RING_MIN = 34.f;
constexpr float BASIN = 5.f, POOL_PLAZA = 11.f, RIM_H = 0.5f, RIM_T = 0.35f, SPOUT = 0.7f, SPOUT_H = 1.8f, BOWL = 1.6f, BOWL_H = 0.25f;
constexpr float FOUNTAIN_MIN = 22.f, PLAY = 10.f, PLAY_MIN = 46.f, HEDGE_H = 1.1f, HEDGE_T = 0.8f;
constexpr float BENCH_W = 1.8f, BENCH_D = 0.55f, BENCH_H = 0.45f, PARK_BENCH_EVERY = 9.f, LAMP = 0.2f, LAMP_H = 4.2f, LAMP_EVERY = 14.f;
constexpr float BIN = 0.5f, BIN_H = 0.9f, POST = 0.12f, SWING_H = 2.4f, SWING_W = 3.6f, FRAME = 2.f, FRAME_H = 1.6f, SEAT = 0.5f;
constexpr float SEAT_Z = 0.5f, SEESAW_L = 3.6f, SEESAW_W = 0.3f, SEESAW_Z = 0.45f, CHAIN = 0.03f;
constexpr float LOT_EDGE = 5.f, BAND_MIN = 40.f, BAND = 7.f, BAND_COL = 0.3f, BAND_H = 3.f, BAND_ROOF = 0.4f, BAND_DECK = 0.6f;
constexpr float COURT_W = 15.f, COURT_D = 28.f, COURT_RUN = 2.f, LINE = 0.12f, FENCE_H = 3.f, GOAL_W = 3.f, GOAL_H = 2.f;
constexpr float GOAL_D = 0.8f, CIRCLE = 3.f, HALF = 0.5f;
constexpr float END_GAP = 0.3f; // between a way's end and the bench there, and that bench and the hedge
constexpr float WAY_END = LAWN_CLEAR + HEDGE_T + 2 * END_GAP + BENCH_D; // a way that leads to no street stops this short of the line
constexpr int GROVE_TRIES = 4; // draws per tree wanted: a place on a path, by a paving or with no room is passed over
constexpr uint32_t GROVE_SALT = 0x9a2cU;

// The lines of an open lot a street runs along, besides its front (which always faces one): a way leads to a line, and the hedge
// opens there, only where a street does; the others are the neighbours' walls.
struct Streets {
	bool left = false, right = false, back = false;
};

struct Lay {
	const Data &data;
	Body &b;
	float W, D;
	int paint(const char *n) { return b.add_paint(data.color(n)); }
	void surface(float x0, float y0, float x1, float y1, int p, CoverLayer layer = COVER_GROUND) { put(b, EL_SURFACE, x0, y0, x1, y1, cover_h(layer), p); }
	// The lot's own ground, from line to line: no part of an open lot is left to whatever lies under it (the hedges, the lawn
	// borders and the furniture stand LAWN_CLEAR in from the lines; the ground does not).
	// Where the lot's ground grows (a lawn), and the hard covers laid over it (a path, a paving, sand, a court): a tree's trunk stands on
	// the first, clear of the second.
	std::vector<Footprint> planted, paved;
	void ground(const char *n, bool grows) {
		surface(-W / 2, 0, W / 2, D, paint(n));
		if (grows) planted.push_back({ -W / 2, 0, W / 2, D });
	}
	void lawn(float x0, float y0, float x1, float y1, int p) {
		surface(x0, y0, x1, y1, p, COVER_WAY);
		planted.push_back({ x0, y0, x1, y1 });
	}
	void hard(float x0, float y0, float x1, float y1, int p, CoverLayer layer = COVER_WAY) {
		surface(x0, y0, x1, y1, p, layer);
		paved.push_back({ x0, y0, x1, y1 });
	}
	// A path: its cover, and its footprint kept, so what is stood beside one path never stands on another.
	std::vector<Footprint> paths;
	void path(float x0, float y0, float x1, float y1, int p) {
		hard(x0, y0, x1, y1, p);
		paths.push_back({ x0, y0, x1, y1 });
	}
	void grove(uint32_t seed);
	// A part stood beside a path (a bench, a lamp, a bin), where no path lies under it.
	void beside(Element k, float x, float y, float w, float d, float h, int p, Facing facing = FACE_STREET) {
		for (const Footprint &f : paths)
			if (x + w / 2 > f.x0 && x - w / 2 < f.x1 && y + d / 2 > f.y0 && y - d / 2 < f.y1) return;
		put_at(b, k, x, y, w, d, h, p, 0, facing);
	}
	void fountain(float cx, float cy, bool plaza);
	void benches_along_x(float y, float x0, float x1, int bench, int lamp, int bin);
	void benches_along_y(float x, float y0, float y1, int bench, int lamp, int bin);
	void playground(float cx, float cy);
	void hedges(float gap, const Streets &on);
};

void Lay::fountain(float cx, float cy, bool plaza) {
	int stone = paint("pool_coping");
	if (plaza) hard(cx - POOL_PLAZA / 2, cy - POOL_PLAZA / 2, cx + POOL_PLAZA / 2, cy + POOL_PLAZA / 2, paint("square_paving"));
	put(b, EL_SURFACE, cx - BASIN / 2, cy - BASIN / 2, cx + BASIN / 2, cy + BASIN / 2, RIM_H * HALF, paint("fountain_water")); // the water half up its rim
	paved.push_back({ cx - BASIN / 2, cy - BASIN / 2, cx + BASIN / 2, cy + BASIN / 2 });
	for (float s : { -1.f, 1.f }) {
		put(b, EL_WALL, cx - BASIN / 2, cy + s * BASIN / 2 - RIM_T / 2, cx + BASIN / 2, cy + s * BASIN / 2 + RIM_T / 2, RIM_H, stone);
		put(b, EL_WALL, cx + s * BASIN / 2 - RIM_T / 2, cy - BASIN / 2, cx + s * BASIN / 2 + RIM_T / 2, cy + BASIN / 2, RIM_H, stone);
	}
	put_at(b, EL_PILLAR, cx, cy, SPOUT, SPOUT, SPOUT_H, stone);
	put_at(b, EL_CANOPY, cx, cy, BOWL, BOWL, BOWL_H, stone, SPOUT_H);
}

void Lay::benches_along_x(float y, float x0, float x1, int bench, int lamp, int bin) {
	for (float x = x0 + PARK_BENCH_EVERY / 2; x < x1 - BENCH_W; x += PARK_BENCH_EVERY) {
		beside(EL_BENCH, x, y - PATH_W / 2 - BENCH_D, BENCH_W, BENCH_D, BENCH_H, bench, FACE_BACK); // on the path's street side, looking at it
		if (std::fmod(x - x0, LAMP_EVERY) < PARK_BENCH_EVERY) beside(EL_LAMP, x + BENCH_W, y + PATH_W / 2 + LAMP, LAMP, LAMP, LAMP_H, lamp);
		else beside(EL_BIN, x + BENCH_W, y - PATH_W / 2 - BIN, BIN, BIN, BIN_H, bin);
	}
}

void Lay::benches_along_y(float x, float y0, float y1, int bench, int lamp, int bin) {
	for (float y = y0 + PARK_BENCH_EVERY / 2; y < y1 - BENCH_W; y += PARK_BENCH_EVERY) {
		beside(EL_BENCH, x - PATH_W / 2 - BENCH_D, y, BENCH_D, BENCH_W, BENCH_H, bench, FACE_RIGHT); // on the path's left, looking at it
		if (std::fmod(y - y0, LAMP_EVERY) < PARK_BENCH_EVERY) beside(EL_LAMP, x + PATH_W / 2 + LAMP, y + BENCH_W, LAMP, LAMP, LAMP_H, lamp);
		else beside(EL_BIN, x - PATH_W / 2 - BIN, y + BENCH_W, BIN, BIN, BIN_H, bin);
	}
}

void Lay::playground(float cx, float cy) {
	hard(cx - PLAY / 2, cy - PLAY / 2, cx + PLAY / 2, cy + PLAY / 2, paint("sand"));
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

// The hedge round the lot, LAWN_CLEAR inside its lines on every side: whole along a line no street runs along, cut by the way's gap
// at the line's middle where one does (the front always).
void Lay::hedges(float gap, const Streets &on) {
	const int h = paint("hedge");
	const float x0 = -W / 2 + LAWN_CLEAR, x1 = W / 2 - LAWN_CLEAR, y0 = LAWN_CLEAR, y1 = D - LAWN_CLEAR;
	const auto run = [&](bool open, float from, float to, float mid, const auto &piece) {
		if (!open) return piece(from, to);
		piece(from, mid - gap / 2);
		piece(mid + gap / 2, to);
	};
	run(true, x0, x1, 0, [&](float a, float c) { put(b, EL_HEDGE, a, y0, c, y0 + HEDGE_T, HEDGE_H, h); });
	run(on.back, x0, x1, 0, [&](float a, float c) { put(b, EL_HEDGE, a, y1 - HEDGE_T, c, y1, HEDGE_H, h); });
	run(on.left, y0, y1, D / 2, [&](float a, float c) { put(b, EL_HEDGE, x0, a, x0 + HEDGE_T, c, HEDGE_H, h); });
	run(on.right, y0, y1, D / 2, [&](float a, float c) { put(b, EL_HEDGE, x1 - HEDGE_T, a, x1, c, HEDGE_H, h); });
}

// How far a point stands from a footprint: half the side of the largest square about it that does not reach it (0 inside it).
float clear_of(const Footprint &f, float x, float y) { return std::max(std::max({ f.x0 - x, x - f.x1, 0.f }), std::max({ f.y0 - y, y - f.y1, 0.f })); }

// The lot's grove (data.h GroveRules): its trees as parts of the lot, laid after everything else. A trunk stands on planted ground,
// clear of every hard cover; the tree's element is the room of its crown, as a lot garden's tree: the square about the trunk, up to the
// crown it asks for, that reaches no line of the lot and holds no other part nor another tree's room (a cover may lie under a crown).
// A tree its room narrows is the same tree smaller; one held too low is not planted, and the next draw is tried.
void Lay::grove(uint32_t seed) {
	const GroveRules &g = data.grove;
	const int wanted = std::min(g.most, int(W * D / g.m2_per_tree));
	std::vector<Footprint> held;
	for (const LotElement &e : b.elements)
		if (e.kind != EL_SURFACE) held.push_back({ e.x - e.w / 2, e.y - e.d / 2, e.x + e.w / 2, e.y + e.d / 2 });
	Xoshiro128 r(seed ^ GROVE_SALT);
	for (int k = 0, stood = 0; k < wanted * GROVE_TRIES && stood < wanted; k++) {
		const float x = (r.f() - HALF) * std::max(0.f, W - 2 * g.edge), y = g.edge + r.f() * std::max(0.f, D - 2 * g.edge);
		const float tall = g.height[0] + (g.height[1] - g.height[0]) * r.f(), c = g.trunk_clear;
		const auto holds = [&](const Footprint &f) { return x - c >= f.x0 && x + c <= f.x1 && y - c >= f.y0 && y + c <= f.y1; };
		const auto meets = [&](const Footprint &f) { return x + c > f.x0 && x - c < f.x1 && y + c > f.y0 && y - c < f.y1; };
		if (std::none_of(planted.begin(), planted.end(), holds) || std::any_of(paved.begin(), paved.end(), meets)) continue;
		const float asked = tall * g.crown_share;
		float half = std::min({ asked * HALF, x + W / 2, W / 2 - x, y, D - y });
		for (const Footprint &f : held) half = std::min(half, clear_of(f, x, y));
		const float room = 2 * half, height = tall * room / asked;
		if (height < g.height_least) continue;
		put_at(b, EL_TREE, x, y, room, room, height, -1);
		held.push_back({ x - half, y - half, x + half, y + half });
		stood++;
	}
}

float ring_of(float W, float D) { return std::min(W, D) * RING_SHARE; }
bool has_ring(float W, float D) { return std::min(W, D) >= RING_MIN; }
bool fountain_fits(float W, float D) { return std::min(W, D) >= FOUNTAIN_MIN; }
bool play_fits(float W, float D) { return std::min(W, D) >= PLAY_MIN; }
// Playground centre: in the back-right quarter, between the cross and the ring walk.
float play_x(float W, float D) { return (PATH_W / 2 + W / 2 - ring_of(W, D)) * HALF; }
float play_y(float W, float D) { return (D / 2 + PATH_W / 2 + D - ring_of(W, D)) * HALF; }

void park(Lay &L, const Streets &on) {
	const float W = L.W, D = L.D, rg = ring_of(W, D);
	L.ground("square_lawn", true);
	const int path = L.paint("path"), bench = L.paint("bench"), lamp = L.paint("lamp_post"), bin = L.paint("bin");
	// The cross of paths. An arm runs to its line, through the hedge's gap onto the footway, where a street runs along that line (the
	// front always); elsewhere it stops short of the hedge and ends at a bench that looks back along it.
	const float xl = on.left ? -W / 2 : -W / 2 + WAY_END, xr = on.right ? W / 2 : W / 2 - WAY_END, yb = on.back ? D : D - WAY_END;
	L.path(xl, D / 2 - PATH_W / 2, xr, D / 2 + PATH_W / 2, path);
	L.path(-PATH_W / 2, 0, PATH_W / 2, yb, path);
	const float off = END_GAP + BENCH_D / 2;
	if (!on.left) put_at(L.b, EL_BENCH, xl - off, D / 2, BENCH_D, BENCH_W, BENCH_H, bench, 0, FACE_RIGHT);
	if (!on.right) put_at(L.b, EL_BENCH, xr + off, D / 2, BENCH_D, BENCH_W, BENCH_H, bench, 0, FACE_LEFT);
	if (!on.back) put_at(L.b, EL_BENCH, 0, yb + off, BENCH_W, BENCH_D, BENCH_H, bench, 0, FACE_STREET);
	float rx0 = -W / 2 + rg, rx1 = W / 2 - rg, ry0 = rg, ry1 = D - rg;
	if (has_ring(W, D)) {
		for (float y : { ry0, ry1 }) L.path(rx0, y - RING_W / 2, rx1, y + RING_W / 2, path);
		for (float x : { rx0, rx1 }) L.path(x - RING_W / 2, ry0, x + RING_W / 2, ry1, path);
	}
	// along each arm as it is laid
	L.benches_along_x(D / 2, std::max(xl, -W / 2 + LAWN_CLEAR), -POOL_PLAZA / 2, bench, lamp, bin);
	L.benches_along_x(D / 2, POOL_PLAZA / 2, std::min(xr, W / 2 - LAWN_CLEAR), bench, lamp, bin);
	L.benches_along_y(0, LAWN_CLEAR, D / 2 - POOL_PLAZA / 2, bench, lamp, bin);
	L.benches_along_y(0, D / 2 + POOL_PLAZA / 2, std::min(yb, D - LAWN_CLEAR), bench, lamp, bin);
	if (fountain_fits(W, D)) L.fountain(0, D / 2, true);
	if (play_fits(W, D)) L.playground(play_x(W, D), play_y(W, D));
	L.hedges(PATH_W + RING_W, on);
}

void square(Lay &L) {
	const float W = L.W, D = L.D;
	L.ground("square_paving", false);
	int lawn = L.paint("square_lawn");
	for (float s : { -1.f, 1.f }) L.lawn(s > 0 ? W / 2 - LAWN_CLEAR - LOT_EDGE : -W / 2 + LAWN_CLEAR, LAWN_CLEAR, s > 0 ? W / 2 - LAWN_CLEAR : -W / 2 + LAWN_CLEAR + LOT_EDGE, D - LAWN_CLEAR, lawn);
	if (fountain_fits(W, D)) L.fountain(0, D / 2, false);
	int bench = L.paint("bench"), lamp = L.paint("lamp_post");
	float rb = BASIN / 2 + PATH_W;
	for (float s : { -1.f, 1.f }) {
		// each of the four looks at the fountain
		put_at(L.b, EL_BENCH, s * rb, D / 2, BENCH_D, BENCH_W, BENCH_H, bench, 0, s > 0 ? FACE_LEFT : FACE_RIGHT);
		put_at(L.b, EL_BENCH, 0, D / 2 + s * rb, BENCH_W, BENCH_D, BENCH_H, bench, 0, s > 0 ? FACE_STREET : FACE_BACK);
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
	L.ground("square_lawn", true);
	float cw = std::min(COURT_W, W - 2 * COURT_RUN), cd = std::min(COURT_D, D - 2 * COURT_RUN), cy = D / 2;
	if (cw <= 0 || cd <= 0) return;
	L.hard(-cw / 2 - COURT_RUN / 2, cy - cd / 2 - COURT_RUN / 2, cw / 2 + COURT_RUN / 2, cy + cd / 2 + COURT_RUN / 2, L.paint("court_run"), COVER_OWN);
	L.hard(-cw / 2, cy - cd / 2, cw / 2, cy + cd / 2, L.paint("court"), COVER_COURT);
	int line = L.paint("court_line"), fence = L.paint("fence"), white = L.paint("sign_white");
	put_at(L.b, EL_SURFACE, 0, cy, cw, LINE, cover_h(COVER_MARK), line);
	put_at(L.b, EL_SURFACE, 0, cy, CIRCLE, CIRCLE, cover_h(COVER_MARK), line);
	put_at(L.b, EL_SURFACE, 0, cy, CIRCLE - LINE * 2, CIRCLE - LINE * 2, cover_h(COVER_INSET), L.paint("court"));
	float fx = cw / 2 + COURT_RUN / 2, fy = cd / 2 + COURT_RUN / 2;
	for (float s : { -1.f, 1.f }) {
		put_at(L.b, EL_SURFACE, s * (cw - LINE) / 2, cy, LINE, cd, cover_h(COVER_MARK), line);
		put_at(L.b, EL_SURFACE, 0, cy + s * (cd - LINE) / 2, cw, LINE, cover_h(COVER_MARK), line);
		put(L.b, EL_FENCE, -fx, cy + s * fy, fx, cy + s * fy + LINE, FENCE_H, fence);
		put(L.b, EL_FENCE, s * fx, cy - fy, s * fx + LINE, cy + fy, FENCE_H, fence);
		float gy = cy + s * (cd / 2 - GOAL_D / 2);
		for (float t : { -1.f, 1.f }) put_at(L.b, EL_PILLAR, t * GOAL_W / 2, gy, POST, POST, GOAL_H, white);
		put_at(L.b, EL_CANOPY, 0, gy, GOAL_W + POST, POST, POST, white, GOAL_H);
	}
}
} // namespace

void open_lot_body(const Data &data, Body &b, int fn, float W, float D, uint32_t seed, const LotEdges &edges) {
	b = Body{};
	b.plan.W = W;
	b.plan.D = D;
	b.wall = b.add_paint(data.color("square_paving"));
	b.trim = b.roof = b.wall;
	Lay L{ data, b, W, D, {}, {}, {} };
	if (fn == FN_PARK) park(L, Streets{ edges.on_street(0), edges.on_street(1), edges.back_on_street() });
	else if (fn == FN_SPORTS_COURT) court(L);
	else square(L);
	L.grove(seed);
}

} // namespace city::b40
