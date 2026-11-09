// The nearest building face under a ray (body_pick.h): records near the ray by the store's hash, then each block's sides and
// top, its roof's things and the lot's free parts as boxes, in its lot's frame. A body is made only for a record the ray passes over.
#include "sim/commands/body_pick.h"

#include "core/math.h"
#include "sim/buildings/b40/gen.h"
#include "sim/buildings/b40/plan_area.h"
#include "sim/buildings/b40/record.h"
#include "sim/buildings/b40/roof_shape.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace city {

using namespace b40;

namespace {
constexpr float GRAZING = 1e-6f; // a ray this parallel to a face misses it
constexpr int ROOF_STEPS = 16; // a slope is walked in this many heights: a click lands within a sixteenth of the ridge

// The ray in a lot's frame: x along the front, y away from the street, z over the building's floor.
struct LocalRay {
	float ox, oy, oz, dx, dy, dz;
};

LocalRay in_lot(const Ray3 &r, const LotFrame &f, float floor) {
	V2 rel = V2(r.ox, r.oz) - f.o, dir(r.dx, r.dz);
	return { rel.x * f.e0.x + rel.y * f.e0.y, rel.x * f.e1.x + rel.y * f.e1.y, r.oy - floor,
		dir.x * f.e0.x + dir.y * f.e0.y, dir.x * f.e1.x + dir.y * f.e1.y, r.dy };
}

int storey_at(const Body &b, const Volume &v, float z) {
	for (int k = v.storeys - 1; k > 0; k--)
		if (z >= b.block_z(v, v.base + k)) return k;
	return 0;
}

void hit_sides(const Body &b, const Volume &v, int vol, const LocalRay &r, BodyHit &best) {
	const Ring ring = ring_of(v);
	float z0 = b.block_z(v, v.base), z1 = b.block_z(v, v.top());
	for (size_t i = 0; i < ring.size(); i++) {
		V2 p = ring[i], q = ring[(i + 1) % ring.size()];
		float ex = q.x - p.x, ey = q.y - p.y, len = std::hypot(ex, ey);
		if (len < EPS) continue;
		ex /= len;
		ey /= len;
		float nx = ey, ny = -ex, facing = r.dx * nx + r.dy * ny; // outward, as the walls are drawn (placed.cpp sides_of)
		if (facing > -GRAZING) continue;
		float t = ((p.x - r.ox) * nx + (p.y - r.oy) * ny) / facing;
		if (t <= 0 || t >= best.t) continue;
		float along = (r.ox + r.dx * t - p.x) * ex + (r.oy + r.dy * t - p.y) * ey, z = r.oz + r.dz * t;
		if (along < 0 || along > len || z < z0 || z > z1) continue;
		int side = int(i), storey = storey_at(b, v, z), bays = bays_of(v, side);
		best.volume = vol;
		best.side = side;
		best.storey = storey;
		best.level = v.base + storey;
		best.bay = std::min(bays - 1, int(along / (len / float(bays))));
		best.t = t;
	}
}

void hit_top(const RoofShape &K, const Body &b, const Volume &v, int vol, const LocalRay &r, BodyHit &best) {
	if (r.dz > -GRAZING) return;
	// Down from the ridge's height to the eaves, the first point under the roof's surface: a pitched roof is hit on its slope.
	float eaves = b.block_z(v, v.top()), rise = roof_rise(K, v), t = -1;
	const Ring ring = ring_of(v);
	for (int i = 0; i <= ROOF_STEPS; i++) {
		float z = eaves + rise * (1 - float(i) / float(ROOF_STEPS)), at = (z - r.oz) / r.dz, x = r.ox + r.dx * at, y = r.oy + r.dy * at;
		if (at <= 0 || !contains_point(ring, { x, y })) continue;
		if (z <= eaves + roof_height_at(K, v, x, y) + EPS) {
			t = at;
			break;
		}
	}
	if (t <= 0 || t >= best.t) return;
	best.volume = vol;
	best.side = -1;
	best.storey = v.storeys - 1;
	best.level = v.top() - 1;
	best.t = t;
}

// Where the ray enters a box standing on the plan at (cx, cy), `hw` by `hd` about its middle, turned by `turn`, from z0 up to z1;
// -1 for a miss (the slab test, in the box's own frame).
float box_hit(const LocalRay &r, float cx, float cy, float hw, float hd, float turn, float z0, float z1) {
	const float c = std::cos(turn), s = std::sin(turn), px = r.ox - cx, py = r.oy - cy;
	const float o[3] = { px * c + py * s, -px * s + py * c, r.oz - (z0 + z1) / 2 }, d[3] = { r.dx * c + r.dy * s, -r.dx * s + r.dy * c, r.dz };
	const float half[3] = { hw, hd, (z1 - z0) / 2 };
	float enter = 0, leave = FAR_AWAY;
	for (int k = 0; k < 3; k++) {
		if (std::fabs(d[k]) < GRAZING) {
			if (std::fabs(o[k]) > half[k]) return -1;
			continue;
		}
		float a = (-half[k] - o[k]) / d[k], b = (half[k] - o[k]) / d[k];
		if (a > b) std::swap(a, b);
		enter = std::max(enter, a);
		leave = std::min(leave, b);
		if (enter > leave) return -1;
	}
	return enter > 0 ? enter : -1;
}

// The lot's free parts and each roof's things, nearer than what was hit: they stand in front of the walls they are set against.
void hit_things(const RoofShape &K, const Body &b, const LocalRay &r, BodyHit &best) {
	for (int i = 0; i < int(b.elements.size()); i++) {
		const LotElement &e = b.elements[size_t(i)];
		const float t = box_hit(r, e.x, e.y, e.w / 2, e.d / 2, e.angle, e.z, e.z + e.base + e.h); // `base`: a fence's wall under it, a flight's start
		if (t <= 0 || t >= best.t) continue;
		best.part = i;
		best.gear = -1;
		best.side = -1;
		best.volume = std::max(best.volume, 0);
		best.t = t;
	}
	for (int vol = 0; vol < int(b.volumes.size()); vol++) {
		const Volume &v = b.volumes[size_t(vol)];
		const float eaves = b.block_z(v, v.top());
		for (int g = 0; g < int(v.gear.size()); g++) {
			const RoofPart &p = v.gear[size_t(g)];
			const float z0 = eaves + roof_height_at(K, v, p.x, p.y), t = box_hit(r, p.x, p.y, p.w / 2, p.d / 2, p.turn, z0, z0 + p.h);
			if (t <= 0 || t >= best.t) continue;
			best.gear = g;
			best.part = -1;
			best.volume = vol;
			best.side = -1;
			best.storey = v.storeys - 1;
			best.level = v.top() - 1;
			best.t = t;
		}
	}
}

// A hit that lands where a void block has taken the wall away is a click on the void: it is the only way to reach one.
void in_a_void(const Body &b, const LocalRay &r, BodyHit &best) {
	const V2 at{ r.ox + r.dx * best.t, r.oy + r.dy * best.t };
	const float z = r.oz + r.dz * best.t;
	for (int i = 0; i < int(b.volumes.size()); i++) {
		const Volume &v = b.volumes[size_t(i)];
		if (v.mode != BLOCK_VOID || z < b.block_z(v, v.base) || z > b.block_z(v, v.top()) || !contains_point(ring_of(v), at)) continue;
		best.volume = i;
		best.side = -1;
		return;
	}
}

// Whether the ray's track on the map passes within `radius` of the point.
bool passes_near(const Ray3 &r, float flat, float x, float z, float radius) {
	float px = x - r.ox, pz = z - r.oz;
	if (flat < GRAZING) return px * px + pz * pz <= radius * radius;
	float side = (px * r.dz - pz * r.dx) / flat;
	return std::fabs(side) <= radius;
}
} // namespace

