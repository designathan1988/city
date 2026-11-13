// The stripes of every way a walker walks (SUMO MSPModel_Striping): a sidewalk, a line round a
// corner or a zebra is a frame (ahead, across) with a room for the walker's centre; every body near it (other walkers
// in any mode, the street furniture's footprints) is remapped into that frame, as SUMO maps the persons of the other
// paths of a walking area into the current one. Internal to sim/traffic (ped_crowd.cpp builds the frames).
#pragma once

#include "sim/traffic/traffic_system.h"

namespace city {
namespace traffic {

// A walker's way at its position: ahead and the direction `u` grows across; where its centre is across now, where it
// may be (lo..hi), where it would walk with nobody about (keep) and past which, to its left, the room is left to
// those coming the other way (reserve_from).
struct WayFrame {
	V2 at, fwd, left;
	float u = 0, lo = 0, hi = 0, keep = 0, reserve_from = BIG;
	bool zebra = false; // over a zebra (SUMO's crossing), else a sidewalk or a line round a corner (its walking area)
	bool narrow = false; // the way itself holds one stripe only (its own width, not the room a line opens with)
	bool queue = false; // before a kerb: those ahead wait for the same leave, so it stands behind them however long
};

// A body in a walker's frame: its extent ahead and across, how near a centre may come across it and ahead of it
// (reach: two body radii for a walker, a body radius and the clearance for furniture) and how it moves.
struct Body {
	float along_lo = 0, along_hi = 0, lat_lo = 0, lat_hi = 0, reach = 0;
	bool furniture = false, oncoming = false;
	bool swath = false; // a disc of a vehicle off the lanes' swath (ped_swaths.cpp): a furniture body
	bool under = false; // a swath's disc the walker stands on: no obstacle ahead (it walks on out of it), and it steps away from it
};
constexpr int MAX_BODIES = 64; // bodies a walker looks at in one step (the nearest first is not needed: all are near)

// The bodies one walker sees this step, in its frame; only those that can stand in its room (ped_seen.cpp fills it).
struct Seen {
	const WayFrame &f;
	Body *out;
	int n = 0;
	float look, touch, reach; // ped_look_m, two body radii, a body radius
	float lie_most = 0; // the longest half of somebody lying: how far behind a walker their body may still reach beside it

	// Full: every later body would be turned away, so the looking stops there (what it saw is the same).
	bool full() const { return n >= MAX_BODIES; }
	// A walker ahead, or beside it (within two radii along, a little behind too): the gap is kept to the first, no step aside
	// is taken into the second (ped_stripes.cpp side_limits). One farther behind keeps its own gap.
	void walker(float along, float lat, bool oncoming) {
		if (along <= -touch || along > look || lat < f.lo - touch || lat > f.hi + touch || n >= MAX_BODIES) return;
		Body &b = out[n++];
		b = Body();
		b.along_lo = b.along_hi = along;
		b.lat_lo = b.lat_hi = lat;
		b.reach = touch;
		b.oncoming = oncoming;
	}
	void furniture(const Body &b) {
		if (b.along_hi < -reach || b.along_lo > look || b.lat_hi < f.lo - reach || b.lat_lo > f.hi + reach || n >= MAX_BODIES) return;
		out[n++] = b;
	}
};

// ped_seen.cpp: what a walker sees. The walkers on its own sidewalk, by arc: those ahead as far as it looks, and those beside it
// (Seen::walker), a little behind too; somebody lying there as the footprint of their body (a piece of furniture: gone round, never
// squeezed through).
void see_sidewalk(const std::vector<CrowdEntry> &list, size_t k, Seen &sn);
// The walkers filed with junction `node` but `self`, remapped into its frame (SUMO's walking areas), somebody lying as their body's
// footprint; a sidewalk walker leaves out its own sidewalk's, already seen by arc.
void see_junction(const TrafficState &t, int node, int self, int own_sidewalk, Seen &sn);
// Its own sidewalk's furniture near it, by arc: `from` is the first piece that may reach back to it, carried along by the caller as it goes
// through the sidewalk's walkers in order of arc.
void see_sidewalk_furniture(const std::vector<StreetObstacle> &list, size_t from, const CrowdEntry &w, Seen &sn);
// The furniture on the sidewalks of junction `n`'s arms as far from it as its walkers may be and look.
void junction_furniture(CityState &s, int n, float look, std::vector<const StreetObstacle *> &out);
void see_footprint(const StreetObstacle &o, Seen &sn); // a footprint's corners projected ahead and across the walker's frame

// The crowd's rules for one step (traffic.json ped_*), with the step's length and the inverses a choice multiplies by.
struct StripeRules {
	float dt = 0, look = 0, gap = 0, inv_headway = 0, accel = 0, aside_share = 0, lateral_cost = 0, keep_cost = 0;
	float stripe = 0, inv_stripe = 0, gain = 0, hold = 0, jam_s = 0, jam_crossing_s = 0, jam_narrow_s = 0, jam_share = 0, moving = 0;
	// stepping aside costs at least what being off its own side does: a walker with nobody about, on the stripe it aims
	// at, keeps it (no other stripe can score better), so its choice needs no stripe scored
	bool settled_keeps = false;
	StripeRules(const TrafficParams &p, float step);
};

// The way's frame of a walker on a sidewalk (`half`: the room either side of its middle, `stripe` a stripe's width; in the
// world only when it meets those near a junction) and on a line round a corner, before a kerb or over a zebra.
void sidewalk_frame(const CrowdEntry &w, float half, float stripe, bool world, WayFrame &f);
void line_frame(const CityState &s, const Pedestrian &w, WayFrame &f);
V2 left_of(const V2 &d); // +u of a direction (lane_geometry.cpp: x + dz * u, z - dx * u)

// ped_swaths.cpp: the swaths of the vehicles off the lanes near a walker at `at`, as bodies in its frame (ahead `fwd`, across
// `left`, its place across f.u), appended to the `n` in `out`; returns how many there are then.
int swath_bodies(const TrafficState &t, const V2 &at, const V2 &fwd, const V2 &left, const WayFrame &f, float look, float body, Body *out, int n);

// The walker's stripe, step aside and pace this step, from the bodies around it (all read at the step's start).
StripeChoice choose_stripe(const StripeRules &r, const CrowdEntry &w, const WayFrame &f, const Body *bodies, int n);

} // namespace traffic
} // namespace city
