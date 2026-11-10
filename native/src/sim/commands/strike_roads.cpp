// A crater in the paving cuts the road when it leaves no way past: the stretch inside it is taken out of the network
// (the segment is split at the crater's edges and the middle piece removed, all through the journal), so the traffic and
// the walkers route round the gap; the player closes it by building the road again. A crater that leaves a strip a
// vehicle fits through only damages the paving (the render shows the hole).
#include "sim/commands/strike_internal.h"

#include "sim/commands/edit_journal.h"
#include "sim/commands/road_edit.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr int CUT_SAMPLES = 64; // the centreline is walked in this many steps to find where the crater covers it
constexpr double END_EPS = 0.01; // a cut this close to a segment's end is that end
constexpr double UNBOUNDED_M = 1e9;

struct Span {
	double t0 = 1, t1 = 0, nearest = UNBOUNDED_M; // the stretch of the curve inside the crater, and its closest approach to the centre
	bool any() const { return t0 <= t1; }
};

Span covered(const RoadSegment &g, double x, double z, double radius) {
	Span sp;
	for (int i = 0; i <= CUT_SAMPLES; i++) {
		const double t = double(i) / CUT_SAMPLES;
		double px, pz;
		g.q.at(t, px, pz);
		const double d = std::hypot(px - x, pz - z);
		sp.nearest = std::min(sp.nearest, d);
		if (d <= radius) {
			sp.t0 = std::min(sp.t0, t);
			sp.t1 = std::max(sp.t1, t);
		}
	}
	return sp;
}

// The segment joining two nodes (the piece a split left), or -1.
int between(const RoadNetwork &net, int a, int b) {
	for (int sg : net.nodes[size_t(a)].segs) {
		const RoadSegment &g = net.segs[size_t(sg)];
		if (g.alive && ((g.a == a && g.b == b) || (g.a == b && g.b == a))) return sg;
	}
	return -1;
}

// The room the crater leaves across the road: the larger strip on either side of it, against the widest vehicle.
bool blocks(const Blow &b, const RoadSegment &g, const Span &sp, float radius) {
	const float half = b.s.catalog.profile(g.profile).half();
	const float off = float(sp.nearest);
	const float free_near = off - radius + half, free_far = half - (off + radius);
	return std::max(free_near, free_far) < b.p.pass_width_m;
}

void cut(Blow &b, int seg, const Span &sp) {
	RoadNetwork &net = b.s.roads;
	const int a = net.segs[size_t(seg)].a, z = net.segs[size_t(seg)].b;
	int n0 = a, n1 = z, piece = seg;
	double u0 = sp.t0;
	if (sp.t1 < 1 - END_EPS) {
		n1 = split_segment(b.s, piece, sp.t1);
		piece = between(net, a, n1);
		u0 = sp.t1 > 0 ? sp.t0 / sp.t1 : 0;
	}
	if (piece < 0) return;
	if (u0 > END_EPS) {
		n0 = split_segment(b.s, piece, u0);
		piece = between(net, n0, n1);
	}
	if (piece < 0) return;
	j_remove_segment(b.s, piece);
	j_remove_node_if_orphan(b.s, n0);
	j_remove_node_if_orphan(b.s, n1);
	b.result.segments++;
}
} // namespace

void cut_roads(Blow &b, float radius) {
	std::vector<std::pair<int, Span>> hits;
	b.s.roads.for_segments_near(b.x - radius, b.z - radius, b.x + radius, b.z + radius, [&](int sg) {
		const RoadSegment &g = b.s.roads.segs[size_t(sg)];
		const Span sp = covered(g, b.x, b.z, radius);
		if (sp.any() && blocks(b, g, sp, radius)) hits.push_back({ sg, sp });
	});
	std::sort(hits.begin(), hits.end(), [](const auto &l, const auto &r) { return l.first < r.first; });
	for (const auto &h : hits) {
		if (b.s.roads.segs[size_t(h.first)].alive) cut(b, h.first, h.second); // an earlier cut may have split it
	}
}

} // namespace city
