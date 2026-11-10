// Road graph (Roadcraft src/roads/network.ts): nodes and quadratic-Bézier segments, each with a profile and the
// zone painted on each side. A segment's height along it is its solved profile (sim/roads/road_grade.h, Roadcraft 4.0
// world/elevation.ts): every edit tells the grade what changed. Node shape (arm setbacks, dead-end bulbs, straight
// joints) is refreshed here; every change marks its chunks in the ChangeLog.
#pragma once

#include "core/grid.h"
#include "core/id_table.h"
#include "core/slot_ids.h"
#include "sim/catalog/catalog.h"
#include "sim/roads/geometry.h"
#include "sim/roads/junction.h"
#include "sim/roads/road_grade.h"
#include "sim/roads/segment_flags.h"
#include "sim/state/change_log.h"

#include <algorithm>
#include <functional>
#include <vector>

namespace city {

// ray_deck: a ray is marched over a deck in steps of this, horizontal metres, then the crossing bisected this many times.
constexpr double DECK_RAY_STEP_M = 0.5;
constexpr int DECK_RAY_REFINE = 16;
constexpr double RAY_PARALLEL_EPS = 1e-12; // a ray this near parallel to a slab never crosses it
constexpr double RAY_RUN_EPS = 1e-9; // a ray whose ground run is shorter is vertical
constexpr int8_t AUTO_SIGNAL = 1, AUTO_ALL_STOP = 2;
// A crossing on a road between junctions (Roadcraft editor/streetObjects.ts commitPedestrianCrossing): a node of two arms with a crosswalk over one of them.
enum NodeCrossing : uint8_t { CROSSING_NONE, CROSSING_ZEBRA, CROSSING_SIGNAL };
// A U-turn through the median (Roadcraft commitUturn), for the traffic arriving by the node's arm of lower id or of higher.
enum NodeUturn : uint8_t { UTURN_NONE, UTURN_FROM_LOW, UTURN_FROM_HIGH };
constexpr int LINK_BANNED = -1;
// What the player set a junction to (Roadcraft 4.0 JunctionControl); the order the Controle tab cycles it in.
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
	double retreat_a = 0, retreat_b = 0, want_a = 0, want_b = 0; // the mouths, metres before each node (clamp_segment_trims); what each asks
	float minx = 0, minz = 0, maxx = 0, maxz = 0;
};

// The street a segment is drawn along (its texture's V, a bridge's piers and towers): the run of segments through joints
// (two arms in line), each laid on from the one before (its a node the other's b). `start`: the arc from the run's first
// segment to this one's a node; `total`: the run's length. In a ring every segment starts where the ring comes back to it.
// `face`: the face (0 right, 1 left of a -> b) the segment's main lamp row takes when both faces have a sidewalk
// (sim/roads/street_furniture.h row_face): its chain's through every node of two arms (RoadNetwork::going_on), whichever way each
// piece was drawn, the east or south face of the chain's lowest-numbered segment. Settled with the runs, so no reader of a segment
// walks its street.
struct StreetRun {
	int run = -1; // the run's first segment (in a ring, the lowest id)
	double start = 0, total = 0;
	int8_t face = 0;
};

constexpr int ROAD_ARC_STEPS = 48; // chords a road's arc table holds
constexpr int STREET_JOINT_ARMS = 2; // a node where a street goes on: only its two segments meet there
constexpr int CROSSWALK_ARMS = 3; // crosswalks are painted at nodes of this many arms or more
constexpr float ROAD_HASH_CELL = 64.f; // the cells roads are filed in, for the queries near a point

