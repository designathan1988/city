// The ribbons of the map layers, one chunk at a time, laid by a job from its cut of the city: the zone ribbons along the zoned
// road sides and over the player's zoned lots, and the carriageways the congestion map colours (see overlay_renderer.h).
#include "render/overlays/overlay_renderer.h"

#include "render/owned_segments.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr int SIDES = 2;
constexpr int QUAD_VERTICES = 6; // a ribbon's step: two triangles
constexpr double MIN_DERIV = 1e-6; // a degenerate curve keeps a finite normal

Color zone_color(const Catalog &catalog, int code, float alpha) {
	Color out = rgba8_to_color(catalog.zone(code).color); // packed zone colours are RGBA8
	out.a = alpha;
	return out;
}

// A convex polygon cut by a line of constant x (or z): what lies on the side `keep` looks to.
std::vector<XZ> clipped(const std::vector<XZ> &q, bool on_x, double line, double keep) {
	std::vector<XZ> out;
	const auto off = [&](const XZ &p) { return ((on_x ? p.x : p.z) - line) * keep; };
	for (size_t i = 0; i < q.size(); i++) {
		const XZ &p = q[i], &r = q[(i + 1) % q.size()];
		const double a = off(p), b = off(r);
		if (a >= 0) out.push_back(p);
		if ((a >= 0) != (b >= 0)) {
			const double t = a / (a - b);
			out.push_back({ p.x + (r.x - p.x) * t, p.z + (r.z - p.z) * t });
		}
	}
	return out;
}
} // namespace

// A zoned lot coloured on the ground (lotOverlay.ts FLAG_FILL): its triangles cut to the cells of the chunk, so the colour lies on the
// relief and each chunk draws its own part of the lot.
void OverlayRenderer::lot_cover(Ribbon &out, const CityView &s, const PlayerLot &lot, int64_t key) const {
	const double cx = double(key_x(key)) * CHUNK, cz = double(key_y(key)) * CHUNK;
	// A lot all of whose corners lie on or past one side of the chunk lays no cell in it (each triangle's cells clamp to none):
	// it is left before it is cut into triangles.
	double lx0 = cx + CHUNK, lz0 = cz + CHUNK, lx1 = cx, lz1 = cz;
	for (const XZ &p : lot.corners) {
		lx0 = std::min(lx0, p.x);
		lz0 = std::min(lz0, p.z);
		lx1 = std::max(lx1, p.x);
		lz1 = std::max(lz1, p.z);
	}
	if (lx1 <= cx || lz1 <= cz || lx0 >= cx + CHUNK || lz0 >= cz + CHUNK) return;
	const Color col = zone_color(s.catalog, lot.zone, zone_alpha_);
	const std::vector<int> tris = lot_triangles(lot.corners);
	for (size_t t = 0; t + 2 < tris.size(); t += 3) {
		const std::vector<XZ> tri = { lot.corners[size_t(tris[t])], lot.corners[size_t(tris[t + 1])], lot.corners[size_t(tris[t + 2])] };
		double x0 = cx + CHUNK, z0 = cz + CHUNK, x1 = cx, z1 = cz;
		for (const XZ &p : tri) {
			x0 = std::min(x0, p.x);
			z0 = std::min(z0, p.z);
			x1 = std::max(x1, p.x);
			z1 = std::max(z1, p.z);
		}
		const int i0 = std::max(0, int(std::floor((x0 - cx) / sample_))), i1 = std::min(int(CHUNK / sample_), int(std::ceil((x1 - cx) / sample_)));
		const int j0 = std::max(0, int(std::floor((z0 - cz) / sample_))), j1 = std::min(int(CHUNK / sample_), int(std::ceil((z1 - cz) / sample_)));
		for (int j = j0; j < j1; j++)
			for (int i = i0; i < i1; i++) {
				const double ax = cx + double(i) * sample_, az = cz + double(j) * sample_;
				const std::vector<XZ> cell = clipped(clipped(clipped(clipped(tri, true, ax, 1), true, ax + sample_, -1), false, az, 1), false, az + sample_, -1);
				const auto at = [&](const XZ &p) { return Vector3(float(p.x), s.terrain.height_at(float(p.x), float(p.z)) + lift_, float(p.z)); };
				for (size_t k = 1; k + 1 < cell.size(); k++)
					for (const XZ &p : { cell[0], cell[k], cell[k + 1] }) {
						out.v.push_back(at(p));
						out.c.push_back(col);
						map_hold::built();
					}
			}
	}
}

