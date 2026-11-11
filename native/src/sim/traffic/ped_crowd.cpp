// Walkers among walkers and street furniture (SUMO's striping pedestrian model, MSPModel_Striping): every step each
// walker on a sidewalk or a line round a corner or over a zebra gets its way's frame, sees the bodies that can be in
// its way remapped into that frame, and ped_stripes.cpp picks its stripe and pace. On a sidewalk it sees the walkers of
// that sidewalk and its furniture by arc. Each junction is a walking area (SUMO): the walkers round its corners, at its
// kerbs and on its arms' sidewalks within reach of it are filed with it, and whoever is near it sees them all; round a
// corner a walker also sees the furniture of its arms' sidewalks near the junction. On a sidewalk the room is the
// footway less ped_margin_m; round a corner as wide either side of its line as the sidewalk it heads for, opening off
// the line's start and closing toward a kerb place it must reach as fast as it steps aside (ped_lateral_share); over a
// zebra its own right half (the other half is the other way's). Everything a choice reads is copied once a step into the walkers' entries
// (CrowdEntry), each sidewalk's in order of arc, so a step reads arrays in order.
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

// The bodies one walker sees this step, in its frame; only those that can stand in its room.
struct Seen {
	const WayFrame &f;
	Body *out;
	int n = 0;
	float look, touch, reach; // ped_look_m, two body radii, a body radius

	// Full: every later body would be turned away, so the looking stops there (what it saw is the same).
	bool full() const { return n >= MAX_BODIES; }
	// A walker ahead, or beside it (within two radii along, a little behind too): the gap is kept to the first, no step aside
	// is taken into the second (ped_stripes.cpp side_limits). One farther behind keeps its own gap.
	void walker(float along, float lat, bool oncoming) {
		if (along <= -touch || along > look || lat < f.lo - touch || lat > f.hi + touch || n >= MAX_BODIES) return;
		Body &b = out[n++];
		b = Body();
		b.along_lo = b.along_hi = along;
		b.lat_lo = b.lat_hi = lat;
		b.reach = touch;
		b.oncoming = oncoming;
	}
	void furniture(const Body &b) {
		if (b.along_hi < -reach || b.along_lo > look || b.lat_hi < f.lo - reach || b.lat_lo > f.hi + reach || n >= MAX_BODIES) return;
		out[n++] = b;
	}
};

// The walkers on its own sidewalk, by arc: those ahead as far as it looks, and those beside it (Seen::walker), a little behind too.
void see_sidewalk(const std::vector<CrowdEntry> &list, size_t k, Seen &sn) {
	const CrowdEntry &w = list[k];
	const float way = w.dir == 0 ? 1.f : -1.f; // ahead is up the arc or down it
	for (size_t j = k + 1; j < list.size() && !sn.full(); j++) {
		const float along = (list[j].s - w.s) * way;
		if (along > sn.look || along <= -sn.touch) break;
		sn.walker(along, list[j].u, list[j].dir != w.dir && list[j].moving);
	}
	for (size_t j = k; j-- > 0 && !sn.full();) {
		const float along = (list[j].s - w.s) * way;
		if (along > sn.look || along <= -sn.touch) break;
		sn.walker(along, list[j].u, list[j].dir != w.dir && list[j].moving);
	}
}

// The walkers filed with junction `node` but `self`, remapped into its frame (SUMO's walking areas); a sidewalk walker leaves
// out its own sidewalk's, already seen by arc.
void see_junction(const TrafficState &t, int node, int self, int own_sidewalk, Seen &sn) {
	for (const CrowdEntry &o : t.node_walkers[size_t(node)]) {
		if (sn.full()) return;
		if (o.id == self || (own_sidewalk >= 0 && o.sidewalk == own_sidewalk)) continue;
		const V2 rel(o.x - sn.f.at.x, o.z - sn.f.at.y);
		const bool facing = o.fx * sn.f.fwd.x + o.fz * sn.f.fwd.y < 0; // heading against its way (SUMO's direction)
		sn.walker(rel.dot(sn.f.fwd), sn.f.u + rel.dot(sn.f.left), facing && o.moving);
	}
}

