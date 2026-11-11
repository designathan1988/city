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
	bool held = false; // ground a vehicle off the lanes holds (ped_held.cpp): a furniture body
	bool under = false; // held ground the walker stands on: no obstacle ahead (it walks on out of it), and it steps away from it
};
constexpr int MAX_BODIES = 64; // bodies a walker looks at in one step (the nearest first is not needed: all are near)

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

// ped_held.cpp: the ground the vehicles off the lanes hold near a walker at `at`, as bodies in its frame (ahead `fwd`, across
// `left`, its place across f.u), appended to the `n` in `out`; returns how many there are then.
int held_bodies(const TrafficState &t, const V2 &at, const V2 &fwd, const V2 &left, const WayFrame &f, float look, float body, Body *out, int n);

// The walker's stripe, step aside and pace this step, from the bodies around it (all read at the step's start).
StripeChoice choose_stripe(const StripeRules &r, const CrowdEntry &w, const WayFrame &f, const Body *bodies, int n);

} // namespace traffic
} // namespace city
