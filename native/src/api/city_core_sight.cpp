// The camera's line of sight against the buildings' volumes: how much of it is clear, how much of a pull in front of a building it asks
// (the Roadcraft's isoViewport), the eye taken out of a building at any distance, and the highest roof near a point (where a photo of the
// roofs is aimed). The volumes come from each record's body as the simulation keeps it (sim/buildings/b40/body_cache.h), stood where
// they are drawn: on the lot's floor (the highest ground under the body and its plinth, b40::lot_floor), each with its own plan and roof.
#include "api/city_core.h"

#include "sim/buildings/b40/plan_area.h"
#include "sim/buildings/b40/record.h"
#include "sim/buildings/b40/roof_shape.h"

#include <algorithm>
#include <cmath>
#include <limits>

using namespace godot;

namespace city {

namespace {
constexpr float NO_ROOF = -std::numeric_limits<float>::infinity();
constexpr int EXIT_STEPS = 20; // halvings of the line to the point where it leaves a building
constexpr float POINT_BOX_M = 1.f; // the box asked for what stands on a point
constexpr float MIDDLE = 0.5f; // of a volume's box: where its roof is highest on a symmetric roof, and where a photo aims
constexpr float MARCH_M = 0.5f; // the arm's step out of a building: a quarter of the narrowest block side (tuning.json build.min_side_m)

// A building's volume as it stands: the lot's frame, its plan there, its roof's shape, the world height of its eaves and of the
// lot's floor it stands on (its foot).
struct Solid {
	b40::LotFrame f;
	b40::Ring ring;
	b40::Volume v;
	float eaves;
	float foot;
};

// How far round the solids the eye's floor eases down, how far over a roof it keeps, how far it steps back out of the margin (the
// rig's camera.json: eye_wall_reach_m, eye_roof_clearance_m, eye_step_back_m).
struct Room {
	float reach, clearance, back;
};

std::vector<Solid> solids_of(const CityState &s, const Building &rec) {
	std::vector<Solid> out;
	if (rec.floors == 0) return out;
	const std::shared_ptr<const b40::Body> b_held = b40::record_mass(s.buildings.designs, rec);
	const b40::Body &b = *b_held;
	const b40::LotFrame f = b40::lot_frame(rec);
	const float floor = b40::lot_floor(b, rec, f, [&](float x, float z) { return s.terrain.height_at(x, z); });
	for (const b40::Volume &v : b.volumes) out.push_back({ f, b40::ring_of(v), v, floor + b.block_z(v, v.top()), floor });
	return out;
}

// The lowest the eye may stand at (x, z): over a volume's plan its highest roof plus the clearance; within `reach` round its box, that
// floor easing down to the volume's foot by a smoothstep of the distance (the camera's radius: the eye meets a wall's margin before
// the wall, so the rig slides along it instead of striking it); NO_ROOF farther from every volume.
float floor_at(const std::vector<Solid> &near, float x, float z, const Room &room) {
	float floor = NO_ROOF;
	for (const Solid &v : near) {
		const float top = v.eaves + b40::roof_rise(v.v) + room.clearance;
		if (top <= floor) continue;
		const V2 rel = V2(x, z) - v.f.o;
		const float lx = rel.dot(v.f.e0), ly = rel.dot(v.f.e1);
		const float ox = std::max({ v.v.x - lx, 0.f, lx - (v.v.x + v.v.w) }), oy = std::max({ v.v.y - ly, 0.f, ly - (v.v.y + v.v.d) });
		const float d = std::hypot(ox, oy);
		if (d >= room.reach) continue;
		const float k = d / room.reach;
		floor = std::max(floor, top - (top - v.foot) * k * k * (3.f - 2.f * k));
	}
	return floor;
}

// The volumes of every building whose lot meets the box, wherever its centre stands (BuildingRecords::meeting).
std::vector<Solid> solids_in(const CityState &s, float x0, float z0, float x1, float z1) {
	std::vector<Solid> near;
	s.buildings.meeting(x0, z0, x1, z1, [&](int id) {
		const std::vector<Solid> own = solids_of(s, s.buildings.items[id]);
		near.insert(near.end(), own.begin(), own.end());
	});
	return near;
}

std::vector<Solid> solids_along(const CityState &s, const Vector3 &a, const Vector3 &b) {
	return solids_in(s, std::min(a.x, b.x), std::min(a.z, b.z), std::max(a.x, b.x), std::max(a.z, b.z));
}

// The top of what stands at (x, z), world height: its roof's surface where its plan holds the point; NO_ROOF on open ground.
float roof_at(const std::vector<Solid> &near, float x, float z) {
	float top = NO_ROOF;
	for (const Solid &v : near) {
		const V2 rel = V2(x, z) - v.f.o;
		const float lx = rel.dot(v.f.e0), ly = rel.dot(v.f.e1);
		if (b40::contains_point(v.ring, { lx, ly })) top = std::max(top, v.eaves + b40::roof_height_at(v.v, lx, ly));
	}
	return top;
}

bool within(const std::vector<Solid> &near, const Vector3 &p) {
	return p.y < roof_at(near, p.x, p.z);
}

// The highest point of the solids' roofs, over the middle of the volume it tops (a symmetric roof's ridge, a flat roof's middle).
bool highest_of(const std::vector<Solid> &near, Vector3 &best) {
	bool found = false;
	for (const Solid &v : near) {
		const float top = v.eaves + b40::roof_rise(v.v);
		if (found && top <= best.y) continue;
		const V2 at = v.f.at(v.v.x + v.v.w * MIDDLE, v.v.y + v.v.d * MIDDLE);
		best = Vector3(at.x, top, at.y);
		found = true;
	}
	return found;
}
} // namespace

// How far from `from` towards `to` the line of sight stays out of the buildings' volumes, as a share of it, in `samples` steps.
float CityCore::clear_reach(const Vector3 &from, const Vector3 &to, int samples) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	const std::vector<Solid> near = solids_along(s, from, to);
	for (int i = 1; i <= samples; i++)
		if (within(near, from.lerp(to, float(i) / float(samples)))) return float(i - 1) / float(samples);
	return 1.f;
}

