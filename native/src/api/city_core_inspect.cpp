// What the Inspect tool, the top bar's warning and the camera ask of the city: the thing under a click, a road's and a
// junction's live numbers (Roadcraft simAnswers.ts count), the nodes the editor would refuse and the roof over a point.
#include "api/city_core.h"

#include "core/units.h"
#include "sim/buildings/b40/record.h"
#include "sim/roads/junction.h"
#include "sim/traffic/traffic_system.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float INSPECT_RADIUS_M = 30.f;
constexpr float HALF = 0.5f;

struct Tally {
	int vehicles = 0, queued = 0, seen = 0;
	double speed = 0, capacity = 0;
};

// Vehicles on the directed segments `wanted` picks; a far vehicle (meso) has no speed of its own to add.
template <typename F>
Tally count_vehicles(const CityState &s, F &&wanted) {
	const TrafficState &t = s.traffic;
	Tally out;
	const float standing = s.catalog.traffic.tail_moving_ms;
	for (const Vehicle &v : t.vehicles) {
		if (!v.alive || v.ds < 0 || !wanted(v.ds)) continue;
		out.vehicles++;
		if (!v.micro) continue;
		out.seen++;
		out.speed += v.v;
		out.queued += v.v < standing ? 1 : 0;
	}
	for (int ds = 0; ds < int(t.g.ds.size()); ds++) {
		const DirSeg &d = t.g.ds[size_t(ds)];
		if (d.ok && wanted(ds)) out.capacity += double(d.lanes) * std::max(1.0, double(d.len) / s.catalog.tune.inspect_lane_vehicle_m);
	}
	return out;
}

void put_tally(Dictionary &d, const Tally &t, int volume) {
	d["vehicles"] = t.vehicles;
	d["queued"] = t.queued;
	d["mean_speed_ms"] = t.seen > 0 ? t.speed / t.seen : -1.0;
	d["capacity"] = int(std::round(t.capacity));
	d["volume"] = volume;
}

int volume_of(const TrafficState &t, int seg) {
	return size_t(seg) < t.seg_volume.size() ? int(t.seg_volume[size_t(seg)]) : 0;
}

void put_road(const CityState &s, Dictionary &d, int seg) {
	const Profile &p = s.catalog.profile(s.roads.segs[seg].profile);
	d["kind"] = "road";
	d["id"] = seg;
	d["profile"] = String(p.name_key.c_str());
	d["profile_id"] = s.roads.segs[seg].profile;
	d["lanes"] = int(p.fwd.size() + p.back.size());
	d["one_way"] = p.one_way;
	d["speed_ms"] = p.speed / KMH_PER_MS;
	d["width_m"] = p.right + p.left;
	d["cost_per_m"] = p.cost_per_m;
	d["length_m"] = s.roads.segs[seg].len;
	Array ends; // its dead ends, start first: the node and whether it turns in a bulb
	for (int n : { s.roads.segs[seg].a, s.roads.segs[seg].b }) {
		if (s.roads.nodes[size_t(n)].segs.size() != 1) continue;
		Dictionary e;
		e["node"] = n;
		e["start"] = n == s.roads.segs[seg].a;
		e["bulb"] = s.roads.nodes[size_t(n)].bulb;
		ends.push_back(e);
	}
	d["ends"] = ends;
}

// A node: what it is, and when a signal rules it what the player set on it (a change shows at once, the traffic takes it up on its
// next step) over each stage's minimum green, which still stands.
void put_node(const CityState &s, std::mutex &traffic, Dictionary &d, int node) {
	d["kind"] = "node";
	d["id"] = node;
	std::lock_guard<std::mutex> tr(traffic);
	const std::vector<Junction> &all = s.traffic.g.junctions;
	if (size_t(node) >= all.size() || all[size_t(node)].ctl != Control::SIGNAL || all[size_t(node)].least.empty()) return;
	const SignalSettings &set = s.roads.nodes[size_t(node)].signal;
	const std::vector<float> &least = all[size_t(node)].least;
	PackedFloat32Array greens;
	for (size_t k = 0; k < least.size(); k++) greens.push_back(set.fixed && k < set.greens.size() ? std::max(least[k], set.greens[k]) : least[k]);
	d["fixed"] = set.fixed;
	d["offset"] = set.offset;
	d["greens"] = greens;
}

