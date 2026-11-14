// The solve of the network's road heights (road_grade.h): the segments a change reached framed again and their design
// lines made (grade_ground.cpp), the nodes at their ends settled with every span at those nodes (grade_settle.cpp), what
// each node settled on kept so the next solve starts from it, and the segments whose heights moved told. In id order:
// the same map gives the same heights.
#include "sim/roads/grade_settle.h"

#include "core/job_pool.h"
#include "sim/roads/road_network.h"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace city {

namespace {
constexpr size_t POOLED_SEGMENTS = 64; // below this an edit's few segments cost less than the pool's hand-off
constexpr float MOVED_EPS = 1e-4f; // a profile moved: past what a station's rounding is
constexpr float NONE = Settling::NONE;

// What the ground's shape under a segment is made of (road_grade_sync.cpp: its curve, casing, build and heights) as a
// profile had it before this solve framed it again.
struct Shape {
	bool valid = false;
	double q[CURVE_NUMBERS] = {};
	float step = 0, half = 0;
	Structure structure = Structure::GROUND;
	bool manual = false, walls = false;
	std::vector<float> h;
	explicit Shape(const GradeProfile &p)
		: valid(p.valid), step(p.step), half(p.half), structure(p.structure), manual(p.manual), walls(p.walls), h(p.h) {
		std::copy(p.q, p.q + CURVE_NUMBERS, q);
	}
	// The profile makes another shape: its curve, casing, build or a height (bit for bit: the shape is made of these floats).
	bool differs(const GradeProfile &p) const {
		return !valid || !std::equal(q, q + CURVE_NUMBERS, p.q) || step != p.step || half != p.half || structure != p.structure ||
				manual != p.manual || walls != p.walls || h.size() != p.h.size() ||
				std::memcmp(h.data(), p.h.data(), h.size() * sizeof(float)) != 0;
	}
};

// The segments a change reached framed again, the ground read under those whose ground or frame changed (over the pool
// when they are many: each reads into its own lines and nothing else), their design lines made; gives the ends of
// every piece of road whose line was made.
std::vector<int> frame_and_line(Settling &sv, const std::vector<int> &read, JobPool *jobs) {
	RoadGrade &g = sv.g;
	std::vector<int> stale, ends;
	for (int s : read) {
		frame_profile(g, sv.net, s);
		if (sv.sc.regrounded[size_t(s)] || !g.ground.by_seg[size_t(s)].read_for(g.prof[size_t(s)])) stale.push_back(s);
	}
	for (int s : stale) g.ground.open(s);
	const auto ground = [&](size_t k) { read_ground(g, sv.net, stale[k], sv.land, sv.t); };
	if (jobs && stale.size() >= POOLED_SEGMENTS) jobs->parallel_for(stale.size(), ground);
	else for (size_t k = 0; k < stale.size(); k++) ground(k);
	for (int s : stale) g.ground.close(s);
	for (int s : read) {
		if (sv.sc.lined[size_t(s)]) continue;
		for (int piece : sv.line_road(s)) {
			ends.push_back(g.prof[size_t(piece)].a);
			ends.push_back(g.prof[size_t(piece)].b);
		}
	}
	std::sort(ends.begin(), ends.end());
	ends.erase(std::unique(ends.begin(), ends.end()), ends.end());
	return ends;
}
} // namespace

