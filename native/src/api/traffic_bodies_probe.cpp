// What the traffic probe says of bodies (traffic_bodies_probe.h).
#include "api/traffic_bodies_probe.h"

#include "sim/state/city_state.h"
#include "sim/traffic/lane_closures.h"
#include "sim/traffic/motion_law.h"
#include "sim/traffic/traffic_system.h"
#include "sim/traffic/vehicle_overlaps.h"
#include "sim/traffic/lot_wait.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <vector>

using namespace godot;

namespace city {

namespace {
// Two bodies side by side (their centres two radii apart, to the rounding of the sum) touch; they do not overlap.
constexpr float TOUCH_EPS_M = 1e-3f;
constexpr int OVERLAP_PAIRS_SHOWN = 16; // the pairs listed for the audit

// Walkers whose body overlaps another's (centres closer than `touch`): [walking, waiting at a kerb].
// Two bodies side by side (their centres two radii apart, to the rounding of the sum) touch; they do not overlap.
// The same, each overlapping pair said once: [the modes of the two (PedMode), where the first stands, whether they walk together (a
// party)], the first `most` of them: what the audit reads a cause from.
Array people_overlap_pairs(const TrafficState &t, float touch, int most) {
	Array out;
	for (size_t i = 0; i < t.peds.size() && out.size() < most; i++) {
		const Pedestrian &a = t.peds[i];
		if (!a.alive) continue;
		for (size_t j = i + 1; j < t.peds.size() && out.size() < most; j++) {
			const Pedestrian &b = t.peds[j];
			const float dx = a.cur.x - b.cur.x, dz = a.cur.z - b.cur.z;
			if (!b.alive || dx * dx + dz * dz >= touch * touch) continue;
			out.push_back(Array::make(int(a.mode), int(b.mode), Vector3(a.cur.x, a.cur.y, a.cur.z), a.mate == int(j) || b.mate == int(i) || a.with == int(j) || b.with == int(i)));
		}
	}
	return out;
}

Array people_overlaps(const TrafficState &t, float touch) {
	std::vector<int> alive;
	for (int i = 0; i < int(t.peds.size()); i++)
		if (t.peds[size_t(i)].alive) alive.push_back(i);
	int walking = 0, waiting = 0;
	for (int i : alive) {
		const Pedestrian &a = t.peds[size_t(i)];
		for (int j : alive) {
			const Pedestrian &b = t.peds[size_t(j)];
			const float dx = a.cur.x - b.cur.x, dz = a.cur.z - b.cur.z;
			if (i == j || dx * dx + dz * dz >= touch * touch) continue;
			(a.mode == PedMode::WAIT ? waiting : walking)++;
			break;
		}
	}
	Array out;
	out.push_back(walking);
	out.push_back(waiting);
	return out;
}

// Vehicles whose bodies stand inside one another (sim/traffic/vehicle_overlaps.h): how many pairs, and with `pairs` the deepest of
// them, each [where (x, depth in metres, z), [the place each body is driven from (BodyPlace), its lorry's or lot car's phase]].
void vehicle_overlaps_to(const CityState &s, bool pairs, Dictionary &d) {
	std::vector<traffic::VehicleOverlap> found;
	int count = 0;
	traffic::vehicle_overlaps(s, TOUCH_EPS_M, OVERLAP_PAIRS_SHOWN, found, count);
	d["vehicle_overlaps"] = count;
	if (!pairs) return;
	// where a vehicle of a pair is driven: [its class, ds, lane, arc, connector, arc on it]; empty for a car parked in a bay
	const auto seat = [&](int id) {
		if (id < 0) return Array();
		const Vehicle &v = s.traffic.vehicles[size_t(id)];
		return Array::make(int(v.type), v.ds, v.lane, v.s, v.conn, v.cs);
	};
	Array list;
	for (const traffic::VehicleOverlap &o : found)
		list.push_back(Array::make(Vector3(o.x, o.depth, o.z), Array::make(int(o.place_a), o.phase_a, int(o.place_b), o.phase_b), Array::make(o.lat_a, o.v_a, o.lat_b, o.v_b),
				// a lot car's own step check now, for each of the two: what holds it (LotWait) and whose body is where it would step
				Array::make(o.a >= 0 ? traffic::lot_car_wait(s, o.a) : -1, o.a >= 0 ? traffic::lot_car_blocker(s, o.a) : -1, o.b >= 0 ? traffic::lot_car_wait(s, o.b) : -1,
						o.b >= 0 ? traffic::lot_car_blocker(s, o.b) : -1, o.a, o.b),
				seat(o.a), seat(o.b)));
	d["vehicle_overlap_pairs"] = list;
}

} // namespace

void bodies_in_one_another(const CityState &s, bool pairs, Dictionary &d) {
	const TrafficState &t = s.traffic;
	const float touch = 2 * s.catalog.traffic.ped_body_m - TOUCH_EPS_M;
	d["people_overlaps"] = people_overlaps(t, touch); // walkers inside another's body: walking, waiting
	if (pairs) d["people_overlap_pairs"] = people_overlap_pairs(t, touch, OVERLAP_PAIRS_SHOWN);
	d["people_in_furniture"] = traffic::walkers_in_furniture(s); // walkers whose body stands in a street piece
	vehicle_overlaps_to(s, pairs, d);
}

void bodies_holding(const CityState &s, int who, Dictionary &d) {
	const TrafficState &t = s.traffic;
	const Vehicle &v = t.vehicles[size_t(who)];
	int body = -1; // a body off the lanes in its path (vehicle_overlaps.h), and what holds that one
	const float ahead = traffic::off_body_ahead(s, who, NO_LEADER, &body);
	if (body >= 0) {
		const Vehicle &o = t.vehicles[size_t(body)];
		d["stuck_off_body"] = Array::make(body, ahead, o.car >= 0 ? int(t.park.cars[size_t(o.car)].phase) : -1, o.lorry >= 0 ? int(t.park.lorries[size_t(o.lorry)].phase) : -1,
				traffic::lot_car_wait(s, body), traffic::lot_car_blocker(s, body), Vector3(o.cur.x, o.cur.y, o.cur.z));
	}
	if (v.ds >= 0) // the body off the lanes whose closure holds it: [its id, lorry phase or -1, lot car phase or -1, where it stands]
		for (const LaneClosure &c : t.closures)
			if (c.key == v.ds * MAX_LANES + v.lane && c.to > v.s) {
				const Vehicle &o = t.vehicles[size_t(c.owner)];
				const int lorry = o.lorry >= 0 ? int(t.park.lorries[size_t(o.lorry)].phase) : -1, car = o.car >= 0 ? int(t.park.cars[size_t(o.car)].phase) : -1;
				d["stuck_closed_by"] = Array::make(c.owner, lorry, car, Vector3(o.cur.x, o.cur.y, o.cur.z), o.alive, o.off, traffic::lot_car_wait(s, c.owner));
				if (const int b = traffic::lot_car_blocker(s, c.owner); b >= 0) { // who stands where that lot car would step
					const Vehicle &w = t.vehicles[size_t(b)];
					d["stuck_blocker"] = Array::make(b, w.off, w.car >= 0 ? int(t.park.cars[size_t(w.car)].phase) : -1, w.ds, w.lane, w.s, w.conn, w.v, Vector3(w.cur.x, w.cur.y, w.cur.z), w.stuck);
				} else if (b != traffic::NO_BODY) {
					d["stuck_blocker"] = Array::make(b); // a car parked in a bay (-2 - the bay)
				}
				break;
			}
}

} // namespace city
