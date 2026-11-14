// The settling of the nodes (grade_settle.h). The ground lines a span needs are asked of the cache as the settling goes
// (lines): standing, they are read; let go, they are made again from the same ground, so a miss never changes a height.
#include "sim/roads/grade_settle.h"

#include "sim/roads/road_network.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr float RISE_EPS = 1e-6f; // a node rose: past rounding
// A node no road at grade reaches stands on the mean ground of a disc round it: its rings, the points of a ring, the
// turn of a ring from the one inside it, its least radius and how far past its roads' casing it reaches (where the
// ground the roads shape has settled back to the land's own).
constexpr int DISC_RINGS = 2, DISC_POINTS = 8;
constexpr float DISC_TURN_RAD = 0.4f, DISC_LEAST_M = 2.4f, DISC_PAST_CASING_M = 36.f;
constexpr float FULL_TURN_RAD = 6.28318531f;

float end_of(const GradeProfile &p, int n) { return p.a == n ? p.h.front() : p.h.back(); }
} // namespace

bool Settling::standing(int s) const { return net.segs[size_t(s)].alive && g.has(s); }

float Settling::cap_of(int n) const {
	if (reached(n)) return sc.cap[size_t(n)];
	return g.node[size_t(n)].capped ? g.node[size_t(n)].cap : NONE;
}

void Settling::stand(int n, float h) {
	if (sc.height[size_t(n)] == NONE) sc.heighted.push_back(n);
	sc.height[size_t(n)] = h;
}

void Settling::reach(int n) {
	if (reached(n) || !net.nodes[size_t(n)].alive) return;
	sc.node_solved[size_t(n)] = 1;
	nodes.push_back(n);
}

void Settling::take(int s) {
	if (sc.seg_solved[size_t(s)]) return;
	sc.seg_solved[size_t(s)] = 1;
	spans.push_back(s);
}

void Settling::reach_from(const std::vector<int> &changed_ends, bool all) {
	for (int n : changed_ends) reach(n);
	if (all) return;
	for (size_t k = 0, first = nodes.size(); k < first; k++)
		for (int s : net.nodes[size_t(nodes[k])].segs) {
			if (!standing(s)) continue;
			reach(g.prof[size_t(s)].a);
			reach(g.prof[size_t(s)].b);
		}
	for (size_t k = 0; k < nodes.size(); k++) { // a node reached here is walked in its turn
		const int n = nodes[k];
		for (int s : net.nodes[size_t(n)].segs) {
			if (!standing(s)) continue;
			const GradeProfile &p = g.prof[size_t(s)];
			const int other = p.a == n ? p.b : p.a;
			const GradeNode &there = g.node[size_t(other)];
			if (reached(other) || !there.known) continue;
			if (there.height > there.start + RISE_EPS && end_of(p, other) >= there.height - RISE_EPS) reach(other);
		}
	}
}

const GroundLines &Settling::lines(int s) {
	GroundLines &l = g.ground.by_seg[size_t(s)];
	if (l.line.size() != g.prof[size_t(s)].h.size() || !l.read_for(g.prof[size_t(s)])) line_road(s);
	l.used = g.ground.solve;
	return l;
}

std::vector<int> Settling::line_road(int s) {
	std::vector<int> pieces;
	design_lines(g, net, s, land, t, pieces);
	for (int piece : pieces) {
		if (sc.lined[size_t(piece)]) continue;
		sc.lined[size_t(piece)] = 1;
		lined.push_back(piece);
	}
	return pieces;
}

// The mean ground over a node's own disc: where a node no road at grade reaches stands.
float Settling::disc_ground(int n) const {
	const RoadNode &nd = net.nodes[size_t(n)];
	float radius = DISC_LEAST_M;
	for (int s : nd.segs) {
		if (!standing(s)) continue;
		const GradeProfile &p = g.prof[size_t(s)];
		radius = std::max({ radius, p.a == n ? p.plate_a : p.plate_b, p.half + DISC_PAST_CASING_M });
	}
	float sum = land(float(nd.x), float(nd.z));
	int count = 1;
	for (int ring = 1; ring <= DISC_RINGS; ring++) {
		const float r = radius * float(ring) / float(DISC_RINGS);
		for (int k = 0; k < DISC_POINTS; k++, count++) {
			const float turn = float(k) / float(DISC_POINTS) * FULL_TURN_RAD + float(ring) * DISC_TURN_RAD;
			sum += land(float(nd.x) + std::cos(turn) * r, float(nd.z) + std::sin(turn) * r);
		}
	}
	return sum / float(count) + t.clearance;
}

