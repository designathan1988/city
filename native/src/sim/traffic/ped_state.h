// The walkers' state in TrafficState (traffic_state.h): a walker with its route, way round a corner and place among the
// others, the crowd step's copy of it, and the street furniture as bodies on its sidewalks; and RouteRef, the slice of
// a route pool that walkers and vehicles both hold. Pure C++; written only by sim/traffic, read by render/agents.
#pragma once

#include "sim/state/pose_buffer.h"

#include <cstdint>
#include <vector>

namespace city {

// A route is a slice of the traffic/people pool: no heap allocation per vehicle or walker.
struct RouteRef {
	int off = -1, len = 0, cap = 0;
};

// A point on a walker's way round a corner: `cross` is the arm segment crossed on the way to it (-1: along the kerb);
// a kerb end of a crosswalk also names the sidewalk of the arm it stands on (`side`, -1 elsewhere).
struct Waypoint {
	float x = 0, y = 0, z = 0;
	int cross = -1, side = -1;
};
constexpr int MAX_WAYS = 12; // two per arm crossed (at most 3 at a 6-arm node) and the exit sidewalk start

// KERB: under a kerb stop's hand (kerb_stops.cpp), between a door and the footway.
enum class PedMode : uint8_t { SIDEWALK = 0, WAY = 1, WAIT = 2, KERB = 3 };

// A piece of street furniture as a body standing on a sidewalk: its footprint along the segment's arc (s0..s1) and
// across from the sidewalk's middle (u0..u1), and in the world (centre, the arc's direction there, half sizes).
struct StreetObstacle {
	float s0 = 0, s1 = 0, u0 = 0, u1 = 0;
	float x = 0, z = 0, fx = 1, fz = 0, half_along = 0, half_across = 0;
};
// A walker as the crowd step reads it (ped_crowd.cpp), copied out once a step so that looking round and choosing read
// one array: on a sidewalk its arc and across, in the world its place and heading, whether it moves, and what its own
// choice starts from (the stripe aimed at and how long held, its pace, how long held up, its speed and own side).
struct CrowdEntry {
	int id = -1, sidewalk = -1; // seg * 2 + side while on a sidewalk, else -1
	float s = 0, u = 0, x = 0, z = 0, fx = 0, fz = 0;
	float u_goal = 0, held = 0, pace = 0, jam = 0, speed = 0, keep = 0;
	uint8_t dir = 0;
	bool moving = false;
};

struct SidewalkFurniture {
	std::vector<StreetObstacle> side[2]; // right (0) and left (1) sidewalk, by s0
	float longest = 0; // the longest footprint along the arc (a search by s0 starts this far back)
};

// A walker with a destination: a route of nodes over streets with sidewalks; along a sidewalk on
// its own track, round each corner and over the crosswalks (waiting at the kerb for the walk phase or a gap).
// What every step reads comes first (two cache lines, copied out by ped_crowd.cpp), the way and the trail after.
struct Pedestrian {
	bool alive = false;
	PedMode mode = PedMode::SIDEWALK;
	uint32_t look = 0; // render hash: clothes, skin, hair
	int seg = -1, dir = 0, side = 0; // walking a -> b (dir 0) on the right (side 0, -u) or left sidewalk
	float s = 0, speed = 0, track = 0, walked = 0, wait = 0, scale = 1;
	float cross_cd = 0; // seconds until a WAIT walker tries the crosswalk again (5 Hz, not every step)
	// among the others (ped_crowd.cpp): across its way (on a sidewalk from its middle, on a line round a corner or over
	// a zebra from that line; +u on the left of a -> b, of the line), the stripe aimed at, how long that has held, the
	// pace the bodies ahead leave it and how long it has been held up (SUMO's jam time)
	float u = 0, u_goal = 0, stripe_held = 0, pace = 0, jam = 0;
	int way_node = -1; // the node it goes round (waypoints below)
	Pose cur;
	RouteRef route; // nodes; route[ri] is the node it walks toward, in TrafficState::ped_pool
	int ri = 0;
	Waypoint ways[MAX_WAYS];
	int wn = 0, wi = 0; // waypoints round node `way_node`, the next one
	float lx = 0, ly = 0, lz = 0, llen = 0, ls = 0; // current straight line: start, length, metres walked on it
	int cross_seg = -1; // the arm it is crossing now (-1 when not on a crosswalk)
	int64_t slot_key = -1; // the kerb a place to wait is held at (TrafficState::kerb_slots), and which place
	int slot = -1;
	float line_off = 0; // its line over the zebra lies this far left of the crosswalk's middle line (its place there)
	PoseTrail trail;
};

} // namespace city
