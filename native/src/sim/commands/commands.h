// Player commands (Roadcraft src/roads/construction/*, features/zones/zoneTool.ts, roads/construction/streetEdits.ts):
// the only code that changes CityState at the player's request. Every command validates first (the same plan the
// preview shows), charges or refunds money, marks the ChangeLog (roads, lots, buildings, terrain) and leaves the
// lane graph to the traffic, which drains the ChangeLog's net lists. Callers hold map_mutex exclusively.
#pragma once

#include "sim/lots/lot_book.h"
#include "sim/roads/road_network.h"
#include "sim/catalog/furniture_types.h"
#include "sim/nature/nature_state.h"
#include "sim/roads/geometry.h"
#include "sim/street/street_works.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace city {

struct CityState;
struct Building;

// Where a click lands on the network (Roadcraft construction/snap.ts): a node, a point on a segment or free ground.
struct Anchor {
	double x = 0, z = 0;
	int node = -1, seg = -1;
	double t = 0;
	std::optional<float> lift; // a free end's height over the grade line, when it is not the one being drawn at
};

// A crossing of the new road with an existing segment at grade: t on the new road, the segment and t on it.
struct Crossing {
	double t_new = 0;
	int seg = -1;
	double t_old = 0;
};

// The validated road (Roadcraft construction/plan.ts RoadPlan). `reason` is a strings.csv key when refused.
struct RoadPlan {
	bool ok = false;
	std::string reason;
	Quad q;
	double len = 0;
	double price = 0; // what building it costs (R$)
	std::vector<Crossing> crossings;
	float lift_a = 0, lift_b = 0; // its ends' height over the grade line, the end held to the steepest grade drawn
	std::vector<float> lifts; // of a path: the height at each joint of its pieces
	float lift_at(double t) const { return lift_a + (lift_b - lift_a) * float(t); }
};

// What a command did. `error` is a strings.csv key (empty when ok); `money` > 0 charged, < 0 refunded.
struct EditResult {
	bool ok = false;
	std::string error;
	double money = 0;
	int segments = 0, buildings_removed = 0, zoned_sides = 0, placed = -1;
	std::string note; // strings.csv key of what happened instead of the plain result
};

// What produced a snapped end, for the guide the tool shows (Roadcraft editor/snap.ts SnapResult).
enum class SnapGuide : uint8_t { NONE, NETWORK, CONTINUE, PERPENDICULAR, ORTHOGONAL, ANGLE };

// The road tool's snapping: one switch for all of it, then each kind. px = metres per screen pixel at the pointer.
struct SnapOptions {
	bool on = true, angles = true, grid = true;
	double px = 1;
	double cell = 0, offset = 0; // the grid drawn on the ground, with the road in its cells (0: the 1 m grid of road_grid_m)
};

struct SnappedEnd {
	double x = 0, z = 0;
	SnapGuide guide = SnapGuide::NONE;
	double angle = 0, length = 0; // radians from +x towards +z, metres from the start
};

// How a road follows the gesture (Roadcraft roadTool.ts alignment).
enum class RoadAlignment : uint8_t { STRAIGHT, CURVE, FREE };

struct StrokePoint {
	double x = 0, z = 0;
};

// A gesture as the tool hands it in: its ends, the bend chosen by a click (has_control), the pointer's samples, and the
// bend under which a dragged curve stays straight (metres: a few screen pixels).
struct RoadStroke {
	RoadAlignment alignment = RoadAlignment::STRAIGHT;
	StrokePoint start, end, control;
	bool has_control = false;
	std::vector<StrokePoint> samples;
	int profile = 0;
	double flat_m = 0;
};

// --- Where a road lands and what it is laid as (road_snap.cpp, road_path.cpp)
// What is under the pointer: a node, a point on a road (anywhere on its surface) or open ground.
// `lift`: only what stands at that height over the grade line is taken (Roadcraft editor/snap.ts anchorForHeight); none takes any.
Anchor find_anchor(const CityState &s, double x, double z, double px, std::optional<float> lift = {});
// Where a road starts: a junction or road keeps its exact point, open ground goes on the grid.
Anchor snap_road_start(const CityState &s, const Anchor &a, const SnapOptions &o);
// The end the pointer asks for from `start`, after the network, the angles and the grid.
SnappedEnd snap_road_end(const CityState &s, const Anchor &start, double x, double z, const SnapOptions &o);
// The curves a gesture is laid as: one, or the chain of a freehand stroke; none are tighter than the road allows.
std::vector<Quad> road_pieces(const CityState &s, const RoadStroke &k);
// plan_road over the pieces: the sum of their prices and lengths, the first refusal.
RoadPlan plan_path(const CityState &s, const std::vector<Quad> &pieces, const Anchor &a, const Anchor &b, int profile);
// Builds every piece, in one undo entry when the caller runs it in one: a refused piece refuses the whole road.
EditResult build_path(CityState &s, const std::vector<Quad> &pieces, const Anchor &a, const Anchor &b, int profile);

