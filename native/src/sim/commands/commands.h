// Player commands (Roadcraft src/roads/construction/*, features/zones/zoneTool.ts, roads/construction/streetEdits.ts):
// the only code that changes CityState at the player's request. Every command validates first (the same plan the
// preview shows), charges or refunds money, marks the ChangeLog (roads, lots, buildings, terrain) and leaves the
// lane graph to the traffic, which drains the ChangeLog's net lists. Callers hold map_mutex exclusively.
#pragma once

#include "sim/catalog/furniture_types.h"
#include "sim/roads/geometry.h"
#include "sim/street/street_works.h"

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
	std::string note; // strings.csv key of what happened instead of the plain result
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

// --- The Street tab (street_snap.cpp, street_commands.cpp, pole_runs.cpp)
struct ItemSnap {
	bool ok = false;
	std::string reason; // strings.csv key when refused
	double x = 0, z = 0;
};
// Where an item of `kind` put at (x, z) would stand, or why it cannot (Roadcraft landscape.ts snapLandscape).
ItemSnap snap_item(const CityState &s, ItemKind kind, double x, double z, double reach);
// A point of a barrier run on the footway's back, or at the kerb for a guardrail and a railing.
void snap_barrier_point(const CityState &s, BarrierKind kind, double &x, double &z);
// Why a run cannot be built: "" or a strings.csv key.
std::string barrier_problem(const CityState &s, BarrierKind kind, const std::vector<float> &pts);
EditResult place_item(CityState &s, ItemKind kind, SignType sign, const std::string &text, double x, double z, double reach);
// A row of items along the sidewalk from a to b, each where the tool would accept it, every_m apart (a drag).
EditResult place_item_row(CityState &s, ItemKind kind, double ax, double az, double bx, double bz, double every_m);
EditResult build_barrier(CityState &s, BarrierKind kind, const std::vector<float> &pts);
// Takes what stands nearest (x, z) within reach: an item, a pole and its wires, a barrier or a piece of the street kit.
bool remove_street_at(CityState &s, double x, double z, double reach);
// A road going: what stands on its sidewalks goes with it.
void remove_street_kit(CityState &s, int seg);
// What stands on a road being reprofiled, kept by its place along the kerb, and put back where the new sidewalks lie.
struct CarriedPiece {
	int item = -1, pole = -1, side = 0;
	double v = 0;
	float from_kerb = 0;
};
std::vector<CarriedPiece> street_kit_of(const CityState &s, int seg);
void carry_street_kit(CityState &s, int seg, const std::vector<CarriedPiece> &kit);

// What a pole run would build (Roadcraft poles.ts PoleRunPlan), the same plan for the preview and the command.
enum class LampMode : uint8_t { NONE = 0, ALTERNATE, ALL };
struct PlannedPole {
	double x = 0, z = 0;
	int existing = -1; // the pole standing here, which the run reuses
	bool lamp = false;
};
struct PoleRunPlan {
	std::vector<PlannedPole> poles;
	std::string refused; // strings.csv key when nothing would be built
	bool from_free = false, to_free = false;
	double from_x = 0, from_z = 0, to_x = 0, to_z = 0;
};
PoleRunPlan plan_pole_run(const CityState &s, double ax, double az, double bx, double bz, double reach, LampMode lamps);
EditResult build_pole_run(CityState &s, double ax, double az, double bx, double bz, double reach, LampMode lamps);

// Value of one segment (R$): length x price per metre x structure factor (ground).
double segment_value(const CityState &s, int seg);

} // namespace city