// Its own sidewalk's furniture near it, by arc: `from` is the first piece that may reach back to it, carried along by
// the caller as it goes through the sidewalk's walkers in order of arc.
void see_sidewalk_furniture(const std::vector<StreetObstacle> &list, size_t from, const CrowdEntry &w, Seen &sn) {
	for (auto it = list.begin() + std::ptrdiff_t(from); it != list.end() && it->s0 <= w.s + sn.look && !sn.full(); ++it) {
		Body b;
		b.furniture = true;
		b.reach = sn.reach;
		b.along_lo = w.dir == 0 ? it->s0 - w.s : w.s - it->s1;
		b.along_hi = w.dir == 0 ? it->s1 - w.s : w.s - it->s0;
		b.lat_lo = it->u0;
		b.lat_hi = it->u1;
		sn.furniture(b);
	}
}

// The furniture on the sidewalks of junction `n`'s arms as far from it as its walkers may be and look.
void junction_furniture(CityState &s, int n, float look, std::vector<const StreetObstacle *> &out) {
	out.clear();
	const float reach = s.catalog.furniture.crossing.zebra_to_m + look + s.catalog.traffic.ped_body_m;
	for (int seg : s.roads.nodes[size_t(n)].segs) {
		if (!walkable(s, seg)) continue;
		const RoadSegment &g = s.roads.segs[size_t(seg)];
		const SidewalkFurniture &sf = sidewalk_furniture(s, seg);
		const float lo = g.a == n ? -BIG : float(g.len - g.retreat_b) - reach - sf.longest;
		const float hi = g.a == n ? float(g.retreat_a) + reach : BIG;
		for (const auto &side : sf.side)
			for (auto it = std::lower_bound(side.begin(), side.end(), lo, [](const StreetObstacle &o, float v) { return o.s0 < v; });
					it != side.end() && it->s0 <= hi; ++it)
				out.push_back(&*it);
	}
}

// A footprint's corners projected ahead and across the walker's frame.
void see_footprint(const StreetObstacle &o, Seen &sn) {
	if (sn.full()) return;
	const V2 c = V2(o.x, o.z) - sn.f.at, a(o.fx, o.fz), l = left_of(a);
	// by its centre first: a piece wholly behind, beyond the look or off the room across is not projected
	const float spread = o.half_along + o.half_across, ahead = c.dot(sn.f.fwd), across = sn.f.u + c.dot(sn.f.left);
	if (ahead + spread < -sn.reach || ahead - spread > sn.look || across + spread < sn.f.lo - sn.reach || across - spread > sn.f.hi + sn.reach) return;
	Body b;
	b.furniture = true;
	b.reach = sn.reach;
	b.along_lo = b.lat_lo = BIG;
	b.along_hi = b.lat_hi = -BIG;
	for (int k = 0; k < 4; k++) {
		const V2 q = c + a * (k & 1 ? o.half_along : -o.half_along) + l * (k & 2 ? o.half_across : -o.half_across);
		const float along = q.dot(sn.f.fwd), lat = sn.f.u + q.dot(sn.f.left);
		b.along_lo = std::min(b.along_lo, along);
		b.along_hi = std::max(b.along_hi, along);
		b.lat_lo = std::min(b.lat_lo, lat);
		b.lat_hi = std::max(b.lat_hi, lat);
	}
	sn.furniture(b);
}

void file(std::vector<std::vector<CrowdEntry>> &lists, std::vector<int> &keys, int key, const CrowdEntry &e) {
	auto &list = lists[size_t(key)];
	if (list.empty()) keys.push_back(key);
	list.push_back(e);
}

// `level`: the share of its own speed it goes at to stay level with the one it walks with (ped_party.cpp company_pace).
CrowdEntry entry_of(const Pedestrian &w, int id, float moving, float level) {
	CrowdEntry e;
	e.id = id;
	e.sidewalk = w.mode == PedMode::SIDEWALK ? w.seg * 2 + w.side : -1;
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
	e.moving = w.mode != PedMode::WAIT && w.pace > moving;
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
		const CrowdEntry e = entry_of(w, id, moving, company_pace(t, s.catalog.traffic, w));
		if (e.sidewalk >= 0) file(t.crowd, t.crowd_keys, e.sidewalk, e);
		else if (w.way_node >= 0)
			for (int k = junctions_round(s, w.way_node, near); k-- > 0;) file(t.node_walkers, t.node_keys, near[k], e);
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
			const int nn = ends.ahead(w.s, w.dir, zone, near);
			WayFrame f;
			sidewalk_frame(w, half, p.ped_stripe_m, nn > 0, f);
			Seen sn{ f, bodies, 0, p.ped_look_m, touch, reach };
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
