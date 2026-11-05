// The solve of the network's road heights (Roadcraft 4.0 world/elevation.ts buildRoadElevation, its behaviour): the
// profiles whose inputs changed made again, the nodes at their ends (one ring further, and those a ramp carried up)
// settled, every profile at those nodes solved from its grade line between them, in rounds until no node rises; then
// the plates pinned and what each node settled on kept, so the next solve starts from it. In id order: the same map
// gives the same heights.
#include "sim/roads/road_grade_internal.h"

#include "sim/roads/road_network.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace city {

namespace {
constexpr int MAX_SOLVE_PASSES = 64; // MAX_SOLVE_PASSES
constexpr float RISE_EPS = 1e-6f; // a node rose: past rounding
constexpr float MOVED_EPS = 1e-4f; // a profile moved: past what a station's rounding is
constexpr float DISC_MIN_M = 2.4f; // balancedAtNode: the least radius of a node's disc of ground
constexpr float PROFILE_FADE_M = 44.f; // roadField.ts PROFILE_FADE: how far out a road's influence fades
constexpr float DISC_INSIDE_FADE_M = 8.f; // balancedAtNode: the disc stops this far inside the fade
constexpr int DISC_RINGS = 2, DISC_POINTS = 8;
constexpr float DISC_TWIST = 0.4f; // radians a ring turns from the one inside it
constexpr float TAU_F = 6.28318531f;
const float NONE = std::numeric_limits<float>::infinity();

struct Solve {
	RoadGrade &g;
	RoadNetwork &net;
	const GradeTune t;
	const GroundAt &ground;
	std::vector<char> seg_solved, node_solved;
	std::vector<float> height, start, grade, cap; // by node, this solve's (NONE: not reached)
	std::vector<char> aloft;
	std::vector<int> solved_nodes, solved_segs;

	bool alive_profile(int s) const { return net.segs[size_t(s)].alive && g.has(s); }
	const GradeNode &memo(int n) const { return g.node[size_t(n)]; }
	float grade_of(int n) const { return node_solved[size_t(n)] ? grade[size_t(n)] : memo(n).grade; }
	bool aloft_at(int n) const { return node_solved[size_t(n)] ? aloft[size_t(n)] != 0 : memo(n).aloft; }
	float cap_of(int n) const {
		if (node_solved[size_t(n)]) return cap[size_t(n)];
		return memo(n).capped ? memo(n).cap : NONE;
	}
	float node_height(int n) const { return height[size_t(n)] != NONE ? height[size_t(n)] : memo(n).height; }
	float end_height(const GradeProfile &p, int n) const { return p.a == n ? p.h.front() : p.h.back(); }

	void solve_node(int n) {
		if (node_solved[size_t(n)] || !net.nodes[size_t(n)].alive) return;
		node_solved[size_t(n)] = 1;
		solved_nodes.push_back(n);
	}

	// The mean ground over a junction's own disc: the height of a node no road at grade reaches (balancedAtNode).
	float balanced_at(int n) const {
		const RoadNode &nd = net.nodes[size_t(n)];
		float radius = DISC_MIN_M;
		for (int s : nd.segs) {
			if (!alive_profile(s)) continue;
			const GradeProfile &p = g.prof[size_t(s)];
			radius = std::max({ radius, p.a == n ? p.plate_a : p.plate_b, p.half + PROFILE_FADE_M - DISC_INSIDE_FADE_M });
		}
		float sum = ground(float(nd.x), float(nd.z));
		int count = 1;
		for (int ring = 1; ring <= DISC_RINGS; ring++) {
			const float r = radius * float(ring) / float(DISC_RINGS);
			for (int k = 0; k < DISC_POINTS; k++) {
				const float a = float(k) / float(DISC_POINTS) * TAU_F + float(ring) * DISC_TWIST;
				sum += ground(float(nd.x) + std::cos(a) * r, float(nd.z) + std::sin(a) * r);
				count++;
			}
		}
		return sum / float(count) + t.clearance;
	}

	// Where the roads meeting at n want it: the mean of their grade lines at their plate edges (only roads at grade), and
	// its lift; aloft when every one of two or more roads there is raised.
	void grade_node(int n) {
		const RoadNode &nd = net.nodes[size_t(n)];
		float sum = 0;
		int count = 0, roads = 0, raised = 0;
		for (int s : nd.segs) {
			if (!alive_profile(s)) continue;
			const GradeProfile &p = g.prof[size_t(s)];
			roads++;
			if (is_raised(p.structure)) raised++;
			if (is_raised(p.structure) || is_sunken(p.structure)) continue;
			sum += p.base[size_t(station_index(p, p.a == n ? p.plate_a : p.len - p.plate_b))];
			count++;
		}
		grade[size_t(n)] = (count > 0 ? sum / float(count) : balanced_at(n)) + nd.lift;
		aloft[size_t(n)] = roads > 1 && raised == roads;
	}

