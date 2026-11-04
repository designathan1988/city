// The pistol (Roadcraft actionsWiring.ts shootAt): one round along the player's line of sight. The ray stops at the
// terrain; the first walker (a standing one a body's reach from the ray, one on the ground a thin capsule along its
// length) or vehicle (an oriented box, a rider's shoulders over a two-wheeler's frame) it meets takes it. Not an undoable
// edit: walkers and traffic are not part of the saved city. A car that takes enough rounds bursts (the caller strikes).
#include "sim/commands/shoot_internal.h"

#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr uint32_t SHOT_SALT = 0x70697374U;
constexpr float LYING_R = 0.3f, LYING_HEIGHT_M = 0.2f; // a body on the ground: a capsule this thick, this high
constexpr float LYING_HEAD_SHARE = 0.35f, LYING_LEGS_SHARE = -0.1f; // along its length from its middle: the head end, where the legs begin
constexpr float EPS = 1e-6f;
constexpr float RIDER_HALF_M = 0.35f; // a two-wheeler's box is at least this wide (the rider's shoulders)

struct Found {
	float t = BIG;
	int id = -1;
	bool vehicle = false, lying = false;
	BodyPart part = BodyPart::TORSO;
	float up = 0;
};

// Closest approach of the ray (o + t d, t >= 0, d unit) to the segment a-b: t along the ray, u along the segment, distance.
void ray_segment(const float *o, const float *d, const float *a, const float *b, float &t, float &u, float &dist) {
	const float ab[3] = { b[0] - a[0], b[1] - a[1], b[2] - a[2] }, w[3] = { o[0] - a[0], o[1] - a[1], o[2] - a[2] };
	const float dd = d[0] * d[0] + d[1] * d[1] + d[2] * d[2], de = d[0] * ab[0] + d[1] * ab[1] + d[2] * ab[2];
	const float ee = ab[0] * ab[0] + ab[1] * ab[1] + ab[2] * ab[2];
	const float dw = d[0] * w[0] + d[1] * w[1] + d[2] * w[2], ew = ab[0] * w[0] + ab[1] * w[1] + ab[2] * w[2];
	const float den = dd * ee - de * de;
	u = den > EPS ? (dd * ew - de * dw) / den : 0.f;
	u = std::clamp(u, 0.f, 1.f);
	t = std::max(0.f, (de * u - dw) / dd);
	float gap = 0;
	for (int i = 0; i < 3; i++) {
		const float q = o[i] + d[i] * t - (a[i] + ab[i] * u);
		gap += q * q;
	}
	dist = std::sqrt(gap);
}

BodyPart part_standing(const BlastParams &p, float h, float side) {
	if (h > p.head_above_m) return BodyPart::HEAD;
	if (h > p.torso_above_m) return std::fabs(side) > p.arm_aside_m ? (side > 0 ? BodyPart::ARM_L : BodyPart::ARM_R) : BodyPart::TORSO;
	return side >= 0 ? BodyPart::LEG_L : BodyPart::LEG_R;
}

void aim_at_walkers(const Round &r, float limit, Found &best) {
	const TrafficState &t = r.s.traffic;
	const BlastParams &p = r.p;
	const float o[3] = { r.ox, r.oy, r.oz }, d[3] = { r.dx, r.dy, r.dz };
	for (int id = 0; id < int(t.peds.size()); id++) {
		const Pedestrian &w = t.peds[size_t(id)];
		if (!w.alive) continue;
		const float height = p.body_height_m * w.scale;
		if (float(t.time) < w.down_until) { // on the ground: along its length, head ahead of the way it faced
			const float half = height * 0.5f;
			const float a[3] = { w.cur.x - w.cur.fx * half, w.cur.y + LYING_HEIGHT_M, w.cur.z - w.cur.fz * half };
			const float b[3] = { w.cur.x + w.cur.fx * half, w.cur.y + LYING_HEIGHT_M, w.cur.z + w.cur.fz * half };
			float tt, u, dist;
			ray_segment(o, d, a, b, tt, u, dist);
			if (dist > LYING_R || tt >= std::min(best.t, limit)) continue;
			const float along = u - 0.5f, px = o[0] + d[0] * tt, pz = o[2] + d[2] * tt;
			const float side = (px - w.cur.x) * -w.cur.fz + (pz - w.cur.z) * w.cur.fx;
			best = Found{ tt, id, false, true, BodyPart::TORSO, 0.f };
			best.part = along > LYING_HEAD_SHARE ? BodyPart::HEAD : along > LYING_LEGS_SHARE ? (std::fabs(side) > p.arm_aside_m ? (side > 0 ? BodyPart::ARM_L : BodyPart::ARM_R) : BodyPart::TORSO)
																								: (side >= 0 ? BodyPart::LEG_L : BodyPart::LEG_R);
			continue;
		}
		// standing: where the ray passes nearest the axis, seen from above, and how high it is there
		const float ax = w.cur.x - r.ox, az = w.cur.z - r.oz, flat2 = r.dx * r.dx + r.dz * r.dz;
		const float tt = flat2 > EPS ? (ax * r.dx + az * r.dz) / flat2 : (w.cur.y + height * 0.5f - r.oy) / r.dy;
		if (tt <= 0.f || tt >= std::min(best.t, limit)) continue;
		const float px = r.ox + r.dx * tt, pz = r.oz + r.dz * tt, py = r.oy + r.dy * tt - w.cur.y;
		if (py < 0.f || py > height || std::hypot(px - w.cur.x, pz - w.cur.z) >= p.body_reach_m * w.scale) continue;
		const float side = (px - w.cur.x) * -w.cur.fz + (pz - w.cur.z) * w.cur.fx;
		best = Found{ tt, id, false, false, part_standing(p, py / w.scale, side), 0.f };
	}
}

