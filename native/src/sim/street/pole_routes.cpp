// The guide lines of the poles and the cheapest way through them, see pole_routes.h.
#include "sim/street/pole_routes.h"

#include "sim/roads/street_furniture.h"
#include "sim/street/footway.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <queue>

namespace city {

namespace {
// The end of a guide line at a node.
struct LineEnd {
	int seg = -1, station = -1;
};
} // namespace

int RouteGraph::add(double x, double z) {
	x_.push_back(x);
	z_.push_back(z);
	edges_.emplace_back();
	return int(x_.size()) - 1;
}

void RouteGraph::link(int a, int b, double extra) {
	const double c = std::hypot(x_[size_t(a)] - x_[size_t(b)], z_[size_t(a)] - z_[size_t(b)]) + extra;
	edges_[size_t(a)].push_back({ b, c });
	edges_[size_t(b)].push_back({ a, c });
}

void RouteGraph::build(const RoadNetwork &net, double x0, double z0, double x1, double z1) {
	x_.clear();
	z_.clear();
	edges_.clear();
	const PoleDef &d = net.cat->furniture.poles;
	std::map<int, std::vector<LineEnd>> ends;
	std::vector<int> segs;
	net.for_segments_near(x0, z0, x1, z1, [&](int sid) { segs.push_back(sid); });
	std::sort(segs.begin(), segs.end());
	for (int sid : segs) {
		const RoadSegment &g = net.segs[size_t(sid)];
		const Profile &p = net.profile_of(sid);
		const double v0 = g.retreat_a, v1 = g.len - g.retreat_b;
		if (p.edges.empty() || g.a == g.b || v1 <= v0) continue;
		const int steps = std::max(1, int(std::ceil((v1 - v0) / d.guide_step_m)));
		std::vector<int> line[2];
		for (int side = 0; side < 2; side++) {
			if (!sidewalk_on(p, side)) continue;
			const float u = sidewalk_u(p, side, d.kerb_inset_m);
			for (int k = 0; k <= steps; k++) {
				double x, z, y;
				footway_point(net, sid, v0 + (v1 - v0) * k / steps, u, x, z, y);
				line[side].push_back(add(x, z));
				if (k > 0) link(line[side][size_t(k) - 1], line[side][size_t(k)], 0);
			}
			ends[g.a].push_back({ sid, line[side].front() });
			ends[g.b].push_back({ sid, line[side].back() });
		}
		if (line[0].empty() || line[1].empty()) continue;
		for (int k = 0; k <= steps; k++) {
			const int a = line[0][size_t(k)], b = line[1][size_t(k)];
			if (std::hypot(x_[size_t(a)] - x_[size_t(b)], z_[size_t(a)] - z_[size_t(b)]) <= d.max_spacing_m) link(a, b, d.crossing_cost_m);
		}
	}
	// Round each corner: every end of a line joins the nearest end of another arm of its node.
	for (const auto &kv : ends) {
		for (const LineEnd &e : kv.second) {
			int best = -1;
			double best_d = d.corner_reach_m;
			for (const LineEnd &o : kv.second) {
				if (o.seg == e.seg) continue;
				const double dist = std::hypot(x_[size_t(e.station)] - x_[size_t(o.station)], z_[size_t(e.station)] - z_[size_t(o.station)]);
				if (dist < best_d) {
					best_d = dist;
					best = o.station;
				}
			}
			if (best >= 0) link(e.station, best, 0);
		}
	}
}

int RouteGraph::nearest(double x, double z, double reach) const {
	int best = -1;
	double best_d = reach;
	for (size_t i = 0; i < x_.size(); i++) {
		const double d = std::hypot(x_[i] - x, z_[i] - z);
		if (d <= best_d) {
			best_d = d;
			best = int(i);
		}
	}
	return best;
}

std::vector<int> RouteGraph::route(int from, int to, double &cost) const {
	using Item = std::pair<double, int>;
	const size_t n = x_.size();
	std::vector<double> dist(n, std::numeric_limits<double>::max());
	std::vector<int> prev(n, -1);
	std::priority_queue<Item, std::vector<Item>, std::greater<Item>> open;
	dist[size_t(from)] = 0;
	open.push({ 0, from });
	while (!open.empty()) {
		const Item top = open.top();
		open.pop();
		if (top.first > dist[size_t(top.second)]) continue;
		if (top.second == to) break;
		for (const Edge &e : edges_[size_t(top.second)]) {
			const double c = top.first + e.cost;
			if (c >= dist[size_t(e.to)]) continue;
			dist[size_t(e.to)] = c;
			prev[size_t(e.to)] = top.second;
			open.push({ c, e.to });
		}
	}
	std::vector<int> path;
	if (from != to && prev[size_t(to)] < 0) return path;
	cost = dist[size_t(to)];
	for (int k = to; k >= 0; k = prev[size_t(k)]) path.push_back(k);
	std::reverse(path.begin(), path.end());
	return path;
}

} // namespace city
