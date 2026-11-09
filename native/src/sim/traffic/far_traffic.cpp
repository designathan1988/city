// The far traffic as the render sees it (the lab's render/densityCars.ts, its places worked out here instead of in a shader): the
// near model's cars past the near publication, and each car of the far (meso) model on a lane within far_publish_m of the camera, placed along its lane from when it joined and when its
// event is due, a queue never closer than a body and meso_s0_m behind the car ahead. Published with the near poses.
#include "sim/traffic/traffic_system.h"

#include <algorithm>

namespace city {
namespace traffic {

void publish_far(const CityState &s, PoseFrame &f, float cx, float cz) {
	const TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	f.far.clear();
	const float r2 = p.far_publish_m * p.far_publish_m, near2 = p.publish_radius_m * p.publish_radius_m;
	// the near model's cars past the near publication (their poses this tick's, publish_poses): drawn as the far ones are
	for (int id : t.micro_list) {
		const Vehicle &v = t.vehicles[size_t(id)];
		const float dx = v.cur.x - cx, dz = v.cur.z - cz, d2 = dx * dx + dz * dz;
		if (!v.alive || v.off || two_wheeler(v.type) || d2 <= near2 || d2 > r2) continue;
		ParkedRecord rec;
		rec.at = v.cur;
		rec.id = uint32_t(id);
		rec.look = uint32_t(v.type) | (uint32_t(v.paint) << LOOK_PAINT_SHIFT);
		f.far.push_back(rec);
	}
	for (size_t key = 0; key < t.meso_head.size(); key++) {
		int id = t.meso_head[key];
		if (id < 0) continue;
		const int ds = int(key) / MAX_LANES, lane = int(key) % MAX_LANES;
		if (ds >= int(t.g.ds.size()) || !t.g.ds[size_t(ds)].ok) continue;
		const float L = t.g.ds[size_t(ds)].len;
		Pose mid;
		lane_pose(s, ds, lane, L * 0.5f, mid);
		const float dx = mid.x - cx, dz = mid.z - cz;
		if (dx * dx + dz * dz > r2) continue;
		float room = L; // the farthest along the next car may be: the lane's end for its first, behind the one ahead for the rest
		for (; id >= 0; id = t.vehicles[size_t(id)].mnext) {
			const Vehicle &v = t.vehicles[size_t(id)];
			const double span = v.t_exit - v.t_in;
			const float share = span > 0 ? float(std::clamp((t.time - v.t_in) / span, 0.0, 1.0)) : 1.f;
			const float len = p.classes[int(v.type)].length, front = std::min(v.s0 + (L - v.s0) * share, room);
			room = front - len - p.meso_s0_m;
			if (front - len < 0) break; // the rest of the queue spills back past the lane's start: not drawn on it
			if (two_wheeler(v.type)) continue; // its rider is no part of a far car's look, and a bike riding by itself is not drawn
			Pose a, b;
			lane_pose(s, ds, lane, front, a);
			lane_pose(s, ds, lane, front - len, b);
			ParkedRecord rec;
			rec.at = a;
			rec.at.x = (a.x + b.x) * 0.5f;
			rec.at.y = (a.y + b.y) * 0.5f + p.body_lift_m;
			rec.at.z = (a.z + b.z) * 0.5f;
			rec.id = uint32_t(id);
			rec.look = uint32_t(v.type) | (uint32_t(v.paint) << LOOK_PAINT_SHIFT);
			f.far.push_back(rec);
		}
	}
}

} // namespace traffic
} // namespace city
