// The micro window around the camera (Roadcraft traffic.ts checkWindow/materialize): vehicles far from the camera
// leave the near model and join their lane's meso queue; vehicles of a meso lane that came inside the window become
// drawn ones, each where its timing says it is and behind the queue already drawn. The far list is kept only as far
// as `meso_window_out_m`, so the window never holds an invisible vehicle in a queue. Walkers exist only inside
// `ped_window_m`, which also sets how many sidewalk metres demand may fill.
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr float MESO_VIEW_STEP = 64.f; // the camera moving this far is enough to look at the window again
constexpr double WINDOW_EVERY_S = 1.0;
constexpr float MAT_TAIL_PAD_M = 2.f, MAT_END_PAD_M = 0.5f;
constexpr float MAT_SPEED_SHARE = 0.7f;
constexpr float FURNITURE_KEEP_WINDOWS = 2.f; // walkers live within 1.2 windows: their furniture is kept within 2

float mid_dist2(const CityState &s, int seg, float cx, float cz) {
	const RoadSegment &g = s.roads.segs[size_t(seg)];
	const float x = float((g.minx + g.maxx) * 0.5), z = float((g.minz + g.maxz) * 0.5);
	return (x - cx) * (x - cx) + (z - cz) * (z - cz);
}

void grow_ped_window(CityState &s, float cx, float cz) {
	TrafficState &t = s.traffic;
	const float w = s.catalog.traffic.ped_window_m, w2 = w * w;
	t.ped_metres = 0;
	t.ped_segs.clear();
	s.roads.for_segments_near(cx - w, cz - w, cx + w, cz + w, [&](int seg) {
		if (mid_dist2(s, seg, cx, cz) >= w2 || !walkable(s, seg)) return;
		const Profile &p = s.roads.profile_of(seg);
		const float len = float(s.roads.segs[size_t(seg)].len);
		t.ped_metres += double(len) * ((p.has_walk_right ? 1 : 0) + (p.has_walk_left ? 1 : 0));
		t.ped_segs.push_back(seg);
	});
	prune_furniture(s, cx, cz, w * FURNITURE_KEEP_WINDOWS); // the walkers' street furniture far behind the camera
}

} // namespace

bool in_window(const CityState &s, int ds) {
	const TrafficState &t = s.traffic;
	if (ds < 0 || ds >= int(t.g.ds.size()) || !t.g.ds[size_t(ds)].ok) return false;
	const DirSeg &d = t.g.ds[size_t(ds)];
	Pose p;
	lane_pose(s, ds, 0, d.len * 0.5f, p);
	const float cx = s.camera_x.load(), cz = s.camera_z.load(), w = s.catalog.traffic.meso_window_m;
	const float dx = p.x - cx, dz = p.z - cz;
	return dx * dx + dz * dz < w * w;
}