void RoadGrade::Scratch::grow(size_t segs, size_t nodes) {
	if (touched.size() < segs) {
		touched.resize(segs, 0);
		regrounded.resize(segs, 0);
		seg_solved.resize(segs, 0);
		lined.resize(segs, 0);
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

double RoadGrade::at(int s, double arc) const {
	const GradeProfile &p = prof[size_t(s)];
	const double last = double(p.h.size() - 1);
	const double f = std::min(last, std::max(0.0, arc / double(p.step)));
	const size_t lo = size_t(std::floor(f)), hi = std::min(p.h.size() - 1, lo + 1);
	return double(p.h[lo]) + double(p.h[hi] - p.h[lo]) * (f - double(lo));
}

std::vector<int> RoadGrade::take_marks(const RoadNetwork &net) {
	const size_t ns = net.segs.size(), nn = net.nodes.size();
	Scratch &sc = scratch_;
	std::vector<int> touched;
	const auto touch = [&](int s, bool reground) {
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
		if (n >= 0 && size_t(n) < nn)
			for (int s : net.nodes[size_t(n)].segs) touch(s, false);
	segs_.clear();
	nodes_.clear();
	reground_.clear();
	all_ = false;
	std::sort(touched.begin(), touched.end()); // in id order: the same map gives the same heights
	return touched;
}

bool RoadGrade::carried(int s, const std::vector<float> &then) {
	const std::vector<float> &now = prof[size_t(s)].h;
	bool differs = then.size() != now.size(), any = differs;
	for (size_t i = 0; !differs && i < now.size(); i++) {
		differs = std::fabs(then[i] - now[i]) > MOVED_EPS;
		any = any || then[i] != now[i];
	}
	if (!differs && any) unshown_[size_t(s)] = 1; // under a station's rounding: kept untold
	return differs;
}

void RoadGrade::solve(RoadNetwork &net, const GroundAt &ground_at, std::vector<int> &moved, std::vector<int> &gone, JobPool *jobs) {
	const size_t ns = net.segs.size(), nn = net.nodes.size();
	prof.resize(ns);
	node.resize(nn);
	unshown_.resize(ns, 0);
	if (ground.by_seg.size() < ns) ground.by_seg.resize(ns);
	ground.solve++;
	Scratch &sc = scratch_;
	sc.grow(ns, nn);
	const bool all = all_;
	const std::vector<int> touched = take_marks(net);
	Settling sv{ *this, net, grade_tune(net.cat->tune), ground_at, sc, {}, {}, {} };
	std::vector<int> read;
	for (int s : touched) {
		if (net.segs[size_t(s)].alive) {
			read.push_back(s);
			continue;
		}
		if (prof[size_t(s)].valid) gone.push_back(s);
		prof[size_t(s)] = GradeProfile();
		unshown_[size_t(s)] = 0;
		ground.drop(s);
	}
	std::vector<Shape> was; // by `read`: what each made the ground's shape of before it is framed again
	was.reserve(read.size());
	for (int s : read) was.emplace_back(prof[size_t(s)]);
	sv.reach_from(frame_and_line(sv, read, jobs), all);
	std::sort(sv.nodes.begin(), sv.nodes.end());
	for (int n : sv.nodes)
		for (int s : net.nodes[size_t(n)].segs)
			if (sv.standing(s)) sv.take(s);
	std::sort(sv.spans.begin(), sv.spans.end());
	std::vector<std::vector<float>> before(sv.spans.size()); // the heights of a span no change reached
	for (size_t k = 0; k < before.size(); k++)
		if (!sc.touched[size_t(sv.spans[k])]) before[k] = prof[size_t(sv.spans[k])].h;
	for (int n : sv.nodes) sv.want_node(n);
	for (int n : sv.nodes) sv.start_node(n);
	sv.rounds();
	for (int n : sc.heighted) {
		GradeNode &m = node[size_t(n)];
		const float cap = sv.cap_of(n);
		m = GradeNode{ sc.height[size_t(n)], sv.reached(n) ? sc.start[size_t(n)] : m.start, sv.grade_of(n), cap == NONE ? 0.f : cap, sv.aloft(n),
			cap != NONE, true };
		net.nodes[size_t(n)].y = m.height;
	}
	for (size_t k = 0; k < sv.spans.size(); k++) {
		const int s = sv.spans[k];
		GradeProfile &p = prof[size_t(s)];
		bool differs;
		// a change reached it: moved when its shape is another or it was never told the heights it has; carried by a node:
		// moved past a station's rounding (a span the rounds took after the first is told whole: it kept no `before`)
		if (sc.touched[size_t(s)]) differs = unshown_[size_t(s)] != 0 || was[size_t(std::lower_bound(read.begin(), read.end(), s) - read.begin())].differs(p);
		else differs = k >= before.size() || carried(s, before[k]);
		if (differs) {
			moved.push_back(s);
			unshown_[size_t(s)] = 0;
		}
		p.ylo = p.h.empty() ? 0.f : *std::min_element(p.h.begin(), p.h.end());
		p.yhi = p.h.empty() ? 0.f : *std::max_element(p.h.begin(), p.h.end());
	}
	// the scratch back at rest: only what this solve wrote
	for (int s : touched) sc.touched[size_t(s)] = sc.regrounded[size_t(s)] = 0;
	for (int s : sv.spans) sc.seg_solved[size_t(s)] = 0;
	for (int s : sv.lined) sc.lined[size_t(s)] = 0;
	for (int n : sv.nodes) {
		sc.node_solved[size_t(n)] = sc.aloft[size_t(n)] = 0;
		sc.start[size_t(n)] = sc.grade[size_t(n)] = 0;
		sc.cap[size_t(n)] = NONE;
	}
	for (int n : sc.heighted) sc.height[size_t(n)] = NONE;
	sc.heighted.clear();
	ground.fit(sv.t.ground_cache);
}

} // namespace city
