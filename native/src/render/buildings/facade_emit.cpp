// The walls of a placed body: every closed volume, side, storey and
// bay; covered stretches cut out; reliefs pushed with their cheeks and caps; pilasters; storey bands; the plinth and
// the entrance steps. Openings and parts per bay are facade_bay.cpp's.
#include "render/buildings/facade_bay.h"
#include "render/buildings/interior_emit.h"
#include "sim/buildings/b40/gen.h"

#include <cmath>

using namespace godot;

namespace city::b40 {

namespace {
constexpr float FACE_CLEAR_IN = 0.01f, OUT_TEST = 0.03f, HALF = 0.5f, PILASTER_K = 1.08f, PIER_MAX_SHARE = 0.3f;
constexpr float PIER_FROM = 0.35f, PIER_TO = 1.35f, GROUND_LIFT = 0.012f, BAND_OUT = 0.07f, BAND_H = 0.22f;
constexpr float BASE_OUT = 0.12f, BASE_H = 0.32f, SLAB_OUT = 0.03f, SLAB_H = 0.24f;
constexpr float SLAB_SHADE = 0.86f, PLINTH_OUT = 0.12f, PLINTH_BURY = 0.8f, STEP_MIN = 0.255f;
constexpr float FACADE_RUN_MIN = 0.3f, STEP_FRONT = 1.5f, STEP_PAD = 0.5f, STEP_FOOT = 0.4f, DOOR_W = 1.3f;
constexpr int MAX_STEPS = 40;
constexpr uint32_t HV = 0x27d4eb2d, HL = 0x165667b1, HS = 0x3c6ef372, HI = 0x85ebca6b, HX = 0x2c1b3c6d, HY = 0x297a2d39;
constexpr int HASH_SH1 = 15, HASH_SH2 = 13, LEVEL_OFF = 7, SIDE_OFF = 3, INDEX_OFF = 11, BAY_Q = 7;


void relief_faces(Shell &s, const BayCtx &c, float push, float left_push, float right_push) {
	auto cheek = [&](float a, float p0, float p1, bool facing_right) {
		if (std::abs(p1 - p0) < EPS) return;
		Basis g{ c.f.at(a, 0, 0), c.f.e2, c.f.e1, facing_right ? c.f.e0 : -c.f.e0 };
		s.rect(g, std::min(p0, p1), std::max(p0, p1), 0, c.H, 0, c.wall, c.wall_fin, false);
	};
	// c.f already sits at the pushed plane: cheeks run back to the neighbour's plane
	if (left_push != push) cheek(0, 0, left_push - push, push < left_push);
	if (right_push != push) cheek(c.W, 0, right_push - push, push > right_push);
}

void relief_caps(Shell &s, const BayCtx &c, float push, bool bottom, bool top) {
	if (push > 0) {
		Basis up{ c.f.o, c.f.e0, -c.f.e2, c.f.e1 };
		if (top) s.rect(up, 0, c.W, 0, push, c.H, c.mould, c.mould_fin);
		if (bottom) s.rect(up, 0, c.W, 0, push, 0, c.mould, c.mould_fin, true);
	} else if (push < 0) {
		Basis up{ c.f.o, c.f.e0, c.f.e2, c.f.e1 };
		if (top) s.rect(up, 0, c.W, 0, -push, c.H, c.wall * HALF, c.wall_fin, true);
		if (bottom) s.rect(up, 0, c.W, 0, -push, GROUND_LIFT, c.wall, c.wall_fin);
	}
}

void pilaster(Shell &s, const BayCtx &c, const FaceGeometry &g, int index, float lift) {
	if (g.pier_every <= 0 || g.pier_d <= 0 || index % g.pier_every != 0) return;
	float pw = std::min(g.pier_w, PIER_MAX_SHARE * c.W);
	s.box(c.f, PIER_FROM * pw, PIER_TO * pw, lift, c.H, 0, g.pier_d, c.wall * PILASTER_K, c.wall_fin, SKIP_E2_NEG | SKIP_E1_NEG | SKIP_E1_POS);
}

void steps(Shell &s, const Placed &pl, const BayCtx &c) {
	float am = c.W / 2;
	Vector3 front = c.f.at(am, 0, STEP_FRONT);
	float rise = front.y - pl.ground(front);
	if (rise <= STEP_MIN) return;
	int n = std::min(MAX_STEPS, int(std::ceil(rise / RISER_M)));
	float w = std::min(c.W - 1, DOOR_W) + STEP_PAD;
	Lin stone = paint_lin(pl.data->color("plinth"));
	for (int j = 1; j <= n; j++) {
		float top = -rise * float(j - 1) / float(n) + GROUND_LIFT;
		s.box(c.f, am - w / 2, am + w / 2, -rise - STEP_FOOT, top, 0, float(j + 1) * FACADE_RUN_MIN, stone, FIN_STONE, SKIP_E2_NEG | SKIP_E1_NEG);
	}
}

struct SideEmit {
	Shell &shell, &parts;
	const Placed &pl;
	const Volume &v;
	int vol;
	void storey(const Side &sd, int k, float z);
	void relief(const Side &sd, int k, float z);
	void pushed_faces(const BayCtx &c, const Side &sd, int k, int i, int n, float push);
	void lines(Shell &s, const Side &sd);
	void plinth(const Side &sd);
	template <typename F>
	void each_bay(const Side &sd, int k, float z, F &&at);
};

// Every bay of a storey not wholly covered, its context filled in.
template <typename F>
void SideEmit::each_bay(const Side &sd, int k, float z, F &&at) {
	const Body &b = *pl.b;
	int level = v.base + k, n = bays_of(v, sd.side);
	float W = sd.length / float(n), H = b.level_h(level);
	BayCtx c;
	c.W = W; c.H = H; c.level = level; c.use = b.use; c.geo = &v.faces[sd.side];
	c.trim = trim_lin(pl); c.trim_fin = b.paints[size_t(b.trim)].finish;
	Paint mould = moulding_of(pl);
	c.mould = paint_lin(mould); c.mould_fin = mould.finish;
	const Paint &wp = [&]() -> const Paint & {
		auto it = v.storey_paint.find(side_key(k, sd.side));
		return it != v.storey_paint.end() ? b.paints[size_t(it->second)] : b.wall_of(v);
	}();
	c.wall = wall_lin(pl, v, k, sd.side); c.wall_fin = wp.finish;
	shell.grime_floor = pl.lot.o.y + b.level_z(v.base);
	for (int i = 0; i < n; i++) {
		float a0 = W * float(i), am = a0 + W / 2;
		Point2 dir{ (sd.p1.x - sd.p0.x) / sd.length, (sd.p1.y - sd.p0.y) / sd.length };
		auto plan_at = [&](float a) { return Point2{ sd.p0.x + dir.x * a + sd.n.x * OUT_TEST, sd.p0.y + dir.y * a + sd.n.y * OUT_TEST }; };
		bool cl = covered(b, v, level, plan_at(a0 + FACE_CLEAR_IN)), cm = covered(b, v, level, plan_at(am)), cr = covered(b, v, level, plan_at(a0 + W - FACE_CLEAR_IN));
		if (cl && cm && cr) continue;
		float push = relief_push(v, sd.side, i, k);
		c.f = Basis{ sd.f.at(a0, z, push), sd.f.e0, sd.f.e1, sd.f.e2 };
		c.wall_before = a0;
		c.wall_after = sd.length - a0 - W;
		c.comp = bay_at(v, k, sd.side, i);
		c.plain = cl || cm || cr;
		c.hash = bay_hash(vol, level, sd.side, i, plan_at(am).x, plan_at(am).y);
		c.vol = vol;
		c.plan_at = Point2{ sd.p0.x + dir.x * a0, sd.p0.y + dir.y * a0 };
		c.plan_along = dir;
		c.plan_out = sd.n;
		c.open_left = i > 0 && bay_at(v, k, sd.side, i - 1) == BAY_GOURMET;
		c.open_right = i + 1 < n && bay_at(v, k, sd.side, i + 1) == BAY_GOURMET;
		at(c, i, n, push);
	}
}

void SideEmit::storey(const Side &sd, int k, float z) {
	const Body &b = *pl.b;
	const int level = v.base + k;
	WallRun run{ shell };
	each_bay(sd, k, z, [&](const BayCtx &c, int i, int n, float push) {
		if (b.cut_level >= 0 && level == b.cut_level && cut_bay(shell, pl, c, sd.n)) {
			run.close();
			return; // the floor seen: this wall came down
		}
		emit_bay(shell, parts, pl, c, run, i, push);
		pushed_faces(c, sd, k, i, n, push);
		pilaster(shell, c, v.faces[sd.side], i, level == 0 ? GROUND_LIFT : 0);
		if (level == 0 && (c.comp == BAY_DOOR || c.comp == BAY_DOUBLE_DOOR) && v.base == 0) steps(shell, pl, c);
	});
	run.close();
}

// The same bays for the mid level: what stands out of the wall, a pilaster once for the whole height.
void SideEmit::relief(const Side &sd, int k, float z) {
	const Body &b = *pl.b;
	each_bay(sd, k, z, [&](const BayCtx &c, int i, int n, float push) {
		if (c.plain) return;
		emit_bay_relief(shell, parts, pl, c, opening_of(*pl.data, c));
		pushed_faces(c, sd, k, i, n, push);
		if (k != 0) return;
		BayCtx whole = c;
		whole.H = b.level_z(v.top()) - b.level_z(v.base);
		pilaster(parts, whole, v.faces[sd.side], i, GROUND_LIFT);
	});
}

// The cheeks and caps of a bay pushed out of its neighbours' plane.
void SideEmit::pushed_faces(const BayCtx &c, const Side &sd, int k, int i, int n, float push) {
	float lp = i > 0 ? relief_push(v, sd.side, i - 1, k) : 0, rp = i + 1 < n ? relief_push(v, sd.side, i + 1, k) : 0;
	relief_faces(shell, c, push, lp, rp);
	if (push != 0) relief_caps(shell, c, push, k == 0 || relief_push(v, sd.side, i, k - 1) != push, k + 1 >= v.storeys || relief_push(v, sd.side, i, k + 1) != push);
}

void SideEmit::lines(Shell &s, const Side &sd) {
	const Body &b = *pl.b;
	Paint mp = moulding_of(pl);
	Lin m = paint_lin(mp);
	Point2 mid{ (sd.p0.x + sd.p1.x) / 2 + sd.n.x * OUT_TEST, (sd.p0.y + sd.p1.y) / 2 + sd.n.y * OUT_TEST };
	for (int k = 1; k < v.storeys; k++) {
		int level = v.base + k;
		if (v.lines == LINES_BASE && level != 1) continue;
		if (covered(b, v, level, mid) && covered(b, v, level - 1, mid)) continue;
		float z = b.level_z(level) - b.level_z(v.base);
		if (v.lines == LINES_EVERY) band(s, sd, z, BAND_OUT, BAND_H, m, mp.finish, false);
		else if (v.lines == LINES_BASE) band(s, sd, z, BASE_OUT, BASE_H, m, mp.finish, false);
		else if (v.lines == LINES_SLAB) band(s, sd, z, SLAB_OUT, SLAB_H, wall_lin(pl, v, k, 0) * SLAB_SHADE, b.wall_of(v).finish, true);
	}
}

void SideEmit::plinth(const Side &sd) {
	Point2 mid{ (sd.p0.x + sd.p1.x) / 2 + sd.n.x * OUT_TEST, (sd.p0.y + sd.p1.y) / 2 + sd.n.y * OUT_TEST };
	if (covered(*pl.b, v, 0, mid)) return;
	Lin stone = paint_lin(pl.data->color("plinth"));
	shell.box(sd.f, -PLINTH_OUT, sd.length + PLINTH_OUT, pl.ground_lo - PLINTH_BURY, GROUND_LIFT, 0, PLINTH_OUT, stone, FIN_STONE, SKIP_E2_NEG);
}
} // namespace

float relief_push(const Volume &v, int side, int bay, int k) { return relief_depth(v, side, bay, k); }

uint32_t bay_hash(int vol, int level, int side, int index, float bx, float by) {
	uint32_t h = (uint32_t(vol + 1) * HV) ^ (uint32_t(level + LEVEL_OFF) * HL) ^ (uint32_t(side + SIDE_OFF) * HS) ^ (uint32_t(index + INDEX_OFF) * HI);
	h ^= (uint32_t(int32_t(std::round(bx * BAY_Q))) * HX) ^ (uint32_t(int32_t(std::round(by * BAY_Q))) * HY);
	h = (h ^ (h >> HASH_SH1)) * HI;
	return h ^ (h >> HASH_SH2);
}

// Each side of each block, in the block's frame, with the storeys walked from its foot.
template <typename F>
static void each_side(Shell &shell, Shell &parts, const Placed &whole, F &&side) {
	const Body &b = *whole.b;
	for (size_t i = 0; i < b.volumes.size(); i++) {
		const Volume &v = b.volumes[i];
		Placed raised;
		const Placed &pl = block_frame(whole, v, raised);
		SideEmit se{ shell, parts, pl, v, int(i) };
		for (const Side &sd : sides_of(pl, v, b.level_z(v.base))) side(se, sd);
	}
}

void emit_relief(Shell &shell, Shell &parts, const Placed &whole) {
	const Body &b = *whole.b;
	each_side(shell, parts, whole, [&](SideEmit &se, const Side &sd) {
		float z = 0;
		for (int k = 0; k < se.v.storeys; k++) {
			se.relief(sd, k, z);
			z += b.level_h(se.v.base + k);
		}
		se.lines(parts, sd);
	});
}

void for_each_bay(const Placed &pl, const std::function<void(const BayCtx &, const Opening &)> &at) {
	const Body &b = *pl.b;
	for (const Volume &v : b.volumes)
		for (const Side &sd : sides_of(pl, v, b.level_z(v.base)))
			for (int k = 0; k < v.storeys; k++) {
				const int n = bays_of(v, sd.side);
				BayCtx c;
				c.W = sd.length / float(n);
				c.H = b.level_h(v.base + k);
				c.level = v.base + k;
				c.geo = &v.faces[sd.side];
				for (int i = 0; i < n; i++) {
					c.comp = bay_at(v, k, sd.side, i);
					at(c, opening_of(*pl.data, c));
				}
			}
}

void emit_facades(Shell &shell, Shell &parts, const Placed &whole) {
	const Body &b = *whole.b;
	each_side(shell, parts, whole, [&](SideEmit &se, const Side &sd) {
		float z = 0;
		for (int k = 0; k < se.v.storeys; k++) {
			se.storey(sd, k, z);
			z += b.level_h(se.v.base + k);
		}
		se.lines(shell, sd);
		if (se.v.base == 0) se.plinth(sd);
	});
}

} // namespace city::b40
