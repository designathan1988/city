// Road graph storage, spatial queries and the bookkeeping of every edit (Roadcraft roads/network.ts).
#include "sim/roads/road_network.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr float LOT_LOOK_M = 160.f; // lots look this far for their streets (sim/citygen/lot_gen.h LOT_MARGIN + setbacks)

int zoned_of(const RoadSegment &g) { return int(g.zl != 0) + int(g.zr != 0); }
} // namespace

void RoadNetwork::init(const Catalog *c, ChangeLog *l) {
	cat = c;
	log = l;
	clear();
}

void RoadNetwork::clear() {
	live_segs_ = live_nodes_ = zoned_sides_ = 0;
	shape_nodes_.clear();
	shape_boxes_.clear();
	edited_segs_.clear();
	edited_boxes_.clear();
	runs_.clear();
	run_dirty_.clear();
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
	id = free_nodes.next(int(nodes.size()));
	nodes.resize(size_t(free_nodes.claim(id, int(nodes.size()))));
	RoadNode &n = nodes[id];
	n = RoadNode();
	n.alive = true;
	n.x = x; n.z = z; n.y = y;
	node_hash.insert_aabb(id, float(x), float(z), float(x), float(z));
	grade.node_changed(id);
	shape_nodes_.push_back(id);
	live_nodes_++;
	version++;
	return id;
}

void RoadNetwork::shape_changed(int s) {
	const RoadSegment &g = segs[size_t(s)];
	shape_boxes_.push_back({ g.minx, g.minz, g.maxx, g.maxz });
	edited_boxes_.push_back(shape_boxes_.back());
	edited_segs_.push_back(s);
	shape_nodes_.push_back(g.a);
	shape_nodes_.push_back(g.b);
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
	id = free_segs.next(int(segs.size()));
	segs.resize(size_t(free_segs.claim(id, int(segs.size()))));
	runs_.resize(segs.size()); // its street is made at the settle (settle_runs)
	RoadSegment &g = segs[id];
	g = RoadSegment();
	g.alive = true;
	g.a = a; g.b = b; g.q = q; g.profile = profile;
	g.zl = uint8_t(zl); g.zr = uint8_t(zr); g.flags = flags;
	g.table = arc_table(q, ROAD_ARC_STEPS);
	g.len = g.table.back();
	update_bounds(id);
	seg_hash.insert_aabb(id, g.minx, g.minz, g.maxx, g.maxz);
	nodes[a].segs.push_back(id);
	nodes[b].segs.push_back(id);
	grade.seg_changed(id);
	shape_changed(id);
	live_segs_++;
	zoned_sides_ += zoned_of(g);
	version++;
	if (log) log->network_changed = true;
	return id;
}

void RoadNetwork::remove_segment(int s) {
	RoadSegment &g = segs[s];
	if (!g.alive) return;
	mark_segment(s);
	shape_changed(s);
	live_segs_--;
	zoned_sides_ -= zoned_of(g);
	seg_hash.remove_aabb(s, g.minx, g.minz, g.maxx, g.maxz);
	for (int n : { g.a, g.b }) {
		auto &l = nodes[n].segs;
		l.erase(std::remove(l.begin(), l.end(), s), l.end());
	}
	g.alive = false;
	free_segs.release(s);
	grade.seg_changed(s);
	for (int n : { g.a, g.b }) grade.node_changed(n); // its nodes' other roads settle again
	version++;
	if (log) log->network_changed = true;
}

void RoadNetwork::remove_node_if_orphan(int n) {
	RoadNode &nd = nodes[n];
	if (!nd.alive || !nd.segs.empty()) return;
	node_hash.remove_aabb(n, float(nd.x), float(nd.z), float(nd.x), float(nd.z));
	nd.alive = false;
	free_nodes.release(n);
	live_nodes_--;
	version++;
}

template <typename T>
void RoadNetwork::take_id(std::vector<T> &items, SlotIds &free_list, int id) {
	items.resize(size_t(free_list.claim(id, int(items.size()))));
}

void RoadNetwork::put_node(int id, const RoadNode &rec) {
	take_id(nodes, free_nodes, id);
	RoadNode &n = nodes[id];
	live_nodes_ += n.alive ? 0 : 1;
	n = rec;
	n.alive = true;
	n.segs.clear();
	node_hash.insert_aabb(id, float(n.x), float(n.z), float(n.x), float(n.z));
	grade.node_changed(id);
	shape_nodes_.push_back(id);
	version++;
}

void RoadNetwork::put_segment(int id, const RoadSegment &rec) {
	take_id(segs, free_segs, id);
	runs_.resize(segs.size()); // its street is made at the settle (settle_runs)
	RoadSegment &g = segs[id];
	live_segs_ += g.alive ? 0 : 1;
	zoned_sides_ -= g.alive ? zoned_of(g) : 0;
	g = rec;
	g.alive = true;
	g.table = arc_table(g.q, ROAD_ARC_STEPS);
	g.len = g.table.back();
	update_bounds(id);
	seg_hash.insert_aabb(id, g.minx, g.minz, g.maxx, g.maxz);
	nodes[g.a].segs.push_back(id);
	nodes[g.b].segs.push_back(id);
	zoned_sides_ += zoned_of(g);
	mark_segment(id);
	grade.seg_changed(id);
	shape_changed(id);
	version++;
	if (log) log->network_changed = true;
}

void RoadNetwork::set_profile(int s, int profile) {
	RoadSegment &g = segs[s];
	mark_segment(s);
	shape_changed(s);
	seg_hash.remove_aabb(s, g.minx, g.minz, g.maxx, g.maxz);
	g.profile = profile;
	update_bounds(s);
	seg_hash.insert_aabb(s, g.minx, g.minz, g.maxx, g.maxz);
	shape_changed(s);
	mark_segment(s);
	grade.seg_changed(s); // its casing, so the ground read under it and its plates
	version++;
	if (log) log->network_changed = true;
}

void RoadNetwork::set_plain(int n, bool plain) {
	if (nodes[size_t(n)].plain == plain) return;
	nodes[size_t(n)].plain = plain;
	shape_nodes_.push_back(n);
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
	edited_segs_.push_back(s);
	if (!log) return;
	const RoadSegment &g = segs[size_t(s)];
	for_chunks(g.minx, g.minz, g.maxx, g.maxz, [&](int64_t key) { log->road_chunks.push_back(key); });
	log->net_seg(s);
	log->net_node(g.a);
	log->net_node(g.b);
	log->furniture_segs.push_back(s);
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

void RoadNetwork::mark_segment(int s) {
	if (!log) return;
	const RoadSegment &g = segs[s];
	auto mark = [](std::vector<int64_t> &v, float x0, float z0, float x1, float z1) {
		for_chunks(x0, z0, x1, z1, [&](int64_t key) { v.push_back(key); });
	};
	mark(log->road_chunks, g.minx, g.minz, g.maxx, g.maxz);
	mark(log->lot_cells, g.minx - LOT_LOOK_M, g.minz - LOT_LOOK_M, g.maxx + LOT_LOOK_M, g.maxz + LOT_LOOK_M);
	mark(log->street_cells, g.minx - LOT_LOOK_M, g.minz - LOT_LOOK_M, g.maxx + LOT_LOOK_M, g.maxz + LOT_LOOK_M);
}

} // namespace city
