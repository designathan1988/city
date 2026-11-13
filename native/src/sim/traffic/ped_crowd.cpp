// Walkers among walkers and street furniture (SUMO's striping pedestrian model, MSPModel_Striping): every step each
// walker on a sidewalk or a line round a corner or over a zebra gets its way's frame, sees the bodies that can be in
// its way remapped into that frame (ped_seen.cpp), and ped_stripes.cpp picks its stripe and pace. On a sidewalk it sees the walkers of
// that sidewalk and its furniture by arc. Each junction is a walking area (SUMO): the walkers round its corners, at its
// kerbs and on its arms' sidewalks within reach of it are filed with it, and whoever is near it sees them all; round a
// corner a walker also sees the furniture of its arms' sidewalks near the junction. On a sidewalk the room is the
// footway less ped_margin_m; round a corner as wide either side of its line as the sidewalk it heads for, opening off
// the line's start and closing toward a kerb place it must reach as fast as it steps aside (ped_lateral_share); over a
// zebra its own right half (the other half is the other way's). Everything a choice reads is copied once a step into the walkers' entries
// (CrowdEntry), each sidewalk's in order of arc, so a step reads arrays in order. A choice is made in its walker's own frame; the room
// between two walkers on different ways is kept in the world, once for all of them, before any choice is applied (ped_apart.h).
#include "sim/traffic/ped_apart.h"
#include "sim/traffic/ped_stripes.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr float NEAR_END_LOOKS = 2.f; // a sidewalk walker this many ped_look_m from a junction is filed with it
constexpr float SHORT_ARM_LOOKS = 3.f; // walkers round a junction also meet those of the next over an arm this short
constexpr int MAX_FILED = 1 + NODE_ARMS_MAX; // junctions a walker is filed with: its own and those over short arms
constexpr size_t POOLED_SIDEWALKS = 32; // below this many sidewalks with walkers, waking the pool costs more than it gives


// A sidewalk's ends as the crowd step reads them: where it starts and ends along the arc and their junctions.
struct SidewalkEnds {
	float lo = 0, hi = 0, reach = 0;
	int a = -1, b = -1;
	SidewalkEnds(const CityState &s, int seg) {
		const RoadSegment &g = s.roads.segs[size_t(seg)];
		lo = float(g.retreat_a);
		hi = float(g.len - g.retreat_b);
		reach = NEAR_END_LOOKS * s.catalog.traffic.ped_look_m;
		a = g.a;
		b = g.b != g.a ? g.b : -1;
	}
	// The junctions a walker at arc `s` is near (both on a short sidewalk).
	int near(float s, int *out) const {
		int n = 0;
		if (s - lo < reach) out[n++] = a;
		if (hi - s < reach && b >= 0) out[n++] = b;
		return n;
	}
	// Those it may meet someone of: near, and either heading for it or still inside its walking area (`zone` metres of
	// arc from the mouth: the kerb places and the lines round its corner); walking away past that, all of it is behind.
	int ahead(float s, int dir, float zone, int *out) const {
		int n = 0;
		if (s - lo < reach && (dir == 1 || s - lo < zone)) out[n++] = a;
		if (hi - s < reach && b >= 0 && (dir == 0 || hi - s < zone)) out[n++] = b;
		return n;
	}
};

// The widest room either side of a sidewalk's middle among the catalog's profiles (a corner line's room is at most this).
float widest_walk_half(const CityState &s) { return std::max(0.f, s.catalog.widest_sidewalk_half() - s.catalog.traffic.ped_margin_m); }

// Round a corner or at a kerb: its junction and the next one over each short arm.
int junctions_round(const CityState &s, int node, int *out) {
	const float look = s.catalog.traffic.ped_look_m;
	int n = 0;
	out[n++] = node;
	for (int seg : s.roads.nodes[size_t(node)].segs) {
		const RoadSegment &g = s.roads.segs[size_t(seg)];
		if (n < MAX_FILED && g.alive && g.a != g.b && g.len - g.retreat_a - g.retreat_b < SHORT_ARM_LOOKS * look)
			out[n++] = g.a == node ? g.b : g.a;
	}
	return n;
}

void file(std::vector<std::vector<CrowdEntry>> &lists, std::vector<int> &keys, int key, const CrowdEntry &e) {
	auto &list = lists[size_t(key)];
	if (list.empty()) keys.push_back(key);
	list.push_back(e);
}

