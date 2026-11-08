// The solve of the network's road heights (Roadcraft 4.0 world/elevation.ts buildRoadElevation, its behaviour): the
// profiles whose inputs changed made again, the nodes at their ends settled in the rounds of road_grade_rounds.cpp; then
// what each node settled on kept, so the next solve starts from it, the segments whose heights moved told, and the
// scratch put back at rest. In id order: the same map gives the same heights.
#include "sim/roads/road_grade_rounds.h"

#include "core/job_pool.h"
#include "core/math.h"

#include "sim/roads/road_network.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>

namespace city {

namespace {
constexpr size_t POOLED_SEGMENTS = 64; // below this an edit's few segments cost less than the pool's hand-off
constexpr float MOVED_EPS = 1e-4f; // a profile moved: past what a station's rounding is
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

// The segments a change reached, in id order, flagged in the scratch (regrounded: their ground read again): every one
// when the whole network changed, those marked, those whose ground changed and those at a marked node.
std::vector<int> touch_changed(RoadGrade::Scratch &sc, const RoadNetwork &net, bool all, const std::vector<int> &marked_segs,
		const std::vector<int> &marked_nodes, const std::vector<int> &marked_ground) {
	const size_t ns = net.segs.size(), nn = net.nodes.size();
	std::vector<int> touched;
	auto touch = [&](int s, bool reground) {
		if (s < 0 || size_t(s) >= ns) return;
		if (!sc.touched[size_t(s)]) {
			sc.touched[size_t(s)] = 1;
			touched.push_back(s);
		}
		if (reground) sc.regrounded[size_t(s)] = 1;
	};
	for (size_t s = 0; all && s < ns; s++) touch(int(s), true);
	for (int s : marked_segs) touch(s, false);
	for (int s : marked_ground) touch(s, true);
	for (int n : marked_nodes)
		if (n >= 0 && size_t(n) < nn) for (int s : net.nodes[size_t(n)].segs) touch(s, false);
	std::sort(touched.begin(), touched.end()); // in id order: the same map gives the same heights
	return touched;
}

// The segments of `touched` still alive, to be read again; a dead one's profile cleared, into `gone` when it had one.
std::vector<int> alive_segments(const RoadNetwork &net, const std::vector<int> &touched, std::vector<GradeProfile> &prof,
		std::vector<char> &unshown, std::vector<int> &gone) {
	std::vector<int> read;
	for (int s : touched) {
		if (!net.segs[size_t(s)].alive) {
			if (prof[size_t(s)].valid) gone.push_back(s);
			prof[size_t(s)] = GradeProfile();
			unshown[size_t(s)] = 0;
			continue;
		}
		read.push_back(s);
	}
	return read;
}

// The segments of `read` made ready for the solve and the roads laid in pieces among them made one line: the nodes at
// their ends, in id order.
std::vector<int> read_segments(RoadGrade &grade, const RoadNetwork &net, const std::vector<int> &read, const GroundAt &ground,
		const GradeTune &t, RoadGrade::Scratch &sc, JobPool *jobs) {
	std::vector<int> ends;
	// each segment reads the ground into its own profile and nothing else: over the pool when there are many (a whole city)
	auto inputs = [&](size_t k) { grade_inputs(grade, net, read[k], ground, sc.regrounded[size_t(read[k])] != 0, t); };
	if (jobs && read.size() >= POOLED_SEGMENTS) jobs->parallel_for(read.size(), inputs);
	else for (size_t k = 0; k < read.size(); k++) inputs(k);
	for (int s : read) {
		ends.push_back(grade.prof[size_t(s)].a);
		ends.push_back(grade.prof[size_t(s)].b);
	}
	grade_chains(grade, net, read, t.ground, ends, sc.chained);
	std::sort(ends.begin(), ends.end());
	ends.erase(std::unique(ends.begin(), ends.end()), ends.end());
	return ends;
}

// The segments at the nodes the change reached, in id order, each to be solved again from its grade line; by its place
// among them, the heights one no change reached had before (the others' left empty).
std::vector<std::vector<float>> take_solved(Solve &sv, const RoadGrade::Scratch &sc) {
	std::sort(sv.solved_nodes.begin(), sv.solved_nodes.end());
	for (int n : sv.solved_nodes)
		for (int s : sv.net.nodes[size_t(n)].segs)
			if (sv.alive_profile(s) && !sv.seg_solved[size_t(s)]) { sv.seg_solved[size_t(s)] = 1; sv.solved_segs.push_back(s); }
	std::sort(sv.solved_segs.begin(), sv.solved_segs.end());
	std::vector<std::vector<float>> before(sv.solved_segs.size()); // the heights of a solved segment no change reached
	for (size_t k = 0; k < sv.solved_segs.size(); k++) {
		GradeProfile &p = sv.g.prof[size_t(sv.solved_segs[k])];
		if (!sc.touched[size_t(sv.solved_segs[k])]) before[k] = p.h;
		p.h = p.base; // solved again from its grade line
	}
	return before;
}

// What each node whose height this solve wrote settled on: kept for the next solve, and the network's node stood on it.
void keep_nodes(Solve &sv) {
	for (int n : sv.heighted) {
		if (sv.height[size_t(n)] == NONE) continue;
		GradeNode &m = sv.g.node[size_t(n)];
		const bool here = sv.node_solved[size_t(n)] != 0;
		m = GradeNode{ sv.height[size_t(n)], here ? sv.start[size_t(n)] : m.start, sv.grade_of(n), sv.cap_of(n) == NONE ? 0.f : sv.cap_of(n),
			sv.aloft_at(n), sv.cap_of(n) != NONE, true };
		sv.net.nodes[size_t(n)].y = m.height;
	}
}

// Each solved segment told moved (into `moved`) or not, and the bounds of its heights set. `read` and `was`: the segments
// read again and what each made the ground's shape of before; `before`: by `sv.solved_segs`, the heights of one no change
// reached; `unshown`: by segment, its heights moved since they were last told, by less than a station's rounding.
void report_moved(Solve &sv, const RoadGrade::Scratch &sc, const std::vector<int> &read, const std::vector<ShapeInputs> &was,
		const std::vector<std::vector<float>> &before, std::vector<char> &unshown, std::vector<int> &moved) {
	for (size_t k = 0; k < sv.solved_segs.size(); k++) {
		const int s = sv.solved_segs[k];
		GradeProfile &p = sv.g.prof[size_t(s)];
		bool differs;
		if (sc.touched[size_t(s)]) {
			// a change reached it: moved when its shape is another or it was never told the heights it has
			const ShapeInputs &old = was[size_t(std::lower_bound(read.begin(), read.end(), s) - read.begin())];
			differs = unshown[size_t(s)] != 0 || old.differs(p);
		} else {
			// carried by a node: moved past a station's rounding (below it the heights are kept untold)
			const std::vector<float> &then = before[k], &now = p.h;
			differs = then.size() != now.size();
			bool any = differs;
			for (size_t i = 0; !differs && i < now.size(); i++) {
				differs = std::fabs(then[i] - now[i]) > MOVED_EPS;
				any = any || then[i] != now[i];
			}
			if (!differs && any) unshown[size_t(s)] = 1;
		}
		if (differs) {
			moved.push_back(s);
			unshown[size_t(s)] = 0;
		}
		p.ylo = p.h.empty() ? 0.f : *std::min_element(p.h.begin(), p.h.end());
		p.yhi = p.h.empty() ? 0.f : *std::max_element(p.h.begin(), p.h.end());
	}
}

// The scratch back at rest: only what this solve wrote.
void rest_scratch(RoadGrade::Scratch &sc, const std::vector<int> &touched, Solve &sv) {
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
} // namespace

void RoadGrade::solve(RoadNetwork &net, const GroundAt &ground, std::vector<int> &moved, std::vector<int> &gone, JobPool *jobs) {
	const size_t ns = net.segs.size(), nn = net.nodes.size();
	prof.resize(ns);
	node.resize(nn);
	unshown_.resize(ns, 0);
	Scratch &sc = scratch_;
	sc.grow(ns, nn);
	const std::vector<int> touched = touch_changed(sc, net, all_, segs_, nodes_, reground_);
	const bool all = all_;
	segs_.clear(); nodes_.clear(); reground_.clear();
	all_ = false;
	Solve sv{ *this, net, grade_tune(net.cat->tune), ground, sc.seg_solved, sc.node_solved, sc.height, sc.start, sc.grade, sc.cap,
		sc.aloft, sc.heighted, {}, {} };
	const std::vector<int> read = alive_segments(net, touched, prof, unshown_, gone);
	std::vector<ShapeInputs> was; // by `read`: what each made the ground's shape of before it is read again
	was.reserve(read.size());
	for (int s : read) was.emplace_back(prof[size_t(s)]);
	reach_nodes(sv, read_segments(*this, net, read, ground, sv.t, sc, jobs), all);
	const std::vector<std::vector<float>> before = take_solved(sv, sc);
	for (int n : sv.solved_nodes) sv.grade_node(n);
	for (int n : sv.solved_nodes) sv.start_node(n);
	sv.rounds();
	keep_nodes(sv);
	report_moved(sv, sc, read, was, before, unshown_, moved);
	rest_scratch(sc, touched, sv);
}

} // namespace city
