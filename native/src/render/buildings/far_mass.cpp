// The far level of a placed body: each closed volume's
// walls as one quad per side and storey band whose windows the shader paints from the opening shares of the style
// (packed into UV2.y), the parapet as a raised wall with its coping, roof slabs, pitched roofs, machine rooms and the
// plinth. A few dozen triangles per building; `lean` (the far level) keeps one band per side and drops the coping and
// machine rooms.
#include "render/buildings/facade_bay.h"
#include "sim/buildings/b40/gen.h"
#include "render/buildings/roof_util.h"

using namespace godot;

namespace city::b40 {

namespace {
constexpr float PARAPET_H = 0.85f, ATTIC_H = 1.5f, COPING = 0.3f;
constexpr float PLINTH_BURY = 0.8f, TEST_OUT = 0.03f, MACHINE_H = 3.f, MODULE = 3.f, SLAB_LIFT = 0.002f;
constexpr uint32_t CELL_OFFSETS = 64; // bays the far level's grid starts from, by building: two buildings light other rooms
constexpr uint32_t GOLDEN = 2654435761u;
constexpr int ID_OFF = 7;

FarWindows windows_of(const Placed &pl, const Volume &v, int k, int side, float W, float H) {
	BayCtx c;
	c.W = W; c.H = H; c.level = v.base + k; c.geo = &v.faces[side];
	auto it = v.bays.find(bay_key(k, side, bays_of(v, side) / 2));
	c.comp = it != v.bays.end() && it->second != BAY_DOOR && it->second != BAY_DOUBLE_DOOR ? it->second : fill_at(v, k, side);
	Opening o = opening_of(*pl.data, c);
	FarWindows w;
	w.width = o.w / W; w.sill = o.h0 / H; w.head = o.h1 / H;
	w.flags = (o.has ? 0 : WINDOW_FLAG_BLANK) | (pl.b->use == USE_COMMERCIAL ? WINDOW_FLAG_OFFICE : 0) | (pane_dark(c.comp) << WINDOW_DARK_SHIFT);
	w.trim = trim_lin(pl);
	return w;
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
		while (k < v.storeys) {
			int level = v.base + k, k1 = level == 0 && !pl.lean ? k + 1 : v.storeys;
			float za = b.level_z(level), zb = b.level_z(v.base + k1);
			if (!covered(b, v, level, mid)) {
				float H = b.level_h(level);
				Vector3 p[4] = { sd.f.at(0, za, 0), sd.f.at(sd.length, za, 0), sd.f.at(sd.length, zb, 0), sd.f.at(0, zb, 0) };
				float kb = float(k1 - k);
				float c0 = pl.lean ? float(pl.id % CELL_OFFSETS) : 0;
				Vector2 cell[4] = { Vector2(c0, 0), Vector2(c0 + float(n), 0), Vector2(c0 + float(n), kb), Vector2(c0, kb) };
				s.grime_floor = pl.lot.o.y + z0;
				s.far_wall(p, sd.f.e2, cell, wall_lin(pl, v, k, sd.side), fin, pl.id, windows_of(pl, v, k, sd.side, W, H));
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
	Paint rp = v.roof == ROOF_TERRACE || covered_by_above(b, v) ? pl.data->color("terrace")
			: v.roof_paint >= 0 ? b.paints[size_t(v.roof_paint)]
			: pl.data->flat_roofs[((uint32_t(pl.id) + ID_OFF) * GOLDEN) % pl.data->flat_roofs.size()];
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
