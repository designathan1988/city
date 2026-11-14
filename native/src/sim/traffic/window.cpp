// The window of detailed traffic round the view: it follows the view (its radius the view's reach by window_view_share, moved and
// resized with a slack so it does not breathe with the camera); its streets are those the far traffic's flows hold as inside
// (flow_window.cpp hands their vehicles over and takes them back). A bus of a far queue (meso.cpp) on a lane that came inside
// becomes a drawn one, where its timing says it is and behind the queue already drawn. Walkers exist only inside `ped_window_m`,
// which also sets how many sidewalk metres their demand may fill.
#include "sim/traffic/traffic_system.h"

#include "sim/traffic/lane_closures.h"

#include <algorithm>
#include <cmath>
#include <iterator>

namespace city {
namespace traffic {

namespace {

constexpr double STREET_SIDE = 0.5;

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
	std::fill(std::begin(t.ped_kind_m), std::end(t.ped_kind_m), 0.0);
	t.ped_segs.clear();
	t.ped_edge_segs.clear();
	const auto outside = [&](int node) {
		const RoadNode &n = s.roads.nodes[size_t(node)];
		return (float(n.x) - cx) * (float(n.x) - cx) + (float(n.z) - cz) * (float(n.z) - cz) >= w2;
	};
	s.roads.for_segments_near(cx - w, cz - w, cx + w, cz + w, [&](int seg) {
		if (mid_dist2(s, seg, cx, cz) >= w2 || !walkable(s, seg)) return;
		const Profile &p = s.roads.profile_of(seg);
		const float len = float(s.roads.segs[size_t(seg)].len);
		const TrafficParams &tp = s.catalog.traffic;
		const RoadSegment &g = s.roads.segs[size_t(seg)];
		const int right = s.catalog.zone(g.zr).base % BASE_ZONES, left = s.catalog.zone(g.zl).base % BASE_ZONES;
		for (int k = 0; k < PLACE_KINDS; k++) { // half of the street to each side's land
			if (p.has_walk_right) t.ped_kind_m[k] += double(len) * STREET_SIDE * tp.zone_kinds[right * PLACE_KINDS + k];
			if (p.has_walk_left) t.ped_kind_m[k] += double(len) * STREET_SIDE * tp.zone_kinds[left * PLACE_KINDS + k];
		}
		t.ped_segs.push_back(seg);
		if (outside(g.a) || outside(g.b)) t.ped_edge_segs.push_back(seg); // its sidewalks run on out of the window: people come in by it
	});
	list_doors(s, cx, cz, w);
	prune_furniture(s, cx, cz, w * FURNITURE_KEEP_WINDOWS); // the walkers' street furniture far behind the camera
}

} // namespace

namespace {
constexpr uint8_t WAS_IN = 1, STAYS = 2, WAY_OUT = 3; // TrafficState::in_win during a look (zero between looks)

// The radius the window should have now: the view's reach (how far the eye stands from the ground it looks at) by
// window_view_share, between window_min_m and meso_window_m; with no view to follow (a run with no camera) it stands at its most.
float wanted_radius(const CityState &s) {
	const TrafficParams &p = s.catalog.traffic;
	const float reach = s.camera_reach.load();
	return reach > 0 ? clampf(reach * p.window_view_share, p.window_min_m, p.meso_window_m) : p.meso_window_m;
}

// The window's streets after it moved or was resized: a directed segment comes in when its middle is within the window's radius
// and goes out only past the out radius (it does not flap at the edge); the flows hand each one that comes in to the detailed
// simulation and take back each one that goes out (flow_window.cpp). Only the streets near where the window was and is are looked
// at. Then the ways out: the streets outside that start where one inside ends.
void look_at_segments(CityState &s) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const StreetFlows &f = t.flows;
	if (t.in_win.size() != t.g.ds.size()) t.in_win.assign(t.g.ds.size(), 0);
	for (int ds : t.window_ds)
		if (size_t(ds) < t.in_win.size()) t.in_win[size_t(ds)] = WAS_IN;
	std::vector<int> was;
	was.swap(t.window_ds);
	t.window_lane_m = 0;
	const float in = t.win_r, out = t.win_r * p.meso_window_out_m / p.meso_window_m;
	s.roads.for_segments_near(t.win_x - out, t.win_z - out, t.win_x + out, t.win_z + out, [&](int seg) {
		const float d2 = mid_dist2(s, seg, t.win_x, t.win_z);
		for (int dir = 0; dir < 2; dir++) {
			const int ds = seg * 2 + dir;
			if (size_t(ds) >= f.live.size() || !f.live[size_t(ds)]) continue;
			const float reach = f.inside[size_t(ds)] ? out : in;
			if (d2 >= reach * reach) continue;
			flow_enters(s, ds); // nothing when it is inside already
			t.in_win[size_t(ds)] = STAYS;
			t.window_ds.push_back(ds);
			t.window_lane_m += double(t.g.ds[size_t(ds)].len) * double(t.g.ds[size_t(ds)].lanes);
		}
	});
	for (int ds : was) {
		if (size_t(ds) >= t.in_win.size()) continue;
		if (t.in_win[size_t(ds)] == WAS_IN) flow_leaves(s, ds);
		t.in_win[size_t(ds)] = 0;
	}
	for (int ds : t.window_ds) t.in_win[size_t(ds)] = 0;
	for (int ds : t.window_out_ds)
		if (size_t(ds) < t.in_win.size()) t.in_win[size_t(ds)] = 0;
	t.window_out_ds.clear();
	for (int ds : t.window_ds) {
		const int node = t.g.ds[size_t(ds)].to;
		if (node < 0 || size_t(node) >= s.roads.nodes.size()) continue;
		for (int seg : s.roads.nodes[size_t(node)].segs) {
			const int way = out_ds(s.roads, seg, node);
			if (size_t(way) >= f.live.size() || !f.live[size_t(way)] || f.inside[size_t(way)] || t.in_win[size_t(way)] == WAY_OUT) continue;
			t.in_win[size_t(way)] = WAY_OUT;
			t.window_out_ds.push_back(way);
		}
	}
	for (int ds : t.window_out_ds) t.in_win[size_t(ds)] = 0;
}

// The window follows the view: moved when the camera is past window_recentre_share of its radius from its middle, resized when
// the view outgrows it or shrinks under window_shrink_share of it. True when it moved or was resized.
bool track_view(CityState &s, float cx, float cz) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const float want = wanted_radius(s);
	const bool resized = t.win_r <= 0 || want > t.win_r || want < t.win_r * p.window_shrink_share;
	const float away = (V2(cx, cz) - V2(t.win_x, t.win_z)).len();
	if (!resized && away <= t.win_r * p.window_recentre_share) return false;
	t.win_x = cx;
	t.win_z = cz;
	if (resized) t.win_r = want;
	return true;
}
} // namespace