// Snaps (x, z) to the nearest node within `radius`, else to a segment within 0.8 radius (its end node when the hit is
// within road_snap_end_m of an end), else returns the free point.
Anchor snap_point(const CityState &s, double x, double z, double radius);
// Validates a road from a to b (control point used when has_control) with the given profile. Changes nothing.
RoadPlan plan_road(const CityState &s, const Anchor &a, bool has_control, double cx, double cz, const Anchor &b, int profile);
// Builds a planned road: splits crossed segments, cuts pieces longer than road_max_seg_m, removes buildings in the way,
// conforms the terrain and charges the price. Refuses without enough money.
EditResult build_road(CityState &s, const Anchor &a, bool has_control, double cx, double cz, const Anchor &b, int profile);
// A connection of a junction toggled by hand (connectors.ts toggleLaneLink): an arriving lane first taken by hand starts from what the traffic
// built (`built`); a lane left with none is refused.
EditResult toggle_lane_link(CityState &s, int node, const LaneLink &link, const std::vector<LaneLink> &built);

// Zones > Lots (lot_commands.cpp): the lots the player draws. Refusals name the hint.
EditResult add_lot_rect(CityState &s, const LotPoint &a, const LotPoint &b, double angle);
EditResult add_lot_polygon(CityState &s, const std::vector<LotPoint> &points);
// A road edit cuts the lots it crosses, in its own step of undo (cutLotsByRoads): each is replaced by the pieces left on land, of its
// zone where they are big enough, and what stood on it goes.
void cut_lots_by_roads(CityState &s, double minx, double minz, double maxx, double maxz);
EditResult move_lot_corner(CityState &s, const LotPoint &from, const LotPoint &to);
EditResult curve_lot_side(CityState &s, const LotPoint &a, const LotPoint &b, const LotPoint &through);
EditResult set_lot_front(CityState &s, int id, int side);
EditResult split_lot(CityState &s, int id, const LotCut &cut);
EditResult join_lots(CityState &s, int a, int b);
EditResult delete_lot(CityState &s, int id);
EditResult zone_lot(CityState &s, int id, int zone);
// A grid of streets laid in one gesture (road_blocks.cpp): `cols` by `rows` blocks of `block_m`, turned by `angle`, centred on (x, z).
struct BlockGrid {
	int cols = 1, rows = 1;
	double block_m = 100, angle = 0;
};
struct BlockLine {
	StrokePoint a, b;
};
std::vector<BlockLine> block_lines(double x, double z, const BlockGrid &g);
EditResult lay_blocks(CityState &s, double x, double z, const BlockGrid &g, int profile);
// Bulldoze at (x, z) (Roadcraft tool `bulldoze`): the building under the point, else the nearest segment within
// road_pick_m (loose nodes go with it; refund road_refund_share of its value unless it was free).
EditResult demolish_at(CityState &s, double x, double z);
// A junction's signal: adaptive or fixed time, its greens and offset (clamped to the longest cycle); a green wave sets the
// fixed plans of the signals along the main road through `node` in one step.
EditResult set_signal(CityState &s, int node, const SignalSettings &to);
EditResult green_wave(CityState &s, int node);
// Zone brush (Roadcraft zoneTool.dab): paints `zone` on every road side whose frontage passes within `radius`;
// buildings of another base zone facing a repainted side are knocked down.
EditResult paint_zone(CityState &s, double x, double z, double radius, int zone);

// --- Road drawer tabs (road_tabs.cpp, roundabout.cpp)
// Splits the segment under (x, z) where clicked (at least edit_split_min_m from its ends).
EditResult split_road_at(CityState &s, double x, double z);
// The one-way runs the other way (road_copies.cpp); a road copied alongside; the map's paint standard, every road drawn again.
EditResult reverse_road(CityState &s, int seg);
EditResult duplicate_road(CityState &s, int seg);
EditResult set_marking_style(CityState &s, int style);
// Puts `profile` on the segment under (x, z), or on its whole street; charges the difference or refunds a share.
EditResult change_profile_at(CityState &s, double x, double z, int profile, bool whole_street);
// How a dead end finishes (Roadcraft doc.ts setNodeEnd): a straight cut, or a turning bulb when there is room for one.
EditResult set_road_end(CityState &s, int node, bool bulb);
// A road as it was drawn, its pieces being cuts of one curve; how far its curve stands from the line between its ends, metres, and where along that line.
Quad road_line(const RoadNetwork &net, int seg);
struct RoadBend {
	double height = 0, at = 0.5;
};
RoadBend road_bend_of(const Quad &line);
// What the Inspector does (road_bend.cpp): a node's height over the grade line, kept within the steepest grade to the nodes round it; the whole road bent by
// `height` at `at` of its length (0: straight); and the two roads in line at a node made one.
EditResult set_node_lift(CityState &s, int node, float metres);
EditResult set_road_bend(CityState &s, int seg, double height, double at, double &mid_x, double &mid_z); // mid: where the road's middle stands after it
EditResult join_roads(CityState &s, int node);
// A crossing where the road passes (x, z), between junctions, or where it passes one (road_crossing.cpp); taken away by its node.
EditResult add_crossing(CityState &s, double x, double z, int kind);
EditResult remove_crossing(CityState &s, int node);
// A U-turn through the median where the road passes (x, z), for the traffic heading for its end b or for its end a.
EditResult add_uturn(CityState &s, double x, double z, bool toward_b);
// The Controle tab: what a junction of three or more arms runs by (NodeControl).
EditResult set_node_control(CityState &s, int node, int ctl);
// The Move tab: the node goes to (x, z) and its roads are laid again to it, each keeping its shape, zones and direction;
// landing on a node joins them, landing on a road splits it. Only the difference in value is charged.
EditResult move_node(CityState &s, int node, double x, double z);
// The curve of a road with its end `at_a` taken to (x, z), its shape kept (Roadcraft world/doc.ts fitCurve).
Quad moved_quad(const Quad &q, bool at_a, double x, double z);
// Why a roundabout cannot go there ("" = it can); `cut` allows cutting the roads inside the circle.
std::string roundabout_refusal(const CityState &s, double cx, double cz, double radius, bool cut);
EditResult build_roundabout(CityState &s, double cx, double cz, double radius, bool cut);