// `level`: the share of its own speed it goes at to stay level with the one it walks with (ped_party.cpp company_pace). Somebody under a
// stop's hand (getting out or in, a truck's mate) is a body on its sidewalk where it stands (kerb_walkers.cpp kerb_place keeps its arc and
// place across), not moving of its own; so is somebody lying on the ground, `lie` the half of their body's length (0 on their feet).
CrowdEntry entry_of(const Pedestrian &w, int id, float moving, float level, float lie) {
	CrowdEntry e;
	e.id = id;
	e.kerb = w.mode == PedMode::KERB;
	e.sidewalk = (w.mode == PedMode::SIDEWALK || e.kerb) && w.side >= 0 ? w.seg * 2 + w.side : -1; // side -1: lying where no sidewalk is
	e.s = w.s;
	e.u = w.u;
	e.x = w.cur.x;
	e.z = w.cur.z;
	e.fx = w.way_x;
	e.fz = w.way_z;
	e.u_goal = w.u_goal;
	e.held = w.stripe_held;
	e.pace = w.pace;
	e.jam = w.jam;
	e.speed = w.speed * level;
	e.keep = w.dir == 0 ? -w.track : w.track; // its own side of the way
	e.dir = uint8_t(w.dir);
	e.moving = w.mode != PedMode::WAIT && !e.kerb && lie <= 0.f && w.pace > moving;
	e.lie = lie;
	e.lie_x = w.fall_x; // from its feet (x, z) the way it went down: it lies still (pedestrians.cpp step_pedestrians)
	e.lie_z = w.fall_z;
	return e;
}

// Each sidewalk's walkers by arc and each junction's walkers, from where they stand now.
void file_walkers(CityState &s) {
	TrafficState &t = s.traffic;
	if (t.crowd.size() != s.roads.segs.size() * 2) {
		t.crowd.assign(s.roads.segs.size() * 2, {});
		t.crowd_keys.clear();
	}
	if (t.node_walkers.size() != s.roads.nodes.size()) {
		t.node_walkers.assign(s.roads.nodes.size(), {});
		t.node_keys.clear();
	}
	for (int key : t.crowd_keys) t.crowd[size_t(key)].clear();
	for (int key : t.node_keys) t.node_walkers[size_t(key)].clear();
	t.crowd_keys.clear();
	t.node_keys.clear();
	int near[MAX_FILED];
	const float moving = s.catalog.traffic.ped_moving_ms;
	for (int id = 0; id < int(t.peds.size()); id++) {
		const Pedestrian &w = t.peds[size_t(id)];
		if (!w.alive) continue;
		const float lie = lying(t, w) ? lying_half(s.catalog.blast, w) : 0.f;
		const CrowdEntry e = entry_of(w, id, moving, company_pace(t, s.catalog.traffic, w), lie);
		if (e.sidewalk >= 0) file(t.crowd, t.crowd_keys, e.sidewalk, e);
		else if (w.way_node >= 0)
			for (int k = junctions_round(s, w.way_node, near); k-- > 0;) file(t.node_walkers, t.node_keys, near[k], e);
	}
	for (const DeadBody &d : t.dead) { // the dead lying: bodies like somebody lying, nobody's choice (no walker: id -1)
		const CrowdEntry e = dead_entry(s.catalog.blast, d);
		if (e.sidewalk >= 0 && size_t(e.sidewalk) < t.crowd.size()) file(t.crowd, t.crowd_keys, e.sidewalk, e);
		else if (d.node >= 0 && size_t(d.node) < s.roads.nodes.size() && s.roads.nodes[size_t(d.node)].alive)
			for (int k = junctions_round(s, d.node, near); k-- > 0;) file(t.node_walkers, t.node_keys, near[k], e);
	}
	for (int key : t.crowd_keys) { // in order of arc; those within reach of an end also with its junction
		auto &list = t.crowd[size_t(key)];
		std::sort(list.begin(), list.end(), [](const CrowdEntry &a, const CrowdEntry &b) { return a.s < b.s; });
		const SidewalkEnds ends(s, key / 2);
		for (const CrowdEntry &e : list)
			for (int k = ends.near(e.s, near); k-- > 0;) file(t.node_walkers, t.node_keys, near[k], e);
	}
}

