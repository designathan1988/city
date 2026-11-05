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
constexpr float PARAPET_H = 0.85f, ATTIC_H = 1.5f, COPING = 0.3f;
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
	std::vector<char> done(size_t(n) * size_t(v.storeys), 0);
	auto cell_of = [&](int k, int i) -> char & { return done[size_t(k) * size_t(n) + size_t(i)]; };
	auto same = [&](int k, int i, int k0, int i0) {
		return !cell_of(k, i) && open(k, i) && bay_at(v, k, sd.side, i) == bay_at(v, k0, sd.side, i0) &&
				relief_push(v, sd.side, i, k) == relief_push(v, sd.side, i0, k0) && b.level_h(v.base + k) == b.level_h(v.base + k0);
	};
	s.grime_floor = pl.lot.o.y + b.level_z(v.base);
	for (int k = 0; k < v.storeys; k++)
		for (int i = 0; i < n; i++) {
			if (cell_of(k, i) || !open(k, i) || !opening(k, i)) continue;
			int i1 = i + 1, k1 = k + 1;
			while (i1 < n && same(k, i1, k, i)) i1++;
			auto whole_row = [&](int row) {
				for (int j = i; j < i1; j++)
					if (!same(row, j, k, i)) return false;
				return true;
			};
			while (k1 < v.storeys && whole_row(k1)) k1++;
			for (int r = k; r < k1; r++)
				for (int j = i; j < i1; j++) cell_of(r, j) = 1;
			const float za = b.level_z(v.base + k), zb = b.level_z(v.base + k1), a0 = W * float(i), a1 = W * float(i1), push = relief_push(v, sd.side, i, k);
			const Vector3 p[4] = { sd.f.at(a0, za, push), sd.f.at(a1, za, push), sd.f.at(a1, zb, push), sd.f.at(a0, zb, push) };
			const Vector2 cell[4] = { Vector2(float(i), 0), Vector2(float(i1), 0), Vector2(float(i1), float(k1 - k)), Vector2(float(i), float(k1 - k)) };
			s.far_wall(p, sd.f.e2, cell, wall_lin(pl, v, k, sd.side), fin, pl.id, windows_of(pl, v, k, sd.side, W, b.level_h(v.base + k), bay_at(v, k, sd.side, i)));
		}
}

// The mid level's wall of a side, bay by bay as the near level has it: each opening an instance (bay_kit.h), the blank bays in
// rectangles of one push, and behind them all the plain side, which no eye reaches and which throws the building's shadow.
void bay_walls(Shell &s, const Placed &pl, const Volume &v, const Side &sd, int n, float W, int8_t fin) {
	const Body &b = *pl.b;
	const Point2 dir{ (sd.p1.x - sd.p0.x) / sd.length, (sd.p1.y - sd.p0.y) / sd.length };
	auto open = [&](int k, int i) {
		const float am = W * (float(i) + HALF);
		return !covered(b, v, v.base + k, Point2{ sd.p0.x + dir.x * am + sd.n.x * TEST_OUT, sd.p0.y + dir.y * am + sd.n.y * TEST_OUT });
	};
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
	for (int k = 0; k < v.storeys; k++)
		for (int i = 0; i < n; i++) {
			if (cell_of(k, i)) continue;
			int i1 = i + 1, k1 = k + 1;
			while (i1 < n && blank(k, i1, k, i)) i1++;
			auto whole_row = [&](int row) {
				for (int j = i; j < i1; j++)
					if (!blank(row, j, k, i)) return false;
				return true;
			};
			while (k1 < v.storeys && whole_row(k1)) k1++;
			for (int r = k; r < k1; r++)
				for (int j = i; j < i1; j++) cell_of(r, j) = 1;
			s.rect(Basis{ sd.f.at(W * float(i), b.level_z(v.base + k), relief_push(v, sd.side, i, k)), sd.f.e0, sd.f.e1, sd.f.e2 }, 0, W * float(i1 - i), 0,
					b.level_z(v.base + k1) - b.level_z(v.base + k), 0, wall_lin(pl, v, k, sd.side), fin);
		}
	s.rect(sd.f, 0, sd.length, b.level_z(v.base), b.level_z(v.top()), -MASS_IN_M, wall_lin(pl, v, 0, sd.side), fin);
}

