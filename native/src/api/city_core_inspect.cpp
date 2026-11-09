// What the Inspect tool, the top bar's warning and the camera ask of the city: the thing under a click (a road with its street's
// name and house numbers), a road's and a junction's live numbers (Roadcraft simAnswers.ts count), the nodes the editor would refuse and the roof over a point.
#include "api/city_core.h"

#include "core/units.h"
#include "sim/buildings/b40/record.h"
#include "sim/commands/commands.h"
#include "sim/roads/junction.h"
#include "sim/street/street_names.h"
#include "sim/traffic/traffic_system.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float HALF = 0.5f;

// What the traffic published for a segment or a node (TrafficView): no traffic_mutex, the panel never waits for a step.
void put_tally(Dictionary &d, const TrafficTally &t, int volume) {
	d["vehicles"] = t.vehicles;
	d["queued"] = t.queued;
	d["mean_speed_ms"] = double(t.mean_speed());
	d["capacity"] = int(std::round(t.capacity));
	d["volume"] = volume;
}

int volume_of(const TrafficView &v, int seg) {
	return size_t(seg) < v.volume.size() ? int(v.volume[size_t(seg)]) : 0;
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
	double length = 0;
	for (int piece : s.roads.pieces_of(seg)) length += s.roads.segs[size_t(piece)].len;
	d["length_m"] = length;
	// its street's name and the numbers along it, over all its pieces (the lab's inspector: "Rua das Flores · …", "No. 120 to 204")
	d["street"] = String::utf8(street_name_of(s.street, seg).c_str());
	StreetNumbers numbers, piece_numbers;
	bool numbered = false;
	for (int piece : s.roads.pieces_of(seg)) {
		if (!street_numbers(s.roads, piece, piece_numbers)) continue;
		numbers.from = numbered ? std::min(numbers.from, piece_numbers.from) : piece_numbers.from;
		numbers.to = numbered ? std::max(numbers.to, piece_numbers.to) : piece_numbers.to;
		numbered = true;
	}
	if (numbered) {
		d["number_from"] = numbers.from;
		d["number_to"] = numbers.to;
	}
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
	const std::vector<int> pieces = s.roads.pieces_of(seg);
	const RoadSegment &first = s.roads.segs[size_t(pieces.front())], &last = s.roads.segs[size_t(pieces.back())];
	const int from = first.a, to = pieces.size() == 1 ? first.b : (last.b == first.a || last.b == first.b ? last.a : last.b);
	d["node_a"] = from;
	d["node_b"] = to;
	d["lift_a"] = s.roads.nodes[size_t(from)].lift;
	d["lift_b"] = s.roads.nodes[size_t(to)].lift;
	const RoadBend bend = road_bend_of(road_line(s.roads, seg));
	d["bend_m"] = bend.height;
	d["bend_at"] = bend.at;
	d["median"] = std::any_of(p.strips.begin(), p.strips.end(), [](const Strip &st) { return st.kind == StripKind::MEDIAN; }) && !p.one_way;
}

// A node: what it is, and when a signal rules it what the player set on it (a change shows at once, the traffic takes it up on its
// next step) over each stage's minimum green, which still stands.
void put_node(const CityState &s, const TrafficView &view, Dictionary &d, int node) {
	d["kind"] = "node";
	d["id"] = node;
	d["lift"] = s.roads.nodes[size_t(node)].lift;
	d["arms"] = int(s.roads.nodes[size_t(node)].segs.size());
	d["crossing"] = int(s.roads.nodes[size_t(node)].crossing);
	d["uturn"] = int(s.roads.nodes[size_t(node)].uturn);
	if (size_t(node) >= view.junction.size() || !view.junction[size_t(node)].signal) return;
	const JunctionView &j = view.junction[size_t(node)];
	if (j.stages == 0 || size_t(j.least_from + j.stages) > view.least.size()) return;
	const SignalSettings &set = s.roads.nodes[size_t(node)].signal;
	PackedFloat32Array greens;
	for (size_t k = 0; k < size_t(j.stages); k++) {
		const float least = view.least[size_t(j.least_from) + k];
		greens.push_back(set.fixed && k < set.greens.size() ? std::max(least, set.greens[k]) : least);
	}
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
	BuildingStore::occupancy(b, s.catalog.zone(b.zone).base, o.floor_share, per, residents, jobs);
	d["residents"] = residents;
	d["jobs"] = jobs;
}

