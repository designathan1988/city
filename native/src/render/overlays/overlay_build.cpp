// CPU meshes of the overlay layers, one per 256 m chunk: the zone ribbons along the zoned road sides and the congestion
// ribbons along each carriageway (see overlay_renderer.h).
#include "render/overlays/overlay_renderer.h"

#include "render/owned_segments.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr int SIDES = 2;
constexpr int RGBA_R_SHIFT = 24, RGBA_G_SHIFT = 16, RGBA_B_SHIFT = 8;
constexpr float BYTE_MAX = 255.f; // packed zone colours are RGBA8
constexpr double MIN_DERIV = 1e-6; // a degenerate curve keeps a finite normal
constexpr float COLOUR_RANGE_MIN = 1e-3f; // free_share == jam_share still divides
constexpr float HALF_JAM = 0.5f;

Color zone_color(const CityState &s, int code, float alpha) {
	const uint32_t c = s.catalog.zone(code).color;
	return Color(float((c >> RGBA_R_SHIFT) & 0xff) / BYTE_MAX, float((c >> RGBA_G_SHIFT) & 0xff) / BYTE_MAX, float((c >> RGBA_B_SHIFT) & 0xff) / BYTE_MAX, alpha);
}

// A convex polygon cut by a line of constant x (or z): what lies on the side `keep` looks to.
std::vector<LotPoint> clipped(const std::vector<LotPoint> &q, bool on_x, double line, double keep) {
	std::vector<LotPoint> out;
	const auto off = [&](const LotPoint &p) { return ((on_x ? p.x : p.z) - line) * keep; };
	for (size_t i = 0; i < q.size(); i++) {
		const LotPoint &p = q[i], &r = q[(i + 1) % q.size()];
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
void OverlayRenderer::lot_cover(Ribbon &out, const CityState &s, const PlayerLot &lot, int64_t key) const {
	const double cx = double(key_x(key)) * CHUNK, cz = double(key_y(key)) * CHUNK;
	// A lot all of whose corners lie on or past one side of the chunk lays no cell in it (each triangle's cells clamp to none):
	// it is left before it is cut into triangles.
	double lx0 = cx + CHUNK, lz0 = cz + CHUNK, lx1 = cx, lz1 = cz;
	for (const LotPoint &p : lot.corners) {
		lx0 = std::min(lx0, p.x);
		lz0 = std::min(lz0, p.z);
		lx1 = std::max(lx1, p.x);
		lz1 = std::max(lz1, p.z);
	}
	if (lx1 <= cx || lz1 <= cz || lx0 >= cx + CHUNK || lz0 >= cz + CHUNK) return;
	const Color col = zone_color(s, lot.zone, zone_alpha_);
	const std::vector<int> tris = lot_triangles(lot.corners);
	for (size_t t = 0; t + 2 < tris.size(); t += 3) {
		const std::vector<LotPoint> tri = { lot.corners[size_t(tris[t])], lot.corners[size_t(tris[t + 1])], lot.corners[size_t(tris[t + 2])] };
		double x0 = cx + CHUNK, z0 = cz + CHUNK, x1 = cx, z1 = cz;
		for (const LotPoint &p : tri) {
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
				const std::vector<LotPoint> cell = clipped(clipped(clipped(clipped(tri, true, ax, 1), true, ax + sample_, -1), false, az, 1), false, az + sample_, -1);
				const auto at = [&](const LotPoint &p) { return Vector3(float(p.x), s.terrain.height_at(float(p.x), float(p.z)) + lift_, float(p.z)); };
				for (size_t k = 1; k + 1 < cell.size(); k++)
					for (const LotPoint &p : { cell[0], cell[k], cell[k + 1] }) {
						out.v.push_back(at(p));
						out.c.push_back(col);
					}
			}
	}
}

// A ribbon along segment `seg` between offsets u0 and u1 (left of a -> b positive), on the road or on the ground.
void OverlayRenderer::strip(Ribbon &out, const CityState &s, int seg, float u0, float u1, const Color &col, bool on_road) const {
	const RoadSegment &g = s.roads.segs[seg];
	int n = std::max(1, int(std::ceil(g.len / sample_)));
	Vector3 prev0, prev1;
	for (int i = 0; i <= n; i++) {
		double t = double(i) / n, x, z, dx, dz;
		g.q.at(t, x, z);
		g.q.deriv(t, dx, dz);
		double l = std::max(MIN_DERIV, std::hypot(dx, dz)), nx = dz / l, nz = -dx / l;
		auto at = [&](float u) {
			float px = float(x + nx * u), pz = float(z + nz * u);
			float y = on_road ? float(s.roads.seg_y(seg, g.len * t)) : s.terrain.height_at(px, pz);
			return Vector3(px, y + lift_, pz);
		};
		Vector3 p0 = at(u0), p1 = at(u1);
		if (i > 0) {
			for (const Vector3 &v : { prev0, prev1, p1, prev0, p1, p0 }) {
				out.v.push_back(v);
				out.c.push_back(col);
			}
		}
		prev0 = p0;
		prev1 = p1;
	}
}

OverlayRenderer::Ribbon OverlayRenderer::build_zone_cpu(const CityState &s, int64_t key) const {
	Ribbon r;
	const bool every = every_zone_.load();
	each_owned_segment(s, key, [&](int sg) {
		const RoadSegment &g = s.roads.segs[size_t(sg)];
		if (!g.alive) return;
		const Profile &p = s.roads.profile_of(sg);
		for (int side = 0; side < SIDES; side++) {
			int code = side == 0 ? g.zl : g.zr;
			const bool players = g.flags & (side == 0 ? SEG_ZONED_LEFT : SEG_ZONED_RIGHT);
			if (code == 0 || (!players && !every)) continue;
			const Color col = zone_color(s, code, every ? zone_alpha_tool_ : zone_alpha_);
			if (side == 0) strip(r, s, sg, p.left, p.left + zone_depth_, col, false);
			else strip(r, s, sg, -p.right - zone_depth_, -p.right, col, false);
		}
	});
	const double cx = double(key_x(key)) * CHUNK, cz = double(key_y(key)) * CHUNK;
	for (int i : s.lot_book.near(cx, cz, cx + CHUNK, cz + CHUNK)) {
		const PlayerLot &lot = s.lot_book.all()[size_t(i)];
		if (lot.zone != 0) lot_cover(r, s, lot, key);
	}
	return r;
}

Color OverlayRenderer::congestion_colour(const PoseFrame &f, int sg) const {
	float share = size_t(sg) < f.seg_speed_share.size() ? f.seg_speed_share[size_t(sg)] : 1.f;
	float k = std::clamp((share - jam_share_) / std::max(COLOUR_RANGE_MIN, free_share_ - jam_share_), 0.f, 1.f);
	Color col = k > HALF_JAM ? jam_colors_[1].lerp(jam_colors_[0], (k - HALF_JAM) * 2) : jam_colors_[2].lerp(jam_colors_[1], k * 2);
	col.a = congestion_alpha_;
	return col;
}

// The chunk's carriageways laid as ribbons (what update_congestion colours): each one's vertices and how many.
OverlayRenderer::Lanes OverlayRenderer::lay_congestion(const CityState &s, int64_t key) const {
	Lanes l;
	Ribbon r;
	each_owned_segment(s, key, [&](int sg) {
		const RoadSegment &g = s.roads.segs[size_t(sg)];
		if (!g.alive) return;
		const Profile &p = s.roads.profile_of(sg);
		if (p.rail) return;
		const int64_t before = r.v.size();
		strip(r, s, sg, -p.asph_right, p.asph_left, Color(), true);
		l.strips.push_back({ sg, int(r.v.size() - before) });
	});
	l.v = r.v;
	return l;
}

} // namespace city