// --- Landscape and Build drawers (land_commands.cpp)
// One dab of the Relief brush (mode = BrushMode) held for dt seconds; level and landforms use the stroke's first height.
// `hardness` 0..1 squeezes the falloff of raise and lower into the rim. A landform lays its rock under the dab; a river
// fills its bed with water and takes the trees standing there.
EditResult terrain_dab(CityState &s, double x, double z, double radius, double strength, double dt, int mode, double hardness);
// The relief back to the land the map started with, the rivers drawn gone with it.
EditResult clear_relief(CityState &s);
// The water the river brush laid goes (clear_relief; also undone).
void unflood(CityState &s);
// The lot a rectangle with corners a and b makes (aligned to the nearest road, facing it); "" or the refusal key.
std::string lot_frame(const CityState &s, double ax, double az, double bx, double bz, Building &b);
// A building on the lot rectangle with corners a and b (aligned to the nearest road, facing it); fn = BuildingFn.
EditResult place_building(CityState &s, double ax, double az, double bx, double bz, int fn, int zone);
// The function and floors of a building (Build drawer, Mass tab).
EditResult edit_building(CityState &s, int id, int fn, int floors);

// --- Demolition (strike_commands.cpp, shoot_commands.cpp)
// The bomb: a blow of force `strength` (1..50) bursting at (x, z) at `height` (the ground, or the roof it landed on).
// Changes the buildings, the ground, the roads, the furniture, the vehicles and the walkers it reaches (one undo step).
EditResult strike_at(CityState &s, double x, double z, double strength, double height);

// The pistol: one round along the line of sight from (ox, oy, oz) in direction (dx, dy, dz) (any length). What it hit, and
// whether the car it hit took enough rounds to burst (the caller then strikes at `burst_*`: a blow of burst_force).
struct ShotResult {
	enum Hit : uint8_t { NONE = 0, WALKER, VEHICLE } hit = NONE;
	bool killed = false, burst = false;
	int id = -1;
	float burst_x = 0, burst_y = 0, burst_z = 0;
};
ShotResult shoot_at(CityState &s, double ox, double oy, double oz, double dx, double dy, double dz);

// Bulldoze everything in the box between two corners: the buildings whose centre is in it, the rubble, the road segments
// whose middle is in it (their refund added up). One undo step.
EditResult demolish_box(CityState &s, double ax, double az, double bx, double bz);

// --- Landscape drawer (nature_commands.cpp): the sky, the painted ground, the planted trees, the map's biome
constexpr int TREE_PLANT = 0, TREE_ONE = 1, TREE_CUT = 2; // what a dab of the tree brush does
EditResult set_weather(CityState &s, const Weather &next);
EditResult add_cloud(CityState &s, const PlacedCloud &c);
EditResult edit_cloud(CityState &s, int id, const PlacedCloud &c);
EditResult remove_cloud(CityState &s, int id);
// Up to `count` clouds over the sky, each like `like` varied by `variation` (0..1).
EditResult scatter_sky(CityState &s, int count, const PlacedCloud &like, float variation);
EditResult clear_clouds(CityState &s);
EditResult fog_dab(CityState &s, const FogDab &dab);
EditResult clear_fog(CityState &s);
EditResult paint_dab(CityState &s, const PaintDab &dab);
EditResult clear_gullies(CityState &s);
EditResult tree_dab(CityState &s, double x, double z, double radius, int mode, int kind, const TreeBrush &brush);
EditResult clear_trees(CityState &s);
// One dab of the element brush: its density per ElementDef::area_m2, size and spacing in metres, the rest shares.
struct ElementBrush {
	float density = 0, size = 0, variation = 0, spacing = 0, strength = 0, intensity = 0;
};
EditResult element_dab(CityState &s, double x, double z, double radius, int kind, const ElementBrush &b, bool erase);
EditResult clear_elements(CityState &s);
// The map's ecosystem: a Biome, or Biome::NONE.
EditResult set_biome(CityState &s, int region);
// Takes the planted trees in a circle away and leaves a clearing there (the woods' trees go too); false when full.
bool cut_trees_at(CityState &s, float x, float z, float radius);

} // namespace city
