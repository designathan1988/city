// The walkers' state in TrafficState (traffic_state.h): a walker with its route, way round a corner and place among the
// others, the crowd step's copy of it, and the street furniture as bodies on its sidewalks; and RouteRef, the slice of
// a route pool that walkers and vehicles both hold. Pure C++; written only by sim/traffic, read by render/agents.
#pragma once

#include "sim/state/pose_buffer.h"
#include "sim/traffic/route_arena.h"

#include <cstdint>
#include <vector>

namespace city {

// A point on a walker's way round a corner: `cross` is the arm segment crossed on the way to it (-1: along the kerb);
// a kerb end of a crosswalk also names the sidewalk of the arm it stands on (`side`, -1 elsewhere).
struct Waypoint {
	float x = 0, y = 0, z = 0;
	int cross = -1, side = -1;
};
constexpr int MAX_WAYS = 12; // two per arm crossed (at most 3 at a 6-arm node) and the exit sidewalk start

// SIDEWALK, WAY (round a corner, over a zebra), WAIT (at a kerb), KERB (under a kerb stop's hand, kerb_stops.cpp: between a door and the
// footway), LOT (between a door and the sidewalk), FOLLOW (with its leader).
enum class PedMode : uint8_t { SIDEWALK, WAY, WAIT, KERB, LOT, FOLLOW };


// A front door as the walkers use it: the sidewalk's arc and place across at the foot of its way, and the door itself.
struct PedDoor {
	int building = -1, segment = -1; // -1: no door
	uint64_t source_version = 0, segment_generation = 0; // canonical building record revision, independent of its visual seed
	float s = 0, u = 0, x = 0, y = 0, z = 0;
	uint8_t kind = 0; // PlaceKind
};

// A deferred source read cannot bind a recycled or subsequently edited building id.
struct PedDoorRead { int building = -1; uint64_t version = 0; };

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
	int party = -1; // its leader's id, or its own: those of a party do not make way for each other
	float s = 0, u = 0, x = 0, z = 0, fx = 0, fz = 0;
	float u_goal = 0, held = 0, pace = 0, jam = 0, speed = 0, keep = 0;
	uint8_t dir = 0;
	bool moving = false, follows = false; // follows: a body the others see, with no choice of its own
};

struct SidewalkFurniture {
	std::vector<StreetObstacle> side[2]; // right (0) and left (1) sidewalk, by s0
	float longest = 0; // the longest footprint along the arc (a search by s0 starts this far back)
};

// A shocking thing on the street (a body, a blast): whoever comes within `reach` of it before `until` runs from it.
struct PedShock {
	float x = 0, z = 0, reach = 0, until = 0;
};

// A walker with a destination: a route of nodes over streets with sidewalks; along a sidewalk on
// its own track, round each corner and over the crosswalks (waiting at the kerb for the walk phase or a gap).
// What every step reads comes first (two cache lines, copied out by ped_crowd.cpp), the way and the trail after.
struct Pedestrian {
	bool alive = false;
	PedMode mode = PedMode::SIDEWALK;
	PedAge age = PedAge::ADULT;
	uint32_t look = 0; // render hash: clothes, skin, hair
	int seg = -1, dir = 0, side = 0; // walking a -> b (dir 0) on the right (side 0, -u) or left sidewalk
	float s = 0, speed = 0, track = 0, walked = 0, wait = 0, scale = 1;
	float hp = 0, bleed = 0; // health (data shot.walker_hp) and what a wound takes from it each second
	float hurt_head = 0, hurt_torso = 0, hurt_limb[4] = {}; // damage taken by part
	uint8_t lost = 0; // limbs gone (damage_event.h CasLimb bits), shot or blown off
	uint8_t hits = 0;
	bool opened = false; // the trunk shot open
	float down_until = 0, panic_until = 0, flinch_until = 0; // t.time until which a blow keeps it on the ground / running / bent over a wound
	float cross_cd = 0; // seconds until a WAIT walker tries the crosswalk again (5 Hz, not every step)
	float rest = 0; // 0 walking .. 1 standing, eased (the render's idle share)
	float act_share = 0; // how far into `act` it is, eased
	PedAct act = PedAct::NONE; // what it does while it stands (ped_acts.cpp)
	double act_from = 0; // t.time `act` began at
	// A bang heard (ped_fright.cpp): what it has it do, stopped and turned to (face_x, face_z), and until when; when it trips and for how long; where
	// the bang was, and that it has yet to turn from it; its facing while it turns; with somebody shot beside it.
	PedAct held_act = PedAct::NONE;
	float held_until = 0, face_x = 0, face_z = 0, trip_at = 0, trip_s = 0, bang_x = 0, bang_z = 0, turn_x = 0, turn_z = 0;
	bool flee_due = false, turning = false, mourning = false;
	// among the others (ped_crowd.cpp): across its way (on a sidewalk from its middle, on a line round a corner or over
	// a zebra from that line; +u on the left of a -> b, of the line), the stripe aimed at, how long that has held, the
	// pace the bodies ahead leave it and how long it has been held up (SUMO's jam time)
	float u = 0, u_goal = 0, stripe_held = 0, pace = 0, jam = 0;
	int way_node = -1; // the node it goes round (waypoints below)
	Pose cur;
	int escape_goal = -1; uint64_t escape_goal_generation = 0; // original desired goal survives newer bangs while its query waits
	uint64_t generation = 0, escape_route_request = 0; // actual birth/request identities, outside the crowd hot fields
	RouteRef route; // nodes; route[ri] is the node it walks toward, in TrafficState::ped_pool
	int ri = 0;
	Waypoint ways[MAX_WAYS];
	int wn = 0, wi = 0; // waypoints round node `way_node`, the next one
	float lx = 0, ly = 0, lz = 0, llen = 0, ls = 0; // current straight line: start, length, metres walked on it
	int cross_seg = -1; // the arm it is crossing now (-1 when not on a crosswalk)
	int64_t slot_key = -1; // the kerb a place to wait is held at (TrafficState::kerb_slots), and which place
	int slot = -1;
	float line_off = 0; // its line over the zebra lies this far left of the crosswalk's middle line (its place there)
	PedDoor door; // the door it goes in at, on the last street of its route
	int goal_side = -1; // the sidewalk of that street it heads for (-1: either)
	bool entering = false, company_due = false; // on its way in at a door; its company comes out of the door after it
	int with = -1, mate = -1; // the walker it follows; the walker following it
	float flank = 1; // the side of its leader it walks on (+u or -u)
	bool on_way = false; // following on a sidewalk (seg, side, s, u read as a walker's)
	PoseTrail trail;
};

} // namespace city
