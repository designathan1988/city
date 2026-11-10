// The solve of the network's road heights (Roadcraft 4.0 world/elevation.ts buildRoadElevation, its behaviour): the
// profiles whose inputs changed made again, the nodes at their ends (one ring further, and those a ramp carried up)
// settled, every profile at those nodes solved from its grade line between them, in rounds until no node rises; then
// the plates pinned and what each node settled on kept, so the next solve starts from it. In id order: the same map
// gives the same heights.
#include "sim/roads/road_grade_internal.h"

#include "core/job_pool.h"

#include "sim/roads/road_network.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>

namespace city {

namespace {
constexpr int MAX_SOLVE_PASSES = 64; // MAX_SOLVE_PASSES
constexpr size_t POOLED_SEGMENTS = 64; // below this an edit's few segments cost less than the pool's hand-off
constexpr float RISE_EPS = 1e-6f; // a node rose: past rounding
constexpr float MOVED_EPS = 1e-4f; // a profile moved: past what a station's rounding is
constexpr float DISC_MIN_M = 2.4f; // balancedAtNode: the least radius of a node's disc of ground
constexpr float PROFILE_FADE_M = 44.f; // roadField.ts PROFILE_FADE: how far out a road's influence fades
constexpr float DISC_INSIDE_FADE_M = 8.f; // balancedAtNode: the disc stops this far inside the fade
constexpr int DISC_RINGS = 2, DISC_POINTS = 8;
constexpr float DISC_TWIST = 0.4f; // radians a ring turns from the one inside it
constexpr float TAU_F = 6.28318531f;
const float NONE = INFINITY;

struct Solve {
	RoadGrade &g;
	RoadNetwork &net;
	const GradeTune t;
	const GroundAt &ground;
	std::vector<char> &seg_solved, &node_solved;
	std::vector<float> &height, &start, &grade, &cap; // by node, this solve's (NONE: not reached)
	std::vector<char> &aloft;
	std::vector<int> &heighted; // the nodes whose height was written (RoadGrade::Scratch)
	std::vector<int> solved_nodes, solved_segs;