// The building whose lot centre is nearest within reach, or -1.
int nearest_building(const CityState &s, const Vector3 &p) {
	int best = -1;
	float bd = s.catalog.tune.inspect_radius_m;
	s.buildings.query(p.x - s.catalog.tune.inspect_radius_m, p.z - s.catalog.tune.inspect_radius_m, p.x + s.catalog.tune.inspect_radius_m, p.z + s.catalog.tune.inspect_radius_m, [&](int id) {
		const Building &b = s.buildings.items[id];
		float dd = std::hypot(b.x - p.x, b.z - p.z);
		if (dd < bd) { bd = dd; best = id; }
	});
	return best;
}
} // namespace

// A road's own surface first, its junction when the click is on the node, then the building, then the nearest road.
// A junction, or a point the player has between two roads: not where a road was only cut in pieces.
static bool is_point(const RoadNetwork &net, int node) {
	const std::vector<int> &arms = net.nodes[size_t(node)].segs;
	if (arms.size() != 2) return arms.size() > 2;
	if (net.nodes[size_t(node)].crossing != CROSSING_NONE || net.nodes[size_t(node)].uturn != UTURN_NONE) return true;
	const std::vector<int> road = net.pieces_of(arms[0]);
	return std::find(road.begin(), road.end(), arms[1]) == road.end();
}

Dictionary CityCore::inspect(const Vector3 &p) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	Dictionary d;
	double t, dist;
	const int seg = s.roads.find_segment_near(p.x, p.z, s.catalog.tune.inspect_radius_m, t, dist);
	const bool on_road = seg >= 0 && dist <= (s.roads.profile_of(seg).right + s.roads.profile_of(seg).left) * HALF;
	if (on_road) {
		const int node = s.roads.find_node_near(p.x, p.z, s.catalog.tune.road_pick_m);
		if (node >= 0 && is_point(s.roads, node)) {
			put_node(s, state_->traffic_views.acquire(), d, node);
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
	const std::vector<int> pieces = net.pieces_of(seg);
	for (size_t k = 0; k < pieces.size(); k++) {
		const RoadSegment &g = net.segs[size_t(pieces[k])];
		// A piece may run against the road: it is followed from the end the last one left at.
		const bool turned = k + 1 < pieces.size() ? (g.a == net.segs[size_t(pieces[k + 1])].a || g.a == net.segs[size_t(pieces[k + 1])].b)
				: k > 0 && (g.b == net.segs[size_t(pieces[k - 1])].a || g.b == net.segs[size_t(pieces[k - 1])].b);
		const float ya = net.nodes[size_t(g.a)].y, yb = net.nodes[size_t(g.b)].y;
		for (int i = 0; i <= ROAD_ARC_STEPS; i++) {
			const double t = double(turned ? ROAD_ARC_STEPS - i : i) / ROAD_ARC_STEPS;
			double x, z;
			g.q.at(t, x, z);
			out.push_back(Vector3(float(x), ya + (yb - ya) * float(t), float(z)));
		}
	}
	return out;
}

Dictionary CityCore::segment_traffic(int seg) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	const TrafficView &v = state_->traffic_views.acquire();
	Dictionary d;
	if (seg < 0 || seg >= int(s.roads.segs.size()) || !s.roads.segs[size_t(seg)].alive || size_t(seg) >= v.seg.size()) return d;
	TrafficTally whole = v.seg[size_t(seg)]; // the road's: what is on all of its pieces
	whole.clear_fleet();
	for (int piece : s.roads.pieces_of(seg)) {
		if (size_t(piece) >= v.seg.size()) continue;
		const TrafficTally &t = v.seg[size_t(piece)];
		whole.vehicles += t.vehicles;
		whole.queued += t.queued;
		whole.seen += t.seen;
		whole.speed += t.speed;
	}
	put_tally(d, whole, volume_of(v, seg));
	return d;
}

Dictionary CityCore::node_traffic(int node) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	const TrafficView &v = state_->traffic_views.acquire();
	Dictionary d;
	if (node < 0 || node >= int(s.roads.nodes.size()) || !s.roads.nodes[size_t(node)].alive) return d;
	const RoadNode &n = s.roads.nodes[size_t(node)];
	int volume = 0;
	for (int seg : n.segs) volume += volume_of(v, seg);
	put_tally(d, size_t(node) < v.node.size() ? v.node[size_t(node)] : TrafficTally(), volume);
	d["connections"] = int(n.segs.size());
	const bool known = size_t(node) < v.junction.size() && v.junction[size_t(node)].known;
	d["known"] = known;
	if (!known) return d;
	const JunctionView &j = v.junction[size_t(node)];
	d["control"] = j.control;
	d["movements"] = j.movements;
	if (!j.signal) return d;
	d["cycle_s"] = j.cycle;
	d["stage"] = j.stage;
	d["stages"] = j.stages;
	Array lamps; // each stage's light, by the first of its approaches
	for (int k = 0; k < j.lamp_n; k++) lamps.push_back(v.lamps[size_t(j.lamp_from + k)]);
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

} // namespace city