void put_building(const CityState &s, Dictionary &d, int id) {
	const Building &b = s.buildings.items[id];
	d["kind"] = "building";
	d["id"] = id;
	d["floors"] = int(b.floors);
	d["zone"] = int(b.zone);
	d["fn"] = int(b.fn);
	const OccupancyDef &o = s.catalog.occupancy;
	const float per[4] = { o.m2_per_resident, o.m2_per_job_commercial, o.m2_per_job_industrial, o.m2_per_person_mixed };
	int residents, jobs;
	BuildingStore::occupancy(b, o.floor_share, per, residents, jobs);
	d["residents"] = residents;
	d["jobs"] = jobs;
}

std::vector<Solid> solids_of(const CityState &s, const Building &rec) {
	std::vector<Solid> out;
	if (rec.floors == 0) return out;
	b40::Body b;
	b40::record_body(s.buildings.designs, rec, b);
	const b40::LotFrame f = b40::lot_frame(rec);
	for (const b40::Volume &v : b.volumes) out.push_back({ f, v.x, v.y, v.x + v.w, v.y + v.d, b.level_z(v.base + v.storeys) });
	return out;
}

// The building whose lot centre is nearest within reach, or -1.
int nearest_building(const CityState &s, const Vector3 &p) {
	int best = -1;
	float bd = INSPECT_RADIUS_M;
	s.buildings.query(p.x - INSPECT_RADIUS_M, p.z - INSPECT_RADIUS_M, p.x + INSPECT_RADIUS_M, p.z + INSPECT_RADIUS_M, [&](int id) {
		const Building &b = s.buildings.items[id];
		float dd = std::hypot(b.x - p.x, b.z - p.z);
		if (dd < bd) { bd = dd; best = id; }
	});
	return best;
}
} // namespace

// A road's own surface first, its junction when the click is on the node, then the building, then the nearest road.
Dictionary CityCore::inspect(const Vector3 &p) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	Dictionary d;
	double t, dist;
	const int seg = s.roads.find_segment_near(p.x, p.z, INSPECT_RADIUS_M, t, dist);
	const bool on_road = seg >= 0 && dist <= (s.roads.profile_of(seg).right + s.roads.profile_of(seg).left) * HALF;
	if (on_road) {
		const int node = s.roads.find_node_near(p.x, p.z, s.catalog.tune.road_pick_m);
		if (node >= 0 && s.roads.nodes[size_t(node)].segs.size() > 2) {
			put_node(s, state_->traffic_mutex, d, node);
			return d;
		}
		put_road(s, d, seg);
		return d;
	}
	const int building = nearest_building(s, p);
	if (building >= 0) put_building(s, d, building);
	else if (seg >= 0) put_road(s, d, seg);
	return d;
}

PackedVector3Array CityCore::segment_path(int seg) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const RoadNetwork &net = state_->roads;
	PackedVector3Array out;
	if (seg < 0 || seg >= int(net.segs.size()) || !net.segs[size_t(seg)].alive) return out;
	const RoadSegment &g = net.segs[size_t(seg)];
	const float ya = net.nodes[size_t(g.a)].y, yb = net.nodes[size_t(g.b)].y;
	for (int i = 0; i <= ROAD_ARC_STEPS; i++) {
		const double t = double(i) / ROAD_ARC_STEPS;
		double x, z;
		g.q.at(t, x, z);
		out.push_back(Vector3(float(x), ya + (yb - ya) * float(t), float(z)));
	}
	return out;
}

Dictionary CityCore::segment_traffic(int seg) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	const CityState &s = *state_;
	Dictionary d;
	if (seg < 0 || seg >= int(s.roads.segs.size()) || !s.roads.segs[size_t(seg)].alive) return d;
	put_tally(d, count_vehicles(s, [seg](int ds) { return ds / 2 == seg; }), volume_of(s.traffic, seg));
	return d;
}