// What the walkers of the sidewalks choose. All of them read the step's start and nothing of each other's choice, so the sidewalks
// run on every core, each writing its own stretch of `next`: the same list, in the same order, as one after the other. The furniture
// is read first, on this thread: it fills a cache.
void sidewalk_choices(CityState &s, const StripeRules &rules, float zone, float touch, float reach, std::vector<CrowdChoice> &next) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const float lie_most = s.catalog.blast.body_height_m * WALKER_SCALE_MAX * 0.5f; // the longest half of somebody lying (lying_half)
	std::vector<size_t> &first = t.crowd_first;
	first.resize(t.crowd_keys.size());
	size_t total = 0;
	for (size_t i = 0; i < t.crowd_keys.size(); i++) {
		const int key = t.crowd_keys[i];
		first[i] = total;
		total += t.crowd[size_t(key)].size();
		if (walkable(s, key / 2)) sidewalk_furniture(s, key / 2);
	}
	next.assign(total, CrowdChoice(-1, StripeChoice()));
	const TrafficState &seen = t;
	const auto one = [&](size_t i) {
		const int key = seen.crowd_keys[i];
		const auto &list = seen.crowd[size_t(key)];
		const int seg = key / 2;
		if (!walkable(s, seg)) return;
		const SidewalkEnds ends(s, seg);
		const SidewalkFurniture &sf = seen.furniture.find(seg)->second;
		const auto &pieces = sf.side[key % 2];
		const float half = walk_half(s, seg, key % 2);
		Body bodies[MAX_BODIES];
		int near[MAX_FILED];
		size_t from = 0; // the first piece within look behind the walker (the walkers come in order of arc)
		for (size_t k = 0; k < list.size(); k++) {
			const CrowdEntry &w = list[k];
			if (w.kerb || w.lie > 0.f) continue; // the stop moves it, or it lies there: a body the others see, no choice of its own
			const int nn = ends.ahead(w.s, w.dir, zone, near);
			WayFrame f;
			sidewalk_frame(w, half, p.ped_stripe_m, nn > 0, f);
			Seen sn{ f, bodies, 0, p.ped_look_m, touch, reach, lie_most };
			see_sidewalk(list, k, sn);
			for (int j = 0; j < nn && !sn.full(); j++) see_junction(seen, near[j], w.id, key, sn);
			while (from < pieces.size() && pieces[from].s0 < w.s - sn.look - sf.longest) from++;
			see_sidewalk_furniture(pieces, from, w, sn);
			const V2 way(w.fx, w.fz); // the swaths of the vehicles off the lanes, in the sidewalk's own across
			sn.n = swath_bodies(seen, V2(w.x, w.z), way, left_of(w.dir == 0 ? way : -way), f, p.ped_look_m, reach, bodies, sn.n);
			next[first[i] + k] = CrowdChoice(w.id, choose_stripe(rules, w, f, bodies, sn.n));
		}
	};
	if (s.jobs && t.crowd_keys.size() >= POOLED_SIDEWALKS) s.jobs->parallel_for(t.crowd_keys.size(), one);
	else
		for (size_t i = 0; i < t.crowd_keys.size(); i++) one(i);
}

// The room between any two walkers (ped_apart.h): every living walker on its feet is a body where it stands, each choice the step it
// takes this tick in the world (ahead by its pace as walk() steps it, aside by its change of stripe), and each choice is cut to the
// share of step its walker takes: the nearest to the one it chose that keeps the room (ahead no further, aside within what its choice
// allows). Two whose ways converge or cross never see each other's step in their own frames: here neither takes more than its half of
// the room between them. Somebody lying takes no step and is no round body where their feet are: their body is the footprint the
// others see in their frames (ped_seen.cpp), which one standing where it fell walks out of.
void hold_apart(CityState &s, float dt, float touch, std::vector<CrowdChoice> &next) {
	TrafficState &t = s.traffic;
	std::vector<Stride> &strides = t.crowd_strides;
	std::vector<int> &of = t.crowd_stride_of;
	strides.clear();
	of.assign(t.peds.size(), -1);
	for (int id = 0; id < int(t.peds.size()); id++) {
		const Pedestrian &w = t.peds[size_t(id)];
		if (!w.alive || lying(t, w)) continue;
		of[size_t(id)] = int(strides.size());
		strides.emplace_back();
		strides.back().at = V2(w.cur.x, w.cur.z);
	}
	for (const CrowdChoice &d : next) {
		if (d.first < 0) continue;
		const Pedestrian &w = t.peds[size_t(d.first)];
		Stride &st = strides[size_t(of[size_t(d.first)])];
		st.ahead = V2(w.way_x, w.way_z); // the way it walks, where it stands
		// across: a sidewalk's own u grows to the left of a -> b whichever way it is walked, a line's to the left of the line
		st.left = left_of(w.mode == PedMode::SIDEWALK && w.dir == 1 ? -st.ahead : st.ahead);
		st.far = d.second.pace * run_share(s, w) * dt;
		st.aside = d.second.u - w.u;
		st.lo = d.second.lo - w.u;
		st.hi = d.second.hi - w.u;
	}
	keep_apart(strides, touch, t.crowd_apart);
	for (CrowdChoice &d : next) {
		if (d.first < 0) continue;
		const Pedestrian &w = t.peds[size_t(d.first)];
		const Stride &st = strides[size_t(of[size_t(d.first)])];
		const float ahead = st.taken.dot(st.ahead);
		if (st.far > 0) d.second.pace *= clampf(ahead / st.far, 0.f, 1.f); // the same share of its pace as of its step ahead
		d.second.u = w.u + st.taken.dot(st.left);
	}
}

} // namespace