	void set_height(int n, float h) {
		if (height[size_t(n)] == NONE) heighted.push_back(n);
		height[size_t(n)] = h;
	}

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
		set_height(n, h);
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
			set_height(n, h);
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

void RoadGrade::Scratch::grow(size_t segs, size_t nodes) {
	if (touched.size() < segs) {
		touched.resize(segs, 0);
		regrounded.resize(segs, 0);
		seg_solved.resize(segs, 0);
		chained.resize(segs, 0);
	}
	if (node_solved.size() < nodes) {
		node_solved.resize(nodes, 0);
		aloft.resize(nodes, 0);
		height.resize(nodes, NONE);
		start.resize(nodes, 0);
		grade.resize(nodes, 0);
		cap.resize(nodes, NONE);
	}
}

namespace {
// What the ground's shape under a segment is made of (road_grade_sync.cpp shape_of: its curve, casing, build and heights)
// as a profile had it before this solve made it again.
struct ShapeInputs {
	bool valid = false;
	double q[CURVE_NUMBERS] = {};
	float step = 0, half = 0;
	Structure structure = Structure::GROUND;
	bool manual = false, walls = false;
	std::vector<float> h;
	explicit ShapeInputs(const GradeProfile &p)
		: valid(p.valid), step(p.step), half(p.half), structure(p.structure), manual(p.manual), walls(p.walls), h(p.h) {
		std::copy(p.q, p.q + CURVE_NUMBERS, q);
	}
	// The profile makes another shape: its curve (p.q is the curve's own after grade_inputs), casing, build or a height.
	bool differs(const GradeProfile &p) const {
		return !valid || !std::equal(q, q + CURVE_NUMBERS, p.q) || step != p.step || half != p.half || structure != p.structure ||
				manual != p.manual || walls != p.walls || h.size() != p.h.size() ||
				std::memcmp(h.data(), p.h.data(), h.size() * sizeof(float)) != 0; // bit for bit: the shape is made of these floats
	}
};
} // namespace

void RoadGrade::solve(RoadNetwork &net, const GroundAt &ground, std::vector<int> &moved, std::vector<int> &gone, JobPool *jobs) {
	const size_t ns = net.segs.size(), nn = net.nodes.size();
	prof.resize(ns);
	node.resize(nn);
	unshown_.resize(ns, 0);
	Scratch &sc = scratch_;
	sc.grow(ns, nn);
	std::vector<int> touched;
	auto touch = [&](int s, bool reground) {
		if (s < 0 || size_t(s) >= ns) return;
		if (!sc.touched[size_t(s)]) {
			sc.touched[size_t(s)] = 1;
			touched.push_back(s);
		}
		if (reground) sc.regrounded[size_t(s)] = 1;
	};
	for (size_t s = 0; all_ && s < ns; s++) touch(int(s), true);
	for (int s : segs_) touch(s, false);
	for (int s : reground_) touch(s, true);
	for (int n : nodes_)
		if (n >= 0 && size_t(n) < nn) for (int s : net.nodes[size_t(n)].segs) touch(s, false);
	const bool all = all_;
	segs_.clear(); nodes_.clear(); reground_.clear();
	all_ = false;
	std::sort(touched.begin(), touched.end()); // in id order: the same map gives the same heights
	Solve sv{ *this, net, grade_tune(net.cat->tune), ground, sc.seg_solved, sc.node_solved, sc.height, sc.start, sc.grade, sc.cap,
		sc.aloft, sc.heighted, {}, {} };
	std::vector<int> ends, read;
	for (int s : touched) {
		if (!net.segs[size_t(s)].alive) {
			if (prof[size_t(s)].valid) gone.push_back(s);
			prof[size_t(s)] = GradeProfile();
			unshown_[size_t(s)] = 0;
			continue;
		}
		read.push_back(s);
	}
	std::vector<ShapeInputs> was; // by `read`: what each made the ground's shape of before it is read again
	was.reserve(read.size());
	for (int s : read) was.emplace_back(prof[size_t(s)]);
	// each segment reads the ground into its own profile and nothing else: over the pool when there are many (a whole city)
	auto inputs = [&](size_t k) { grade_inputs(*this, net, read[k], ground, sc.regrounded[size_t(read[k])] != 0, sv.t); };
	if (jobs && read.size() >= POOLED_SEGMENTS) jobs->parallel_for(read.size(), inputs);
	else for (size_t k = 0; k < read.size(); k++) inputs(k);
	for (int s : read) {
		ends.push_back(prof[size_t(s)].a);
		ends.push_back(prof[size_t(s)].b);
	}
	grade_chains(*this, net, read, sv.t.ground, ends, sc.chained);
	std::sort(ends.begin(), ends.end());
	ends.erase(std::unique(ends.begin(), ends.end()), ends.end());
	reach_nodes(sv, ends, all);
	std::sort(sv.solved_nodes.begin(), sv.solved_nodes.end());
	for (int n : sv.solved_nodes)
		for (int s : net.nodes[size_t(n)].segs)
			if (sv.alive_profile(s) && !sv.seg_solved[size_t(s)]) { sv.seg_solved[size_t(s)] = 1; sv.solved_segs.push_back(s); }
	std::sort(sv.solved_segs.begin(), sv.solved_segs.end());
	std::vector<std::vector<float>> before(sv.solved_segs.size()); // the heights of a solved segment no change reached
	for (size_t k = 0; k < sv.solved_segs.size(); k++) {
		GradeProfile &p = prof[size_t(sv.solved_segs[k])];
		if (!sc.touched[size_t(sv.solved_segs[k])]) before[k] = p.h;
		p.h = p.base; // solved again from its grade line
	}
	for (int n : sv.solved_nodes) sv.grade_node(n);
	for (int n : sv.solved_nodes) sv.start_node(n);
	sv.rounds();
	for (int n : sv.heighted) {
		if (sv.height[size_t(n)] == NONE) continue;
		GradeNode &m = node[size_t(n)];
		const bool here = sv.node_solved[size_t(n)] != 0;
		m = GradeNode{ sv.height[size_t(n)], here ? sv.start[size_t(n)] : m.start, sv.grade_of(n), sv.cap_of(n) == NONE ? 0.f : sv.cap_of(n),
			sv.aloft_at(n), sv.cap_of(n) != NONE, true };
		net.nodes[size_t(n)].y = m.height;
	}
	for (size_t k = 0; k < sv.solved_segs.size(); k++) {
		const int s = sv.solved_segs[k];
		GradeProfile &p = prof[size_t(s)];
		bool differs;
		if (sc.touched[size_t(s)]) {
			// a change reached it: moved when its shape is another or it was never told the heights it has
			const ShapeInputs &old = was[size_t(std::lower_bound(read.begin(), read.end(), s) - read.begin())];
			differs = unshown_[size_t(s)] != 0 || old.differs(p);
		} else {
			// carried by a node: moved past a station's rounding (below it the heights are kept untold)
			const std::vector<float> &then = before[k], &now = p.h;
			differs = then.size() != now.size();
			bool any = differs;
			for (size_t i = 0; !differs && i < now.size(); i++) {
				differs = std::fabs(then[i] - now[i]) > MOVED_EPS;
				any = any || then[i] != now[i];
			}
			if (!differs && any) unshown_[size_t(s)] = 1;
		}
		if (differs) {
			moved.push_back(s);
			unshown_[size_t(s)] = 0;
		}
		p.ylo = p.h.empty() ? 0.f : *std::min_element(p.h.begin(), p.h.end());
		p.yhi = p.h.empty() ? 0.f : *std::max_element(p.h.begin(), p.h.end());
	}
	// the scratch back at rest: only what this solve wrote
	for (int s : touched) sc.touched[size_t(s)] = sc.regrounded[size_t(s)] = 0;
	for (int s : sv.solved_segs) sc.seg_solved[size_t(s)] = 0;
	for (int n : sv.solved_nodes) {
		sc.node_solved[size_t(n)] = sc.aloft[size_t(n)] = 0;
		sc.start[size_t(n)] = sc.grade[size_t(n)] = 0;
		sc.cap[size_t(n)] = NONE;
	}
	for (int n : sv.heighted) sc.height[size_t(n)] = NONE;
	sv.heighted.clear();
}

} // namespace city
