// The lot round the building (after jobs/b40/lot/furnish*.ts, condensed): front surface and its path to the door,
// side drives and paths, the back (yard with terrace and pool, parking with stalls, service yard with wall ACs,
// courtyard) and the boundary (fence on a low wall, hedge or wall in front, walls round the sides and back) with
// gates at the door and on drives. Elements are boxes in the lot frame, laid by lot_emit.cpp.
#include "sim/buildings/b40/gen.h"
#include "sim/buildings/b40/paint.h"

#include <algorithm>

namespace city::b40 {

namespace {
constexpr float PAVING_Y = 0.06f, PATH_W = 1.4f, POOL_W_MAX = 8.f, POOL_D = 3.5f, POOL_MIN_D = 6.f, POOL_MIN_W = 11.f;
constexpr float WATER_SHARE = 0.45f, TERRACE_D = 3.f, STALL_W = 2.5f, STALL_D = 5.f, AISLE = 6.f, LAMP_H = 5.f;
constexpr float BOUNDARY_T = 0.2f, FENCE_T = 0.12f, HEDGE_T = 0.7f, GATE_PERSON_HOUSE = 1.2f, GATE_PERSON_FLATS = 1.6f;
constexpr float GATE_PERSON_SHOP = 2.f, GATE_H = 1.4f, GATE_TALL_H = 1.8f, GATE_WIDE = 2.f, DRIVE_GATE_TRIM = 0.4f;
constexpr float SIDE_LOW_H = 1.3f, AC_W = 0.8f, AC_D = 0.35f, AC_H = 0.6f, AC_Z = 2.6f, AC_EVERY = 2.4f;
constexpr float PLANTER = 1.f, PLANTER_H = 0.6f, BENCH_W = 1.6f, BENCH_D = 0.5f, BENCH_H = 0.45f, BOLLARD = 0.2f;
constexpr float BOLLARD_H = 0.9f, BOLLARD_EVERY = 2.f, PLAZA_EVERY = 6.f, PLAZA_PLANTER = 2.f, BIN_W = 1.1f, BIN_D = 0.6f;
constexpr float BIN_H = 1.1f, SKIP_W = 2.2f, SKIP_D = 1.4f, SKIP_H = 1.3f, HALF = 0.5f, EDGE = 0.3f, LAMP_INSET = 1.f;
constexpr int HOUSE_FRONTS = 2;
constexpr float STRIPE_W = 0.1f, LINE_SHARE = 0.72f, FOOTPATH = 1.2f, POOL_ROOM = 5.f, POOL_LIFT = 0.015f, COURT_D = 5.f, COURT_W = 6.f, HOUSE_WALL_H = 2.f, WALL_H = 2.2f;
constexpr float HEDGE_H = 1.1f, FRONT_WALL_H = 1.4f, RAILING_H = 1.f, FLATS_FENCE_H = 1.6f, HOUSE_FENCE_BASE = 0.6f, FENCE_BASE = 0.45f;

struct Furnisher {
	const Data &data;
	Body &b;
	Xoshiro128 &r;
	float L = 0, R = 0, D = 0; // lot edges in the lot frame

	int paint(const char *name) { return b.add_paint(data.color(name)); }
	void add(Element k, float x0, float y0, float x1, float y1, float h, int p, float z = 0, float base = 0) {
		if (x1 - x0 < EPS || y1 - y0 < EPS) return;
		LotElement e;
		e.kind = k; e.x = (x0 + x1) / 2; e.y = (y0 + y1) / 2; e.w = x1 - x0; e.d = y1 - y0; e.h = h; e.z = z; e.paint = p; e.base = base;
		b.elements.push_back(e);
	}
	void surface(float x0, float y0, float x1, float y1, const char *name) { add(EL_SURFACE, x0, y0, x1, y1, PAVING_Y, paint(name)); }
	void box(Element k, float x, float y, float w, float d, float h, int p, float z = 0) { add(k, x - w / 2, y - d / 2, x + w / 2, y + d / 2, h, p, z); }

