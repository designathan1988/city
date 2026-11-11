// The swaths of the vehicles off the lanes (swaths.h): ways laid once, swaths filed from the lots' cars and the lorries, and what the
// vehicles ask of them. A question reads the filed discs and the asker's laid way in place: nothing is built or allocated for it.
#include "sim/traffic/swaths.h"

#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr int BODY_DISCS = 3; // along a body: its middle and a point toward either end, half its width round
constexpr int PROBE_POSES = 2; // room_on_way: the body where it would stand, and its gap beyond

// A body of class `c` standing at `at`, heading `heading`, as discs from `out` on.
void body_discs(const VehicleClass &c, const V2 &at, const V2 &heading, SwathDisc *out) {
	const V2 reach = heading * std::max(0.f, c.length * 0.5f - c.half_width);
	for (int k = 0; k < BODY_DISCS; k++) {
		const V2 q = at + reach * float(k - 1);
		out[k] = { q.x, q.y, c.half_width };
	}
}

struct Box {
	float x0 = BIG, z0 = BIG, x1 = -BIG, z1 = -BIG;
	void take(const SwathDisc &d, float pad) {
		x0 = std::min(x0, d.x - d.r - pad);
		x1 = std::max(x1, d.x + d.r + pad);
		z0 = std::min(z0, d.z - d.r - pad);
		z1 = std::max(z1, d.z + d.r + pad);
	}
	bool apart(const Swath &w) const { return x1 < w.x0 || x0 > w.x1 || z1 < w.z0 || z0 > w.z1; }
};

// Some of `n` discs at `mine`, `pad` wider (their box `box`), meets one of discs [from, end) of swath `w`.
bool meets(const ParkingState &pk, const Swath &w, int from, int end, const SwathDisc *mine, int n, float pad, const Box &box) {
	if (box.apart(w)) return false;
	for (int k = w.first + from; k < w.first + end; k++) {
		const SwathDisc &d = pk.swath_discs[size_t(k)];
		for (int i = 0; i < n; i++)
			if (std::hypot(mine[i].x - d.x, mine[i].z - d.z) < mine[i].r + pad + d.r) return true;
	}
	return false;
}

// One vehicle's swath: its body where it stands and, when it drives a laid way, the rest of it from `from`.
void file_one(ParkingState &pk, const TrafficParams &p, const Vehicle &v, int id, const std::vector<SwathDisc> *way, int from, uint32_t order,
		bool outward) {
	Swath w;
	w.first = int(pk.swath_discs.size());
	w.vehicle = id;
	w.order = order;
	w.outward = outward;
	w.body = BODY_DISCS;
	pk.swath_discs.resize(pk.swath_discs.size() + BODY_DISCS);
	body_discs(p.classes[int(v.type)], V2(v.cur.x, v.cur.z), V2(v.cur.fx, v.cur.fz), &pk.swath_discs[size_t(w.first)]);
	if (way && from < int(way->size())) pk.swath_discs.insert(pk.swath_discs.end(), way->begin() + from, way->end());
	w.count = int(pk.swath_discs.size()) - w.first;
	Box box;
	for (int k = w.first; k < w.first + w.count; k++) box.take(pk.swath_discs[size_t(k)], 0.f);
	w.x0 = box.x0;
	w.z0 = box.z0;
	w.x1 = box.x1;
	w.z1 = box.z1;
	pk.swaths.push_back(w);
}

// The box of discs `from` on of a laid way, `pad` wider.
Box way_box(const std::vector<SwathDisc> &way, int from, float pad) {
	Box b;
	for (int k = from; k < int(way.size()); k++) b.take(way[size_t(k)], pad);
	return b;
}

} // namespace

void lay_way(const CityState &s, const VehicleClass &c, const Manoeuvre &path, std::vector<SwathDisc> &out) {
	const float step = s.catalog.traffic.man_disc_step_m;
	const int poses = int(std::ceil(path.length() / step)) + 1;
	out.resize(size_t(poses * BODY_DISCS));
	for (int i = 0; i < poses; i++) {
		const OffPose q = path.pose_at(std::min(path.length(), float(i) * step));
		body_discs(c, V2(q.x, q.z), v2_from_angle(q.angle), &out[size_t(i * BODY_DISCS)]);
	}
}

