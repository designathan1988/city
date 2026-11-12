// Geometry and paint queries of a placed body: the sides of a volume as facade frames (rectangles and shaped
// outlines), the covered test between volumes of one building, and the paints of walls, trims and mouldings.
#include "render/buildings/facade.h"

#include "sim/buildings/b40/body_air.h"

#include <cmath>

using namespace godot;

namespace city::b40 {

namespace {
constexpr float MOULD_K = 1.1f, BAND_LOW = 0.45f, BAND_MID = 0.1f, LOW_TIER = 0.94f;
constexpr uint32_t GOLDEN = 2654435761u;
constexpr int ID_OFF = 7;
} // namespace

std::vector<Side> sides_of(const Placed &pl, const Volume &v, float z) {
	// The sides in plan are the body's (sim/buildings/b40/body_air.h face_runs: what the lot's layout reads too); here their frames.
	std::vector<Side> out;
	for (const FaceRun &r : face_runs(v)) {
		Side s;
		s.side = r.side;
		s.length = r.length;
		s.p0 = r.a; s.p1 = r.b;
		s.n = r.out;
		const Point2 e{ (r.b.x - r.a.x) / r.length, (r.b.y - r.a.y) / r.length };
		Vector3 along = (pl.lot.e0 * e.x + pl.lot.e1 * e.y).normalized();
		Vector3 nrm = (pl.lot.e0 * s.n.x + pl.lot.e1 * s.n.y).normalized();
		s.f = Basis{ pl.lot.at(r.a.x, r.a.y, z), along, pl.lot.e2, nrm };
		out.push_back(s);
	}
	return out;
}

bool covered(const Body &b, const Volume &self, int level, Point2 p) {
	const float z0 = b.block_z(self, level), z1 = z0 + b.level_h(level);
	for (const Volume &o : b.volumes) {
		if (&o == &self) continue;
		// At the same height a block stands against the storey or not; on a split level it must stand against all of it, or the wall shows.
		const bool against = o.lift == self.lift ? level >= o.base && level < o.top() : (o.base == 0 || b.block_z(o, o.base) <= z0) && b.block_z(o, o.top()) >= z1;
		if (against && p.x > o.x && p.x < o.x + o.w && p.y > o.y && p.y < o.y + o.d) return true;
	}
	return false;
}

Lin paint_lin(const Paint &p) { return lin_of(p.rgb); }

Lin wall_lin(const Placed &pl, const Volume &v, int storey, int side) {
	const Body &b = *pl.b;
	auto it = v.storey_paint.find(side_key(storey, side));
	const Paint &p = it != v.storey_paint.end() ? b.paints[size_t(it->second)] : b.wall_of(v);
	return lin_of(p.rgb) * pl.shade;
}

Lin trim_lin(const Placed &pl) { return lin_of(pl.b->paints[size_t(pl.b->trim)].rgb); }

Paint moulding_of(const Placed &pl) {
	const Body &b = *pl.b;
	const Paint &trim = b.paints[size_t(b.trim)];
	if (trim.finish != FIN_METAL) return trim;
	const Paint &wall = b.paints[size_t(b.wall)];
	if (wall.finish == FIN_GLASS) return pl.data->color("coping_metal");
	return Paint{ FIN_PLASTER, scale_rgb(wall.rgb, MOULD_K) };
}

// The block's or the building's own roof, unless it is a pitched roof's or the generic slab: then one of the flat roofs, by the building.
Paint flat_roof_of(const Placed &pl, const Volume &v) {
	const Body &b = *pl.b;
	const int own = v.roof_paint >= 0 ? v.roof_paint : b.roof;
	if (own >= 0) {
		const Paint &p = b.paints[size_t(own)];
		const Paint slab = pl.data->color("office_roof");
		if (p.finish != FIN_TILE && p.finish != FIN_SLATE && !(p.finish == slab.finish && p.rgb == slab.rgb)) return p;
	}
	return pl.data->flat_roofs[((uint32_t(pl.id) + ID_OFF) * GOLDEN) % pl.data->flat_roofs.size()];
}

void band(Shell &s, const Side &sd, float z, float out, float h, const Lin &lin, int8_t fin, bool flush) {
	if (flush) {
		s.box(sd.f, -out, sd.length + out, z - h / 2, z + h / 2, 0, out, lin, fin, SKIP_E2_NEG);
		return;
	}
	float lo = out * BAND_LOW;
	s.box(sd.f, -lo, sd.length + lo, z - h / 2, z - BAND_MID * h, 0, lo, lin * LOW_TIER, fin, SKIP_E2_NEG);
	s.box(sd.f, -out, sd.length + out, z - BAND_MID * h, z + h / 2, 0, out, lin, fin, SKIP_E2_NEG);
}

} // namespace city::b40
