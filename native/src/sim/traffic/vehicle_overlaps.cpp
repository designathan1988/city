// Vehicles whose bodies stand inside one another (vehicle_overlaps.h): oriented boxes, separated by their four axes.
#include "sim/traffic/vehicle_overlaps.h"

#include "sim/state/city_state.h"
#include "sim/traffic/motion_law.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace city {
namespace traffic {

namespace {
constexpr float HALF = 0.5f;

struct Box {
	int id = -1;
	float x = 0, z = 0, fx = 1, fz = 0, half_len = 0, half_wid = 0;
};

// The box's half extent along the unit axis (ax, az).
float reach(const Box &b, float ax, float az) {
	return std::fabs(b.fx * ax + b.fz * az) * b.half_len + std::fabs(-b.fz * ax + b.fx * az) * b.half_wid;
}

// How far two boxes go into each other: the least overlap over the four axes, negative when one axis separates them.
float depth(const Box &a, const Box &b) {
	const float dx = b.x - a.x, dz = b.z - a.z;
	float least = std::numeric_limits<float>::max();
	const float axes[4][2] = { { a.fx, a.fz }, { -a.fz, a.fx }, { b.fx, b.fz }, { -b.fz, b.fx } };
	for (const auto &ax : axes) {
		const float gap = reach(a, ax[0], ax[1]) + reach(b, ax[0], ax[1]) - std::fabs(dx * ax[0] + dz * ax[1]);
		least = std::min(least, gap);
	}
	return least;
}

BodyPlace place_of(const Vehicle &v) {
	if (!v.off) return v.conn >= 0 ? BodyPlace::CONNECTOR : BodyPlace::LANE;
	if (v.lorry >= 0) return BodyPlace::LORRY;
	return v.car >= 0 ? BodyPlace::LOT_CAR : BodyPlace::OFF;
}

// The near body of vehicle `id` as it stands, false for one with none.
bool box_of(const CityState &s, int id, Box &out) {
	const Vehicle &v = s.traffic.vehicles[size_t(id)];
	if (!v.alive || !(v.micro || v.off)) return false; // a far vehicle has no body: it is a place in its lane's queue
	const float n = std::hypot(v.cur.fx, v.cur.fz);
	if (n <= 0) return false;
	const VehicleClass &c = s.catalog.traffic.classes[int(v.type)];
	out = { id, v.cur.x, v.cur.z, v.cur.fx / n, v.cur.fz / n, c.length * HALF, c.half_width };
	return true;
}

// A car standing in bay `bay` (a parked row: no vehicle, parking_sources.cpp publishes it): its middle on the stall's, nose in.
bool row_box(const CityState &s, int bay, Box &out) {
	const Bay &b = s.traffic.park.bays[size_t(bay)];
	if (!b.alive || !b.has_row) return false;
	const VehicleClass &c = s.catalog.traffic.classes[b.row_type];
	out = { -2 - bay, b.at.x, b.at.y, -b.open.x, -b.open.y, c.length * HALF, c.half_width };
	return true;
}

int phase_of(const CityState &s, const Vehicle &v) {
	const ParkingState &p = s.traffic.park;
	if (v.off && v.lorry >= 0 && size_t(v.lorry) < p.lorries.size()) return int(p.lorries[size_t(v.lorry)].phase);
	if (v.off && v.car >= 0 && size_t(v.car) < p.cars.size()) return int(p.cars[size_t(v.car)].phase);
	return -1;
}
} // namespace

void vehicle_overlaps(const CityState &s, float touch, int most, std::vector<VehicleOverlap> &out, int &count) {
	const TrafficState &t = s.traffic;
	std::vector<Box> boxes;
	Box b;
	for (int id = 0; id < int(t.vehicles.size()); id++)
		if (box_of(s, id, b)) boxes.push_back(b);
	for (int bay : t.park.rows)
		if (row_box(s, bay, b)) boxes.push_back(b);
	std::sort(boxes.begin(), boxes.end(), [](const Box &a, const Box &b) { return a.x - a.half_len < b.x - b.half_len; });
	out.clear();
	count = 0;
	for (size_t i = 0; i < boxes.size(); i++) {
		const Box &a = boxes[i];
		const float a_end = a.x + std::max(a.half_len, a.half_wid);
		for (size_t j = i + 1; j < boxes.size(); j++) {
			const Box &b = boxes[j];
			if (b.x - std::max(b.half_len, b.half_wid) > a_end) break; // the rest stand past it along x
			const float d = depth(a, b);
			if (d <= touch) continue;
			count++;
			VehicleOverlap o{ a.id, b.id, BodyPlace::PARKED, BodyPlace::PARKED, -1, -1, d, (a.x + b.x) * HALF, (a.z + b.z) * HALF };
			if (a.id >= 0) {
				const Vehicle &va = t.vehicles[size_t(a.id)];
				o.place_a = place_of(va), o.phase_a = phase_of(s, va), o.lat_a = va.lat, o.v_a = va.v;
			}
			if (b.id >= 0) {
				const Vehicle &vb = t.vehicles[size_t(b.id)];
				o.place_b = place_of(vb), o.phase_b = phase_of(s, vb), o.lat_b = vb.lat, o.v_b = vb.v;
			}
			out.push_back(o);
		}
	}
	std::sort(out.begin(), out.end(), [](const VehicleOverlap &a, const VehicleOverlap &b) { return a.depth > b.depth; });
	if (int(out.size()) > most) out.resize(size_t(most));
}

bool body_free(const CityState &s, int id, float x, float z, float angle, float touch) {
	return spot_free(s, id, int(s.traffic.vehicles[size_t(id)].type), x, z, angle, touch);
}

bool spot_free(const CityState &s, int id, int type, float x, float z, float angle, float touch) { return spot_blocker(s, id, type, x, z, angle, touch) == NO_BODY; }

int spot_blocker(const CityState &s, int id, int type, float x, float z, float angle, float touch) {
	const TrafficState &t = s.traffic;
	const VehicleClass &c = s.catalog.traffic.classes[type];
	const Box me{ id, x, z, std::cos(angle), std::sin(angle), c.length * HALF, c.half_width };
	const float mine = std::hypot(me.half_len, me.half_wid);
	Box o;
	const auto clear_of = [&](int other) {
		if (other == id || !box_of(s, other, o)) return true;
		const float reach = mine + std::hypot(o.half_len, o.half_wid);
		if (std::fabs(o.x - x) > reach || std::fabs(o.z - z) > reach) return true;
		return depth(me, o) <= touch;
	};
	for (int other : t.micro_list)
		if (!clear_of(other)) return other;
	for (const LotCar &car : t.park.cars)
		if (t.vehicles[size_t(car.id)].off && !clear_of(car.id)) return car.id;
	for (const Lorry &l : t.park.lorries)
		if (t.vehicles[size_t(l.id)].off && !clear_of(l.id)) return l.id;
	for (int bay : t.park.rows) { // the cars standing in the bays
		if (!row_box(s, bay, o)) continue;
		const float reach = mine + std::hypot(o.half_len, o.half_wid);
		if (std::fabs(o.x - x) <= reach && std::fabs(o.z - z) <= reach && depth(me, o) > touch) return -2 - bay;
	}
	return NO_BODY;
}

float off_body_ahead(const CityState &s, int id, float look, int *who) {
	if (who) *who = -1;
	const TrafficState &t = s.traffic;
	Box me;
	if (!box_of(s, id, me)) return NO_LEADER;
	float best = NO_LEADER;
	Box o;
	const auto check = [&](int other) {
		if (!t.vehicles[size_t(other)].off || !box_of(s, other, o)) return;
		const float reach = me.half_len + look + std::hypot(o.half_len, o.half_wid);
		if (std::fabs(o.x - me.x) > reach || std::fabs(o.z - me.z) > reach) return;
		// the other box's outline walked in steps no wider than this body's half width, in this body's frame: a point within its
		// width and ahead of its rear is in its path
		const float step = std::max(me.half_wid, HALF * HALF);
		const float ax[2] = { o.fx, o.fz }, ay[2] = { -o.fz, o.fx };
		const float hl = o.half_len, hw = o.half_wid;
		const float corners[4][2] = { { hl, hw }, { -hl, hw }, { -hl, -hw }, { hl, -hw } };
		for (int e = 0; e < 4; e++) {
			const float *a = corners[e], *b = corners[(e + 1) % 4];
			const int n = std::max(1, int(std::ceil(std::hypot(b[0] - a[0], b[1] - a[1]) / step)));
			for (int k = 0; k <= n; k++) {
				const float u = a[0] + (b[0] - a[0]) * float(k) / float(n), w = a[1] + (b[1] - a[1]) * float(k) / float(n);
				const float px = o.x + ax[0] * u + ay[0] * w - me.x, pz = o.z + ax[1] * u + ay[1] * w - me.z;
				const float along = px * me.fx + pz * me.fz, across = -px * me.fz + pz * me.fx;
				if (std::fabs(across) > me.half_wid || along < -me.half_len || along - me.half_len > look) continue;
				const float gap = std::max(0.f, along - me.half_len);
				if (gap < best && who) *who = other;
				best = std::min(best, gap);
			}
		}
	};
	for (const LotCar &car : t.park.cars) check(car.id);
	for (const Lorry &l : t.park.lorries) check(l.id);
	return best;
}

} // namespace traffic
} // namespace city
