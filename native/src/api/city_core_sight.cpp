// The camera's line of sight against the buildings' volumes: how much of it is clear, how much of a pull in front of a building it asks
// (the Roadcraft's isoViewport), and the eye taken out of a building at any distance. The volumes come from each record's body as the
// simulation keeps it (sim/buildings/b40/body_cache.h).
#include "api/city_core.h"

#include "sim/buildings/b40/record.h"

#include <algorithm>
#include <cmath>
#include <limits>

using namespace godot;

namespace city {

namespace {
constexpr float NO_ROOF = -std::numeric_limits<float>::infinity();
constexpr int EXIT_STEPS = 20; // halvings of the line to the point where it leaves a building
constexpr float POINT_BOX_M = 1.f; // the box asked for what stands on a point

std::vector<Solid> solids_of(const CityState &s, const Building &rec) {
	std::vector<Solid> out;
	if (rec.floors == 0) return out;
	const std::shared_ptr<const b40::Body> b_held = b40::record_mass(s.buildings.designs, rec);
	const b40::Body &b = *b_held;
	const b40::LotFrame f = b40::lot_frame(rec);
	for (const b40::Volume &v : b.volumes) out.push_back({ f, v.x, v.y, v.x + v.w, v.y + v.d, b.block_z(v, v.base + v.storeys) });
	return out;
}

// The volumes of the buildings in the box.
std::vector<Solid> solids_in(const CityState &s, float x0, float z0, float x1, float z1) {
	std::vector<Solid> near;
	s.buildings.query(x0, z0, x1, z1, [&](int id) {
		const std::vector<Solid> own = solids_of(s, s.buildings.items[id]);
		near.insert(near.end(), own.begin(), own.end());
	});
	return near;
}

std::vector<Solid> solids_along(const CityState &s, const Vector3 &a, const Vector3 &b) {
	return solids_in(s, std::min(a.x, b.x), std::min(a.z, b.z), std::max(a.x, b.x), std::max(a.z, b.z));
}

// The top of what stands at (x, z), world height; NO_ROOF on open ground.
float roof_at(const CityState &s, const std::vector<Solid> &near, float x, float z) {
	const float ground = s.terrain.height_at(x, z);
	float top = NO_ROOF;
	for (const Solid &v : near) {
		const V2 rel = V2(x, z) - v.f.o;
		const float lx = rel.dot(v.f.e0), ly = rel.dot(v.f.e1);
		if (lx >= v.x0 && lx <= v.x1 && ly >= v.y0 && ly <= v.y1) top = std::max(top, ground + v.top);
	}
	return top;
}

bool within(const CityState &s, const std::vector<Solid> &near, const Vector3 &p) {
	return p.y < roof_at(s, near, p.x, p.z);
}
} // namespace

// How far from `from` towards `to` the line of sight stays out of the buildings' volumes, as a share of it, in `samples` steps.
float CityCore::clear_reach(const Vector3 &from, const Vector3 &to, int samples) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	const std::vector<Solid> near = solids_along(s, from, to);
	for (int i = 1; i <= samples; i++)
		if (within(s, near, from.lerp(to, float(i) / float(samples)))) return float(i - 1) / float(samples);
	return 1.f;
}

// The share of the line from `eye` to `focus` a pull takes off, 0..1. None while the point looked at is itself in a building: there is nothing
// to bring into view. Each sample in a building asks to pass it, by how deep it is up to `soft` metres, so a line that grazes a roof moves the
// eye a little and never all at once.
float CityCore::sight_pull(const Vector3 &focus, const Vector3 &eye, int samples, float soft) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	const std::vector<Solid> near = solids_along(s, focus, eye);
	if (within(s, near, focus)) return 0.f;
	float wanted = 0.f;
	for (int i = 0; i < samples; i++) {
		const float t = float(i) / float(samples);
		const Vector3 p = eye.lerp(focus, t);
		const float deep = roof_at(s, near, p.x, p.z) - p.y;
		if (deep <= 0.f) continue;
		const float k = std::min(1.f, deep / soft);
		wanted = std::max(wanted, k * k * (3.f - 2.f * k) * std::min(1.f, t + 1.f / float(samples)));
	}
	return wanted;
}

// [focus, eye] with the eye out of any building, at every distance: along the line to where it leaves the walls, or over the roof when the point
// looked at is inside one too (then the point rises with it, `lift` over that roof). An eye in the open costs one look at its own spot.
PackedVector3Array CityCore::eye_free(const Vector3 &focus, const Vector3 &eye, float lift) {
	PackedVector3Array out;
	Vector3 at = focus, from = eye;
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	if (!s.ready) { // the camera stands before there is a city
		out.push_back(at);
		out.push_back(from);
		return out;
	}
	const std::vector<Solid> here = solids_in(s, eye.x - POINT_BOX_M, eye.z - POINT_BOX_M, eye.x + POINT_BOX_M, eye.z + POINT_BOX_M);
	if (within(s, here, from)) {
		const std::vector<Solid> near = solids_along(s, focus, eye);
		if (within(s, near, at)) {
			const float rise = roof_at(s, near, at.x, at.z) + lift - at.y;
			at.y += rise;
			from.y += rise;
		}
		if (within(s, near, from) && !within(s, near, at)) {
			float lo = 0.f, hi = 1.f;
			for (int i = 0; i < EXIT_STEPS; i++) {
				const float mid = (lo + hi) / 2;
				(within(s, near, from.lerp(at, mid)) ? lo : hi) = mid;
			}
			from = from.lerp(at, hi);
		} else if (within(s, near, from)) {
			from.y = roof_at(s, near, from.x, from.z);
		}
	}
	out.push_back(at);
	out.push_back(from);
	return out;
}

} // namespace city
