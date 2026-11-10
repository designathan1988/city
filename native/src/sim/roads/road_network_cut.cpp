// A cut of the road network (RoadNetwork::cut): the records a reader of a box needs, copied under their own ids while the map
// is held for a moment, so a mesh job builds from the copy with the lock let go (sim/state/city_cut.h).
#include "sim/roads/road_network.h"

#include <algorithm>
#include <vector>

namespace city {

namespace {
constexpr int CUT_RINGS = 2; // how many times the arms of the taken records' nodes are taken too (road_network.h cut)

// A segment's solved heights as a reader takes them (RoadGrade::at, has): its stations, not the ground the solve read under it.
GradeProfile heights_of(const GradeProfile &p) {
	GradeProfile out;
	out.valid = p.valid;
	out.structure = p.structure;
	out.manual = p.manual;
	out.walls = p.walls;
	out.ends[0] = p.ends[0];
	out.ends[1] = p.ends[1];
	out.a = p.a;
	out.b = p.b;
	out.len = p.len;
	out.step = p.step;
	out.half = p.half;
	out.plate_a = p.plate_a;
	out.plate_b = p.plate_b;
	out.h = p.h;
	out.ylo = p.ylo;
	out.yhi = p.yhi;
	return out;
}

// The ids a hash window files, each once, ascending: the order the copy is made in never depends on the hash's own.
std::vector<int> filed_in(const SpatialHash &window) {
	std::vector<int> ids;
	for (const auto &cell : window.cells)
		for (const SpatialHash::Entry &e : cell.second) ids.push_back(e.id);
	std::sort(ids.begin(), ids.end());
	ids.erase(std::unique(ids.begin(), ids.end()), ids.end());
	return ids;
}
} // namespace

RoadNetwork RoadNetwork::cut(float minx, float minz, float maxx, float maxz) const {
	RoadNetwork c;
	c.cat = cat;
	c.version = version;
	c.nodes = IdTable<RoadNode>::cut(nodes.size());
	c.segs = IdTable<RoadSegment>::cut(segs.size());
	c.runs_ = IdTable<StreetRun>::cut(runs_.size());
	c.grade.prof = IdTable<GradeProfile>::cut(grade.prof.size());
	c.grade.node = IdTable<GradeNode>::cut(grade.node.size());
	c.seg_hash = seg_hash.window(minx, minz, maxx, maxz);
	c.node_hash = node_hash.window(minx, minz, maxx, maxz);
	std::vector<int> fresh_segs, fresh_nodes; // taken, their nodes' arms not yet
	const auto take_node = [&](int n) {
		if (c.nodes.put(size_t(n), nodes[size_t(n)])) fresh_nodes.push_back(n);
	};
	const auto take_seg = [&](int s) {
		if (!c.segs.put(size_t(s), segs[size_t(s)])) return;
		if (size_t(s) < runs_.size()) c.runs_.put(size_t(s), runs_[size_t(s)]);
		if (grade.has(s)) c.grade.prof.put(size_t(s), heights_of(grade.prof[size_t(s)]));
		fresh_segs.push_back(s);
	};
	for (int s : filed_in(c.seg_hash)) take_seg(s);
	for (int n : filed_in(c.node_hash)) take_node(n);
	for (int ring = 0; ring < CUT_RINGS; ring++) {
		std::vector<int> from_segs, from_nodes;
		from_segs.swap(fresh_segs);
		from_nodes.swap(fresh_nodes);
		for (int s : from_segs) {
			take_node(segs[size_t(s)].a);
			take_node(segs[size_t(s)].b);
		}
		from_nodes.insert(from_nodes.end(), fresh_nodes.begin(), fresh_nodes.end());
		fresh_nodes.clear();
		for (int n : from_nodes)
			for (int arm : nodes[size_t(n)].segs) take_seg(arm);
	}
	for (int s : fresh_segs) { // the last ring's arms stand between their nodes
		take_node(segs[size_t(s)].a);
		take_node(segs[size_t(s)].b);
	}
	return c;
}

} // namespace city
