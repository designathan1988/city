// Where the street furniture stands: the one owner of every lamp, signal post, hydrant, bin, bench and street tree,
// read by the walkers (they keep off the footprints, sim/traffic/ped_furniture.cpp) and by the render (meshes, wires,
// each lit lamp's pool of light and street trees). The rules are Brazilian standards (sidewalk zones of ABNT NBR 9050, traffic
// signals of CONTRAN's MBST, distribution spans, urban tree planting, urban hydrants), each cited with its values in
// data/catalog/street_furniture.json. A place is in its segment's frame: arc v from node a and u across the profile
// (+u on the left of a -> b).
#pragma once

#include "sim/roads/parking_layout.h"
#include "sim/roads/road_network.h"

#include <cstdint>
#include <vector>

namespace city {

constexpr uint8_t ROW_HEAD = 1, ROW_TAIL = 2; // FurnitureSpot::row_ends: the main row's first pole (at node a), its last (at node b)
// A segment's furniture level (SEG_FURNITURE_MASK): 0 the full kit, 1 basic, 2 none.
constexpr int FURNITURE_LEVEL_FULL = 0, FURNITURE_LEVEL_NONE = 2;
inline int furniture_level(const RoadSegment &g) { return (g.flags & SEG_FURNITURE_MASK) >> SEG_FURNITURE_SHIFT; }

struct FurnitureSpot {
	FurnitureKind kind = FurnitureKind::LAMP;
	int seg = -1; // the segment it stands on
	int node = -1, approach = -1; // a signal post: the junction, and the arm whose arriving traffic it shows
	int side = -1; // the sidewalk it stands on: 0 the right one (edges.front()), 1 the left one, -1 none
	int row = 0; // a lamp: 0 the main row (its poles are wired), 1 the row on the other side
	int station = -1; // a lamp: its place along the row (a cut pole breaks the wires there)
	// A pole of the main row: the station of the pole before it that its wires come from (-1: the row starts here), the row's one owner of
	// its spans (a station the layout could not stand is bridged; one the player cut, a blow knocked out or a buried stretch hides takes
	// its wires with it); and whether it is the row's first or last pole, strung on across its node where the street goes on.
	int strung = -1;
	uint8_t row_ends = 0; // ROW_HEAD, ROW_TAIL
	int corner = -1; // a junction's corner pole (street_lines.h): its corner of `node`, listed on the corner's lower arm
	bool lit = true; // a lamp: this pole carries a light (lamps.pole_lit_every)
	int item = -1; // what the player placed: its record (sim/street/street_works.h)
	double v = 0;
	float u = 0;
	float face_left = 0, face_fwd = 0; // the piece's +z in the segment frame (toward the road or the arriving traffic)
	float half_v = 0, half_u = 0; // half its footprint along and across the segment
};

// A segment's lamps: the line's face (`side`) and its poles' u there, the lighting columns' u on the face across from it when the
// street is lit from both faces (`twin`), and the stations: `stations` poles from `origin`, `spacing` apart.
struct LampRow {
	bool any = false, twin = false;
	// The row's end stations it places itself: not where its line ends on a junction's corner pole or a joint's pole another
	// segment places (street_lines.h line_end_at).
	bool own_first = true, own_last = true;
	bool lone = false; // a block too short for a line (poles.min_gap_m between its end poles): one light, on a column at its middle, no wires
	int side = 0;
	float pole_u = 0, pole_u2 = 0; // pole_u2 on face 1 - side
	// The stations are spaced from the pole each end is carried on (a joint's pole beyond the segment's end, a corner pole), so no span
	// is longer than the spacing; a pole of the row stands no nearer either node than first_v and last_v, past each mouth's crosswalk
	// and the place of its signal post (street_lines.h line_end_setback).
	double origin = 0, spacing = 1, first_v = 0, last_v = 0;
	int stations = 0;
};
LampRow lamp_row(const RoadNetwork &net, int sid);
// Whether a lamp's arm and light reach over the street from the sidewalk it stands on (its +z across toward the road, as lamp_row
// faces its rows): false for one turned over the footway's back and the lots. A piece on no sidewalk faces its street.
bool faces_road(const FurnitureSpot &s);
// The ground a lit lamp lights at night, where it stands (data lamps.pool_*, as the Roadcraft 4.0 lays its pools; the render draws one a
// lamp, render/props/lamp_pools.h): a lighting column's (`column`) reaches `across` the street either side of its middle (pool_share of
// segment `sid`'s carriageway plus pool_margin_m, at least pool_least_m) and `along` it (pool_along_m), its middle `out` from the column
// toward the road (pool_back_m short of `across`, at least the lens's reach_m); a utility pole's lamp lays a round pool of pool_least_m
// under its head (`out` the lens's reach). `sid` -1: no street under it (a column's pool is then round too).
struct LampPool {
	float across = 0, along = 0, out = 0;
};
LampPool lamp_pool(const RoadNetwork &net, int sid, bool column);
// The share of a road (0 at a, 1 at b) that is not buried, by its ends' authored heights (Roadcraft 4.0 powerLine.ts openStretch): false when none is.
bool open_stretch(const RoadNetwork &net, const RoadSegment &g, float &from, float &to);

// The side has a sidewalk; the u `from_kerb` metres in from its kerb.
bool sidewalk_on(const Profile &p, int side);
float sidewalk_u(const Profile &p, int side, float from_kerb);
// How far in from the kerb's face the middle of a kerbside piece of kind `k` stands: its face kerb_clear_m from the kerb.
float kerbside_reach(const RoadNetwork &net, FurnitureKind k);
// How far from the kerb a plate's post stands on segment `seg`: the plate clears the road by more in a curve.
float plate_kerb(const RoadNetwork &net, int seg);
// Where a piece of kind `k` stands on `side`, its face kerb_clear_m from the kerb; false when it would leave no free width.
bool kerbside_place(const RoadNetwork &net, int seg, const Profile &p, int side, FurnitureKind k, float &u);

constexpr uint32_t furniture_bit(FurnitureKind k) { return 1u << uint32_t(k); }
constexpr uint32_t FURNITURE_ALL = (1u << FURNITURE_KINDS) - 1u;
constexpr uint32_t LIT_FURNITURE = furniture_bit(FurnitureKind::LAMP) | furniture_bit(FurnitureKind::COLUMN); // what lights a street at night

// A plate a rule stands on a segment (a junction's Stop or Yield, a speed plate: sim/traffic/street_signs.cpp says which): the arc
// and the sidewalk its rule wants it at, and the node whose plate it is. Its post is a piece of the segment like any other: it
// stands there or, with a piece in the way, at the first free place on from it, away from that node.
struct PlateWanted {
	double v = 0;
	int side = 0, node = -1;
};

// The pieces standing on segment `sid` whose kind is in `kinds`: the signal posts of its signalled ends (signalled[0]
// at a, [1] at b; the junction control is sim/traffic's) that stand on it, then by its furniture level the lamps,
// hydrants, bins, benches and trees, in that order; what comes after the lamps keeps off them and they read none of it, so it is
// placed only when one of its kinds is asked (a reader of the lamps alone pays for them alone). A hydrant, bin, bench or tree that would leave a walker no way past
// the pieces beside it is not placed (traffic.json ped_body_m and ped_margin_m: the walkers' own sizes). `gates`: the
// lots' gates on each side as arc spans (sim/buildings/lot_access.h street_gates), or null; every piece but a signal post
// stands clear of them. `plates`: the plates the rules stand on the segment, or null; each stands after the lamps, as a
// FurnitureKind::SIGN whose `station` is its place in that list and whose `node` is its junction, and what comes after keeps off it.
void segment_furniture(const RoadNetwork &net, int sid, const bool signalled[2], const std::vector<ArcSpan> *gates,
		const std::vector<PlateWanted> *plates, uint32_t kinds, std::vector<FurnitureSpot> &out);
// The signal post for the traffic arriving at signalled node `n` on arm `approach`: across the junction, on the arm it
// goes straight on into, at the right of that traffic; on its own corner when it has no straight continuation. False
// when the arm has no lanes into `n`.
bool signal_post(const RoadNetwork &net, int n, int approach, FurnitureSpot &out);
// The square of hydrant coverage holding (x, z): hydrants.reach_m / √2 wide, so any point of it is within reach of any
// other. Its hydrant stands at the corner of its node nearest its centre, so a node added in it, removed from it or
// with other arms can move that hydrant to another segment of the square.
void hydrant_square(const RoadNetwork &net, double x, double z, double &x0, double &z0, double &x1, double &z1);

} // namespace city