// The mean of the design lines of its roads at grade at their plates, and its lift; aloft when every one of two or more
// roads there is raised.
void Settling::want_node(int n) {
	const RoadNode &nd = net.nodes[size_t(n)];
	float sum = 0;
	int at_grade = 0, roads = 0, raised = 0;
	for (int s : nd.segs) {
		if (!standing(s)) continue;
		const GradeProfile &p = g.prof[size_t(s)];
		roads++;
		if (is_raised(p.structure)) raised++;
		if (is_raised(p.structure) || is_sunken(p.structure)) continue;
		sum += lines(s).line[size_t(p.a == n ? plate_last_a(p) : plate_first_b(p))];
		at_grade++;
	}
	sc.grade[size_t(n)] = (at_grade > 0 ? sum / float(at_grade) : disc_ground(n)) + nd.lift;
	sc.aloft[size_t(n)] = roads > 1 && raised == roads;
}

// The deck a raised span wants at its a or b end: its clearance over the highest ground of the plate.
float Settling::deck_at(int s, bool at_a) {
	const GradeProfile &p = g.prof[size_t(s)];
	const std::vector<float> &top = lines(s).top;
	const int from = at_a ? 0 : plate_first_b(p), to = at_a ? plate_last_a(p) : int(p.h.size()) - 1;
	return *std::max_element(top.begin() + from, top.begin() + to + 1) + t.of(p.structure).clearance;
}

// An aloft node starts at the deck its spans want, no higher than a ramp from a land end can climb in its span.
void Settling::start_node(int n) {
	sc.cap[size_t(n)] = NONE;
	float h = sc.grade[size_t(n)];
	if (sc.aloft[size_t(n)]) {
		float climbed = NONE;
		for (int s : net.nodes[size_t(n)].segs) {
			if (!standing(s) || !is_raised(g.prof[size_t(s)].structure)) continue;
			const GradeProfile &p = g.prof[size_t(s)];
			h = std::max(h, deck_at(s, p.a == n));
			const int other = p.a == n ? p.b : p.a;
			if (aloft(other)) continue;
			climbed = std::min(climbed, grade_of(other) + ramp_gain(std::max(0.f, p.len - p.plate_a - p.plate_b), t));
		}
		if (climbed != NONE) sc.cap[size_t(n)] = std::max(sc.grade[size_t(n)], climbed);
		if (climbed < h) h = std::max(sc.grade[size_t(n)], climbed);
	}
	stand(n, h);
	sc.start[size_t(n)] = h;
}

void Settling::solve_span(int s) {
	GradeProfile &p = g.prof[size_t(s)];
	const float ha = height(p.a), hb = height(p.b);
	if (is_raised(p.structure)) {
		const RaisedEnds ends{ !aloft(p.a), !aloft(p.b), cap_of(p.a) != NONE, cap_of(p.b) != NONE };
		span_raised(p, lines(s), ha, hb, ends, t);
	} else if (is_sunken(p.structure)) {
		// a bore's depth: under the lowest of its ground and of where its two ends want to stand
		const std::vector<float> &top = lines(s).top;
		const float lowest = std::min({ *std::min_element(top.begin(), top.end()), grade_of(p.a), grade_of(p.b) });
		span_sunken(p, ha, hb, lowest + t.of(p.structure).clearance, t);
	} else span_at_grade(p, lines(s), ha, hb, p.manual ? t.ramp : t.ground, t);
}

// The nodes at the ends of the spans just solved rise to their highest end (capped); the spans of a node that rose are
// solved again.
std::vector<int> Settling::settle(const std::vector<int> &solved) {
	std::vector<int> ends, again;
	for (int s : solved) {
		ends.push_back(g.prof[size_t(s)].a);
		ends.push_back(g.prof[size_t(s)].b);
	}
	std::sort(ends.begin(), ends.end());
	ends.erase(std::unique(ends.begin(), ends.end()), ends.end());
	for (int n : ends) {
		const float was = height(n);
		float h = was;
		for (int s : net.nodes[size_t(n)].segs)
			if (standing(s)) h = std::max(h, end_of(g.prof[size_t(s)], n));
		if (cap_of(n) != NONE) h = std::min(h, cap_of(n));
		stand(n, h);
		if (h <= was + RISE_EPS) continue;
		for (int s : net.nodes[size_t(n)].segs)
			if (standing(s)) again.push_back(s);
	}
	std::sort(again.begin(), again.end());
	again.erase(std::unique(again.begin(), again.end()), again.end());
	return again;
}

// grade.rounds_most is a guard, never a rule: met, what was still rising is counted (RoadGrade::unsettled).
void Settling::rounds() {
	std::vector<int> rising = spans;
	for (int round = 0; round < t.rounds_most && !rising.empty(); round++) {
		for (int s : rising) {
			solve_span(s);
			take(s);
		}
		rising = settle(rising);
	}
	g.unsettled = int(rising.size());
	for (int s : rising) { // what the guard cut short: solved at the heights its nodes reached, and of this solve like the rest
		solve_span(s);
		take(s);
	}
	for (int s : spans) level_plates(g.prof[size_t(s)], height(g.prof[size_t(s)].a), height(g.prof[size_t(s)].b));
}

} // namespace city
