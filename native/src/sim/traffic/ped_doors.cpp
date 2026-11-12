// The front doors the walkers use, filed by the sidewalk each one opens onto. The lots of the walking window are read a few a
// step (a door needs the lot's body, sim/buildings/lot_doors.h), so nothing stalls and nothing far off is ever read; a door
// whose building went is dropped when it is drawn, and one that grew is found at the next look round the window.
#include "sim/traffic/traffic_system.h"

#include "sim/buildings/b40/civic.h"
#include "sim/buildings/b40/record.h"
#include "sim/buildings/lot_doors.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr int ARC_REFINES = 3; // a curve's parameter to its arc: each pass takes the error to a fraction
constexpr float FACES_STREET = 0.7071f; // a lot's front lies within 45 degrees of its street: else it is the corner's other street

PlaceKind kind_of(const Building &rec, const b40::Body &b) {
	if (rec.fn == FN_SUPERMARKET || rec.fn == FN_MALL || rec.fn == FN_GAS_STATION) return PlaceKind::SHOP;
	if (rec.fn != FN_NONE) return PlaceKind::CIVIC;
	if (b.kind == b40::KIND_OFFICE || b.use == b40::USE_INDUSTRIAL) return PlaceKind::WORK;
	return b.use == b40::USE_COMMERCIAL ? PlaceKind::SHOP : PlaceKind::HOME;
}

// The sidewalk under `at`: its segment, side, arc and place across. False when none is within reach.
bool sidewalk_under(const CityState &s, V2 at, V2 front, PedDoor &d, int &seg, int &side) {
	double t = 0, dist = 0;
	seg = s.roads.find_segment_near(at.x, at.y, s.catalog.traffic.ped_door_reach_m, t, dist);
	if (seg < 0 || !walkable(s, seg)) return false;
	const RoadSegment &g = s.roads.segs[size_t(seg)];
	float arc = float(t * g.len);
	Pose mid;
	for (int k = 0; k < ARC_REFINES; k++) {
		sidewalk_pose(s, seg, 0, arc, 0.f, mid);
		arc = clampf(arc + (at.x - mid.x) * mid.fx + (at.y - mid.z) * mid.fz, float(g.retreat_a), float(g.len - g.retreat_b));
	}
	if (std::fabs(mid.fx * front.x + mid.fz * front.y) < FACES_STREET) return false;
	const Profile &pf = s.roads.profile_of(seg);
	const float across = (at.x - mid.x) * mid.fz - (at.y - mid.z) * mid.fx + pf.walk_right; // from the street's middle line
	side = pf.has_walk_right && (!pf.has_walk_left || std::fabs(across - pf.walk_right) < std::fabs(across - pf.walk_left)) ? 0 : 1;
	if (side == 1 && !pf.has_walk_left) return false;
	d.s = arc;
	sidewalk_pose(s, seg, side, arc, 0.f, mid);
	const float half = walk_half(s, seg, side);
	d.u = clampf((at.x - mid.x) * mid.fz - (at.y - mid.z) * mid.fx, -half, half);
	return true;
}

void file_door(CityState &s, int id) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const Building &rec = s.buildings.items[size_t(id)];
	if (!rec.alive || rec.floors == 0 || b40::is_open_lot(rec.fn)) return;
	b40::Body body;
	b40::record_mass(s.buildings.designs, rec, body);
	LotDoor way;
	const b40::LotFrame frame = b40::lot_frame(rec);
	if (!lot_door(body, frame, p.ped_door_off_m, p.ped_gate_out_m, way)) return;
	PedDoor d;
	int seg = -1, side = 0;
	if (!sidewalk_under(s, way.foot, frame.e0, d, seg, side)) return;
	d.building = id; d.segment = seg; d.segment_generation = s.roads.segment_generation(seg);
	d.source_version = s.buildings.item_version(id);
	d.kind = uint8_t(kind_of(rec, body));
	d.x = way.door.x;
	d.z = way.door.y;
	d.y = s.terrain.height_at(way.door.x, way.door.y);
	auto &list = t.doors[size_t(seg * 2 + side)];
	const size_t key = size_t(seg * 2 + side);
	if (t.door_side_pos[key] < 0) { t.door_side_pos[key] = int(t.door_sides.size()); t.door_sides.push_back(int(key)); }
	list.push_back(d);
}