// A ribbon along segment `seg` between offsets u0 and u1 (left of a -> b positive), on the road or on the ground: its vertices
// appended to `v`, and how many.
int OverlayRenderer::strip(PackedVector3Array &v, const CityView &s, int seg, float u0, float u1, bool on_road) const {
	const RoadSegment &g = s.roads.segs[size_t(seg)];
	const int n = std::max(1, int(std::ceil(g.len / sample_)));
	const int64_t first = v.size();
	v.resize(first + int64_t(n) * QUAD_VERTICES);
	Vector3 *out = v.ptrw() + first;
	Vector3 prev0, prev1;
	for (int i = 0; i <= n; i++) {
		double t = double(i) / n, x, z, dx, dz;
		g.q.at(t, x, z);
		g.q.deriv(t, dx, dz);
		const double l = std::max(MIN_DERIV, std::hypot(dx, dz)), nx = dz / l, nz = -dx / l;
		const auto at = [&](float u) {
			const float px = float(x + nx * u), pz = float(z + nz * u);
			const float y = on_road ? float(s.roads.seg_y(seg, g.len * t)) : s.terrain.height_at(px, pz);
			return Vector3(px, y + lift_, pz);
		};
		const Vector3 p0 = at(u0), p1 = at(u1);
		if (i > 0)
			for (const Vector3 &p : { prev0, prev1, p1, prev0, p1, p0 }) {
				*out++ = p;
				map_hold::built();
			}
		prev0 = p0;
		prev1 = p1;
	}
	return n * QUAD_VERTICES;
}

OverlayRenderer::Ribbon OverlayRenderer::build_zone_cpu(const CityView &s, const std::vector<PlayerLot> &lots, int64_t key) const {
	Ribbon r;
	const bool every = every_zone_.load();
	each_owned_segment(s, key, [&](int sg) {
		const RoadSegment &g = s.roads.segs[size_t(sg)];
		if (!g.alive) return;
		const Profile &p = s.roads.profile_of(sg);
		for (int side = 0; side < SIDES; side++) {
			const int code = side == 0 ? g.zl : g.zr;
			const bool players = g.flags & (side == 0 ? SEG_ZONED_LEFT : SEG_ZONED_RIGHT);
			if (code == 0 || (!players && !every)) continue;
			const int count = side == 0 ? strip(r.v, s, sg, p.left, p.left + zone_depth_, false) : strip(r.v, s, sg, -p.right - zone_depth_, -p.right, false);
			const int64_t at = r.c.size();
			r.c.resize(at + count);
			std::fill_n(r.c.ptrw() + at, count, zone_color(s.catalog, code, every ? zone_alpha_tool_ : zone_alpha_));
		}
	});
	for (const PlayerLot &lot : lots) lot_cover(r, s, lot, key);
	return r;
}

// The chunk's carriageways laid as ribbons, each vertex with its segment's id (a float holds every id under 2^24 exactly): what
// the level texture colours.
OverlayRenderer::Ribbon OverlayRenderer::lay_congestion(const CityView &s, int64_t key) const {
	Ribbon r;
	each_owned_segment(s, key, [&](int sg) {
		const RoadSegment &g = s.roads.segs[size_t(sg)];
		if (!g.alive) return;
		const Profile &p = s.roads.profile_of(sg);
		if (p.rail) return;
		const int count = strip(r.v, s, sg, -p.asph_right, p.asph_left, true);
		const int64_t at = r.slot.size();
		r.slot.resize(at + count);
		std::fill_n(r.slot.ptrw() + at, count, Vector2(float(sg), 0.f));
		r.slots = std::max(r.slots, sg + 1);
	});
	return r;
}

} // namespace city
