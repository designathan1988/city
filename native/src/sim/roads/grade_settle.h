// One solve's settling of the road heights' nodes (road_grade.cpp runs it; internal to sim/roads): which nodes and spans a
// change reaches, where each node wants to stand, the spans solved between them in rounds until no node rises. Its working
// values by node and by segment are the solve's scratch (RoadGrade::Scratch), put back at rest by the solve.
#pragma once

#include "sim/roads/grade_parts.h"

#include <limits>
#include <vector>

namespace city {

class RoadNetwork;

struct Settling {
	static constexpr float NONE = std::numeric_limits<float>::infinity(); // a height or a cap not set
	RoadGrade &g;
	RoadNetwork &net;
	const GradeTune t;
	const GroundAt &land; // the ground handed to the solve
	RoadGrade::Scratch &sc; // height, start, grade, cap by node: this solve's (NONE: not reached)
	std::vector<int> nodes, spans; // the nodes and the segments this solve settles
	std::vector<int> lined; // the segments whose design line this solve made

	bool standing(int s) const;
	bool reached(int n) const { return sc.node_solved[size_t(n)] != 0; }
	float grade_of(int n) const { return reached(n) ? sc.grade[size_t(n)] : g.node[size_t(n)].grade; }
	bool aloft(int n) const { return reached(n) ? sc.aloft[size_t(n)] != 0 : g.node[size_t(n)].aloft; }
	float cap_of(int n) const;
	float height(int n) const { return sc.height[size_t(n)] != NONE ? sc.height[size_t(n)] : g.node[size_t(n)].height; }

	// Node n is settled by this solve; segment s is.
	void reach(int n);
	void take(int s);
	// The nodes a change reaches: the ends of what changed, one ring further, and on from a node a raised road carried
	// above where it started, along each road whose end stood at that height.
	void reach_from(const std::vector<int> &changed_ends, bool all);
	// The design line of s and of every piece of its road made; gives the pieces.
	std::vector<int> line_road(int s);
	// Where the roads meeting at n want it, and where it starts the rounds.
	void want_node(int n);
	void start_node(int n);
	// The spans solved and the nodes raised to their highest ends until none rises; then every plate level at its node.
	void rounds();

private:
	const GroundLines &lines(int s);
	void stand(int n, float h);
	float disc_ground(int n) const;
	float deck_at(int s, bool at_a);
	void solve_span(int s);
	std::vector<int> settle(const std::vector<int> &solved);
};

} // namespace city
