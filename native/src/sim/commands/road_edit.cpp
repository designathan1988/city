// Shared steps of the road commands (Roadcraft construction/execute.ts splitSeg, anchorNode, clearAndConform).
#include "sim/commands/road_edit.h"

#include "sim/roads/street_furniture.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace city {

namespace {
constexpr double STILL_THERE_M = 0.5; // execute.ts stillThere: the point is still on the segment, away from its ends
constexpr double CLEAR_STEP_M = 2; // clearAndConform samples the centreline every 2 m
constexpr double CLEAR_SLACK_M = 0.3; // the lot rectangle grown by the half width minus this
constexpr double CONFORM_STEP_M = 16; // terrain corridor points along a curved segment
constexpr double STREET_MARK_PAD_M = 40; // what the player places stands on a segment within this (the widest street's footway)
constexpr float NODE_REFRESH_PAD_M = 60; // nodes this far past the edit may change shape (bulbs see their neighbours)

bool still_there(const CityState &s, int seg, double x, double z) {
	const RoadNetwork &net = s.roads;
	if (seg < 0 || seg >= int(net.segs.size()) || !net.segs[seg].alive) return false;
	const RoadSegment &g = net.segs[seg];
	double t, d;
	closest_t(g.q, x, z, t, d);
	return d < STILL_THERE_M && std::hypot(net.nodes[g.a].x - x, net.nodes[g.a].z - z) > STILL_THERE_M &&
			std::hypot(net.nodes[g.b].x - x, net.nodes[g.b].z - z) > STILL_THERE_M;
}
} // namespace

double arc_at(const std::vector<double> &table, double t) {
	int n = int(table.size()) - 1;
	double f = std::max(0.0, std::min(1.0, t)) * n;
	int i = std::min(n - 1, int(std::floor(f)));
	return table[i] + (table[i + 1] - table[i]) * (f - i);
}

double road_value(const CityState &s, int profile, double len) {
	return len * s.catalog.profile(profile).cost_per_m * s.catalog.structure.ground;
}

double segment_value(const CityState &s, int seg) {
	const RoadSegment &g = s.roads.segs[seg];
	return road_value(s, g.profile, g.len);
}

int split_segment(CityState &s, int seg, double t) {
	RoadNetwork &net = s.roads;
	RoadSegment rec = net.segs[seg];
	auto halves = split_quad(rec.q, t);
	double y = net.seg_y(seg, t * rec.len);
	j_remove_segment(s, seg);
	int n = j_add_node(s, halves[0].x1, halves[0].z1, y);
	j_add_segment(s, rec.a, n, halves[0], rec.profile, rec.zl, rec.zr, rec.flags);
	j_add_segment(s, n, rec.b, halves[1], rec.profile, rec.zl, rec.zr, rec.flags);
	return n;
}

int anchor_node(CityState &s, const Anchor &a) {
	RoadNetwork &net = s.roads;
	if (a.node >= 0 && a.node < int(net.nodes.size()) && net.nodes[a.node].alive) return a.node;
	if (a.seg >= 0 && still_there(s, a.seg, a.x, a.z)) {
		double t, d;
		closest_t(net.segs[a.seg].q, a.x, a.z, t, d);
		return split_segment(s, a.seg, t);
	}
	Anchor again = snap_point(s, a.x, a.z, s.catalog.tune.road_merge_m);
	if (again.node >= 0) return again.node;
	if (again.seg >= 0) return split_segment(s, again.seg, again.t);
	return j_add_node(s, a.x, a.z, s.terrain.height_at(float(a.x), float(a.z)));
}

int clear_under(CityState &s, int seg) {
	const RoadSegment &g = s.roads.segs[seg];
	double half = s.catalog.profile(g.profile).right;
	int n = std::max(2, int(std::ceil(g.len / CLEAR_STEP_M)) + 1);
	std::vector<double> px(static_cast<size_t>(n)), pz(static_cast<size_t>(n));
	for (int i = 0; i < n; i++) g.q.at(double(i) / (n - 1), px[i], pz[i]);
	std::vector<int> doomed;
	float reach = float(half + std::max(g.maxx - g.minx, g.maxz - g.minz));
	s.buildings.query(g.minx - reach, g.minz - reach, g.maxx + reach, g.maxz + reach, [&](int id) {
		const Building &b = s.buildings.items[id];
		double c = std::cos(b.rot), sn = std::sin(b.rot), hw = b.w / 2 + half - CLEAR_SLACK_M, hd = b.d / 2 + half - CLEAR_SLACK_M;
		for (int k = 0; k < n; k++) {
			double dx = px[k] - b.x, dz = pz[k] - b.z;
			if (std::fabs(dx * c - dz * sn) < hw && std::fabs(dx * sn + dz * c) < hd) {
				doomed.push_back(id);
				return;
			}
		}
	});
	for (int id : doomed) j_remove_building(s, id);
	return int(doomed.size());
}