// The share of the line from `eye` to `focus` a pull takes off, 0..1. None while the point looked at is itself in a building: there is nothing
// to bring into view. Each sample in a building asks to pass it, by how deep it is up to `soft` metres, so a line that grazes a roof moves the
// eye a little and never all at once.
float CityCore::sight_pull(const Vector3 &focus, const Vector3 &eye, int samples, float soft) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	const std::vector<Solid> near = solids_along(s, focus, eye);
	if (within(near, focus)) return 0.f;
	float wanted = 0.f;
	for (int i = 0; i < samples; i++) {
		const float t = float(i) / float(samples);
		const Vector3 p = eye.lerp(focus, t);
		const float deep = roof_at(near, p.x, p.z) - p.y;
		if (deep <= 0.f) continue;
		const float k = std::min(1.f, deep / soft);
		wanted = std::max(wanted, k * k * (3.f - 2.f * k) * std::min(1.f, t + 1.f / float(samples)));
	}
	return wanted;
}

namespace {
// Where along the line from `eye` (t 0) towards `focus` (t 1) the eye first leaves the walls, marched from the eye and halved down;
// -1 when it never does.
float first_exit(const std::vector<Solid> &near, const Vector3 &eye, const Vector3 &focus) {
	const int steps = std::max(1, int(std::ceil(focus.distance_to(eye) / MARCH_M)));
	for (int i = 1; i <= steps; i++) {
		if (within(near, eye.lerp(focus, float(i) / float(steps)))) continue;
		float lo = float(i - 1) / float(steps), hi = float(i) / float(steps); // in at lo, out at hi
		for (int k = 0; k < EXIT_STEPS; k++) {
			const float mid = (lo + hi) / 2;
			(within(near, eye.lerp(focus, mid)) ? lo : hi) = mid;
		}
		return hi;
	}
	return -1.f;
}

// The eye out of the walls but inside a wall's margin (under the floor): along its line, the way its clearance over the floor grows, to
// the margin's edge, up to `room.back` metres: towards the point when the wall stands behind the eye, back away from it when the wall
// stands between them. Unmoved when that way meets a wall or stays under the floor all its length. The margin's edge moves with the eye,
// so the eye slides along it instead of striking the wall (beyond the reference, whose eye only steps back: a wall behind the eye met
// it at full speed).
Vector3 out_of_margin(const std::vector<Solid> &near, const Vector3 &eye, const Vector3 &focus, const Room &room) {
	const auto clear = [&](const Vector3 &p) { return p.y - floor_at(near, p.x, p.z, room); };
	const float length = std::max(eye.distance_to(focus), MARCH_M);
	const float probe = MARCH_M / length; // one step of the march, as a share of the line (t below 0 lies behind the eye)
	const float way = clear(eye.lerp(focus, probe)) >= clear(eye.lerp(focus, -probe)) ? 1.f : -1.f;
	const float end = way * std::min(room.back / length, 1.f); // never past the point looked at
	const int steps = std::max(1, int(std::ceil(std::abs(end) / probe)));
	for (int i = 1; i <= steps; i++) {
		const Vector3 p = eye.lerp(focus, end * float(i) / float(steps));
		if (within(near, p)) return eye;
		if (clear(p) < 0.f) continue;
		float under = end * float(i - 1) / float(steps), out = end * float(i) / float(steps);
		for (int k = 0; k < EXIT_STEPS; k++) {
			const float mid = (under + out) / 2;
			(clear(eye.lerp(focus, mid)) < 0.f ? under : out) = mid;
		}
		return eye.lerp(focus, out);
	}
	return eye;
}
} // namespace

