// Geometry and paint queries of a placed body: the sides of a volume as facade frames (rectangles and shaped
// outlines), the covered test between volumes of one building, and the paints of walls, trims and mouldings.
#include "render/buildings/facade.h"

#include <cmath>

using namespace godot;

namespace city::b40 {

namespace {
constexpr float MOULD_K = 1.1f, BAND_LOW = 0.45f, BAND_MID = 0.1f, LOW_TIER = 0.94f;
} // namespace

std::vector<Side> sides_of(const Placed &pl, const Volume &v, float z) {
	std::vector<Point2> pts;
	if (v.outline.empty()) pts = { { v.x, v.y }, { v.x + v.w, v.y }, { v.x + v.w, v.y + v.d }, { v.x, v.y + v.d } };
	else
		for (const Point2 &q : v.outline) pts.push_back({ v.x + q.x * v.w, v.y + q.y * v.d });
	float area = 0;
	for (size_t i = 0; i < pts.size(); i++) {
		const Point2 &a = pts[i], &b = pts[(i + 1) % pts.size()];
		area += a.x * b.y - b.x * a.y;
	}
	std::vector<Side> out;
	for (size_t i = 0; i < pts.size(); i++) {
		Point2 a = pts[i], b = pts[(i + 1) % pts.size()];
		if (area < 0) std::swap(a, b);
		float len = std::hypot(b.x - a.x, b.y - a.y);
		if (len < EPS) continue;
		Side s;
		s.side = int(i);
		s.length = len;
		s.p0 = a; s.p1 = b;
		Point2 e{ (b.x - a.x) / len, (b.y - a.y) / len };
		s.n = Point2{ e.y, -e.x };
		Vector3 along = (pl.lot.e0 * e.x + pl.lot.e1 * e.y).normalized();
		Vector3 nrm = (pl.lot.e0 * s.n.x + pl.lot.e1 * s.n.y).normalized();
		s.f = Basis{ pl.lot.at(a.x, a.y, z), along, pl.lot.e2, nrm };
		out.push_back(s);
	}
	return out;
}

bool covered(const Body &b, const Volume &self, int level, Point2 p) {
	for (const Volume &o : b.volumes) {
		if (&o == &self || level < o.base || level >= o.top()) continue;
		if (p.x > o.x && p.x < o.x + o.w && p.y > o.y && p.y < o.y + o.d) return true;
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
