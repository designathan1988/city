// Road graph (Roadcraft src/roads/network.ts): nodes and quadratic-Bézier segments, each with a profile and the
// zone painted on each side. A segment's height along it is its solved profile (sim/roads/road_grade.h, Roadcraft 4.0
// world/elevation.ts): every edit tells the grade what changed. Node shape (arm setbacks, dead-end bulbs, straight
// joints) is refreshed here; every change marks its chunks in the ChangeLog.
#pragma once

#include "core/grid.h"
#include "sim/catalog/catalog.h"
#include "sim/roads/geometry.h"
#include "sim/roads/road_grade.h"
#include "sim/state/change_log.h"

#include <functional>
#include <vector>

namespace city {

// Segment flags (Roadcraft roads/flags.ts and network.ts).
enum SegFlag : uint16_t {
	SEG_FURNITURE_MASK = 0x06, // bits 1-2: 0 full street furniture, 1 basic, 2 none
	SEG_GENERATED = 0x80, // laid by the city generator: its footways carry the generated city's kit (the 4.0 plants no street
	// trees there). Money does not read it: a generated road is worth its price, as in the 4.0 (saves before the economy
	// carry this bit as SEG_FREE, with the same meaning)
	SEG_ZONED_LEFT = 0x100, // the player zoned this side (what the Zones layer shows)
	SEG_ZONED_RIGHT = 0x200,
	SEG_CUT_WALLS = 0x40, // its cutting is held by retaining walls (Roadcraft 4.0 RoadSegment.cutWalls)
	SEG_RUNS_ON_A = 0x08, SEG_RUNS_ON_B = 0x10, // the road goes on past this end: a cut of the laying, no node the player made
	SEG_RULE_A_MASK = 0x0C00, // bits 10-11, 12-13: the plate the player put on its a and b approach (SegRule)
	SEG_RULE_B_MASK = 0x3000,
	SEG_STRUCTURE_MASK = 0xC000, // bits 14-15: how it is built (Structure: ground, elevated, bridge, tunnel)
};
constexpr int SEG_FURNITURE_SHIFT = 1, SEG_STRUCTURE_SHIFT = 14;
// ray_deck: a ray is marched over a deck in steps of this, horizontal metres, then the crossing bisected this many times.
constexpr double DECK_RAY_STEP_M = 0.5;
constexpr int DECK_RAY_REFINE = 16;
constexpr double RAY_PARALLEL_EPS = 1e-12; // a ray this near parallel to a slab never crosses it
constexpr double RAY_RUN_EPS = 1e-9; // a ray whose ground run is shorter is vertical
constexpr Structure structure_of(uint16_t flags) { return Structure((flags & SEG_STRUCTURE_MASK) >> SEG_STRUCTURE_SHIFT); }
constexpr uint16_t with_structure(uint16_t flags, Structure s) {
	return uint16_t((flags & ~SEG_STRUCTURE_MASK) | (uint16_t(s) << SEG_STRUCTURE_SHIFT));
}
constexpr int SEG_RULE_A_SHIFT = 10, SEG_RULE_B_SHIFT = 12;
enum SegRule : uint16_t { RULE_NONE = 0, RULE_STOP = 1, RULE_YIELD = 2 };
constexpr int8_t AUTO_SIGNAL = 1, AUTO_ALL_STOP = 2;
constexpr uint16_t seg_rule_mask(int end) { return end == 0 ? SEG_RULE_A_MASK : SEG_RULE_B_MASK; }
constexpr int seg_rule_shift(int end) { return end == 0 ? SEG_RULE_A_SHIFT : SEG_RULE_B_SHIFT; }
// The flags of a segment laid the other way round: what was on its left is on its right, its a end is its b end.
constexpr uint16_t flags_reversed(uint16_t f) {
	const uint16_t keep = uint16_t(f & ~(SEG_ZONED_LEFT | SEG_ZONED_RIGHT | SEG_RULE_A_MASK | SEG_RULE_B_MASK | SEG_RUNS_ON_A | SEG_RUNS_ON_B));
	const uint16_t sides = uint16_t(((f & SEG_ZONED_LEFT) ? SEG_ZONED_RIGHT : 0) | ((f & SEG_ZONED_RIGHT) ? SEG_ZONED_LEFT : 0));
	const uint16_t rules = uint16_t((((f & SEG_RULE_A_MASK) >> SEG_RULE_A_SHIFT) << SEG_RULE_B_SHIFT) | (((f & SEG_RULE_B_MASK) >> SEG_RULE_B_SHIFT) << SEG_RULE_A_SHIFT));
	const uint16_t runs = uint16_t(((f & SEG_RUNS_ON_A) ? SEG_RUNS_ON_B : 0) | ((f & SEG_RUNS_ON_B) ? SEG_RUNS_ON_A : 0));
	return uint16_t(keep | sides | rules | runs);
}

// What the player set a junction to (Roadcraft 4.0 JunctionControl); the order the Controle tab cycles it in.
// A crossing on a road between junctions (Roadcraft editor/streetObjects.ts commitPedestrianCrossing): a node of two arms with a crosswalk over one of them.
enum NodeCrossing : uint8_t { CROSSING_NONE, CROSSING_ZEBRA, CROSSING_SIGNAL };
// A U-turn through the median (Roadcraft commitUturn), for the traffic arriving by the node's arm of lower id or of higher.
enum NodeUturn : uint8_t { UTURN_NONE, UTURN_FROM_LOW, UTURN_FROM_HIGH };
constexpr int LINK_BANNED = -1;
enum NodeControl : uint8_t { CTL_AUTO, CTL_SIGNAL, CTL_PRIORITY, CTL_STOP, CTL_YIELD, CTL_NONE, CTL_MINI, CTL_COUNT };
// What the player sets on a junction's signal (the plan itself is the traffic's: junction_control.cpp). Fixed time: every
// stage its own green in order, every cycle, `offset` seconds after the common clock's zero (CONTRAN MBST vol. V).
struct SignalSettings {
	bool fixed = false;
	float offset = 0;
	std::vector<float> greens; // per stage; a stage's minimum green still stands under them
	bool operator==(const SignalSettings &o) const { return fixed == o.fixed && offset == o.offset && greens == o.greens; }
};

// A connection through a junction the player made by hand (Roadcraft world/roads/connectors.ts): the arriving road and lane, the leaving ones.
struct LaneLink {
	int from = -1, from_lane = 0, to = -1, to_lane = 0; // both lanes LINK_BANNED: not a lane's link but the movement from arm to arm banned
	bool operator==(const LaneLink &o) const { return from == o.from && from_lane == o.from_lane && to == o.to && to_lane == o.to_lane; }
};

struct RoadNode {
	bool alive = false;
	double x = 0, z = 0;
	double y = 0; // the height it was solved to (road_grade.h): derived, never saved
	float lift = 0; // the height the player gave it over the grade line, metres (Roadcraft 4.0 RoadNode.heightOffset)
	std::vector<int> segs;
	uint8_t ctl = CTL_AUTO; // NodeControl
	int8_t auto_signal = -1; // what the flows chose on automatic (sim/traffic/flow_control.h): -1 nothing yet, 0 none, 1 a signal, AUTO_ALL_STOP a Stop on every leg; derived, never saved
	SignalSettings signal;
	bool bulb = false; // dead end with a turning bulb
	bool joint = false; // two arms in line: one street going on
	bool plain = false; // a dead end ends in a straight cut: every player road's, until set_road_end asks for a bulb
	uint8_t crossing = CROSSING_NONE; // NodeCrossing
	uint8_t uturn = UTURN_NONE; // NodeUturn
	std::vector<LaneLink> links; // an arriving lane named here goes only where these say (save_links.h)
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
constexpr int CROSSWALK_ARMS = 3; // crosswalks are painted at nodes of this many arms or more
constexpr float ROAD_HASH_CELL = 64.f;

class RoadNetwork {
public:
	std::vector<RoadNode> nodes;
	std::vector<RoadSegment> segs;
	std::vector<int> free_nodes, free_segs;
	SpatialHash seg_hash, node_hash;
	int version = 0; // bumps on every topology change
	int player_sides = 0; // road sides the player zoned
	const Catalog *cat = nullptr;
	ChangeLog *log = nullptr;
	RoadGrade grade; // the solved heights (sim/commands/road_grade_sync.cpp solves what each edit told it)

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
	void set_player_zone(int seg, int side, bool on);
	int find_node_near(double x, double z, double radius) const;
	// The nearest whose height over the grade line `level` takes.
	int find_node_near(double x, double z, double radius, const std::function<bool(float)> &level) const;
	float lift_on(int seg, double t) const; // the height over the grade line at share t of a segment
	int find_segment_near(double x, double z, double radius, double &t_out, double &d_out) const;
	// The pieces of the road `seg` is one of, in order along it: a road is laid in pieces (lay_pieces) and to the player it is one thing.
	std::vector<int> pieces_of(int seg) const;
	// Height of segment s at arc length `at` from node a: its solved profile (linear between its nodes until solved).
	double seg_y(int s, double at) const;
	// Where a ray (origin o, direction d, up to max_t) first comes down onto a road deck: the surface the player sees there
	// (the 4.0's worldAtScreen and sceneHeightAt). A road in a tunnel is under the ground, never met. False: no deck before max_t.
	bool ray_deck(double ox, double oy, double oz, double dx, double dy, double dz, double max_t, double &t_hit) const;
	// The height the player gave node n over its grade line, and how segment s is built: both solved again.
	void set_lift(int n, float lift);
	void set_structure(int s, Structure st);
	// Segment s stands at other heights: its road chunks, its lanes and its street furniture are made again.
	void mark_heights(int s);
	// A node whose mouths get crosswalks, stop lines and dropped kerbs: three or more arms, no roundabout, no rails.
	bool has_crosswalks(int node) const;
	bool mid_block(int node) const; // a crossing between junctions
	int crossing_arm(int node) const; // which of its two arms carries the crosswalk: a rule, no id to outlive an edit
	bool crosswalk_on(int node, int seg) const; // a crosswalk painted over `seg` at that node
	int uturn_arm(int node) const; // the arm whose traffic may turn back at a node of two arms, -1 none
	bool movement_banned(int node, int from, int to) const; // nobody goes from this arm to that one (a link with no lanes: LaneLink)
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
