// Vehicles whose bodies stand inside one another: every near vehicle (on a lane, on a connector, or off the lanes in and out of a
// lot) as the box of its class round its published pose. What the audit and the traffic tests read (traffic_probe): a body inside
// another is a defect whatever caused it, so the rule is stated on the bodies, not on any one mechanism.
#pragma once

#include <cstdint>
#include <vector>

namespace city {

struct CityState;

namespace traffic {

// Two bodies side by side touch; they do not stand in each other: the one tolerance of every check below (a float's rounding of
// two boxes laid edge to edge).
constexpr float BODY_TOUCH_M = 1e-3f;

// Where a vehicle is driven from, for saying which mechanisms two overlapping bodies came from.
enum class BodyPlace : uint8_t { LANE, CONNECTOR, LORRY, LOT_CAR, OFF, PARKED }; // PARKED: a car standing in a bay (a parked row)

struct VehicleOverlap {
	int a = -1, b = -1; // vehicle ids; a parked row is -2 - its bay
	BodyPlace place_a = BodyPlace::LANE, place_b = BodyPlace::LANE;
	int phase_a = -1, phase_b = -1; // a lorry's LorryPhase or a lot car's CarPhase, -1 otherwise
	float depth = 0; // how far the two boxes go into each other, metres (the least push that parts them)
	float x = 0, z = 0; // between the two
	float lat_a = 0, lat_b = 0, v_a = 0, v_b = 0; // each one's lane change still to cover (Vehicle::lat) and its speed
};

// The overlapping pairs, deepest first, at most `most` listed; `count` gets them all. Two bodies that only touch (within `touch`
// metres) do not overlap.
void vehicle_overlaps(const CityState &s, float touch, int most, std::vector<VehicleOverlap> &out, int &count);

// Whether vehicle `id`'s body, set at (x, z) heading `angle`, would stand clear of every other near body (within `touch`). What a
// body off the lanes asks before each step: nothing else keeps it from another lot's car, a lorry, or the traffic beside a gate.
bool body_free(const CityState &s, int id, float x, float z, float angle, float touch);
// The same for a body of class `c` that is not a vehicle yet (`skip` the one vehicle not to count, -1 none).
bool spot_free(const CityState &s, int skip, int type, float x, float z, float angle, float touch);
// A body of `type` may stand on lane `key` with its front at `at` (demand.cpp: the one check of every place a body is put on a lane).
bool lane_free(const CityState &s, int key, float at, int type, int skip);
// The vehicle whose body stands there (-2 - its bay for a parked row), NO_BODY when the spot is free (what spot_free asks; the probes
// name it).
constexpr int NO_BODY = -1;
int spot_blocker(const CityState &s, int skip, int type, float x, float z, float angle, float touch);

// For a vehicle on a lane or a connector: the room from its front to the nearest body off the lanes (a lot car, a lorry) standing in
// its path ahead (its width along its heading) within `look` metres; NO_LEADER (motion_law.h) when none. What it stops short of, as of a
// standing leader: a lane's closures (lane_closures.h) hold back only the lanes of the gate's own segment, and a body turning in
// near a corner stands over the junction's connectors and the cross street too.
float off_body_ahead(const CityState &s, int id, float look, int *who = nullptr); // `who`: that body's vehicle, -1 none

} // namespace traffic
} // namespace city