void aim_at_vehicles(const Round &r, float limit, Found &best) {
	const TrafficState &t = r.s.traffic;
	const TrafficParams &tp = r.s.catalog.traffic;
	for (int id = 0; id < int(t.vehicles.size()); id++) {
		const Vehicle &v = t.vehicles[size_t(id)];
		if (!v.alive) continue;
		const bool two = two_wheeler(v.type);
		const float half_l = tp.classes[int(v.type)].length * 0.5f, half_w = std::max(tp.classes[int(v.type)].half_width, two ? RIDER_HALF_M : 0.f);
		const float height = r.p.vehicle_box_m[int(v.type)];
		// the ray in the vehicle's frame: along its heading, up, across; clipped by each pair of faces (slabs)
		const float rx = r.ox - v.cur.x, rz = r.oz - v.cur.z;
		const float o3[3] = { rx * v.cur.fx + rz * v.cur.fz, r.oy - v.cur.y, -rx * v.cur.fz + rz * v.cur.fx };
		const float d3[3] = { r.dx * v.cur.fx + r.dz * v.cur.fz, r.dy, -r.dx * v.cur.fz + r.dz * v.cur.fx };
		const float lo[3] = { -half_l, 0.f, -half_w }, hi[3] = { half_l, height, half_w };
		float t0 = 0.f, t1 = std::min(best.t, limit);
		for (int i = 0; i < 3 && t0 <= t1; i++) {
			if (std::fabs(d3[i]) < EPS) {
				if (o3[i] < lo[i] || o3[i] > hi[i]) t0 = t1 + 1.f;
				continue;
			}
			float ta = (lo[i] - o3[i]) / d3[i], tb = (hi[i] - o3[i]) / d3[i];
			if (ta > tb) std::swap(ta, tb);
			t0 = std::max(t0, ta);
			t1 = std::min(t1, tb);
		}
		if (t0 > t1 || t0 >= best.t) continue;
		best = Found{ t0, id, true, false, BodyPart::TORSO, (o3[1] + d3[1] * t0) / height };
	}
}
} // namespace

ShotResult shoot_at(CityState &s, double ox, double oy, double oz, double dx, double dy, double dz) {
	ShotResult out;
	const double len = std::sqrt(dx * dx + dy * dy + dz * dz);
	if (len < double(EPS)) return out;
	const BlastParams &p = s.catalog.blast;
	Round r{ s, p, float(ox), float(oy), float(oz), float(dx / len), float(dy / len), float(dz / len), 0, 0, 0, SHOT_SALT ^ s.damage.strikes ^ uint32_t(s.damage.shots), out };
	s.damage.shots++;
	float limit = p.max_range_m, gx, gy, gz;
	if (s.terrain.raycast(V2(r.ox, r.oz), r.oy, V2(r.dx, r.dz), r.dy, p.max_range_m, gx, gy, gz)) limit = std::hypot(std::hypot(gx - r.ox, gz - r.oz), gy - r.oy);
	Found best;
	aim_at_walkers(r, limit, best);
	aim_at_vehicles(r, limit, best);
	const float reach = best.id >= 0 ? best.t : limit;
	r.x = r.ox + r.dx * reach;
	r.y = r.oy + r.dy * reach;
	r.z = r.oz + r.dz * reach;
	DamageEvent line;
	line.kind = DamageEvent::SHOT;
	line.x = r.x; line.y = r.y; line.z = r.z;
	line.dx = r.dx; line.dz = r.dz; line.a = r.dy;
	line.u = best.id < 0 ? 0u : best.vehicle ? 2u : 1u;
	r.emit(line);
	if (best.id < 0) return out;
	if (best.vehicle) shoot_vehicle(r, best.id, best.up, r.x, r.y, r.z);
	else shoot_walker(r, best.id, best.part, r.x, r.y, r.z, best.lying);
	return out;
}

} // namespace city
