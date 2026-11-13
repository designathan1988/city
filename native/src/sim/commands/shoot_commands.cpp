// The pistol (Roadcraft actionsWiring.ts shootAt): one round along the player's line of sight. The ray stops at the
// terrain; the first walker (a standing one a body's reach from the ray, one on the ground a thin capsule along its
// length), dead body lying (the same capsule: ped_dead.cpp) or vehicle (an oriented box, a rider's shoulders over a two-wheeler's frame) it meets takes it. Not an undoable
// edit: walkers and traffic are not part of the saved city. A car that takes enough rounds bursts (the caller strikes).
#include "sim/commands/shoot_internal.h"

#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr uint32_t SHOT_SALT = 0x70697374U;
using traffic::LYING_HEIGHT_M; // a body on the ground: the capsule ped_system.h states, which the crowd goes round
using traffic::LYING_R;
constexpr float LYING_HEAD_SHARE = 0.35f, LYING_LEGS_SHARE = -0.1f; // along its length from its middle: the head end, where the legs begin
constexpr float EPS = 1e-6f;
constexpr float RIDER_HALF_M = 0.35f; // a two-wheeler's box is at least this wide (the rider's shoulders)

struct Found {
	float t = BIG;
	int id = -1;
	bool vehicle = false, lying = false;
	BodyPart part = BodyPart::TORSO;
	float up = 0;
	int dead = -1; // a dead body met (TrafficState::dead), id -1 then
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

// A body on the ground (ped_system.h lying), its middle `mid` at height `y`, lying the way (lx, lz) and `half` long either way, its head at
// the far end: whether the round meets it nearer than `nearest`, how far along the round (tt) and on which part.
bool lying_hit(const Round &r, V2 mid, float y, float lx, float lz, float half, float nearest, float &tt, BodyPart &part) {
	const float o[3] = { r.ox, r.oy, r.oz }, d[3] = { r.dx, r.dy, r.dz };
	const float a[3] = { mid.x - lx * half, y + LYING_HEIGHT_M, mid.y - lz * half };
	const float b[3] = { mid.x + lx * half, y + LYING_HEIGHT_M, mid.y + lz * half };
	float u, dist;
	ray_segment(o, d, a, b, tt, u, dist);
	if (dist > LYING_R || tt >= nearest) return false;
	const float along = u - 0.5f, px = o[0] + d[0] * tt, pz = o[2] + d[2] * tt;
	const float side = (px - mid.x) * -lz + (pz - mid.y) * lx;
	part = along > LYING_HEAD_SHARE ? BodyPart::HEAD : along > LYING_LEGS_SHARE ? (std::fabs(side) > r.p.arm_aside_m ? (side > 0 ? BodyPart::ARM_L : BodyPart::ARM_R) : BodyPart::TORSO)
																					: (side >= 0 ? BodyPart::LEG_L : BodyPart::LEG_R);
	return true;
}

// The dead lying (TrafficState::dead), each the capsule of somebody lying.
void aim_at_dead(const Round &r, float limit, Found &best) {
	const std::deque<DeadBody> &dead = r.s.traffic.dead;
	for (int k = 0; k < int(dead.size()); k++) {
		const DeadBody &b = dead[size_t(k)];
		float tt;
		BodyPart part;
		if (!lying_hit(r, traffic::dead_middle(r.p, b), b.y, b.fall_x, b.fall_z, traffic::dead_half(r.p, b), std::min(best.t, limit), tt, part)) continue;
		best = Found{ tt, -1, false, true, part, 0.f, k };
	}
}

void aim_at_walkers(const Round &r, float limit, Found &best) {
	const TrafficState &t = r.s.traffic;
	const BlastParams &p = r.p;
	for (int id = 0; id < int(t.peds.size()); id++) {
		const Pedestrian &w = t.peds[size_t(id)];
		if (!w.alive) continue;
		const float height = p.body_height_m * w.scale;
		if (traffic::lying(t, w)) { // on the ground: from its feet the way it went down, the head at the far end (ped_system.h lying)
			float tt;
			BodyPart part;
			if (lying_hit(r, traffic::lying_middle(p, w), w.cur.y, w.fall_x, w.fall_z, traffic::lying_half(p, w), std::min(best.t, limit), tt, part))
				best = Found{ tt, id, false, true, part, 0.f };
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
	const bool landed = s.terrain.raycast(V2(r.ox, r.oz), r.oy, V2(r.dx, r.dz), r.dy, p.max_range_m, gx, gy, gz);
	if (landed) limit = std::hypot(std::hypot(gx - r.ox, gz - r.oz), gy - r.oy);
	Found best;
	aim_at_walkers(r, limit, best);
	aim_at_dead(r, limit, best);
	aim_at_vehicles(r, limit, best);
	const bool met = best.id >= 0 || best.dead >= 0;
	const float reach = met ? best.t : limit;
	r.x = r.ox + r.dx * reach;
	r.y = r.oy + r.dy * reach;
	r.z = r.oz + r.dz * reach;
	DamageEvent line;
	line.kind = DamageEvent::SHOT;
	line.x = r.x; line.y = r.y; line.z = r.z;
	line.dx = r.dx; line.dz = r.dz; line.a = r.dy;
	line.u = !met ? 0u : best.vehicle ? 2u : 1u;
	r.emit(line);
	if (best.dead >= 0) shoot_dead(r, best.dead, best.part, r.x, r.y, r.z);
	else if (best.id >= 0 && best.vehicle) shoot_vehicle(r, best.id, best.up, r.x, r.y, r.z);
	else if (best.id >= 0) shoot_walker(r, best.id, best.part, r.x, r.y, r.z, best.lying);
	// Heard where it lands, whatever it lands on; a round into the sky lands nowhere near anybody.
	if (met || landed) traffic::startle_pedestrians(s, r.x, r.z, p.startle_m, s.catalog.traffic.ped_shot_fright_s);
	return out;
}

} // namespace city
