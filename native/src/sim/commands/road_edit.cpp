// Shared steps of the road commands (Roadcraft construction/execute.ts splitSeg, anchorNode, clearAndConform).
#include "sim/commands/road_edit.h"

#include "sim/commands/road_grade_sync.h"
#include "sim/roads/street_furniture.h"
#include "sim/street/footway.h"
#include "sim/street/street_names.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace city {

namespace {
constexpr double STILL_THERE_M = 0.5; // execute.ts stillThere: the point is still on the segment, away from its ends
constexpr double CLEAR_STEP_M = 2; // clearAndConform samples the centreline every 2 m
constexpr double CLEAR_SLACK_M = 0.3; // the lot rectangle grown by the half width minus this

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

int split_segment(CityState &s, int seg, double t) {
	RoadNetwork &net = s.roads;
	RoadSegment rec = net.segs[seg];
	auto halves = split_quad(rec.q, t);
	double y = net.seg_y(seg, t * rec.len);
	// The new node keeps the road where it was: its authored height the ends' along the road (the crossing reads it so).
	const double along = rec.len > 0 ? arc_at(rec.table, t) / rec.len : 0; // by length, as the 4.0's segmentOffsetAt
	const float lift = net.nodes[size_t(rec.a)].lift + (net.nodes[size_t(rec.b)].lift - net.nodes[size_t(rec.a)].lift) * float(along);
	j_remove_segment(s, seg);
	int n = j_add_node(s, halves[0].x1, halves[0].z1, y, lift);
	j_add_segment(s, rec.a, n, halves[0], rec.profile, rec.zl, rec.zr, uint16_t(rec.flags & ~SEG_RUNS_ON_B)); // a node of the player's: the road ends there
	j_add_segment(s, n, rec.b, halves[1], rec.profile, rec.zl, rec.zr, uint16_t(rec.flags & ~SEG_RUNS_ON_A));
	return n;
}

float anchor_lift(const CityState &s, const Anchor &a) {
	const RoadNetwork &net = s.roads;
	if (a.node >= 0 && a.node < int(net.nodes.size()) && net.nodes[size_t(a.node)].alive) return net.nodes[size_t(a.node)].lift;
	if (a.seg >= 0 && a.seg < int(net.segs.size()) && net.segs[size_t(a.seg)].alive) return net.lift_on(a.seg, a.t);
	return a.lift.value_or(s.new_road_lift);
}

int anchor_node(CityState &s, const Anchor &a, float lift) {
	RoadNetwork &net = s.roads;
	if (a.node >= 0 && a.node < int(net.nodes.size()) && net.nodes[a.node].alive) return a.node;
	if (a.seg >= 0 && still_there(s, a.seg, a.x, a.z)) {
		double t, d;
		closest_t(net.segs[a.seg].q, a.x, a.z, t, d);
		return split_segment(s, a.seg, t);
	}
	// What stands at another height there is passed, not joined (commit.ts materializeFree).
	const Anchor again = snap_point(s, a.x, a.z, s.catalog.tune.road_merge_m);
	const float join = s.catalog.tune.grade_join_m;
	if (again.node >= 0 && std::fabs(net.nodes[size_t(again.node)].lift - lift) <= join) return again.node;
	if (again.seg >= 0 && std::fabs(net.lift_on(again.seg, again.t) - lift) <= join) return split_segment(s, again.seg, again.t);
	return j_add_node(s, a.x, a.z, s.terrain.height_at(float(a.x), float(a.z)) + lift, lift);
}

int clear_under(CityState &s, int seg) {
	const RoadSegment &g = s.roads.segs[seg];
	if (structure_of(g.flags) == Structure::TUNNEL) return 0; // what stands over a bore stays (validate.ts touchesRoad)
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

void mark_ground(CityState &s, double minx, double minz, double maxx, double maxz) {
	for_chunks(minx, minz, maxx, maxz, [&](int64_t k) {
		s.log.road_chunks.push_back(k);
		s.log.building_chunks.push_back(k);
		s.log.lot_cells.push_back(k);
		s.log.street_cells.push_back(k);
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
	const double r = footway_reach(s.roads, s.catalog.furniture.items.stand_reach_m); // the segments whose footway it may stand on
	s.roads.for_segments_near(x - r, z - r, x + r, z + r, [&](int seg) { s.log.furniture_segs.push_back(seg); });
}

void settle_network(CityState &s) {
	s.roads.settle_shapes();
	sync_grade(s); // the heights of what the edit changed (its plates included), and the ground under them
	settle_name_plates(s.street, s.roads, s.log); // the streets' names stand on their corners: the shapes and heights above
}

void finish_command(CityState &s, bool recorded) {
	RoadNetwork &net = s.roads;
	const bool derived = net.shapes_pending() || net.grade.pending() || !s.log.lot_cells.empty();
	settle_network(s);
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
	std::vector<int64_t> cells = s.log.lot_cells;
	ChangeLog::dedup(cells);
	s.lots.process_dirty(s);
	for (int64_t k : cells)
		if (s.lots.lots_of(k)) s.growth.push(k);
	if (!recorded && !derived) return;
	s.edit_revision++;
	s.touch();
}

} // namespace city
