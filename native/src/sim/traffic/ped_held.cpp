// The ground a vehicle off the lanes holds, solid to the walkers (the reference game's rule; the car's half of it is
// lot_traffic.cpp may_go): the body of each car of the lots and each lorry where it stands, as three discs (its middle and a
// point either end, half its width round), and for a car driving into a bay or out of one the same discs along the rest of
// its way, from when it asks for it. Filed once a tick before the crowd step, which shows a walker the discs near it as
// bodies it never walks into nor squeezes through; one that stands on a disc walks on out of it and steps away from it
// (ped_stripes.cpp); somebody waiting at a kerb walks off it too, and nobody is born onto it. So a car that asks finds its
// way clear, and takes it.
#include "sim/traffic/ped_stripes.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr int DISCS = 3; // along a body: its middle and either end

void body_discs(TrafficState &t, const VehicleClass &c, const V2 &at, const V2 &heading) {
	const V2 reach = heading * std::max(0.f, c.length * 0.5f - c.half_width);
	for (int k = 0; k < DISCS; k++) {
		const V2 q = at + reach * float(k - 1);
		t.held.push_back({ q.x, q.y, c.half_width });
	}
}

// The discs filed since `first` are one vehicle's: under their box.
void close_ground(TrafficState &t, int first) {
	HeldGround g;
	g.first = first;
	g.count = int(t.held.size()) - first;
	g.x0 = g.z0 = BIG;
	g.x1 = g.z1 = -BIG;
	for (int k = first; k < int(t.held.size()); k++) {
		const HeldDisc &d = t.held[size_t(k)];
		g.x0 = std::min(g.x0, d.x - d.r);
		g.x1 = std::max(g.x1, d.x + d.r);
		g.z0 = std::min(g.z0, d.z - d.r);
		g.z1 = std::max(g.z1, d.z + d.r);
	}
	t.held_of.push_back(g);
}

bool near_box(const HeldGround &g, float x, float z, float reach) {
	return x >= g.x0 - reach && x <= g.x1 + reach && z >= g.z0 - reach && z <= g.z1 + reach;
}

} // namespace

void file_held_ground(CityState &s) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	t.held.clear();
	t.held_of.clear();
	for (const LotCar &car : t.park.cars) {
		const Vehicle &v = t.vehicles[size_t(car.id)];
		if (!v.off) continue;
		const VehicleClass &c = p.classes[int(v.type)];
		const int first = int(t.held.size());
		body_discs(t, c, V2(v.cur.x, v.cur.z), V2(v.cur.fx, v.cur.fz));
		const bool moving = car.phase == CarPhase::PARKING || car.phase == CarPhase::LEAVING;
		for (float at = car.path.s; moving; at = std::min(car.path.length(), at + p.man_disc_step_m)) {
			const OffPose q = car.path.pose_at(at); // the rest of its way: asked for while it waits, its own once taken
			body_discs(t, c, V2(q.x, q.z), v2_from_angle(q.angle));
			if (at >= car.path.length()) break;
		}
		close_ground(t, first);
	}
	for (const Lorry &l : t.park.lorries) {
		const Vehicle &v = t.vehicles[size_t(l.id)];
		if (!v.off) continue;
		const int first = int(t.held.size());
		body_discs(t, p.classes[int(v.type)], V2(v.cur.x, v.cur.z), V2(v.cur.fx, v.cur.fz));
		close_ground(t, first);
	}
}

int held_bodies(const TrafficState &t, const V2 &at, const V2 &fwd, const V2 &left, const WayFrame &f, float look, float body, Body *out, int n) {
	for (const HeldGround &g : t.held_of) {
		if (!near_box(g, at.x, at.y, look + body)) continue;
		for (int k = g.first; k < g.first + g.count && n < MAX_BODIES; k++) {
			const HeldDisc &d = t.held[size_t(k)];
			const V2 rel(d.x - at.x, d.z - at.y);
			const float along = rel.dot(fwd), lat = f.u + rel.dot(left), reach = d.r + body;
			if (along < -reach || along - reach > look || lat + reach < f.lo || lat - reach > f.hi) continue;
			Body &b = out[n++];
			b = Body();
			b.along_lo = b.along_hi = along;
			b.lat_lo = b.lat_hi = lat;
			b.reach = reach;
			b.furniture = b.held = true;
			b.under = std::hypot(along, lat - f.u) < reach; // it stands on this one: it walks on out of it and steps away
		}
	}
	return n;
}

bool on_held_ground(const TrafficState &t, float x, float z, float body) {
	for (const HeldGround &g : t.held_of) {
		if (!near_box(g, x, z, body)) continue;
		for (int k = g.first; k < g.first + g.count; k++) {
			const HeldDisc &d = t.held[size_t(k)];
			if (std::hypot(x - d.x, z - d.z) < d.r + body) return true;
		}
	}
	return false;
}

void step_off_held(const CityState &s, Pedestrian &w, float dt) {
	const TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const float step = w.speed * dt; // it walks off it
	for (const HeldGround &g : t.held_of) {
		if (!near_box(g, w.cur.x, w.cur.z, p.ped_body_m)) continue;
		for (int k = g.first; k < g.first + g.count; k++) {
			const HeldDisc &d = t.held[size_t(k)];
			const V2 off(w.cur.x - d.x, w.cur.z - d.z);
			const float far = off.len();
			if (far >= d.r + p.ped_body_m) continue;
			const V2 away = far > step ? off * (1.f / far) : V2(-w.way_z, w.way_x); // on its middle: to its own side
			w.cur.x += away.x * step;
			w.cur.z += away.y * step;
			w.walked += step;
			return;
		}
	}
}

} // namespace traffic
} // namespace city
