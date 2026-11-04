// Road graph storage, spatial queries and node shape (Roadcraft roads/network.ts, roads/junction.ts bulbAt).
#include "sim/roads/road_network.h"

#include "sim/roads/junction.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr double STRAIGHT_ANGLE_COS = -0.9986; // two arms this close to opposite are one street going on
constexpr double BULB_REACH_PAD = 3, BULB_SEARCH_PAD = 30;
constexpr float LOT_LOOK_M = 160.f; // lots look this far for their streets (sim/citygen/lot_gen.h LOT_MARGIN + setbacks)
} // namespace

void RoadNetwork::init(const Catalog *c, ChangeLog *l) {
	cat = c;
	log = l;
	clear();
}

void RoadNetwork::clear() {
	player_sides = 0;
	nodes.clear();
	segs.clear();
	free_nodes.clear();
	free_segs.clear();
	seg_hash.clear();
	node_hash.clear();
	seg_hash.cell = node_hash.cell = ROAD_HASH_CELL;
	version++;
}

int RoadNetwork::add_node(double x, double z, double y) {
	int id;
	if (!free_nodes.empty()) { id = free_nodes.back(); free_nodes.pop_back(); }
	else { id = int(nodes.size()); nodes.emplace_back(); }
	RoadNode &n = nodes[id];
	n = RoadNode();
	n.alive = true;
	n.x = x; n.z = z; n.y = y;
	node_hash.insert_aabb(id, float(x), float(z), float(x), float(z));
	version++;
	return id;
}

void RoadNetwork::update_bounds(int s) {
	RoadSegment &g = segs[s];
	float h = cat->profile(g.profile).half();
	g.minx = float(std::min({ g.q.x0, g.q.cx, g.q.x1 })) - h;
	g.maxx = float(std::max({ g.q.x0, g.q.cx, g.q.x1 })) + h;
	g.minz = float(std::min({ g.q.z0, g.q.cz, g.q.z1 })) - h;
	g.maxz = float(std::max({ g.q.z0, g.q.cz, g.q.z1 })) + h;
}

int RoadNetwork::add_segment(int a, int b, const Quad &q, int profile, int zl, int zr, uint16_t flags) {
	int id;
	if (!free_segs.empty()) { id = free_segs.back(); free_segs.pop_back(); }
	else { id = int(segs.size()); segs.emplace_back(); }
	RoadSegment &g = segs[id];
	g = RoadSegment();
	g.alive = true;
	g.a = a; g.b = b; g.q = q; g.profile = profile;
	g.zl = uint8_t(zl); g.zr = uint8_t(zr); g.flags = flags;
	player_sides += int((flags & SEG_ZONED_LEFT) != 0) + int((flags & SEG_ZONED_RIGHT) != 0);
	g.table = arc_table(q, ROAD_ARC_STEPS);
	g.len = g.table.back();
	update_bounds(id);
	seg_hash.insert_aabb(id, g.minx, g.minz, g.maxx, g.maxz);
	nodes[a].segs.push_back(id);
	nodes[b].segs.push_back(id);
	version++;
	if (log) log->network_changed = true;
	return id;
}

void RoadNetwork::remove_segment(int s) {
	RoadSegment &g = segs[s];
	if (!g.alive) return;
	mark_segment(s);
	player_sides -= int((g.flags & SEG_ZONED_LEFT) != 0) + int((g.flags & SEG_ZONED_RIGHT) != 0);
	seg_hash.remove_aabb(s, g.minx, g.minz, g.maxx, g.maxz);
	for (int n : { g.a, g.b }) {
		auto &l = nodes[n].segs;
		l.erase(std::remove(l.begin(), l.end(), s), l.end());
	}
	g.alive = false;
	free_segs.push_back(s);
	version++;
	if (log) log->network_changed = true;
}

void RoadNetwork::remove_node_if_orphan(int n) {
	RoadNode &nd = nodes[n];
	if (!nd.alive || !nd.segs.empty()) return;
	node_hash.remove_aabb(n, float(nd.x), float(nd.z), float(nd.x), float(nd.z));
	nd.alive = false;
	free_nodes.push_back(n);
	version++;
}

template <typename T>
void RoadNetwork::take_id(std::vector<T> &items, std::vector<int> &free_list, int id) {
	if (id >= int(items.size())) {
		for (int k = int(items.size()); k < id; k++) free_list.push_back(k);
		items.resize(size_t(id) + 1);
	}
	free_list.erase(std::remove(free_list.begin(), free_list.end(), id), free_list.end());
}

void RoadNetwork::put_node(int id, const RoadNode &rec) {
	take_id(nodes, free_nodes, id);
	RoadNode &n = nodes[id];
	n = rec;
	n.alive = true;
	n.segs.clear();
	node_hash.insert_aabb(id, float(n.x), float(n.z), float(n.x), float(n.z));
	version++;
}

void RoadNetwork::put_segment(int id, const RoadSegment &rec) {
	take_id(segs, free_segs, id);
	RoadSegment &g = segs[id];
	g = rec;
	g.alive = true;
	player_sides += int((g.flags & SEG_ZONED_LEFT) != 0) + int((g.flags & SEG_ZONED_RIGHT) != 0);
	g.table = arc_table(g.q, ROAD_ARC_STEPS);
	g.len = g.table.back();
	update_bounds(id);
	seg_hash.insert_aabb(id, g.minx, g.minz, g.maxx, g.maxz);
	nodes[g.a].segs.push_back(id);
	nodes[g.b].segs.push_back(id);
	mark_segment(id);
	version++;
	if (log) log->network_changed = true;
}

