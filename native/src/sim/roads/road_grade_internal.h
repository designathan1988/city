// The parts of the road grade's solve (road_grade.h), internal to sim/roads: the figures it is tuned by, read once a
// solve from data/catalog/tuning.json (grade), and the solvers of one profile (road_grade_profile.cpp).
#pragma once

#include "sim/roads/road_grade.h"

#include <vector>

namespace city {

// The tuned figures of a solve (Roadcraft 4.0 roads/tuning.ts ROAD_TUNING and world/structures.ts ROAD_STRUCTURES).
struct GradeTune {
	float ground = 0; // grade.ground: steepest grade of a road at grade
	float ramp = 0; // grade.ramp: the straight part of a ramp up to a deck
	float deck = 0; // grade.deck: steepest a deck takes to clear a rise under it
	float deck_tie = 0; // grade.deckTie: steepest a deck is carried up to a higher deck it meets
	float tunnel = 0; // grade.tunnel: a tunnel's approach ramps
	float clearance = 0; // ROAD_GROUND_CLEARANCE: a road at grade over the ground it is laid on
	std::array<StructureSpec, STRUCTURE_COUNT> structure{}; // structure_spec of each Structure, by its value
	const StructureSpec &of(Structure s) const { return structure[size_t(s)]; }
};
GradeTune grade_tune(const Tuning &t);

// Segment s's profile made ready for the solve (road_grade.cpp): stations, the ground under them (read again only when its
// curve, casing or ground changed), plates and structure.
// The roads laid in pieces among `read` (a cut carries one road on): the design line of each made along all of it, as of one profile, and the ends of its
// pieces added to `ends`: on a hill a line made piece by piece disagrees at every cut, and the road breaks there to meet itself.
void grade_chains(RoadGrade &grade, const RoadNetwork &net, const std::vector<int> &read, float ground_grade, std::vector<int> &ends);
void grade_inputs(RoadGrade &grade, const RoadNetwork &net, int s, const GroundAt &ground, bool reground, const GradeTune &t);

// Station of arc s (rounded, inside the profile).
int station_index(const GradeProfile &p, float s);
// Height a ramp has gained d metres past its plate edge: a sag curve, then straight (rampRise).
float ramp_rise(float d, const GradeTune &t);

// The ground under a profile at its stations: ceil and base (readStations), the moving mean and the grade limit.
void smooth_profile(std::vector<float> &v, float step, float reach);
void slope_limit(std::vector<float> &h, float step, float grade);
void dilate(std::vector<float> &v, int reach);

// What an aloft profile's ends are: on land (a node with a road at grade), and capped by the ramp from a land end.
struct RaisedEnds {
	bool land_a = true, land_b = true, cap_a = false, cap_b = false;
};

// One profile solved between its nodes' heights (solveGround, solveVariable, solveRaised, solveSunken).
void solve_ground(GradeProfile &p, float ha, float hb, const GradeTune &t);
void solve_variable(GradeProfile &p, float ha, float hb, const GradeTune &t);
void solve_raised(GradeProfile &p, float ha, float hb, const RaisedEnds &e, const GradeTune &t);
void solve_sunken(GradeProfile &p, float ha, float hb, float wanted, const GradeTune &t);
// Both plates held flat at exactly the heights their nodes settled on (pinPlates).
void pin_plates(GradeProfile &p, float ha, float hb);

} // namespace city