int way_from(const CityState &s, float at, int size) {
	return std::min(size, int(at / s.catalog.traffic.man_disc_step_m) * BODY_DISCS);
}

bool near_swath(const Swath &w, float x, float z, float reach) {
	return x >= w.x0 - reach && x <= w.x1 + reach && z >= w.z0 - reach && z <= w.z1 + reach;
}

void file_swaths(CityState &s) {
	TrafficState &t = s.traffic;
	ParkingState &pk = t.park;
	const TrafficParams &p = s.catalog.traffic;
	pk.swath_discs.clear();
	pk.swaths.clear();
	for (const LotCar &car : pk.cars) {
		const Vehicle &v = t.vehicles[size_t(car.id)];
		if (!v.off) continue;
		// driving in or out: the rest of its way too, even before it starts on it (the walkers step out of it meanwhile)
		const bool driving = car.phase == CarPhase::PARKING || car.phase == CarPhase::LEAVING;
		file_one(pk, p, v, car.id, driving ? &car.way : nullptr, way_from(s, car.path.s, int(car.way.size())), car.start_order,
				car.phase == CarPhase::LEAVING);
	}
	for (const Lorry &l : pk.lorries) {
		const Vehicle &v = t.vehicles[size_t(l.id)];
		if (!v.off) continue;
		const bool driving = l.phase == LorryPhase::ARRIVING || l.phase == LorryPhase::LEAVING || l.phase == LorryPhase::JOINING;
		file_one(pk, p, v, l.id, driving ? &l.way : nullptr, way_from(s, l.path.s, int(l.way.size())), l.start_order,
				driving && l.phase != LorryPhase::ARRIVING);
	}
}

uint32_t next_start(CityState &s) { return ++s.traffic.park.starts; }

bool way_has_standing(const CityState &s, int vehicle, const std::vector<SwathDisc> &way, int from) {
	const ParkingState &pk = s.traffic.park;
	const float pad = s.catalog.traffic.man_disc_pad_m;
	if (from >= int(way.size())) return false;
	const Box box = way_box(way, from, pad);
	for (const Swath &w : pk.swaths)
		if (w.vehicle != vehicle && w.order == 0 && meets(pk, w, 0, w.body, way.data() + from, int(way.size()) - from, pad, box)) return true;
	return false;
}

bool way_meets_outward(const CityState &s, int vehicle, const std::vector<SwathDisc> &way, int from) {
	const ParkingState &pk = s.traffic.park;
	const float pad = s.catalog.traffic.man_disc_pad_m;
	if (from >= int(way.size())) return false;
	const Box box = way_box(way, from, pad);
	const SwathDisc *mine = way.data() + from;
	const int n = int(way.size()) - from;
	for (const Swath &w : pk.swaths) {
		if (w.vehicle == vehicle) continue;
		if (w.order == 0 ? meets(pk, w, 0, w.body, mine, n, pad, box) : w.outward && meets(pk, w, 0, w.count, mine, n, pad, box)) return true;
	}
	return false;
}

float room_on_way(const CityState &s, int vehicle, uint32_t order, const Manoeuvre &path, const VehicleClass &c, float want) {
	const ParkingState &pk = s.traffic.park;
	const float pad = s.catalog.traffic.man_disc_pad_m;
	SwathDisc body[PROBE_POSES * BODY_DISCS]; // where its body would stand, and its gap beyond that along its way
	const float ahead[PROBE_POSES] = { want, want + c.min_gap };
	Box box;
	for (int i = 0; i < PROBE_POSES; i++) {
		const OffPose q = path.pose_at(std::min(path.length(), path.s + ahead[i]));
		body_discs(c, V2(q.x, q.z), v2_from_angle(q.angle), body + i * BODY_DISCS);
	}
	for (const SwathDisc &d : body) box.take(d, pad);
	for (const Swath &w : pk.swaths)
		if (w.vehicle != vehicle && w.order > 0 && w.order < order && meets(pk, w, 0, w.count, body, PROBE_POSES * BODY_DISCS, pad, box)) return 0.f;
	return want;
}

} // namespace traffic
} // namespace city
