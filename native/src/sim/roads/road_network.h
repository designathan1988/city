// Road graph (Roadcraft src/roads/network.ts): nodes and quadratic-Bézier segments, each with a profile and the
// zone painted on each side. Heights are linear between the two nodes by arc length. Node shape (arm setbacks,
// dead-end bulbs, straight joints) is refreshed here; every change marks its chunks in the ChangeLog.
#pragma once

#include "core/grid.h"
#include "sim/catalog/catalog.h"
#include "sim/roads/geometry.h"
#include "sim/state/change_log.h"

#include <vector>

namespace city {

// Segment flags (Roadcraft roads/flags.ts and network.ts).
enum SegFlag : uint16_t {
	SEG_FURNITURE_MASK = 0x06, // bits 1-2: 0 full street furniture, 1 basic, 2 none
	SEG_FREE = 0x80, // built by the generator: worth nothing when demolished
	SEG_RULE_A_MASK = 0x0300, // bits 8-9, 10-11: the plate the player put on its a and b approach (SegRule)
	SEG_RULE_B_MASK = 0x0C00,
};
constexpr int SEG_FURNITURE_SHIFT = 1;
constexpr int SEG_RULE_A_SHIFT = 8, SEG_RULE_B_SHIFT = 10;
enum SegRule : uint16_t { RULE_NONE = 0, RULE_STOP = 1, RULE_YIELD = 2 };

struct RoadNode {
	bool alive = false;
	double x = 0, z = 0, y = 0;
	std::vector<int> segs;
	uint8_t ctl = 0; // junction control (roads/control.ts), 0 = automatic
	bool bulb = false; // dead end with a turning bulb
	bool joint = false; // two arms in line: one street going on
	bool plain = false; // a dead end that never gets a bulb (roundabout approaches waiting to be extended)
};

struct RoadSegment {
	bool alive = false;
	int a = -1, b = -1;
	Quad q;
	int profile = 0;
	uint8_t zl = 0, zr = 0; // zone on the left (+u) and right side
	uint16_t flags = 0;
	std::vector<double> table; // arc length at t = i / ARC_STEPS
	double len = 0;
	double retreat_a = 0, retreat_b = 0; // the mouths: metres before each node
	float minx = 0, minz = 0, maxx = 0, maxz = 0;
};

constexpr int ROAD_ARC_STEPS = 48;
constexpr float ROAD_HASH_CELL = 64.f;

class RoadNetwork {
public:
	std::vector<RoadNode> nodes;
	std::vector<RoadSegment> segs;
	std::vector<int> free_nodes, free_segs;
	SpatialHash seg_hash, node_hash;
	int version = 0; // bumps on every topology change
	const Catalog *cat = nullptr;
	ChangeLog *log = nullptr;

	void init(const Catalog *c, ChangeLog *l);
	void clear();
	const Profile &profile_of(int s) const { return cat->profile(segs[s].profile); }
	int add_node(double x, double z, double y);
	int add_segment(int a, int b, const Quad &q, int profile, int zl, int zr, uint16_t flags);
	void remove_segment(int s);
	void remove_node_if_orphan(int n);
	// Undo / redo / load (Roadcraft table allocAt): the record goes back under the same id.
	void put_node(int id, const RoadNode &rec);
	void put_segment(int id, const RoadSegment &rec);
	void set_profile(int s, int profile);
	void set_zone(int s, int side, int zone);
	int find_node_near(double x, double z, double radius) const;
	int find_segment_near(double x, double z, double radius, double &t_out, double &d_out) const;
	// Height of segment s at arc length `at` from node a.
	double seg_y(int s, double at) const;
	// Recomputes the node's arm setbacks, bulb and joint flags (and its segments' mouths).
	void refresh_node(int n);
	void mark_segment(int s); // its road chunks and lot cells
	int alive_segments() const;

	template <typename F>
	void for_segments_near(double minx, double minz, double maxx, double maxz, F &&fn) const {
		std::vector<int> seen;
		seg_hash.query(float(minx), float(minz), float(maxx), float(maxz), [&](int s) {
			if (!segs[s].alive || std::find(seen.begin(), seen.end(), s) != seen.end()) return;
			const RoadSegment &g = segs[s];
			if (g.maxx < minx || g.minx > maxx || g.maxz < minz || g.minz > maxz) return;
			seen.push_back(s);
			fn(s);
		});
	}

private:
	void update_bounds(int s);
	template <typename T>
	static void take_id(std::vector<T> &items, std::vector<int> &free_list, int id);
	bool bulb_clear(int n) const;
};

} // namespace city
