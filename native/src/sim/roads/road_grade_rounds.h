// The rounds of a road grade solve (Roadcraft 4.0 world/elevation.ts buildRoadElevation, its behaviour), internal to
// sim/roads: a solve's state by node and by segment, the nodes a change reaches, where each settles and every profile at
// them solved between its nodes, in rounds until no node rises (road_grade_rounds.cpp). road_grade_solve.cpp drives them.
#pragma once

#include "sim/roads/road_grade_internal.h"
#include "sim/roads/road_network.h"

#include <cmath>
#include <vector>

namespace city {

const float NONE = INFINITY; // a node's height or cap this solve has not reached

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

	// Where a node stands before the rounds: the ground of its disc, its roads' grade lines and the decks they want.
	float balanced_at(int n) const;
	void grade_node(int n);
	float wanted_end(const GradeProfile &p, bool at_a) const;
	void start_node(int n);
	// One profile solved between its nodes, by how it is built; the rounds of that and of settling its nodes.
	float wanted_of(const GradeProfile &p) const;
	void solve_profile(int s);
	std::vector<int> settle(const std::vector<int> &dirty);
	void rounds();
};

// The nodes a change reaches: the ends of what changed, one ring further, and on from a node a raised road carried
// above where it started, along each road whose end stood at that height.
void reach_nodes(Solve &sv, const std::vector<int> &changed_ends, bool all);

} // namespace city
