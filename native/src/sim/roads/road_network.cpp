// Road graph storage, spatial queries and node shape (Roadcraft roads/network.ts, roads/junction.ts bulbAt).
#include "sim/roads/road_network.h"
#include "sim/citygen/lot_gen.h"

#include "sim/roads/junction.h"
#include "sim/street/street_placement.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr double STRAIGHT_ANGLE_COS = -0.9986; // two arms this close to opposite are one street going on
constexpr double BULB_REACH_PAD = 3, BULB_SEARCH_PAD = 30;
constexpr float LOT_LOOK_M = 160.f; // lots look this far for their streets (sim/citygen/lot_gen.h LOT_MARGIN + setbacks)
constexpr float NODE_REFRESH_PAD_M = 60.f; // a changed road can change a nearby dead end's bulb
} // namespace

void RoadNetwork::init(const Catalog *c, ChangeLog *l, StreetPlacement *placement) {
	cat = c;
	log = l;
	placement_ = placement;
	clear();
}

void RoadNetwork::clear() {
	ground_regions.reset();
	geometry_revision = next_generation();
	chains_.clear();
	chain_revisions_.clear();
	if (placement_) placement_->clear();
	alive_segments_ = alive_nodes_ = zoned_sides_ = 0;
	player_sides = 0;
	nodes.clear();
	segs.clear();
	free_nodes.clear();
	free_segs.clear();
	seg_hash.clear();
	node_hash.clear();
	seg_hash.cell = node_hash.cell = ROAD_HASH_CELL;
	grade = RoadGrade();
	version++;
}

int RoadNetwork::add_node(double x, double z, double y) {
	int id;
	if (!free_nodes.empty()) { id = free_nodes.back(); free_nodes.pop_back(); }
	else { id = int(nodes.size()); nodes.emplace_back(); }
	RoadNode &n = nodes[id];
	n = RoadNode();
	n.generation = next_generation();
	n.alive = true;
	alive_nodes_++;
	n.x = x; n.z = z; n.y = y;
	chain_changed(id);
	node_hash.insert_aabb(id, float(x), float(z), float(x), float(z));
	grade.node_changed(id);
	mark_shapes(x, z, x, z);
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
	alive_segments_++;
	g = RoadSegment();
	g.generation = next_generation();
	g.alive = true;
	g.a = a; g.b = b; g.q = q; g.profile = profile;
	g.zl = uint8_t(zl); g.zr = uint8_t(zr); g.flags = flags;
	zoned_sides_ += int(g.zl != 0) + int(g.zr != 0);
	player_sides += int((flags & SEG_ZONED_LEFT) != 0) + int((flags & SEG_ZONED_RIGHT) != 0);
	g.table = arc_table(q, ROAD_ARC_STEPS);
	g.lot_table = arc_table(q, LOT_ARC_STEPS);
	g.len = g.table.back();
	update_bounds(id);
	seg_hash.insert_aabb(id, g.minx, g.minz, g.maxx, g.maxz);
	nodes[a].segs.push_back(id);
	nodes[b].segs.push_back(id);
	grade.seg_changed(id);
	mark_shapes(g.minx, g.minz, g.maxx, g.maxz);
	mark_segment(id);
	version++;
	if (log) log->network_changed = true;
	return id;
}

void RoadNetwork::remove_segment(int s) {
	RoadSegment &g = segs[s];
	if (!g.alive) return;
	alive_segments_--;
	zoned_sides_ -= int(g.zl != 0) + int(g.zr != 0);
	mark_shapes(g.minx, g.minz, g.maxx, g.maxz);
	mark_segment(s);
	player_sides -= int((g.flags & SEG_ZONED_LEFT) != 0) + int((g.flags & SEG_ZONED_RIGHT) != 0);
	seg_hash.remove_aabb(s, g.minx, g.minz, g.maxx, g.maxz);
	for (int n : { g.a, g.b }) {
		auto &l = nodes[n].segs;
		l.erase(std::remove(l.begin(), l.end(), s), l.end());
	}
	g.alive = false;
	free_segs.push_back(s);
	grade.seg_changed(s);
	for (int n : { g.a, g.b }) grade.node_changed(n); // its nodes' other roads settle again
	version++;
	if (log) log->network_changed = true;
}

void RoadNetwork::remove_node_if_orphan(int n) {
	RoadNode &nd = nodes[n];
	if (!nd.alive || !nd.segs.empty()) return;
	mark_shapes(nd.x, nd.z, nd.x, nd.z);
	node_hash.remove_aabb(n, float(nd.x), float(nd.z), float(nd.x), float(nd.z));
	nd.alive = false;
	alive_nodes_--;
	chain_changed(n);
	free_nodes.push_back(n);
	version++;
}

