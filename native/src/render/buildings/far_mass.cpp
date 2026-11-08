// The mid and far levels of a placed body: each closed volume's walls, the mid level's bay by bay (`bay_walls`), the far level's
// one quad per side, whose windows the shader paints from the opening shares of the style (packed into UV2.y), the parapet as a raised wall with its coping, roof slabs, pitched roofs, machine rooms and the
// plinth. A few dozen triangles per building; `lean` (the far level) keeps one band per side and drops the coping and
// machine rooms.
#include "render/buildings/facade_bay.h"
#include "sim/buildings/b40/gen.h"
#include "render/buildings/roof_util.h"

#include "core/random.h"

#include <vector>

using namespace godot;

namespace city::b40 {

namespace {
constexpr float ROOF_EDGE_H = 0.85f, ATTIC_H = 1.5f, COPING = 0.3f;
constexpr float PLINTH_BURY = 0.8f, TEST_OUT = 0.03f, MACHINE_H = 3.f, MODULE = 3.f, SLAB_LIFT = 0.002f;
constexpr float MASS_IN_M = 0.7f, HALF = 0.5f; // the plain side stands this far behind the bays: past the deepest opening
constexpr uint32_t CELL_OFFSETS = 64; // bays the far level's grid starts from, by building: two buildings light other rooms

FarWindows windows_of(const Placed &pl, const Volume &v, int k, int side, float W, float H, uint8_t comp) {
	BayCtx c;
	c.W = W; c.H = H; c.level = v.base + k; c.geo = &v.faces[side];
	c.comp = comp;
	Opening o = opening_of(*pl.data, c);
	FarWindows w;
	w.width = o.w / W; w.sill = o.h0 / H; w.head = o.h1 / H;
	w.flags = (o.has ? 0 : WINDOW_FLAG_BLANK) | (pl.b->use == USE_COMMERCIAL ? WINDOW_FLAG_OFFICE : 0) | (pane_dark(c.comp) << WINDOW_DARK_SHIFT);
	w.trim = trim_lin(pl);
	return w;
}

// The component the far level paints a whole side with: the middle bay's, a door giving way to the side's fill.
uint8_t side_comp(const Volume &v, int k, int side) {
	auto it = v.bays.find(bay_key(k, side, bays_of(v, side) / 2));
	return it != v.bays.end() && it->second != BAY_DOOR && it->second != BAY_DOUBLE_DOOR ? it->second : fill_at(v, k, side);
}

// The cells of a side of `n` bays in rectangles, greedy: `start(k, i)` a cell one may begin at, `join(k, i, k0, i0)` a cell that goes with the one begun
// at (k0, i0), `emit(k, i, k1, i1)` each rectangle, its ends past it.
template <typename Start, typename Join, typename Emit>
void rectangles(int storeys, int n, const Start &start, const Join &join, const Emit &emit) {
	std::vector<char> done(size_t(n) * size_t(storeys), 0);
	auto cell_of = [&](int k, int i) -> char & { return done[size_t(k) * size_t(n) + size_t(i)]; };
	for (int k = 0; k < storeys; k++)
		for (int i = 0; i < n; i++) {
			if (cell_of(k, i) || !start(k, i)) continue;
			int i1 = i + 1, k1 = k + 1;
			while (i1 < n && !cell_of(k, i1) && join(k, i1, k, i)) i1++;
			auto whole_row = [&](int row) {
				for (int j = i; j < i1; j++)
					if (cell_of(row, j) || !join(row, j, k, i)) return false;
				return true;
			};
			while (k1 < storeys && whole_row(k1)) k1++;
			for (int r = k; r < k1; r++)
				for (int j = i; j < i1; j++) cell_of(r, j) = 1;
			emit(k, i, k1, i1);
		}
}

// Whether a cell of a side stands in the open: no other block of the body at that storey in front of the middle of its bay.
struct OpenCell {
	const Body &b;
	const Volume &v;
	const Side &sd;
	float W;
	bool operator()(int k, int i) const {
		const float along = W * (float(i) + HALF) / sd.length;
		return !covered(b, v, v.base + k, Point2{ sd.p0.x + (sd.p1.x - sd.p0.x) * along + sd.n.x * TEST_OUT, sd.p0.y + (sd.p1.y - sd.p0.y) * along + sd.n.y * TEST_OUT });
	}
};

// A rectangle of a side's cells, painted by the shader: its grid from `grid`, a storey and a bay a cell, as `comp` opens.
void wall_rect(Shell &s, const Placed &pl, const Volume &v, const Side &sd, float W, int8_t fin, int k, int i, int k1, int i1, float push, const Vector2 &grid, uint8_t comp) {
	const Body &b = *pl.b;
	const float za = b.level_z(v.base + k), zb = b.level_z(v.base + k1), a0 = W * float(i), a1 = W * float(i1);
	const Vector3 p[4] = { sd.f.at(a0, za, push), sd.f.at(a1, za, push), sd.f.at(a1, zb, push), sd.f.at(a0, zb, push) };
	const float x0 = grid.x + float(i), x1 = grid.x + float(i1), y1 = grid.y + float(k1 - k);
	const Vector2 cell[4] = { Vector2(x0, grid.y), Vector2(x1, grid.y), Vector2(x1, y1), Vector2(x0, y1) };
	s.far_wall(p, sd.f.e2, cell, wall_lin(pl, v, k, sd.side), fin, pl.id, windows_of(pl, v, k, sd.side, W, b.level_h(v.base + k), comp));
}

// The bays with an opening past the geometry's reach: rectangles of one component and one push, the side's real composition at one
// quad for many bays, because a shader run over a bay of a few pixels costs its quads several times over.
template <typename Open>
void flat_bays(Shell &s, const Placed &pl, const Volume &v, const Side &sd, int n, float W, int8_t fin, const Open &open) {
	const Body &b = *pl.b;
	auto opening = [&](int k, int i) {
		BayCtx c;
		c.W = W; c.H = b.level_h(v.base + k); c.level = v.base + k; c.geo = &v.faces[sd.side];
		c.comp = bay_at(v, k, sd.side, i);
		return opening_of(*pl.data, c).has;
	};
	s.grime_floor = pl.lot.o.y + b.level_z(v.base);
	rectangles(v.storeys, n, [&](int k, int i) { return open(k, i) && opening(k, i); },
			[&](int k, int i, int k0, int i0) {
				return open(k, i) && bay_at(v, k, sd.side, i) == bay_at(v, k0, sd.side, i0) &&
						relief_push(v, sd.side, i, k) == relief_push(v, sd.side, i0, k0) && b.level_h(v.base + k) == b.level_h(v.base + k0);
			},
			[&](int k, int i, int k1, int i1) { wall_rect(s, pl, v, sd, W, fin, k, i, k1, i1, relief_push(v, sd.side, i, k), Vector2(0, 0), bay_at(v, k, sd.side, i)); });
}

// The mid level's wall of a side, bay by bay as the near level has it: each opening an instance (bay_kit.h), the blank bays in
// rectangles of one push, and behind them all the plain side, which no eye reaches and which throws the building's shadow.
void bay_walls(Shell &s, const Placed &pl, const Volume &v, const Side &sd, int n, float W, int8_t fin) {
	const Body &b = *pl.b;
	const OpenCell open{ b, v, sd, W };
	std::vector<char> done(size_t(n) * size_t(v.storeys), 0);
	auto cell_of = [&](int k, int i) -> char & { return done[size_t(k) * size_t(n) + size_t(i)]; };
	for (int k = 0; k < v.storeys; k++) {
		const float H = b.level_h(v.base + k), z = b.level_z(v.base + k);
		for (int i = 0; i < n; i++) {
			if (!open(k, i)) {
				cell_of(k, i) = 1;
				continue;
			}
			BayCtx c;
			c.W = W; c.H = H; c.level = v.base + k; c.geo = &v.faces[sd.side];
			c.comp = bay_at(v, k, sd.side, i);
			const Opening o = opening_of(*pl.data, c);
			if (!o.has) continue;
			cell_of(k, i) = 1;
			const Basis f{ sd.f.at(W * float(i), z, relief_push(v, sd.side, i, k)), sd.f.e0, sd.f.e1, sd.f.e2 };
			s.bay(f, W, H, wall_lin(pl, v, k, sd.side), fin, hash_combine(pl.id, uint32_t(k * n + i)), windows_of(pl, v, k, sd.side, W, H, c.comp), o.depth);
		}
	}
	if (s.flat) flat_bays(*s.flat, pl, v, sd, n, W, fin, open);
	auto blank = [&](int k, int i, int k0, int i0) {
		return !cell_of(k, i) && relief_push(v, sd.side, i, k) == relief_push(v, sd.side, i0, k0) && b.level_h(v.base + k) == b.level_h(v.base + k0);
	};
	rectangles(v.storeys, n, [&](int k, int i) { return !cell_of(k, i); }, blank, [&](int k, int i, int k1, int i1) {
		s.rect(Basis{ sd.f.at(W * float(i), b.level_z(v.base + k), relief_push(v, sd.side, i, k)), sd.f.e0, sd.f.e1, sd.f.e2 }, 0, W * float(i1 - i), 0,
				b.level_z(v.base + k1) - b.level_z(v.base + k), 0, wall_lin(pl, v, k, sd.side), fin);
	});
	s.rect(sd.f, 0, sd.length, b.level_z(v.base), b.level_z(v.top()), -MASS_IN_M, wall_lin(pl, v, 0, sd.side), fin);
}

// The far level's wall of a side: what of it stands in the open, in rectangles of bays and storeys that read alike. A neighbour covers a side only
// where it stands: a lower block leaves the wall above it, a wing the wall past it.
void lean_walls(Shell &s, const Placed &pl, const Volume &v, const Side &sd, int n, float W, int8_t fin) {
	const Body &b = *pl.b;
	const OpenCell open{ b, v, sd, W };
	auto alike = [&](int k, int k0) {
		const Lin a = wall_lin(pl, v, k, sd.side), c = wall_lin(pl, v, k0, sd.side);
		return side_comp(v, k, sd.side) == side_comp(v, k0, sd.side) && b.level_h(v.base + k) == b.level_h(v.base + k0) && a.r == c.r && a.g == c.g && a.b == c.b;
	};
	const float c0 = float(pl.id % CELL_OFFSETS);
	s.grime_floor = pl.lot.o.y + b.level_z(v.base);
	rectangles(v.storeys, n, open, [&](int k, int i, int k0, int) { return open(k, i) && alike(k, k0); },
			[&](int k, int i, int k1, int i1) { wall_rect(s, pl, v, sd, W, fin, k, i, k1, i1, 0, Vector2(c0, float(k)), side_comp(v, k, sd.side)); });
}

void walls(Shell &s, const Placed &pl, const Volume &v, bool roofs) {
	const Body &b = *pl.b;
	float z0 = b.level_z(v.base);
	bool parapet = roofs && roof_flat(v.roof) && v.roof != ROOF_TERRACE && !covered_by_above(b, v);
	float ph = v.crown == CROWN_ATTIC ? ATTIC_H : ROOF_EDGE_H;
	Paint mp = moulding_of(pl);
	for (const Side &sd : sides_of(pl, v, 0)) {
		Point2 mid{ (sd.p0.x + sd.p1.x) / 2 + sd.n.x * TEST_OUT, (sd.p0.y + sd.p1.y) / 2 + sd.n.y * TEST_OUT };
		int n = bays_of(v, sd.side);
		float W = sd.length / float(n);
		int8_t fin = b.wall_of(v).finish;
		s.grime_floor = pl.lot.o.y + z0;
		if (pl.lean) lean_walls(s, pl, v, sd, n, W, fin);
		else bay_walls(s, pl, v, sd, n, W, fin);
		float top = b.level_z(v.top());
		if (parapet) {
			s.rect(Basis{ sd.f.o + sd.f.e1 * top, sd.f.e0, sd.f.e1, sd.f.e2 }, 0, sd.length, 0, ph, 0, wall_lin(pl, v, v.storeys - 1, sd.side), fin);
			Basis cap{ sd.f.at(0, top + ph, 0), sd.f.e0, -sd.f.e2, sd.f.e1 };
			s.rect(cap, 0, sd.length, 0, COPING, 0, paint_lin(mp), mp.finish);
		}
		if (v.base == 0 && !covered(b, v, 0, mid))
			s.rect(sd.f, 0, sd.length, pl.ground_lo - PLINTH_BURY, 0, 0, paint_lin(pl.data->color("plinth")), FIN_STONE);
	}
}

void roof(Shell &s, const Placed &pl, const Volume &v, int vol) {
	const Body &b = *pl.b;
	float eaves = b.level_z(v.top());
	if (!roof_flat(v.roof)) { emit_pitched(s, pl, v, eaves); return; }
	Paint rp = v.roof == ROOF_TERRACE || covered_by_above(b, v) ? pl.data->color("terrace") : flat_roof_of(pl, v);
	Lin lin = paint_lin(rp);
	std::vector<Vector3> p;
	if (v.outline.empty()) p = { pl.lot.at(v.x, v.y, eaves), pl.lot.at(v.x + v.w, v.y, eaves), pl.lot.at(v.x + v.w, v.y + v.d, eaves), pl.lot.at(v.x, v.y + v.d, eaves) };
	else
		for (const Point2 &q : v.outline) p.push_back(pl.lot.at(v.x + q.x * v.w, v.y + q.y * v.d, eaves + SLAB_LIFT));
	Vector3 c = pl.lot.at(v.x + v.w / 2, v.y + v.d / 2, eaves + SLAB_LIFT);
	s.grime_floor = -FAR_AWAY;
	for (size_t i = 0; i < p.size(); i++) s.tri(c, p[i], p[(i + 1) % p.size()], pl.lot.e2, lin, rp.finish);
	for (const Core &cr : b.cores) {
		float cx = cr.x + MODULE / 2, cy = cr.y + MODULE / 2;
		if (!(cx > v.x && cx < v.x + v.w && cy > v.y && cy < v.y + v.d) || covered_by_above(b, v)) continue;
		const Paint &wp = b.wall_of(v);
		s.box(pl.lot, cr.x, cr.x + MODULE, cr.y, cr.y + MODULE, eaves, eaves + MACHINE_H, paint_lin(wp) * pl.shade, wp.finish, SKIP_E2_NEG);
	}
	if (pl.lean && v.roof != ROOF_TERRACE && v.outline.empty() && !covered_by_above(b, v)) { // its plant a metre or more across, the same as near; what is finer goes
		Shell finer;
		emit_roof_plant(s, finer, pl, v, vol, eaves);
	}
}
} // namespace

void emit_far(Shell &shell, const Placed &whole, bool roofs) {
	for (size_t i = 0; i < whole.b->volumes.size(); i++) {
		const Volume &v = whole.b->volumes[i];
		Placed raised;
		const Placed &pl = block_frame(whole, v, raised);
		walls(shell, pl, v, roofs);
		if (roofs) roof(shell, pl, v, int(i));
	}
}

} // namespace city::b40