void materialize(CityState &s, int key) {
	TrafficState &t = s.traffic;
	const int ds = key / MAX_LANES;
	if (key < 0 || ds < 0 || ds >= int(t.g.ds.size()) || !t.g.ds[size_t(ds)].ok || key >= int(t.meso_head.size())) return;
	const DirSeg &d = t.g.ds[size_t(ds)];
	const float L = d.len, free_speed = d.speed, margin = s.catalog.traffic.stop_margin_m;
	// the far model waits for green at any other light (meso.cpp): out of it, a vehicle is at most where it stops for
	// that light and slow enough to stop there comfortably, as the near model would have it
	const bool held = light_for(s, ds) != Light::GREEN;
	const float stop = held ? L - d.stop_back - margin : L - margin;
	const int tail = t.lane_vehicles[size_t(key)].empty() ? -1 : t.lane_vehicles[size_t(key)].front(); // the rearmost drawn one
	const float tail_len = tail >= 0 ? s.catalog.traffic.classes[int(t.vehicles[size_t(tail)].type)].length : 0.f;
	float limit = tail >= 0 ? t.vehicles[size_t(tail)].s - tail_len - MAT_TAIL_PAD_M : held ? stop : L - MAT_END_PAD_M;
	int id = t.meso_head[size_t(key)];
	while (id >= 0) {
		Vehicle &v = t.vehicles[size_t(id)];
		const int next = v.mnext;
		if (!v.alive || v.micro) { id = next; continue; }
		const float span = float(v.t_exit - v.t_in);
		const float walked = span > 0 ? clampf(float((t.time - v.t_in) / span), 0.f, 1.f) : 1.f;
		const float est = v.s0 + (L - v.s0) * walked;
		const float len = s.catalog.traffic.classes[int(v.type)].length;
		if (limit < len) break; // no room behind the drawn queue: it stays in the far model
		const float at = std::max(len, std::min(est, limit));
		meso_unlink(s, id);
		v.s = at;
		v.conn = -1;
		v.pass = -1;
		v.amber_hold = -1;
		v.stood = false;
		v.held = 0;
		v.wait = 0;
		const float b = std::max(s.catalog.traffic.classes[int(v.type)].b, 0.f);
		v.v = at < est - 1 ? 0.f : std::min(free_speed * v.vf, free_speed * MAT_SPEED_SHARE);
		v.v = std::min(v.v, std::sqrt(2 * b * std::max(0.f, stop - at)));
		v.acc = 0;
		micro_add(t, id);
		lane_add(s, key, id);
		limit = at - len - MAT_TAIL_PAD_M;
		id = next;
	}
}

void check_window(CityState &s) {
	TrafficState &t = s.traffic;
	const float cx = s.camera_x.load(), cz = s.camera_z.load();
	const float dx = cx - t.win_x, dz = cz - t.win_z;
	if (t.win_t > -double(BIG) && dx * dx + dz * dz < MESO_VIEW_STEP * MESO_VIEW_STEP && t.time - t.win_t < WINDOW_EVERY_S) return;
	t.win_x = cx;
	t.win_z = cz;
	t.win_t = t.time;
	// near vehicles now outside the out radius go back to their lane's far queue
	const float out = s.catalog.traffic.meso_window_out_m, out2 = out * out;
	for (int i = int(t.micro_list.size()) - 1; i >= 0; i--) {
		const int id = t.micro_list[size_t(i)];
		Vehicle &v = t.vehicles[size_t(id)];
		if (!v.alive || v.conn >= 0 || v.ds < 0 || v.ds >= int(t.g.ds.size())) continue;
		const DirSeg &d = t.g.ds[size_t(v.ds)];
		Pose p;
		lane_pose(s, v.ds, v.lane, d.len * 0.5f, p);
		if ((p.x - cx) * (p.x - cx) + (p.z - cz) * (p.z - cz) <= out2) continue;
		meso_enter(s, id, v.ds * MAX_LANES + v.lane, true); // leaves its lane list and micro_list there
	}
	// far vehicles on lanes now inside the window become drawn ones (whoever is inside is never invisible)
	const float w = s.catalog.traffic.meso_window_m, w2 = w * w;
	s.roads.for_segments_near(cx - w, cz - w, cx + w, cz + w, [&](int seg) {
		if (mid_dist2(s, seg, cx, cz) >= w2) return;
		for (int dir = 0; dir < 2; dir++) {
			const int ds = seg * 2 + dir;
			if (ds >= int(t.g.ds.size()) || !t.g.ds[size_t(ds)].ok) continue;
			for (int lane = 0; lane < t.g.ds[size_t(ds)].lanes; lane++) {
				const int key = ds * MAX_LANES + lane;
				if (size_t(key) < t.meso_head.size() && t.meso_head[size_t(key)] >= 0) materialize(s, key);
			}
		}
	});
	grow_ped_window(s, cx, cz);
}

} // namespace traffic
} // namespace city