template <typename T>
void RoadNetwork::take_id(std::vector<T> &items, FreeSlots &free_list, int id) {
	if (id >= int(items.size())) {
		for (int k = int(items.size()); k < id; k++) free_list.push_back(k);
		items.resize(size_t(id) + 1);
	}
	free_list.erase(id);
}

void RoadNetwork::put_node(int id, const RoadNode &rec) {
	take_id(nodes, free_nodes, id);
	RoadNode &n = nodes[id];
	const uint64_t live = n.alive ? n.generation : 0;
	const uint64_t generation = live ? live : next_generation();
	if (!n.alive) alive_nodes_++;
	n = rec; n.generation = generation;
	n.alive = true;
	n.segs.clear();
	chain_changed(id);
	node_hash.insert_aabb(id, float(n.x), float(n.z), float(n.x), float(n.z));
	grade.node_changed(id);
	mark_shapes(n.x, n.z, n.x, n.z);
	version++;
}

void RoadNetwork::put_segment(int id, const RoadSegment &rec) {
	take_id(segs, free_segs, id);
	RoadSegment &g = segs[id];
	const uint64_t live = g.alive ? g.generation : 0;
	const uint64_t generation = live ? live : next_generation();
	if (!g.alive) alive_segments_++;
	else zoned_sides_ -= int(g.zl != 0) + int(g.zr != 0);
	g = rec; g.generation = generation;
	zoned_sides_ += int(g.zl != 0) + int(g.zr != 0);
	g.alive = true;
	player_sides += int((g.flags & SEG_ZONED_LEFT) != 0) + int((g.flags & SEG_ZONED_RIGHT) != 0);
	g.table = arc_table(g.q, ROAD_ARC_STEPS);
	g.lot_table = arc_table(g.q, LOT_ARC_STEPS);
	g.len = g.table.back();
	update_bounds(id);
	seg_hash.insert_aabb(id, g.minx, g.minz, g.maxx, g.maxz);
	nodes[g.a].segs.push_back(id);
	nodes[g.b].segs.push_back(id);
	mark_segment(id);
	grade.seg_changed(id);
	mark_shapes(g.minx, g.minz, g.maxx, g.maxz);
	version++;
	if (log) log->network_changed = true;
}

void RoadNetwork::set_profile(int s, int profile) {
	RoadSegment &g = segs[s];
	if (g.profile == profile) return;
	mark_shapes(g.minx, g.minz, g.maxx, g.maxz);
	mark_segment(s);
	seg_hash.remove_aabb(s, g.minx, g.minz, g.maxx, g.maxz);
	g.profile = profile;
	update_bounds(s);
	mark_shapes(g.minx, g.minz, g.maxx, g.maxz);
	seg_hash.insert_aabb(s, g.minx, g.minz, g.maxx, g.maxz);
	mark_segment(s);
	grade.seg_changed(s); // its casing, so the ground read under it and its plates
	version++;
	if (log) log->network_changed = true;
}

void RoadNetwork::set_zone(int s, int side, int zone) {
	uint8_t &value = side == 0 ? segs[s].zl : segs[s].zr;
	const uint8_t next = uint8_t(zone);
	if (value == next) return;
	if (segs[s].alive) zoned_sides_ += int(next != 0) - int(value != 0);
	value = next;
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
	return find_node_near(x, z, radius, [](float) { return true; });
}

int RoadNetwork::find_node_near(double x, double z, double radius, const std::function<bool(float)> &level) const {
	int best = -1;
	double bd = radius;
	node_hash.query(float(x - radius), float(z - radius), float(x + radius), float(z + radius), [&](int n) {
		double d = std::hypot(nodes[n].x - x, nodes[n].z - z);
		if (nodes[n].alive && d <= bd && level(nodes[n].lift)) { bd = d; best = n; }
	});
	return best;
}