BodyHit pick_body(const CityState &s, const Ray3 &ray, float reach) {
	BodyHit best;
	best.t = reach;
	const Tuning &t = s.catalog.tune;
	float step = float(t.build_pick_step_m), wide = float(t.build_max_side_m), flat = std::hypot(ray.dx, ray.dz);
	std::unordered_set<int> seen;
	auto test = [&](int id) {
		if (!seen.insert(id).second) return;
		const Building &rec = s.buildings.items[id];
		// A body the player edited may stand past its lot, by as much as a block may measure.
		float radius = std::hypot(rec.w, rec.d) / 2 + (rec.body ? wide : 0);
		if (!passes_near(ray, flat, rec.x, rec.z, radius)) return;
		const std::shared_ptr<const Body> body_held = record_body(s.buildings.designs, rec);
		const Body &body = *body_held;
		LotFrame frame = lot_frame(rec);
		float floor = lot_floor(body, rec, frame, [&](float x, float z) { return s.terrain.height_at(x, z); });
		LocalRay r = in_lot(ray, frame, floor);
		int before = best.volume;
		float was = best.t;
		const RoofShape &K = s.buildings.designs.shapes.roof;
		for (size_t i = 0; i < body.volumes.size(); i++) {
			if (body.volumes[i].mode == BLOCK_VOID || body.volumes[i].mode == BLOCK_INTERSECT) continue; // space, not a wall to hit
			hit_sides(body, body.volumes[i], int(i), r, best);
			hit_top(K, body, body.volumes[i], int(i), r, best);
		}
		const float wall = best.t;
		if (best.t < was) best.part = best.gear = -1; // a wall of this building, nearer than the last one's thing
		hit_things(K, body, r, best);
		if (best.t < was || best.volume != before) {
			best.id = id;
			if (best.t == wall) in_a_void(body, r, best);
		}
	};
	for (float at = 0; at <= reach; at += step) {
		float x = ray.ox + ray.dx * at, z = ray.oz + ray.dz * at, pad = step + wide;
		s.buildings.query(x - pad, z - pad, x + pad, z + pad, test);
		if (best.id >= 0 && at > best.t + step) break; // what is left lies behind what was hit
	}
	return best;
}

} // namespace city
