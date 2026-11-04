// The ground round the road: a ribbon from the paving's outer edge, at its height, easing down to just under the terrain, so
// the land meets the footway with no wall (Roadcraft render/roadSurfaceReads.ts vergeTop). Drawn by render/roads/verge_renderer.h
// in the terrain's grass; where the ribbon goes under, the terrain carries on.
#include "render/roads/road_mesh.h"
#include "render/roads/road_section.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
const Vector3 UP(0, 1, 0);

float ease(float t) { return t * t * (3.f - 2.f * t); }

// How far the ground stands at share `t` of the way out from `p`.
Vector3 ground_at(const RoadEmit &e, const Vector3 &p, const Vector3 &away, float t) {
	const StreetLook &L = e.L;
	Vector3 q = p + away * (L.verge_reach * t);
	const float ground = e.s.terrain.height_at(q.x, q.z) - L.verge_under;
	q.y = p.y + (ground - p.y) * ease(t);
	return q;
}

// Up, leaning off the fall from a to b.
Vector3 normal_of(const Vector3 &a, const Vector3 &b, const Vector3 &away) {
	const float run = std::max<float>(Vector2(b.x - a.x, b.z - a.z).length(), CMP_EPSILON);
	return (UP - away * ((b.y - a.y) / run)).normalized();
}
} // namespace

void verge_ribbon(const RoadEmit &e, const std::vector<Vector3> &edge, const std::vector<Vector3> &away) {
	const int steps = e.L.verge_strips;
	std::vector<int> prev;
	for (size_t i = 0; i < edge.size(); i++) {
		std::vector<int> ids;
		for (int k = 0; k <= steps; k++) {
			const float t = float(k) / float(steps), dt = 1.f / float(steps);
			const Vector3 a = ground_at(e, edge[i], away[i], std::max(0.f, t - dt)), b = ground_at(e, edge[i], away[i], std::min(1.f, t + dt));
			ids.push_back(e.mb.add(ground_at(e, edge[i], away[i], t), normal_of(a, b, away[i]), Color(1, 1, 1)));
		}
		if (i > 0) {
			for (int k = 0; k < steps; k++) {
				e.mb.tri(prev[k], ids[k], ids[k + 1], UP);
				e.mb.tri(prev[k], ids[k + 1], prev[k + 1], UP);
			}
		}
		prev = std::move(ids);
	}
}

void emit_verge_segment(const RoadEmit &e, int sid) {
	const RoadSegment &g = e.s.roads.segs[sid];
	const Profile &p = e.s.catalog.profile(g.profile);
	const double v0 = g.retreat_a, v1 = g.len - g.retreat_b;
	if (p.edges.empty() || p.rail || v1 <= v0) return;
	const std::vector<Piece> ps = pieces_of(e.L, p);
	// A straight road is linear in height and the ground is held to it, so the ribbon needs no more points than the road has (road_segment.cpp)
	const int n = g.q.is_straight(STRAIGHT_TOL) ? 1 : std::max(1, int(std::ceil((v1 - v0) / e.L.step)));
	std::vector<Vector3> right, right_out, left, left_out;
	for (int k = 0; k <= n; k++) {
		const SegFrame f = seg_frame(e.s, sid, v0 + (v1 - v0) * k / n);
		right.push_back(f.c + f.left * ps.front().u0 + Vector3(0, ps.front().h, 0));
		right_out.push_back(-f.left);
		left.push_back(f.c + f.left * ps.back().u1 + Vector3(0, ps.back().h, 0));
		left_out.push_back(f.left);
	}
	verge_ribbon(e, right, right_out);
	verge_ribbon(e, left, left_out);
}

} // namespace city