void forget_seen(TrafficState &t, const PedDoor &d) {
	if (size_t(d.building) < t.door_seen.size() && t.door_seen[size_t(d.building)] == d.source_version)
		t.door_seen[size_t(d.building)] = 0; // an old door must not dirty a newer prepared revision
}

} // namespace

bool door_current(const CityState &s, const PedDoor &d) {
	const uint64_t version = s.buildings.item_version(d.building);
	return version != 0 && version == d.source_version && d.segment_generation != 0 && s.roads.segment_generation(d.segment) == d.segment_generation;
}

void list_doors(CityState &s, float cx, float cz, float reach) {
	TrafficState &t = s.traffic;
	t.door_seen.resize(s.buildings.items.size(), 0);
	t.door_todo.clear();
	s.buildings.query(cx - reach, cz - reach, cx + reach, cz + reach, [&](int id) {
		const uint64_t version = s.buildings.item_version(id);
		if (version != 0 && t.door_seen[size_t(id)] != version) t.door_todo.push_back({id, version});
	});
}

void read_doors(CityState &s) {
	TrafficState &t = s.traffic;
	t.doors.resize(s.roads.segs.size() * 2);
	t.door_side_pos.resize(t.doors.size(), -1);
	t.door_seen.resize(s.buildings.items.size(), 0);
	for (int k = 0; k < int(s.catalog.traffic.ped_door_budget) && !t.door_todo.empty(); k++) {
		const PedDoorRead read = t.door_todo.back();
		t.door_todo.pop_back();
		if (size_t(read.building) >= t.door_seen.size() || read.version == 0 ||
			s.buildings.item_version(read.building) != read.version || t.door_seen[size_t(read.building)] == read.version) continue;
		t.door_seen[size_t(read.building)] = read.version;
		file_door(s, read.building);
	}
}

void forget_doors(CityState &s, int seg) {
	TrafficState &t = s.traffic;
	for (int key : { seg * 2, seg * 2 + 1 }) {
		if (size_t(key) >= t.doors.size()) continue;
		for (const PedDoor &d : t.doors[size_t(key)])
			forget_seen(t, d);
		t.doors[size_t(key)].clear();
	}
}

int door_sidewalk(CityState &s) {
	TrafficState &t = s.traffic;
	while (!t.door_sides.empty()) {
		const size_t k = size_t(t.rng.next() % uint32_t(t.door_sides.size()));
		const int key = t.door_sides[k];
		if (size_t(key) < t.doors.size()) {
			auto &list = t.doors[size_t(key)];
			while (!list.empty() && !door_current(s, list.back())) {
				forget_seen(t, list.back()); list.pop_back();
			}
			if (!list.empty()) return key;
		}
		if (size_t(key) < t.door_side_pos.size()) t.door_side_pos[size_t(key)] = -1;
		t.door_sides[k] = t.door_sides.back();
		t.door_sides.pop_back();
		if (k < t.door_sides.size() && size_t(t.door_sides[k]) < t.door_side_pos.size()) t.door_side_pos[size_t(t.door_sides[k])] = int(k);
	}
	return -1;
}

bool pick_door(CityState &s, const PeopleNow &now, int seg, int side, bool out, float from, float to, PedDoor &door) {
	TrafficState &t = s.traffic;
	const size_t key = size_t(seg * 2 + side);
	if (key >= t.doors.size()) return false;
	auto &list = t.doors[key];
	float sum = 0;
	bool found = false;
	for (size_t k = 0; k < list.size();) {
		const PedDoor &d = list[k];
		if (!door_current(s, d)) { // it went: its lot is read again when the window next looks
			forget_seen(t, d);
			list[k] = list.back();
			list.pop_back();
			continue;
		}
		k++;
		if (d.s < from || d.s > to) continue;
		const float w = now.per_m[d.kind] * (out ? now.out[d.kind] : 1 - now.out[d.kind]);
		sum += w;
		if (w > 0 && t.rng.f() * sum < w) { // drawn by what each has on its street now
			door = d;
			found = true;
		}
	}
	if (found) door.y = s.terrain.height_at(door.x, door.z); // height is not building identity
	return found;
}

} // namespace traffic
} // namespace city