// [focus, eye] by the reference's rules (its isoViewport), in this order: the eye and the point both inside walls: both rise over the
// point's roof plus `lift` (the point rises only then); the eye inside walls and the point not: the eye comes in along the line to where
// it first leaves them; still inside: the eye over the floor; out of the walls but inside a wall's margin: along the line to the margin's
// edge (out_of_margin), so the eye meets a wall's margin and slides along it instead of striking the wall. `room`: (the margin's reach,
// the clearance over a roof, how far the eye moves out of a margin), the rig's camera.json scaled with the view; a `room.z` of 0 asks the
// walls alone (rules 1 to 3), as the rig does for the eye it has eased. An eye in the open costs one look round its own spot.
PackedVector3Array CityCore::eye_free(const Vector3 &focus, const Vector3 &eye, float lift, const Vector3 &room_m) {
	PackedVector3Array out;
	Vector3 at = focus, from = eye;
	const Room room{ room_m.x, room_m.y, room_m.z };
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	const float pad = room.reach + POINT_BOX_M;
	const std::vector<Solid> here = s.ready ? solids_in(s, eye.x - pad, eye.z - pad, eye.x + pad, eye.z + pad) : std::vector<Solid>{};
	if (within(here, from) || from.y < floor_at(here, from.x, from.z, room)) { // else the eye stands clear: nothing to do
		const float wide = pad + room.back; // the margin's move may take the eye this far past its own spot
		std::vector<Solid> near = solids_in(s, std::min(focus.x - pad, eye.x - wide), std::min(focus.z - pad, eye.z - wide),
				std::max(focus.x + pad, eye.x + wide), std::max(focus.z + pad, eye.z + wide));
		if (within(near, from) && within(near, at)) {
			const float rise = roof_at(near, at.x, at.z) + lift - at.y;
			at.y += rise;
			from.y += rise;
		}
		if (within(near, from) && !within(near, at)) {
			const float t = first_exit(near, from, at);
			if (t >= 0.f) from = from.lerp(at, t);
		}
		if (within(near, from)) from.y = std::max(from.y, floor_at(near, from.x, from.z, room));
		const bool margin = room.back > 0.f && !within(near, from) && from.y < floor_at(near, from.x, from.z, room);
		if (margin) from = out_of_margin(near, from, at, room);
	}
	out.push_back(at);
	out.push_back(from);
	return out;
}

// The lowest the eye may stand at (x, z) (floor_at above): what the rig measures the near plane's room by. -inf on open ground.
float CityCore::eye_floor(float x, float z, const Vector3 &room_m) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	if (!s.ready) return NO_ROOF;
	const Room room{ room_m.x, room_m.y, room_m.z };
	const float pad = room.reach + POINT_BOX_M;
	return floor_at(solids_in(s, x - pad, z - pad, x + pad, z + pad), x, z, room);
}

// The highest roof within `radius` metres of (x, z): {"hit": true, "position": the top over the middle of the volume it crowns}, or
// {"hit": false} with no building there. A plan aims a photo of the roofs at it (look.gd setViewRoof): the ground under a town's centre
// is its streets, and the roofs stand tens of metres over them.
Dictionary CityCore::highest_roof(float x, float z, float radius) {
	Dictionary d;
	d["hit"] = false;
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	if (!s.ready) return d;
	Vector3 best;
	if (!highest_of(solids_in(s, x - radius, z - radius, x + radius, z + radius), best)) return d;
	d["hit"] = true;
	d["position"] = best;
	return d;
}

} // namespace city
