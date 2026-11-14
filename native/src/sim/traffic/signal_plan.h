// A signalled junction's plan and what its controller asks of it (signal_plan.cpp, signal_demand.cpp): which approaches share each
// stage's green, the turns that give way inside it and their own short stage, where the walkers cross in each stage and their own
// stage; a stage's demand and queue, a bus near, the people on its crossings, a crossing's lamp. The controller that runs the plan
// in time is signal_timing.cpp.
#pragma once

#include "core/math.h"
#include "sim/traffic/lane_graph_types.h"

#include <vector>

namespace city {

struct CityState;
struct Vehicle;

namespace traffic {

// An arm of the junction as the plan reads it: its segment, the approach arriving by it (-1: it only leaves), the way it points out.
struct StageArm {
	int seg = -1, in = -1;
	V2 out;
};

// What a crossing's lamp shows the walkers: stop, flash (who is on it finishes, nobody starts) or walk.
enum class WalkLamp : uint8_t { STOP = 0, FLASH = 1, WALK = 2 };

void plan_stages(CityState &s, int n, const std::vector<StageArm> &arms, Junction &j);

// Whether somebody wants stage `k`'s green: a vehicle that called or stands in the detection reach of one of its approaches (a
// turn stage: a head bound for one of its turns that give way), or a walker waiting at a crossing that walks in it.
bool stage_asked(const CityState &s, int n, const Junction &j, int k);
float stage_queue(const CityState &s, const Junction &j, int k); // the vehicles in the detection reach of its approaches
bool bus_near(const CityState &s, const Junction &j, int k); // a bus in the detection reach of one of its approaches
bool people_on(const CityState &s, int n, const Junction &j, int k); // walkers on a crossing that walks in it
float walkers_wait(const CityState &s, int n, const Junction &j, int k); // the longest wait at a crossing that walks in it (-1: nobody)
WalkLamp walk_lamp(const CityState &s, int node, int arm_seg);

// A walker held at the kerb of the crossing over `arm_seg` says so, with the seconds it has waited; and the longest wait there
// while somebody still waits (-1: nobody). The cell a (node, arm) is JunctionState::kerb.
// A driver gives way to who waits at the kerb of a crossing connector `ci` drives over, `line` metres before its stop line
// (ped_crossing.cpp).
bool waiting_walkers(const CityState &s, const Vehicle &v, int ci, float line);
int arm_slot(const CityState &s, int node, int arm_seg); // the arm's place among its node's arms (ped_crossing.cpp), -1: none kept
void kerb_word(CityState &s, int node, int arm_seg, float waited);
float kerb_waited(const CityState &s, int node, int arm_seg);

} // namespace traffic
} // namespace city