	// The deck a raised span wants at its a or b end: its clearance over the ground of the plate.
	float wanted_end(const GradeProfile &p, bool at_a) const {
		const float clearance = t.of(p.structure).clearance;
		float top = -NONE;
		const float from = at_a ? 0.f : p.len - p.plate_b, to = at_a ? p.plate_a : p.len;
		for (float s = from; s <= to + RISE_EPS; s += p.step) top = std::max(top, p.ceil[size_t(station_index(p, s))] + clearance);
		return top;
	}

	// An aloft node starts at the deck its spans want, no higher than a ramp from a land end can climb in its span.
	void start_node(int n) {
		cap[size_t(n)] = NONE;
		float h = grade[size_t(n)];
		if (aloft[size_t(n)]) {
			float reachable = NONE;
			for (int s : net.nodes[size_t(n)].segs) {
				if (!alive_profile(s)) continue;
				const GradeProfile &p = g.prof[size_t(s)];
				if (!is_raised(p.structure)) continue;
				h = std::max(h, wanted_end(p, p.a == n));
				const int other = p.a == n ? p.b : p.a;
				if (aloft_at(other)) continue;
				const float run = std::max(0.f, p.len - p.plate_a - p.plate_b);
				reachable = std::min(reachable, grade_of(other) + ramp_rise(run, t));
			}
			if (reachable != NONE) cap[size_t(n)] = std::max(grade[size_t(n)], reachable);
			if (reachable < h) h = std::max(grade[size_t(n)], reachable);
		}
		height[size_t(n)] = h;
		start[size_t(n)] = h;
	}

	// The free height a sunken span wants: a tunnel's depth under the lowest of its ground and its ends.
	float wanted_of(const GradeProfile &p) const {
		const float floor = *std::min_element(p.ceil.begin(), p.ceil.end());
		return std::min(floor, std::min(grade_of(p.a), grade_of(p.b))) + t.of(p.structure).clearance;
	}

	void solve_profile(int s) {
		GradeProfile &p = g.prof[size_t(s)];
		const float ha = node_height(p.a), hb = node_height(p.b);
		if (is_raised(p.structure)) {
			RaisedEnds e;
			e.land_a = !aloft_at(p.a);
			e.land_b = !aloft_at(p.b);
			e.cap_a = cap_of(p.a) != NONE;
			e.cap_b = cap_of(p.b) != NONE;
			solve_raised(p, ha, hb, e, t);
		} else if (is_sunken(p.structure)) solve_sunken(p, ha, hb, wanted_of(p), t);
		else if (p.manual) solve_variable(p, ha, hb, t);
		else solve_ground(p, ha, hb, t);
	}

	// The nodes at the ends of the roads just solved rise to the highest end (capped); their roads are solved again.
	std::vector<int> settle(const std::vector<int> &dirty) {
		std::vector<int> ends, next;
		for (int s : dirty) {
			ends.push_back(g.prof[size_t(s)].a);
			ends.push_back(g.prof[size_t(s)].b);
		}
		std::sort(ends.begin(), ends.end());
		ends.erase(std::unique(ends.begin(), ends.end()), ends.end());
		for (int n : ends) {
			const float was = node_height(n);
			float h = was;
			for (int s : net.nodes[size_t(n)].segs)
				if (alive_profile(s)) h = std::max(h, end_height(g.prof[size_t(s)], n));
			if (cap_of(n) != NONE) h = std::min(h, cap_of(n));
			height[size_t(n)] = h;
			if (h > was + RISE_EPS)
				for (int s : net.nodes[size_t(n)].segs)
					if (alive_profile(s)) next.push_back(s);
		}
		std::sort(next.begin(), next.end());
		next.erase(std::unique(next.begin(), next.end()), next.end());
		return next;
	}

	void rounds() {
		std::vector<int> dirty = solved_segs;
		for (int pass = 0; pass < MAX_SOLVE_PASSES && !dirty.empty(); pass++) {
			for (int s : dirty) {
				solve_profile(s);
				if (!seg_solved[size_t(s)]) { seg_solved[size_t(s)] = 1; solved_segs.push_back(s); }
			}
			dirty = settle(dirty);
		}
		for (int s : dirty) solve_profile(s);
		for (int s : solved_segs) pin_plates(g.prof[size_t(s)], node_height(g.prof[size_t(s)].a), node_height(g.prof[size_t(s)].b));
	}
};

// The nodes a change reaches: the ends of what changed, one ring further, and on from a node a raised road carried
// above where it started, along each road whose end stood at that height.
void reach_nodes(Solve &sv, const std::vector<int> &changed_ends, bool all) {
	for (int n : changed_ends) sv.solve_node(n);
	if (all) return;
	for (size_t k = 0, first = sv.solved_nodes.size(); k < first; k++)
		for (int s : sv.net.nodes[size_t(sv.solved_nodes[k])].segs)
			if (sv.alive_profile(s)) { sv.solve_node(sv.g.prof[size_t(s)].a); sv.solve_node(sv.g.prof[size_t(s)].b); }
	for (size_t k = 0; k < sv.solved_nodes.size(); k++) {
		const int n = sv.solved_nodes[k];
		for (int s : sv.net.nodes[size_t(n)].segs) {
			if (!sv.alive_profile(s)) continue;
			const GradeProfile &p = sv.g.prof[size_t(s)];
			const int other = p.a == n ? p.b : p.a;
			if (sv.node_solved[size_t(other)] || !sv.memo(other).known) continue;
			const GradeNode &there = sv.memo(other);
			if (there.height <= there.start + RISE_EPS || sv.end_height(p, other) < there.height - RISE_EPS) continue;
			sv.solve_node(other); // pushed at the back: walked in turn
		}
	}
}
} // namespace

