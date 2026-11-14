// The motion law the lane's step (vehicle_motion.cpp) and the connector's step (connector_motion.cpp) share: the leader a vehicle
// drives against (on its lane and beyond its line: lane_leads.cpp), a body's length, the following law that anticipates, the bends
// it slows for and the braking curve that keeps it to them (vehicle_speed.cpp), the acceleration after the emergency cap and the
// jerk limit, and the move by it.
#pragma once

#include "sim/traffic/traffic_system.h"

namespace city {
namespace traffic {

// The nearest leader of a vehicle: the gap to its rear, its speed and its acceleration (NO_LEADER: none; a thing that stands has
// neither speed nor acceleration).
struct Lead {
	float gap = NO_LEADER, v = 0, a = 0;
	void take(float g, float lv, float la = 0.f) {
		if (g < gap) {
			gap = g;
			v = lv;
			a = la;
		}
	}
};

inline float body_length(const CityState &s, const Vehicle &v) { return s.catalog.traffic.classes[int(v.type)].length; }

// The acceleration after the emergency cap and the jerk limit, never more than stops it short of its leader (vehicle_motion.cpp).
float finish_accel(const CityState &s, const Vehicle &v, float acc, const Lead &l, float dt);
// Moves `v` by the commanded `acc` along `pos` (its lane's s or its connector's cs) and keeps the acceleration that speed change
// really was (vehicle_motion.cpp).
void integrate(const CityState &s, Vehicle &v, float acc, float dt, float &pos);
// Moves the vehicles on one connector and hands those over its end to the lane it enters (connector_motion.cpp).
void step_connector(CityState &s, int ci, float dt);
// What `v`, its front `along` connector `ci`'s way (negative before the line), drives against on the other branches of its lane's
// fork (Conn::forks): a body on one of them, or come off it with its body still on its way, further along than `v` and not yet clear
// of it, its gap measured as on one path from the line (connector_motion.cpp). The lane's head takes it at its line, a vehicle on the
// connector all the way over: whoever took the other branch first is followed until its body has left where the ways part.
void fork_leads(const CityState &s, int ci, const Vehicle &v, float along, Lead &l);
// What the head of a lane sees beyond its stop line, `dist` ahead, on the way onto connector `ci`: a stretch of the next lane held
// by a body off the lanes, the connector's last vehicle or the next lane's tail (lane_leads.cpp).
void lead_beyond(const CityState &s, int ci, float dist, Lead &l);
// What the k-th vehicle of a lane's list (sorted by s) drives against on the lane: the first ahead not out of its way, a closure, a
// body changing out of the lane that still stands over it, whoever is ahead on the lane its own body is still changing out of, its
// kerb stop. True: nobody is ahead of it on the lane, it heads it (lane_leads.cpp).
bool lane_leads(const CityState &s, int ds, int lane, const std::vector<int> &list, int k, Lead &l);

// vehicle_speed.cpp. Against a leader `gap` ahead going `vl` and accelerating `al`: the improved IDM blended, by `coolness`, with
// what the driver must do if the leader keeps that acceleration; the improved IDM alone on a free road.
float follow_accel(const VehicleClass &c, float coolness, float v, float v0, float gap, float vl, float al);
// Bends: the speed `v`'s driver takes a bend of curvature `k` (1/m) at, by the lateral acceleration it accepts at that speed
// (NO_LEADER: no bend to slow for); the most it drives over connector `c` (its exit's limit, its curve's bend); the fastest it goes
// `from` metres along directed segment `ds` (negative: that far before it) on `lane` and still takes that segment's bend at its
// own speed; the fastest it goes `dist` before its lane's end on the way onto `c`, braking down to the speed it takes `c`'s bend at.
float bend_pace(const CityState &s, const Vehicle &v, float k);
float conn_top(const CityState &s, const Vehicle &v, const Conn &c);
float bend_ahead(const CityState &s, const Vehicle &v, int ds, int lane, float from);
float turn_limit(const CityState &s, const Vehicle &v, const Conn &c, float dist);
// The acceleration `acc` held to a braking curve: no faster next tick than `cap` (what a bend ahead allows here), braking for it no
// harder than the comfortable `b`. The following law only leans toward a falling target; the curve keeps a vehicle to its bend.
float held_to(float acc, float v, float cap, float b, float dt);
// The gap, seconds, driver `me` asks before a movement that goes first, by its own turn (junction_gate.cpp; `on_main`: it turns
// off the road that has the way).
float turn_gap(const TrafficParams &tp, const Vehicle &me, Turn turn, bool on_main);
// A ring's metering (junction_admission.cpp): connector `c` comes onto a ring from a road outside it and the ring's vehicles, with
// one more, would pass ring_meter_share of its room: nobody from outside goes on, so those on it can leave.
bool ring_full(CityState &s, const Conn &c);

} // namespace traffic
} // namespace city