void conform_under(CityState &s, int seg) {
	const RoadSegment &g = s.roads.segs[seg];
	int n = std::max(1, int(std::ceil(g.len / CONFORM_STEP_M)));
	std::vector<float> pts;
	for (int i = 0; i <= n; i++) {
		double x, z;
		g.q.at(double(i) / n, x, z);
		pts.insert(pts.end(), { float(x), float(s.roads.seg_y(seg, g.len * i / n)), float(z) });
	}
	j_add_modifier(s, pts, s.catalog.profile(g.profile).half(), s.catalog.tune.terrain_min_run_m);
}

void mark_ground(CityState &s, double minx, double minz, double maxx, double maxz) {
	for_chunks(minx, minz, maxx, maxz, [&](int64_t k) {
		s.log.road_chunks.push_back(k);
		s.log.building_chunks.push_back(k);
		s.log.lot_cells.push_back(k);
		s.log.nature_chunks.push_back(k);
		s.log.water_chunks.push_back(k);
	});
}

void mark_nature(CityState &s, double minx, double minz, double maxx, double maxz) {
	for_chunks(minx, minz, maxx, maxz, [&](int64_t k) { s.log.nature_chunks.push_back(k); });
}

void mark_paint(CityState &s, const int rect[4]) {
	s.log.paint_rects.insert(s.log.paint_rects.end(), rect, rect + 4);
	const Heightfield &t = s.terrain;
	mark_nature(s, t.x_of(rect[0]), t.x_of(rect[1]), t.x_of(rect[2]), t.x_of(rect[3]));
}

void mark_furniture(CityState &s, int seg) {
	const RoadSegment &g = s.roads.segs[size_t(seg)];
	s.log.furniture_segs.push_back(seg);
	for (int j = int(std::floor(g.minz / CHUNK)); j <= int(std::floor(g.maxz / CHUNK)); j++)
		for (int i = int(std::floor(g.minx / CHUNK)); i <= int(std::floor(g.maxx / CHUNK)); i++) s.log.road_chunks.push_back(cell_key(i, j));
}

void mark_street(CityState &s, double x, double z) {
	s.log.prop_chunks.push_back(chunk_of(float(x), float(z)));
	const double r = STREET_MARK_PAD_M;
	s.roads.for_segments_near(x - r, z - r, x + r, z + r, [&](int seg) { s.log.furniture_segs.push_back(seg); });
}

void finish_edit(CityState &s, double minx, double minz, double maxx, double maxz) {
	RoadNetwork &net = s.roads;
	std::vector<std::pair<double, double>> squares; // a hydrant may move to another corner of the changed nodes' squares
	for (int n : s.log.net_nodes) {
		double x0, z0, x1, z1;
		hydrant_square(net, net.nodes[size_t(n)].x, net.nodes[size_t(n)].z, x0, z0, x1, z1);
		squares.push_back({ x0, z0 });
	}
	std::sort(squares.begin(), squares.end());
	squares.erase(std::unique(squares.begin(), squares.end()), squares.end());
	std::vector<int> segs;
	for (const auto &sq : squares) {
		double x0, z0, x1, z1;
		hydrant_square(net, sq.first, sq.second, x0, z0, x1, z1);
		net.node_hash.query(float(x0), float(z0), float(x1), float(z1), [&](int k) {
			if (net.nodes[size_t(k)].alive) segs.insert(segs.end(), net.nodes[size_t(k)].segs.begin(), net.nodes[size_t(k)].segs.end());
		});
	}
	ChangeLog::dedup(segs);
	for (int seg : segs) mark_furniture(s, seg);
	float p = NODE_REFRESH_PAD_M;
	std::vector<int> nodes;
	net.node_hash.query(float(minx) - p, float(minz) - p, float(maxx) + p, float(maxz) + p, [&](int n) {
		if (net.nodes[n].alive && std::find(nodes.begin(), nodes.end(), n) == nodes.end()) nodes.push_back(n);
	});
	for (int n : nodes) net.refresh_node(n);
	std::vector<int64_t> cells = s.log.lot_cells;
	ChangeLog::dedup(cells);
	s.lots.process_dirty(s);
	for (int64_t k : cells)
		if (s.lots.lots_of(k)) s.growth.push(k);
	s.edit_revision++;
	s.touch();
}

} // namespace city
