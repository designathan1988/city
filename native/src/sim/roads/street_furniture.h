// Where the street furniture stands: the one owner of every lamp, signal post, hydrant, bin, bench and street tree,
// read by the walkers (they keep off the footprints, sim/traffic/ped_furniture.cpp) and by the render (meshes, wires,
// night light pools and street trees). The rules are Brazilian standards (sidewalk zones of ABNT NBR 9050, traffic
// signals of CONTRAN's MBST, distribution spans, urban tree planting, urban hydrants), each cited with its values in
// data/catalog/street_furniture.json. A place is in its segment's frame: arc v from node a and u across the profile
// (+u on the left of a -> b).
#pragma once

#include "sim/roads/road_network.h"

#include <cstdint>
#include <vector>

namespace city {

struct FurnitureSpot {
	FurnitureKind kind = FurnitureKind::LAMP;
	int seg = -1; // the segment it stands on
	int node = -1, approach = -1; // a signal post: the junction, and the arm whose arriving traffic it shows
	int side = -1; // the sidewalk it stands on: 0 the right one (edges.front()), 1 the left one, -1 none
	int row = 0; // a lamp: 0 the main row (its poles are wired), 1 the row on the other side
	int station = -1; // a lamp: its place along the row (a cut pole breaks the wires there)
	bool lit = true; // a lamp: this pole carries a light (lamps.pole_lit_every)
	int item = -1; // what the player placed: its record (sim/street/street_works.h)
	double v = 0;
	float u = 0;
	float face_left = 0, face_fwd = 0; // the piece's +z in the segment frame (toward the road or the arriving traffic)
	float half_v = 0, half_u = 0; // half its footprint along and across the segment
};

// A segment's lamps: the poles' u on the main sidewalk (and on the other, when the street is lit from both sides), the
// heads `reach` toward the carriageway, and the stations: `stations` poles from `origin`, `spacing` apart.
struct LampRow {
	bool any = false, twin = false;
	int side = 0;
	float pole_u = 0, head_u = 0, pole_u2 = 0, head_u2 = 0;
	float centre = 0, across = 1; // the night pools' middle line and how far they reach from it, from the profile's width
	double origin = 0, spacing = 1, span = 1; // span: lamps.span_m, the period the night pools repeat with
	int stations = 0;
	// A point's place in the night pools: du across, in shares of `across` and signed so a quad interpolates it, dv the arc
	// from the first station on a period of `span` (the stations' own spacing stretched to it).
	void pool(float u, double at, float &du, float &dv) const {
		du = (u - centre) / across;
		dv = float((at - origin) * span / spacing);
	}
};
LampRow lamp_row(const RoadNetwork &net, int sid);
// The share of a road (0 at a, 1 at b) that is not buried, by its ends' authored heights (Roadcraft 4.0 powerLine.ts openStretch): false when none is.
bool open_stretch(const RoadNetwork &net, const RoadSegment &g, float &from, float &to);

// The side has a sidewalk; the u `from_kerb` metres in from its kerb.
bool sidewalk_on(const Profile &p, int side);
float sidewalk_u(const Profile &p, int side, float from_kerb);
// How far from the kerb a plate's post stands on segment `seg`: the plate clears the road by more in a curve.
float plate_kerb(const RoadNetwork &net, int seg);
// Where a piece of kind `k` stands on `side`, its face kerb_clear_m from the kerb; false when it would leave no free width.
bool kerbside_place(const RoadNetwork &net, int seg, const Profile &p, int side, FurnitureKind k, float &u);

constexpr uint32_t furniture_bit(FurnitureKind k) { return 1u << uint32_t(k); }
constexpr uint32_t FURNITURE_ALL = (1u << FURNITURE_KINDS) - 1u;

// The pieces standing on segment `sid` whose kind is in `kinds`: the signal posts of its signalled ends (signalled[0]
// at a, [1] at b; the junction control is sim/traffic's) that stand on it, then by its furniture level the lamps,
// hydrants, bins, benches and trees, in that order. A hydrant, bin, bench or tree that would leave a walker no way past
// the pieces beside it is not placed (traffic.json ped_body_m and ped_margin_m: the walkers' own sizes).
void segment_furniture(const RoadNetwork &net, int sid, const bool signalled[2], uint32_t kinds, std::vector<FurnitureSpot> &out);
// The signal post for the traffic arriving at signalled node `n` on arm `approach`: across the junction, on the arm it
// goes straight on into, at the right of that traffic; on its own corner when it has no straight continuation. False
// when the arm has no lanes into `n`.
bool signal_post(const RoadNetwork &net, int n, int approach, FurnitureSpot &out);
// The square of hydrant coverage holding (x, z): hydrants.reach_m / √2 wide, so any point of it is within reach of any
// other. Its hydrant stands at the corner of its node nearest its centre, so a node added in it, removed from it or
// with other arms can move that hydrant to another segment of the square.
void hydrant_square(const RoadNetwork &net, double x, double z, double &x0, double &z0, double &x1, double &z1);

} // namespace city
