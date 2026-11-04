// Player commands (Roadcraft src/roads/construction/*, features/zones/zoneTool.ts, roads/construction/streetEdits.ts):
// the only code that changes CityState at the player's request. Every command validates first (the same plan the
// preview shows), charges or refunds money, marks the ChangeLog (roads, lots, buildings, terrain) and leaves the
// lane graph to the traffic, which drains the ChangeLog's net lists. Callers hold map_mutex exclusively.
#pragma once

#include "sim/roads/geometry.h"

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
};

// What a command did. `error` is a strings.csv key (empty when ok); `money` > 0 charged, < 0 refunded.
struct EditResult {
	bool ok = false;
	std::string error;
	double money = 0;
	int segments = 0, buildings_removed = 0, zoned_sides = 0;
};

// Snaps (x, z) to the nearest node within `radius`, else to a segment within 0.8 radius (its end node when the hit is
// within road_snap_end_m of an end), else returns the free point.
Anchor snap_point(const CityState &s, double x, double z, double radius);
// Validates a road from a to b (control point used when has_control) with the given profile. Changes nothing.
RoadPlan plan_road(const CityState &s, const Anchor &a, bool has_control, double cx, double cz, const Anchor &b, int profile);
// Builds a planned road: splits crossed segments, cuts pieces longer than road_max_seg_m, removes buildings in the way,
// conforms the terrain and charges the price. Refuses without enough money.
EditResult build_road(CityState &s, const Anchor &a, bool has_control, double cx, double cz, const Anchor &b, int profile);
// Bulldoze at (x, z) (Roadcraft tool `bulldoze`): the building under the point, else the nearest segment within
// road_pick_m (loose nodes go with it; refund road_refund_share of its value unless it was free).
EditResult demolish_at(CityState &s, double x, double z);
// Zone brush (Roadcraft zoneTool.dab): paints `zone` on every road side whose frontage passes within `radius`;
// buildings of another base zone facing a repainted side are knocked down.
EditResult paint_zone(CityState &s, double x, double z, double radius, int zone);

// --- Road drawer tabs (road_tabs.cpp, roundabout.cpp)
// Splits the segment under (x, z) where clicked (at least edit_split_min_m from its ends).
EditResult split_road_at(CityState &s, double x, double z);
// Puts `profile` on the segment under (x, z), or on its whole street; charges the difference or refunds a share.
EditResult change_profile_at(CityState &s, double x, double z, int profile, bool whole_street);
// The street of a segment: through the nodes, the straightest continuation (edit_street_cos) with the same profile.
std::vector<int> street_chain(const CityState &s, int seg);
// Why a roundabout cannot go there ("" = it can); `cut` allows cutting the roads inside the circle.
std::string roundabout_refusal(const CityState &s, double cx, double cz, double radius, bool cut);
EditResult build_roundabout(CityState &s, double cx, double cz, double radius, bool cut);

// --- Landscape and Build drawers (land_commands.cpp)
// One dab of the Relief brush (mode = BrushMode) held for dt seconds; level and landforms use the stroke's first height.
EditResult terrain_dab(CityState &s, double x, double z, double radius, double strength, double dt, int mode);
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

// Value of one segment (R$): length x price per metre x structure factor (ground).
double segment_value(const CityState &s, int seg);

} // namespace city