void RoadNetwork::set_profile(int s, int profile) {
	RoadSegment &g = segs[s];
	mark_segment(s);
	seg_hash.remove_aabb(s, g.minx, g.minz, g.maxx, g.maxz);
	g.profile = profile;
	update_bounds(s);
	seg_hash.insert_aabb(s, g.minx, g.minz, g.maxx, g.maxz);
	mark_segment(s);
	version++;
	if (log) log->network_changed = true;
}

void RoadNetwork::set_zone(int s, int side, int zone) {
	(side == 0 ? segs[s].zl : segs[s].zr) = uint8_t(zone);
	mark_segment(s);
}

static int player_sides_of(uint16_t flags) {
	return int((flags & SEG_ZONED_LEFT) != 0) + int((flags & SEG_ZONED_RIGHT) != 0);
}

void RoadNetwork::set_player_zone(int s, int side, bool on) {
	uint16_t &flags = segs[s].flags;
	player_sides -= player_sides_of(flags);
	const uint16_t bit = side == 0 ? SEG_ZONED_LEFT : SEG_ZONED_RIGHT;
	flags = on ? uint16_t(flags | bit) : uint16_t(flags & ~bit);
	player_sides += player_sides_of(flags);
}

int RoadNetwork::find_node_near(double x, double z, double radius) const {
	int best = -1;
	double bd = radius;
	node_hash.query(float(x - radius), float(z - radius), float(x + radius), float(z + radius), [&](int n) {
		double d = std::hypot(nodes[n].x - x, nodes[n].z - z);
		if (nodes[n].alive && d <= bd) { bd = d; best = n; }
	});
	return best;
}

int RoadNetwork::find_segment_near(double x, double z, double radius, double &t_out, double &d_out) const {
	int best = -1;
	d_out = radius;
	for_segments_near(x - radius, z - radius, x + radius, z + radius, [&](int s) {
		double t, d;
		closest_t(segs[s].q, x, z, t, d);
		if (d <= d_out) { d_out = d; t_out = t; best = s; }
	});
	return best;
}

bool RoadNetwork::has_crosswalks(int node) const {
	const RoadNode &n = nodes[size_t(node)];
	if (int(n.segs.size()) < CROSSWALK_ARMS) return false;
	for (int seg : n.segs) {
		const Profile &p = profile_of(seg);
		if (p.ring || p.rail) return false;
	}
	return true;
}

double RoadNetwork::seg_y(int s, double at) const {
	const RoadSegment &g = segs[s];
	double ya = nodes[g.a].y, yb = nodes[g.b].y;
	return g.len > 0 ? ya + (yb - ya) * std::max(0.0, std::min(1.0, at / g.len)) : ya;
}

bool RoadNetwork::bulb_clear(int n) const {
	const RoadNode &nd = nodes[n];
	int s = nd.segs[0];
	const RoadSegment &g = segs[s];
	if (cat->profile(g.profile).rail) return false;
	if (bulb_retreat(arm_of(g.q, cat->profile(g.profile), g.a == n ? 0 : 1), g.len) <= 0) return false;
	double reach = BULB_R + BULB_WALK + BULB_REACH_PAD, pad = reach + BULB_SEARCH_PAD;
	bool clear = true;
	for_segments_near(nd.x - pad, nd.z - pad, nd.x + pad, nd.z + pad, [&](int o) {
		if (!clear || o == s) return;
		double t, d;
		closest_t(segs[o].q, nd.x, nd.z, t, d);
		if (d - cat->profile(segs[o].profile).right < reach) clear = false;
	});
	return clear;
}

void RoadNetwork::refresh_node(int n) {
	RoadNode &nd = nodes[n];
	if (!nd.alive) return;
	size_t k = nd.segs.size();
	nd.bulb = k == 1 && !nd.plain && bulb_clear(n);
	nd.joint = false;
	std::vector<Arm> arms;
	std::vector<double> lens, out;
	std::vector<int> order;
	for (int s : nd.segs) {
		arms.push_back(arm_of(segs[s].q, profile_of(s), segs[s].a == n ? 0 : 1));
		lens.push_back(segs[s].len);
	}
	if (k == 2) nd.joint = arms[0].dx * arms[1].dx + arms[0].dz * arms[1].dz < STRAIGHT_ANGLE_COS;
	junction_retreats(arms, lens, out, order, nd.bulb);
	for (size_t i = 0; i < k; i++) {
		RoadSegment &g = segs[nd.segs[i]];
		(g.a == n ? g.retreat_a : g.retreat_b) = out[i];
		mark_segment(nd.segs[i]);
		// the mouths moved: the lane graph rebuilds this arm and its node on the next traffic step
		if (log) {
			log->net_seg(nd.segs[i]);
			log->net_node(n);
		}
	}
}

void RoadNetwork::mark_segment(int s) {
	if (!log) return;
	const RoadSegment &g = segs[s];
	auto mark = [](std::vector<int64_t> &v, float x0, float z0, float x1, float z1) {
		for (int j = int(std::floor(z0 / CHUNK)); j <= int(std::floor(z1 / CHUNK)); j++)
			for (int i = int(std::floor(x0 / CHUNK)); i <= int(std::floor(x1 / CHUNK)); i++) v.push_back(cell_key(i, j));
	};
	mark(log->road_chunks, g.minx, g.minz, g.maxx, g.maxz);
	mark(log->lot_cells, g.minx - LOT_LOOK_M, g.minz - LOT_LOOK_M, g.maxx + LOT_LOOK_M, g.maxz + LOT_LOOK_M);
}

int RoadNetwork::alive_segments() const {
	int c = 0;
	for (const RoadSegment &g : segs) c += g.alive ? 1 : 0;
	return c;
}

} // namespace city
