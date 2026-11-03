// CPU meshes of the overlay layers, one per 256 m chunk: the zone ribbons along the zoned road sides and the congestion
// ribbons along each carriageway (see overlay_renderer.h).
#include "render/overlays/overlay_renderer.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr int SIDES = 2;
constexpr double MIN_DERIV = 1e-6; // a degenerate curve keeps a finite normal
constexpr float MIN_SPAN = 1e-3f; // free_share == jam_share still divides
constexpr float HALF_JAM = 0.5f;
} // namespace

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
	float x0 = key_x(key) * CHUNK, z0 = key_y(key) * CHUNK;
	s.roads.for_segments_near(x0, z0, x0 + CHUNK, z0 + CHUNK, [&](int sg) {
		const RoadSegment &g = s.roads.segs[size_t(sg)];
		if (!g.alive) return;
		double mx, mz;
		g.q.at(0.5, mx, mz);
		if (chunk_of(float(mx), float(mz)) != key) return;
		const Profile &p = s.roads.profile_of(sg);
		for (int side = 0; side < SIDES; side++) {
			int code = side == 0 ? g.zl : g.zr;
			if (code == 0) continue;
			uint32_t c = s.catalog.zone(code).color;
			Color col(float((c >> 24) & 0xff) / 255.f, float((c >> 16) & 0xff) / 255.f, float((c >> 8) & 0xff) / 255.f, float(c & 0xff) / 255.f);
			if (side == 0) strip(r, s, sg, p.left, p.left + zone_depth_, col, false);
			else strip(r, s, sg, -p.right - zone_depth_, -p.right, col, false);
		}
	});
	return r;
}

OverlayRenderer::Ribbon OverlayRenderer::build_congestion_cpu(const CityState &s, const PoseFrame &f, int64_t key) const {
	Ribbon r;
	float x0 = key_x(key) * CHUNK, z0 = key_y(key) * CHUNK;
	s.roads.for_segments_near(x0, z0, x0 + CHUNK, z0 + CHUNK, [&](int sg) {
		const RoadSegment &g = s.roads.segs[size_t(sg)];
		if (!g.alive) return;
		double mx, mz;
		g.q.at(0.5, mx, mz);
		if (chunk_of(float(mx), float(mz)) != key) return;
		const Profile &p = s.roads.profile_of(sg);
		if (p.rail) return;
		float share = size_t(sg) < f.seg_speed_share.size() ? f.seg_speed_share[size_t(sg)] : 1.f;
		float k = std::clamp((share - jam_share_) / std::max(MIN_SPAN, free_share_ - jam_share_), 0.f, 1.f);
		Color col = k > HALF_JAM ? jam_colors_[1].lerp(jam_colors_[0], (k - HALF_JAM) * 2) : jam_colors_[2].lerp(jam_colors_[1], k * 2);
		strip(r, s, sg, -p.asph_right, p.asph_left, col, true);
	});
	return r;
}

} // namespace city