class RoadNetwork {
public:
	IdTable<RoadNode> nodes;
	IdTable<RoadSegment> segs;
	SlotIds free_nodes, free_segs;
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
	// A dead end ends in a straight cut (true) or may take a bulb: its shape is settled again.
	void set_plain(int n, bool plain);
	// Recomputes the node's arm setbacks, bulb and joint flags (and its segments' mouths). Only what changed is told: every
	// arm and the node when its arms may have changed (`arms_changed`), its bulb or its joint changed, else each arm whose
	// mouth moved. True when anything was told.
	bool refresh_node(int n, bool arms_changed = true);
	// Refreshes every node whose shape inputs changed since the last settle (refresh_node reads a node's arms, its plain flag
	// and, for a dead end, every road within BULB_LOOK_M): the nodes of the changed roads and the nodes near them.
	void settle_shapes();
	bool shapes_pending() const { return !shape_nodes_.empty() || !shape_boxes_.empty(); }
	// Every node refreshed (a load, a new city): nothing is left to settle.
	void refresh_all();
	void mark_segment(int s); // its road chunks and lot cells
	// The chunks segment s's drawing reaches: its box, a terrain cell round it (a cut cell is drawn by the chunk of its middle:
	// render/roads/ground_seam.h) and a turning bulb's disc round either node, whether it has one now or had one.
	template <typename F>
	void for_drawn_chunks(int s, F &&fn) const {
		const RoadSegment &g = segs[size_t(s)];
		const double cell = cat->tune.world_height_spacing_m, disc = bulb_reach(cat->tune) + cell;
		std::vector<int64_t> keys;
		for_chunks(g.minx - cell, g.minz - cell, g.maxx + cell, g.maxz + cell, [&](int64_t k) { keys.push_back(k); });
		for (const int n : { g.a, g.b }) {
			const RoadNode &nd = nodes[size_t(n)];
			for_chunks(nd.x - disc, nd.z - disc, nd.x + disc, nd.z + disc, [&](int64_t k) { keys.push_back(k); });
		}
		std::sort(keys.begin(), keys.end());
		keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
		for (const int64_t k : keys) fn(k);
	}
	// What changed on the roads since the last take_edits, for what is derived from them later in the settle (the streets'
	// name plates): the segments whose curve, casing, mouths or heights changed, and the boxes of the roads that came, went
	// or changed casing.
	struct ChangedBox {
		double minx, minz, maxx, maxz;
	};
	void take_edits(std::vector<int> &segs, std::vector<ChangedBox> &boxes) {
		segs.swap(edited_segs_);
		boxes.swap(edited_boxes_);
		edited_segs_.clear();
		edited_boxes_.clear();
	}
	// The street of segment s (StreetRun), as the last settle left it.
	const StreetRun &street_run(int s) const { return runs_[size_t(s)]; }
	// The piece going on from segment `from` through node `n` when only the two meet there (alive, not a loop on one node), else -1:
	// the chain a street's lamp row and power line follow.
	int going_on(int n, int from) const;
	// What a reader of the box needs of the network, copied under its own ids (sim/state/city_cut.h): every segment and node the
	// hashes file in the cells the box touches (the copy's hashes answer for the box alone), then twice the arms of the nodes of
	// what was taken and their far nodes (a node's shape, its crossings and a joint's shared pole read every arm; an arm stands
	// between its nodes), each segment with its street and the heights of its solved profile.
	RoadNetwork cut(float minx, float minz, float maxx, float maxz) const;
	int alive_segments() const { return live_segs_; }
	int alive_nodes() const { return live_nodes_; }
	int zoned_sides() const { return zoned_sides_; } // sides of live roads with a zone

	template <typename F>
	void for_segments_near(double minx, double minz, double maxx, double maxz, F &&fn) const {
		seg_hash.query_unique(float(minx), float(minz), float(maxx), float(maxz), [&](int s) {
			if (!segs[s].alive) return;
			const RoadSegment &g = segs[s];
			if (g.maxx < minx || g.minx > maxx || g.maxz < minz || g.minz > maxz) return;
			fn(s);
		});
	}

private:
	// What changed a node's shape since the last settle_shapes: nodes whose own arms or plain flag changed, and the boxes
	// of the roads that came, went or changed casing (a dead end within BULB_LOOK_M of one may gain or lose its bulb).
	std::vector<int> shape_nodes_;
	std::vector<ChangedBox> shape_boxes_;
	std::vector<int> edited_segs_;
	std::vector<ChangedBox> edited_boxes_;
	IdTable<StreetRun> runs_; // by segment
	std::vector<int> run_dirty_; // segments whose run may have changed: an arm of a node whose arms or joint changed
	void settle_runs();
	void settle_faces(const std::vector<int> &dirty); // StreetRun::face of every chain a dirty segment is on
	int live_segs_ = 0, live_nodes_ = 0, zoned_sides_ = 0;

	void update_bounds(int s);
	void shape_changed(int s); // segment s's box and its two nodes
	template <typename T>
	static void take_id(IdTable<T> &items, SlotIds &free_list, int id);
	bool bulb_clear(int n) const;
};

} // namespace city