float walk_half(const CityState &s, int seg, int side) {
	const Profile &pf = s.roads.profile_of(seg);
	if (pf.edges.empty()) return 0.f;
	const StripEdge &e = side == 0 ? pf.edges.front() : pf.edges.back();
	return std::max(0.f, (e.u1 - e.u0) * 0.5f - s.catalog.traffic.ped_margin_m);
}

void crowd_step(CityState &s, float dt) {
	TrafficState &t = s.traffic;
	if (!s.traffic_levels.crowd_on) { // the Optimisations tab: nobody minds anybody, each at their own speed
		for (Pedestrian &w : t.peds)
			if (w.alive) w.pace = w.speed * company_pace(t, s.catalog.traffic, w);
		return;
	}
	const TrafficParams &p = s.catalog.traffic;
	file_walkers(s);
	const StripeRules rules(p, dt);
	std::vector<CrowdChoice> &next = t.crowd_next; // kept by the traffic: a step reuses the room of the last
	std::vector<const StreetObstacle *> &around = t.crowd_around;
	Body bodies[MAX_BODIES];
	const float touch = 2 * p.ped_body_m, reach = p.ped_body_m;
	int near[MAX_FILED];
	// a junction's walking area reaches this far along its arms: the zebra, a corner line's room either side, a body
	const float zone = s.catalog.furniture.crossing.zebra_to_m + widest_walk_half(s) + p.ped_body_m;
	sidewalk_choices(s, rules, zone, touch, reach, next);
	for (int node : t.node_keys) { // round the corners and over the zebras of each junction
		bool gathered = false;
		for (const CrowdEntry &e : t.node_walkers[size_t(node)]) {
			if (e.lie > 0.f) continue; // lying on a corner or a zebra (a dead body is no walker at all): no choice either
			const Pedestrian &w = t.peds[size_t(e.id)];
			if (w.mode != PedMode::WAY || w.way_node != node) continue;
			if (!gathered) junction_furniture(s, node, p.ped_look_m, around);
			gathered = true;
			WayFrame f;
			line_frame(s, w, f);
			Seen sn{ f, bodies, 0, p.ped_look_m, touch, reach };
			for (int j = junctions_round(s, node, near); j-- > 0 && !sn.full();) see_junction(t, near[j], e.id, -1, sn);
			for (size_t o = 0; o < around.size() && !sn.full(); o++) see_footprint(*around[o], sn);
			sn.n = swath_bodies(t, f.at, f.fwd, f.left, f, p.ped_look_m, reach, bodies, sn.n);
			next.emplace_back(e.id, choose_stripe(rules, e, f, bodies, sn.n));
		}
	}
	hold_apart(s, dt, touch, next); // no two steps of this tick meet, whatever the ways they are taken on
	for (const CrowdChoice &d : next) {
		if (d.first < 0) continue; // a sidewalk that is no longer walked: no choice made
		Pedestrian &w = t.peds[size_t(d.first)];
		w.u = d.second.u;
		w.u_goal = d.second.u_goal;
		w.stripe_held = d.second.held;
		w.pace = d.second.pace;
		w.jam = d.second.jam;
	}
	around.clear(); // pointers into the furniture, which an edit may drop before the next step
}

} // namespace traffic
} // namespace city