	void front();
	void sides();
	void back();
	void boundary();
};

void Furnisher::front() {
	const LotPlan &p = b.plan;
	float F = p.front_depth;
	if (F <= 0) return;
	const char *s = p.front == FRONT_CARPAD ? "concrete" : p.front == FRONT_FORECOURT ? "pavers" : p.front == FRONT_PLAZA ? "paving"
			: p.front == FRONT_APRON ? "asphalt" : "lawn";
	surface(L, 0, R, F, s);
	float door = b.entrance_x;
	if (p.front == FRONT_GARDEN || p.front == FRONT_CARPAD) {
		surface(door - PATH_W / 2, 0, door + PATH_W / 2, F, b.kind == KIND_HOUSE ? "house_path" : "pavers");
		int pl = paint("planter");
		for (float x = L + LAMP_INSET + HALF; x < R - LAMP_INSET; x += PLAZA_EVERY / 2)
			if (std::abs(x - door) > PATH_W) box(EL_PLANTER, x, F - HALF, PLANTER, PLANTER * HALF, BENCH_H, pl);
	} else if (p.front == FRONT_FORECOURT || p.front == FRONT_PLAZA) {
		int pl = paint("planter"), bn = paint("bench"), bo = paint("lamp_post");
		for (float x = L + PLAZA_EVERY / 2; x < R - PLAZA_EVERY / 4; x += PLAZA_EVERY) {
			if (std::abs(x - door) < PLAZA_EVERY / 2) continue;
			bool big = p.front == FRONT_PLAZA;
			box(EL_PLANTER, x, F / 2, big ? PLAZA_PLANTER : PLANTER, big ? PLAZA_PLANTER : PLANTER, PLANTER_H, pl);
			if (big || r.f() < HALF) box(EL_BENCH, x, F / 2 + (big ? PLAZA_PLANTER : PLANTER), BENCH_W, BENCH_D, BENCH_H, bn);
		}
		for (float x = L + BOLLARD_EVERY / 2; x < R; x += BOLLARD_EVERY)
			if (std::abs(x - door) > PATH_W) box(EL_BOLLARD, x, EDGE, BOLLARD, BOLLARD, BOLLARD_H, bo);
	}
}

void Furnisher::sides() {
	const LotPlan &p = b.plan;
	for (int s = 0; s < 2; s++) {
		const SidePlan &sp = s == 0 ? p.left : p.right;
		if (sp.use == SIDE_ATTACHED || sp.width <= 0) continue;
		float x0 = s == 0 ? L : R - sp.width, x1 = x0 + sp.width;
		float y1 = p.back == BACK_NONE ? D : D - p.back_depth;
		bool drive = sp.use == SIDE_DRIVE || sp.use == SIDE_DRIVE_PATH;
		const char *name = sp.use == SIDE_GARDEN ? "lawn" : drive ? (b.kind == KIND_HOUSE ? "concrete" : "asphalt")
				: b.kind == KIND_HOUSE ? "path" : "stone_path";
		surface(x0, p.front_depth, x1, y1, name);
		if (sp.use == SIDE_DRIVE_PATH) surface(s == 0 ? x1 - FOOTPATH : x0, p.front_depth, s == 0 ? x1 : x0 + FOOTPATH, y1, "stone_path");
	}
}

void Furnisher::back() {
	const LotPlan &p = b.plan;
	if (p.back == BACK_NONE) return;
	float y0 = D - p.back_depth;
	if (p.back == BACK_YARD) {
		surface(L, y0, R, std::min(D, y0 + TERRACE_D), "tiles");
		surface(L, std::min(D, y0 + TERRACE_D), R, D, "lawn");
		float yd = D - y0 - TERRACE_D;
		if (yd >= POOL_MIN_D && R - L >= POOL_MIN_W && r.f() < WATER_SHARE) {
			float pw = std::min(POOL_W_MAX, R - L - POOL_ROOM), cx = (L + R) / 2, cy = y0 + TERRACE_D + yd / 2;
			add(EL_SURFACE, cx - pw / 2 - EDGE, cy - POOL_D / 2 - EDGE, cx + pw / 2 + EDGE, cy + POOL_D / 2 + EDGE, PAVING_Y + POOL_LIFT, paint("pool_coping"));
			add(EL_SURFACE, cx - pw / 2, cy - POOL_D / 2, cx + pw / 2, cy + POOL_D / 2, PAVING_Y + 2 * POOL_LIFT, paint("pool"));
		}
	} else if (p.back == BACK_PARKING) {
		surface(L, y0, R, D, "asphalt");
		int st = paint("stall_line");
		for (int row = 0; row < p.rows; row++) {
			float ys = row == 0 ? D - STALL_D : y0;
			for (float x = L + EDGE; x <= R - EDGE + EPS; x += STALL_W) add(EL_PARKING, x - STRIPE_W / 2, ys, x + STRIPE_W / 2, ys + STALL_D * LINE_SHARE, PAVING_Y + POOL_LIFT, st);
		}
		int lp = paint("lamp_post");
		box(EL_LAMP, L + LAMP_INSET, y0 + AISLE / 2, BOLLARD, BOLLARD, LAMP_H, lp);
		box(EL_LAMP, R - LAMP_INSET, y0 + AISLE / 2, BOLLARD, BOLLARD, LAMP_H, lp);
	} else if (p.back == BACK_SERVICE || p.back == BACK_LOADING) {
		surface(L, y0, R, D, "concrete");
		box(EL_BIN, L + BIN_W, D - BIN_D, BIN_W, BIN_D, BIN_H, paint("bin"));
		box(EL_BIN, L + BIN_W + SKIP_W, D - SKIP_D / 2 - EDGE, SKIP_W, SKIP_D, SKIP_H, paint("skip"));
		int ac = paint("wall_ac");
		for (float x = L + AC_EVERY; x < R - AC_EVERY / 2; x += AC_EVERY) box(EL_AC, x, y0 + AC_D / 2, AC_W, AC_D, AC_H, ac, AC_Z);
	} else {
		surface(L, y0, R, D, "lawn");
		float court = std::min(D - 1, y0 + COURT_D);
		surface((L + R) / 2 - COURT_W / 2, y0 + 1, (L + R) / 2 + COURT_W / 2, court, "paving");
		int bn = paint("bench");
		box(EL_BENCH, (L + R) / 2, court - BENCH_D, BENCH_W, BENCH_D, BENCH_H, bn);
	}
}

void Furnisher::boundary() {
	const LotPlan &p = b.plan;
	int wall_paint = b.add_paint(data.lot_walls[size_t(b.palette) % data.lot_walls.size()]);
	int fence = paint("fence"), hedge = paint("hedge"), gate = paint("gate");
	float side_h = b.kind == KIND_HOUSE ? HOUSE_WALL_H : WALL_H;
	float F = p.front_depth;
	// side and back walls
	add(EL_WALL, L, F, L + BOUNDARY_T, D, side_h, wall_paint);
	add(EL_WALL, R - BOUNDARY_T, F, R, D, side_h, wall_paint);
	add(EL_WALL, L, D - BOUNDARY_T, R, D, side_h, wall_paint);
	if (F <= 0) return;
	float low = std::min(side_h, SIDE_LOW_H);
	if (b.kind == KIND_HOUSE || b.kind == KIND_FLATS || b.kind == KIND_TOWER) {
		add(EL_WALL, L, 0, L + BOUNDARY_T, F, low, wall_paint);
		add(EL_WALL, R - BOUNDARY_T, 0, R, F, low, wall_paint);
	}
	int kind = b.kind == KIND_HOUSE ? r.irange(0, HOUSE_FRONTS) : -1;
	float door = b.entrance_x;
	float gw = b.kind == KIND_HOUSE ? GATE_PERSON_HOUSE : b.kind == KIND_SHOP || b.kind == KIND_OFFICE ? GATE_PERSON_SHOP : GATE_PERSON_FLATS;
	std::vector<std::pair<float, float>> gates{ { door - gw / 2, door + gw / 2 } };
	for (int s = 0; s < 2; s++) {
		const SidePlan &sp = s == 0 ? p.left : p.right;
		if (sp.use == SIDE_DRIVE || sp.use == SIDE_DRIVE_PATH) {
			float x0 = s == 0 ? L : R - sp.width;
			gates.push_back({ x0 + DRIVE_GATE_TRIM / 2, x0 + sp.width - DRIVE_GATE_TRIM / 2 });
		}
	}
	std::sort(gates.begin(), gates.end());
	float at = L;
	auto run = [&](float x0, float x1) {
		if (x1 - x0 < EDGE) return;
		if (kind == 1) add(EL_HEDGE, x0, 0, x1, HEDGE_T, HEDGE_H, hedge);
		else if (kind == 2) add(EL_WALL, x0, 0, x1, BOUNDARY_T, FRONT_WALL_H, wall_paint);
		else if (b.kind == KIND_SHOP) add(EL_RAILING, x0, 0, x1, FENCE_T, RAILING_H, fence);
		else add(EL_FENCE, x0, 0, x1, FENCE_T, b.kind == KIND_HOUSE ? HEDGE_H : b.kind == KIND_OFFICE ? FRONT_WALL_H : FLATS_FENCE_H, fence, 0,
				b.kind == KIND_HOUSE ? HOUSE_FENCE_BASE : FENCE_BASE);
	};
	for (auto &g : gates) {
		run(at, g.first);
		float w = g.second - g.first;
		add(EL_GATE, g.first, 0, g.second, FENCE_T, w > GATE_WIDE ? GATE_TALL_H : GATE_H, gate);
		at = g.second;
	}
	run(at, R);
}
} // namespace

void furnish_lot(const Data &data, Body &b, Xoshiro128 &r) {
	Furnisher f{ data, b, r };
	f.L = -b.plan.W / 2;
	f.R = b.plan.W / 2;
	f.D = b.plan.D;
	f.front();
	f.sides();
	f.back();
	f.boundary();
}

} // namespace city::b40