Dictionary CityCore::node_traffic(int node) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	const CityState &s = *state_;
	const TrafficState &t = s.traffic;
	Dictionary d;
	if (node < 0 || node >= int(s.roads.nodes.size()) || !s.roads.nodes[size_t(node)].alive) return d;
	const RoadNode &n = s.roads.nodes[size_t(node)];
	int volume = 0;
	for (int seg : n.segs) volume += volume_of(t, seg);
	put_tally(d, count_vehicles(s, [&](int ds) { return t.g.ds[size_t(ds)].ok && t.g.ds[size_t(ds)].to == node; }), volume);
	d["connections"] = int(n.segs.size());
	const bool known = size_t(node) < t.g.junctions.size();
	d["known"] = known;
	if (!known) return d;
	const Junction &j = t.g.junctions[size_t(node)];
	d["control"] = int(j.ctl);
	int moves = 0;
	for (int c = t.g.node_conn[size_t(node)]; c >= 0; c = t.g.conns[size_t(c)].next_at_node) moves += t.g.conns[size_t(c)].alive ? 1 : 0;
	d["movements"] = moves;
	if (j.ctl != Control::SIGNAL) return d;
	d["cycle_s"] = j.cycle;
	d["stage"] = j.run;
	d["stages"] = int(j.green.size());
	Array lamps; // each stage's light, by the first of its approaches
	for (const std::vector<int> &approaches : j.approaches) lamps.push_back(approaches.empty() ? int(Light::RED) : int(traffic::light_for(s, approaches[0])));
	d["lamps"] = lamps;
	return d;
}

// Network.impossible: the nodes whose legs meet too sharply, kept until the network changes.
Array CityCore::impossible_nodes() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	if (inspect_.sharp_version != s.roads.version) {
		inspect_.sharp = nodes_too_sharp(s.roads, s.catalog.tune.inspect_min_leg_deg * DEG_TO_RAD);
		inspect_.sharp_version = s.roads.version;
	}
	Array out;
	for (const SharpNode &n : inspect_.sharp) {
		const RoadNode &node = s.roads.nodes[size_t(n.node)];
		Dictionary d;
		d["id"] = n.node;
		d["gap_deg"] = n.gap * RAD_TO_DEG;
		d["position"] = Vector3(float(node.x), float(node.y), float(node.z));
		out.push_back(d);
	}
	return out;
}

// How far from `from` towards `to` the line of sight stays out of the buildings' volumes, as a share of it, in `samples` steps
// (Roadcraft isoViewport: the pull in front of a building). The volumes come from each record's body, kept until the city changes.
float CityCore::clear_reach(const Vector3 &from, const Vector3 &to, int samples) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	if (inspect_.solids_revision != s.revision) {
		inspect_.solids.clear();
		inspect_.solids_revision = s.revision;
	}
	std::vector<const std::vector<Solid> *> near;
	const float x0 = std::min(from.x, to.x), x1 = std::max(from.x, to.x), z0 = std::min(from.z, to.z), z1 = std::max(from.z, to.z);
	s.buildings.query(x0, z0, x1, z1, [&](int id) {
		auto found = inspect_.solids.find(id);
		if (found == inspect_.solids.end()) found = inspect_.solids.emplace(id, solids_of(s, s.buildings.items[id])).first;
		near.push_back(&found->second);
	});
	for (int i = 1; i <= samples; i++) {
		const float t = float(i) / float(samples);
		const Vector3 p = from.lerp(to, t);
		const float ground = s.terrain.height_at(p.x, p.z);
		for (const std::vector<Solid> *list : near)
			for (const Solid &v : *list) {
				const V2 rel = V2(p.x, p.z) - v.f.o;
				const float lx = rel.dot(v.f.e0), ly = rel.dot(v.f.e1);
				if (lx >= v.x0 && lx <= v.x1 && ly >= v.y0 && ly <= v.y1 && p.y < ground + v.top) return float(i - 1) / float(samples);
			}
	}
	return 1.f;
}

} // namespace city