void walls(Shell &s, const Placed &pl, const Volume &v, bool roofs) {
	const Body &b = *pl.b;
	float z0 = b.level_z(v.base);
	bool parapet = roofs && roof_flat(v.roof) && v.roof != ROOF_TERRACE && !covered_by_above(b, v);
	float ph = v.crown == CROWN_ATTIC ? ATTIC_H : PARAPET_H;
	Paint mp = moulding_of(pl);
	for (const Side &sd : sides_of(pl, v, 0)) {
		Point2 mid{ (sd.p0.x + sd.p1.x) / 2 + sd.n.x * TEST_OUT, (sd.p0.y + sd.p1.y) / 2 + sd.n.y * TEST_OUT };
		int n = bays_of(v, sd.side);
		float W = sd.length / float(n);
		int8_t fin = b.wall_of(v).finish;
		int k = 0;
		if (!pl.lean) {
			s.grime_floor = pl.lot.o.y + z0;
			bay_walls(s, pl, v, sd, n, W, fin);
			k = v.storeys;
		}
		while (k < v.storeys) {
			int level = v.base + k, k1 = v.storeys;
			float za = b.level_z(level), zb = b.level_z(v.base + k1);
			if (!covered(b, v, level, mid)) {
				float H = b.level_h(level);
				Vector3 p[4] = { sd.f.at(0, za, 0), sd.f.at(sd.length, za, 0), sd.f.at(sd.length, zb, 0), sd.f.at(0, zb, 0) };
				float kb = float(k1 - k);
				float c0 = pl.lean ? float(pl.id % CELL_OFFSETS) : 0;
				Vector2 cell[4] = { Vector2(c0, 0), Vector2(c0 + float(n), 0), Vector2(c0 + float(n), kb), Vector2(c0, kb) };
				s.grime_floor = pl.lot.o.y + z0;
				s.far_wall(p, sd.f.e2, cell, wall_lin(pl, v, k, sd.side), fin, pl.id, windows_of(pl, v, k, sd.side, W, H, side_comp(v, k, sd.side)));
			}
			k = k1;
		}
		float top = b.level_z(v.top());
		if (parapet) {
			s.rect(Basis{ sd.f.o + sd.f.e1 * top, sd.f.e0, sd.f.e1, sd.f.e2 }, 0, sd.length, 0, ph, 0, wall_lin(pl, v, v.storeys - 1, sd.side), fin);
			Basis cap{ sd.f.at(0, top + ph, 0), sd.f.e0, -sd.f.e2, sd.f.e1 };
			if (!pl.lean) s.rect(cap, 0, sd.length, 0, COPING, 0, paint_lin(mp), mp.finish);
		}
		if (v.base == 0 && !covered(b, v, 0, mid))
			s.rect(sd.f, 0, sd.length, pl.ground_lo - PLINTH_BURY, 0, 0, paint_lin(pl.data->color("plinth")), FIN_STONE);
		(void)z0;
	}
}

void roof(Shell &s, const Placed &pl, const Volume &v) {
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
		if (pl.lean) break;
		float cx = cr.x + MODULE / 2, cy = cr.y + MODULE / 2;
		if (!(cx > v.x && cx < v.x + v.w && cy > v.y && cy < v.y + v.d) || covered_by_above(b, v)) continue;
		const Paint &wp = b.wall_of(v);
		s.box(pl.lot, cr.x, cr.x + MODULE, cr.y, cr.y + MODULE, eaves, eaves + MACHINE_H, paint_lin(wp) * pl.shade, wp.finish, SKIP_E2_NEG);
	}
}
} // namespace

void emit_far(Shell &shell, const Placed &pl, bool roofs) {
	for (const Volume &v : pl.b->volumes) {
		walls(shell, pl, v, roofs);
		if (roofs) roof(shell, pl, v);
	}
}

} // namespace city::b40