void RoadGrade::solve(RoadNetwork &net, const GroundAt &ground, std::vector<int> &moved, std::vector<int> &gone) {
	const size_t ns = net.segs.size(), nn = net.nodes.size();
	prof.resize(ns);
	node.resize(nn);
	std::vector<char> touched(ns, 0), regrounded(ns, 0);
	auto touch = [&](int s, bool reground) {
		if (s < 0 || size_t(s) >= ns) return;
		touched[size_t(s)] = 1;
		if (reground) regrounded[size_t(s)] = 1;
	};
	for (size_t s = 0; all_ && s < ns; s++) touch(int(s), true);
	for (int s : segs_) touch(s, false);
	for (int s : reground_) touch(s, true);
	for (int n : nodes_)
		if (n >= 0 && size_t(n) < nn) for (int s : net.nodes[size_t(n)].segs) touch(s, false);
	const bool all = all_;
	segs_.clear(); nodes_.clear(); reground_.clear();
	all_ = false;
	Solve sv{ *this, net, grade_tune(net.cat->tune), ground, std::vector<char>(ns, 0), std::vector<char>(nn, 0),
		std::vector<float>(nn, NONE), std::vector<float>(nn, 0), std::vector<float>(nn, 0), std::vector<float>(nn, NONE),
		std::vector<char>(nn, 0), {}, {} };
	std::vector<int> ends;
	for (size_t s = 0; s < ns; s++) {
		if (!touched[s]) continue;
		if (!net.segs[s].alive) {
			if (prof[s].valid) gone.push_back(int(s));
			prof[s] = GradeProfile();
			continue;
		}
		grade_inputs(*this, net, int(s), ground, regrounded[s] != 0, sv.t);
		ends.push_back(prof[s].a);
		ends.push_back(prof[s].b);
	}
	std::sort(ends.begin(), ends.end());
	ends.erase(std::unique(ends.begin(), ends.end()), ends.end());
	reach_nodes(sv, ends, all);
	std::sort(sv.solved_nodes.begin(), sv.solved_nodes.end());
	for (int n : sv.solved_nodes)
		for (int s : net.nodes[size_t(n)].segs)
			if (sv.alive_profile(s) && !sv.seg_solved[size_t(s)]) { sv.seg_solved[size_t(s)] = 1; sv.solved_segs.push_back(s); }
	std::sort(sv.solved_segs.begin(), sv.solved_segs.end());
	std::vector<std::vector<float>> before(sv.solved_segs.size());
	for (size_t k = 0; k < sv.solved_segs.size(); k++) {
		GradeProfile &p = prof[size_t(sv.solved_segs[k])];
		before[k] = touched[size_t(sv.solved_segs[k])] ? std::vector<float>() : p.h;
		p.h = p.base; // solved again from its grade line
	}
	for (int n : sv.solved_nodes) sv.grade_node(n);
	for (int n : sv.solved_nodes) sv.start_node(n);
	sv.rounds();
	for (size_t n = 0; n < nn; n++) {
		if (sv.height[n] == NONE) continue;
		GradeNode &m = node[n];
		const bool here = sv.node_solved[n] != 0;
		m = GradeNode{ sv.height[n], here ? sv.start[n] : m.start, sv.grade_of(int(n)), sv.cap_of(int(n)) == NONE ? 0.f : sv.cap_of(int(n)),
			sv.aloft_at(int(n)), sv.cap_of(int(n)) != NONE, true };
		net.nodes[n].y = m.height;
	}
	for (size_t k = 0; k < sv.solved_segs.size(); k++) {
		const std::vector<float> &was = before[k], &now = prof[size_t(sv.solved_segs[k])].h;
		bool differs = was.size() != now.size();
		for (size_t i = 0; !differs && i < now.size(); i++) differs = std::fabs(was[i] - now[i]) > MOVED_EPS;
		if (differs) moved.push_back(sv.solved_segs[k]);
	}
}

} // namespace city
