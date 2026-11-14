// The parts of the road heights' solve (road_grade.h), internal to sim/roads: the figures it is tuned by, read once a
// solve from project/data/catalog/tuning.json (grade); a segment's frame (grade_ground.cpp: stations, plates, build), the
// ground read under it and its design line; one span solved between its two nodes' heights (grade_span.cpp).
#pragma once

#include "sim/roads/road_grade.h"

#include <array>
#include <vector>

namespace city {

// The tuned figures of a solve (tuning.json grade). The grades and the structures' figures are the game's own modelling
// values; the reaches below were settled by calculation on the acceptance's hill crossroads (doc/work/roads.md, case 1).
struct GradeTune {
	float ground = 0; // steepest grade of a road at grade
	float ramp = 0; // the straight part of a ramp up to a deck; an authored span's grade
	float deck = 0; // steepest a deck takes to clear a rise under it
	float deck_tie = 0; // steepest a deck is carried up to a higher deck it meets
	float tunnel = 0; // a tunnel's approach ramps
	float clearance = 0; // a road at grade over the ground it is laid on
	float line_reach = 0; // line_reach_m: the design line is the mean ground over this either side
	float plate_curve = 0; // plate_curve_m: past a plate the line's grade is taken up over this
	float tie = 0, tie_share = 0; // tie_m, tie_share: what stands between road and line at a plate is eased out over this
	float curve = 0; // curve_m: half the vertical curve at a change of grade
	float crest = 0; // crest_m: the vertical curve between a ramp and its deck
	float ramp_sag = 0; // ramp_sag_m: the sag curve at the foot of a ramp
	float ends_margin = 0; // the grade a span is given over what its two ends need
	float under_deck = 0; // under_deck_m: nothing comes closer than this under a deck's structure
	float floor_fade = 0, floor_ramp_share = 0; // floor_fade_m: past a plate the ground stops being a ramp's floor
	int rounds_most = 0; // the guard of the nodes' rounds: met, it is counted (RoadGrade::unsettled), never a rule
	size_t ground_cache = 0; // ground_cache_mb, in bytes
	std::array<StructureSpec, STRUCTURE_COUNT> structure{}; // structure_spec of each Structure, by its value
	const StructureSpec &of(Structure s) const { return structure[size_t(s)]; }
};
GradeTune grade_tune(const Tuning &t);

// Segment s's frame as the network has it now: its stations, plates, build and curve; its heights left as they were.
void frame_profile(RoadGrade &grade, const RoadNetwork &net, int s);
// The ground under segment s read into its lines (top and mean); its design line is made by design_lines.
void read_ground(RoadGrade &grade, const RoadNetwork &net, int s, const GroundAt &ground_at, const GradeTune &t);
// The design line of segment s and of every piece of the road it is a piece of (a joint that carries one road on makes one
// line along all of it: made piece by piece it would disagree at every cut and the road break there to meet itself). The
// ground of a piece whose lines were let go is read again. `pieces` takes every segment whose line was made.
void design_lines(RoadGrade &grade, const RoadNetwork &net, int s, const GroundAt &ground_at, const GradeTune &t, std::vector<int> &pieces);

// The last station of the a plate and the first of the b plate (a station is of a plate when its arc is within it).
int plate_last_a(const GradeProfile &p);
int plate_first_b(const GradeProfile &p);
// The station nearest arc s, inside the profile.
int station_near(const GradeProfile &p, float s);
// Height a ramp has gained d metres past its plate edge: a sag curve, then straight.
float ramp_gain(float d, const GradeTune &t);

// What an aloft profile's ends are: on land (a node with a road at grade), and capped by the ramp from a land end.
struct RaisedEnds {
	bool land_a = true, land_b = true, cap_a = false, cap_b = false;
};

// One span solved between the heights its nodes stand at: at grade on its design line (`grade`: the ground grade, or the
// ramp's for an authored one), raised on a deck, sunken to `depth`.
void span_at_grade(GradeProfile &p, const GroundLines &g, float ha, float hb, float grade, const GradeTune &t);
void span_raised(GradeProfile &p, const GroundLines &g, float ha, float hb, const RaisedEnds &e, const GradeTune &t);
void span_sunken(GradeProfile &p, float ha, float hb, float depth, const GradeTune &t);
// Both plates level at exactly the heights their nodes settled on.
void level_plates(GradeProfile &p, float ha, float hb);

} // namespace city