double window_lane_metres(const CityState &s) { return s.traffic.window_lane_m; }

bool in_window(const CityState &s, int ds) {
	const StreetFlows &f = s.traffic.flows;
	return ds >= 0 && size_t(ds) < f.inside.size() && f.inside[size_t(ds)] != 0;
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
	const float shut = closure_ahead(t, key, 0.f); // a stretch a body off the lanes holds: the queue is drawn behind the first
	if (shut >= 0) limit = std::min(limit, shut - MAT_TAIL_PAD_M);
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
	const float dx = cx - t.look_x, dz = cz - t.look_z;
	const bool same_view = t.win_t > -double(BIG) && dx * dx + dz * dz < MESO_VIEW_STEP * MESO_VIEW_STEP && t.time - t.win_t < WINDOW_EVERY_S;
	if (same_view && !t.flows.relook) return; // nothing to look at: the view as it was a moment ago, the streets as they were
	t.look_x = cx;
	t.look_z = cz;
	t.win_t = t.time;
	// the window of detailed traffic follows the view; whoever drives out of it is the far traffic's (flow_window.cpp flow_begin).
	// Its streets are looked at again when it moved or was resized, and when the streets themselves changed (refit_flows)
	const bool moved = track_view(s, cx, cz);
	if (moved || t.flows.relook || t.window_ds.empty()) look_at_segments(s);
	t.flows.relook = false;
	// the buses of the far queues on lanes now inside the window become drawn ones (whoever is inside is never invisible)
	for (int ds : t.window_ds)
		for (int lane = 0; lane < t.g.ds[size_t(ds)].lanes; lane++) {
			const int key = ds * MAX_LANES + lane;
			if (size_t(key) < t.meso_head.size() && t.meso_head[size_t(key)] >= 0) materialize(s, key);
		}
	grow_ped_window(s, cx, cz);
}

} // namespace traffic
} // namespace city