float RoadNetwork::lift_on(int seg, double t) const {
	const RoadSegment &g = segs[size_t(seg)];
	return nodes[size_t(g.a)].lift + (nodes[size_t(g.b)].lift - nodes[size_t(g.a)].lift) * float(t);
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

double RoadNetwork::seg_y(int s, double at) const {
	if (grade.has(s)) return grade.at(s, at);
	const RoadSegment &g = segs[s];
	double ya = nodes[g.a].y, yb = nodes[g.b].y;
	return g.len > 0 ? ya + (yb - ya) * std::max(0.0, std::min(1.0, at / g.len)) : ya;
}

void RoadNetwork::set_lift(int n, float lift) {
	if (nodes[size_t(n)].lift == lift) return;
	nodes[size_t(n)].lift = lift;
	grade.node_changed(n);
	for (int s : nodes[size_t(n)].segs) grade.seg_changed(s); // an authored alignment, or no more
}

void RoadNetwork::set_structure(int s, Structure st) {
	RoadSegment &g = segs[size_t(s)];
	if (structure_of(g.flags) == st) return;
	g.flags = with_structure(g.flags, st);
	grade.seg_changed(s);
	mark_segment(s);
}

void RoadNetwork::mark_heights(int s) {
	geometry_revision = next_generation();
	if (!log) return;
	const RoadSegment &g = segs[size_t(s)];
	for (int j = int(std::floor(g.minz / CHUNK)); j <= int(std::floor(g.maxz / CHUNK)); j++)
		for (int i = int(std::floor(g.minx / CHUNK)); i <= int(std::floor(g.maxx / CHUNK)); i++) log->road_chunks.push_back(cell_key(i, j));
	log->net_seg(s);
	log->net_node(g.a);
	log->net_node(g.b);
	log->furniture_segs.push_back(s);
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

std::vector<int> RoadNetwork::pieces_of(int seg) const {
	// The piece past `node`, -1 where the road ends there.
	const auto beyond = [&](int at, int node) {
		const RoadNode &nd = nodes[size_t(node)];
		if (nd.segs.size() != 2) return -1;
		const int next = nd.segs[0] == at ? nd.segs[1] : nd.segs[0];
		const auto runs = [&](int s) { return (segs[size_t(s)].flags & (segs[size_t(s)].a == node ? SEG_RUNS_ON_A : SEG_RUNS_ON_B)) != 0; };
		return runs(at) && runs(next) ? next : -1;
	};
	const auto walk = [&](int node, std::vector<int> &out) {
		for (int at = seg, next = beyond(at, node); next >= 0 && next != seg; next = beyond(at, node)) {
			out.push_back(next);
			node = segs[size_t(next)].a == node ? segs[size_t(next)].b : segs[size_t(next)].a;
			at = next;
		}
	};
	std::vector<int> back, ahead;
	walk(segs[size_t(seg)].a, back);
	walk(segs[size_t(seg)].b, ahead);
	std::vector<int> out(back.rbegin(), back.rend());
	out.push_back(seg);
	out.insert(out.end(), ahead.begin(), ahead.end());
	return out;
}

void RoadNetwork::mark_shapes(double x0, double z0, double x1, double z1) {
	if (!log) return;
	node_hash.query(float(x0) - NODE_REFRESH_PAD_M, float(z0) - NODE_REFRESH_PAD_M, float(x1) + NODE_REFRESH_PAD_M, float(z1) + NODE_REFRESH_PAD_M,
			[&](int n) { if (nodes[size_t(n)].alive) log->shape_nodes.push_back(n); });
}

void RoadNetwork::refresh_node(int n) {
	RoadNode &nd = nodes[n];
	if (!nd.alive) return;
	size_t k = nd.segs.size();
	const bool was_joint = nd.joint;
	const bool was_bulb = nd.bulb;
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
	const bool shape_changed = nd.joint != was_joint || nd.bulb != was_bulb;
	if (shape_changed) grade.node_changed(n); // joint/bulb rules both change a plate even when its mouth stays
	for (size_t i = 0; i < k; i++) {
		RoadSegment &g = segs[nd.segs[i]];
		double &mouth = g.a == n ? g.retreat_a : g.retreat_b;
		const bool moved = mouth != out[i];
		if (moved) grade.seg_changed(nd.segs[i]); // its plate there moved
		mouth = out[i];
		if (!moved && !shape_changed) continue;
		mark_segment(nd.segs[i]);
		// the mouths moved: the lane graph rebuilds this arm and its node on the next traffic step
		if (log) {
			log->net_seg(nd.segs[i]);
			log->net_node(n);
		}
	}
}

void RoadNetwork::chain_changed(int node) {
	if (node < 0) return;
	if (size_t(node) >= chain_revisions_.size()) chain_revisions_.resize(size_t(node) + 1, 0);
	chain_revisions_[size_t(node)] = next_generation();
}

void RoadNetwork::mark_segment(int s) {
	geometry_revision = next_generation();
	const RoadSegment &g = segs[s];
	ground_regions.touch(g.minx, g.minz, g.maxx, g.maxz);
	chain_changed(g.a); chain_changed(g.b);
	invalidate_furniture(s);
	if (!log) return;
	auto mark = [](std::vector<int64_t> &v, float x0, float z0, float x1, float z1) {
		for (int j = int(std::floor(z0 / CHUNK)); j <= int(std::floor(z1 / CHUNK)); j++)
			for (int i = int(std::floor(x0 / CHUNK)); i <= int(std::floor(x1 / CHUNK)); i++) v.push_back(cell_key(i, j));
	};
	mark(log->road_chunks, g.minx, g.minz, g.maxx, g.maxz);
	mark(log->lot_cells, g.minx - LOT_LOOK_M, g.minz - LOT_LOOK_M, g.maxx + LOT_LOOK_M, g.maxz + LOT_LOOK_M);
}

void RoadNetwork::invalidate_furniture(int s) {
	if (placement_) placement_->invalidate(s);
}

int RoadNetwork::alive_segments() const {
	return alive_segments_;
}

} // namespace city
