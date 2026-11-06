// The nearest building face under a ray (body_pick.h): records near the ray by the store's hash, then each block's sides and
// top in its lot's frame. A body is made only for a record the ray passes over.
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
constexpr float GRAZING = 1e-6f;
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
		if (z >= b.level_z(v.base + k)) return k;
	return 0;
}

void hit_sides(const Body &b, const Volume &v, int vol, const LocalRay &r, BodyHit &best) {
	const Ring ring = ring_of(v);
	float z0 = b.level_z(v.base), z1 = b.level_z(v.top());
	for (size_t i = 0; i < ring.size(); i++) {
		Point2 p = ring[i], q = ring[(i + 1) % ring.size()];
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

void hit_top(const Body &b, const Volume &v, int vol, const LocalRay &r, BodyHit &best) {
	if (r.dz > -GRAZING) return;
	// Down from the ridge's height to the eaves, the first point under the roof's surface: a pitched roof is hit on its slope.
	float eaves = b.level_z(v.top()), rise = roof_rise(v), t = -1;
	const Ring ring = ring_of(v);
	for (int i = 0; i <= ROOF_STEPS; i++) {
		float z = eaves + rise * (1 - float(i) / float(ROOF_STEPS)), at = (z - r.oz) / r.dz, x = r.ox + r.dx * at, y = r.oy + r.dy * at;
		if (at <= 0 || !contains_point(ring, { x, y })) continue;
		if (z <= eaves + roof_height_at(v, x, y) + EPS) {
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
		Body body;
		record_body(s.buildings.designs, rec, body);
		LotFrame frame = lot_frame(rec);
		float floor = lot_floor(body, rec, frame, [&](float x, float z) { return s.terrain.height_at(x, z); });
		LocalRay r = in_lot(ray, frame, floor);
		int before = best.volume;
		float was = best.t;
		for (size_t i = 0; i < body.volumes.size(); i++) {
			if (body.volumes[i].mode == BLOCK_VOID || body.volumes[i].mode == BLOCK_INTERSECT) continue; // space, not a wall to hit
			hit_sides(body, body.volumes[i], int(i), r, best);
			hit_top(body, body.volumes[i], int(i), r, best);
		}
		if (best.t < was || best.volume != before) best.id = id;
	};
	for (float at = 0; at <= reach; at += step) {
		float x = ray.ox + ray.dx * at, z = ray.oz + ray.dz * at, pad = step + wide;
		s.buildings.query(x - pad, z - pad, x + pad, z + pad, test);
		if (best.id >= 0 && at > best.t + step) break; // what is left lies behind what was hit
	}
	return best;
}

} // namespace city
