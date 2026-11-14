// A signalled junction's plan (junction_control.cpp build_junction): the approaches that share each stage's green (each arm with the
// arm most opposite it, CONTRAN MBST vol. V; an arm left alone joins a stage none of whose movements any of its own meets), the
// turns that give way all through their stage and the short stage of their own they get, run on their demand; where the walkers
// cross in each stage (an arm nobody arrives by nor drives straight into then) and a stage of their own for a crossing no stage
// can walk; each stage's least and most green and its intergreen (signal_timing.cpp). Read by the controller through signal_plan.h.
#include "sim/traffic/signal_plan.h"

#include "sim/traffic/traffic_system.h"

#include <algorithm>

namespace city {
namespace traffic {

namespace {

constexpr int NO_STAGE = -1;
constexpr uint32_t START_HASH = 2654435761u, START_STEPS = 1000; // where in its first green each junction starts

// Which of two green movements that meet goes first: straight on, then a right turn, a left, a turn back.
int movement_rank(Turn t) {
	switch (t) {
		case Turn::STRAIGHT: return 3;
		case Turn::RIGHT: return 2;
		case Turn::LEFT: return 1;
		default: return 0;
	}
}

bool among(const std::vector<int> &list, int value) { return std::find(list.begin(), list.end(), value) != list.end(); }

// A movement out of approach `a` meets one out of approach `b` (the sweeps' conflicts, junction_sweeps.cpp).
bool approaches_meet(const LaneGraph &g, int n, int a, int b) {
	for (int ci = g.node_conn[size_t(n)]; ci >= 0; ci = g.conns[size_t(ci)].next_at_node) {
		const Conn &c = g.conns[size_t(ci)];
		if (!c.alive || c.from_ds != a) continue;
		for (int other : c.conflicts)
			if (g.conns[size_t(other)].from_ds == b) return true;
	}
	return false;
}

// Each arriving arm with the arm most opposite it: the stage of each arm (NO_STAGE: nobody arrives by it); returns the stages.
int pair_opposite(const std::vector<StageArm> &arms, float opposite_dot, std::vector<int> &stage) {
	int count = 0;
	for (size_t i = 0; i < arms.size(); i++) {
		if (arms[i].in < 0 || stage[i] != NO_STAGE) continue;
		stage[i] = count;
		int best = -1;
		float least = opposite_dot;
		for (size_t k = 0; k < arms.size(); k++) {
			const float dot = arms[i].out.dot(arms[k].out);
			if (arms[k].in >= 0 && stage[k] == NO_STAGE && dot < least) { least = dot; best = int(k); }
		}
		if (best >= 0) stage[size_t(best)] = count;
		count++;
	}
	return count;
}

// An arm alone in its stage joins the stage whose arms it meets nobody of, the most opposite first; its own stage goes.
int join_free(const LaneGraph &g, int n, const std::vector<StageArm> &arms, std::vector<int> &stage, int count) {
	for (size_t i = 0; i < arms.size(); i++) {
		if (stage[i] == NO_STAGE || std::count(stage.begin(), stage.end(), stage[i]) != 1) continue;
		int best = NO_STAGE;
		float least = 1.f;
		for (int k = 0; k < count; k++) {
			if (k == stage[i]) continue;
			bool free = true;
			float dot = 1.f;
			for (size_t m = 0; m < arms.size(); m++) {
				if (stage[m] != k) continue;
				free = free && !approaches_meet(g, n, arms[i].in, arms[m].in) && !approaches_meet(g, n, arms[m].in, arms[i].in);
				dot = std::min(dot, arms[i].out.dot(arms[m].out));
			}
			if (free && dot < least) { least = dot; best = k; }
		}
		if (best == NO_STAGE) continue;
		const int gone = stage[i];
		stage[i] = best;
		for (int &st : stage)
			if (st > gone) st--;
		count--;
	}
	return count;
}

// The turns that give way all through their stage: a green movement of another approach of that stage, of a higher rank, meets
// them (Conn::gives_way). An approach with one gets a stage of its own after the ordinary ones (Junction::turn_of).
void turn_stages(LaneGraph &g, int n, Junction &j) {
	const size_t ordinary = j.approaches.size();
	for (int ci = g.node_conn[size_t(n)]; ci >= 0; ci = g.conns[size_t(ci)].next_at_node) {
		Conn &c = g.conns[size_t(ci)];
		c.gives_way = false;
		if (!c.alive || c.turn == Turn::STRAIGHT || g.ds[size_t(c.from_ds)].stage < 0) continue;
		const std::vector<int> &together = j.approaches[size_t(g.ds[size_t(c.from_ds)].stage)];
		for (int other : c.conflicts) {
			const Conn &o = g.conns[size_t(other)];
			c.gives_way = c.gives_way || (o.from_ds != c.from_ds && among(together, o.from_ds) && movement_rank(o.turn) > movement_rank(c.turn));
		}
	}
	for (size_t k = 0; k < ordinary; k++)
		for (int ds : std::vector<int>(j.approaches[k])) { // a copy: the list of stages grows
			bool waits = false;
			for (int ci = g.node_conn[size_t(n)]; ci >= 0; ci = g.conns[size_t(ci)].next_at_node)
				waits = waits || (g.conns[size_t(ci)].alive && g.conns[size_t(ci)].from_ds == ds && g.conns[size_t(ci)].gives_way);
			if (!waits) continue;
			g.ds[size_t(ds)].turn_stage = int(j.approaches.size());
			j.approaches.push_back({ ds });
			j.turn_of.push_back(ds);
		}
}

// Where the walkers cross in each ordinary stage: over an arm nobody of that stage arrives by nor drives straight into (who turns
// into it gives way to them). A crossing no stage walks gets the walkers' own stage, the last one, with nobody driving.
void walking_stages(const CityState &s, int n, const std::vector<StageArm> &arms, Junction &j, size_t ordinary) {
	const LaneGraph &g = s.traffic.g;
	std::vector<int> nowhere;
	for (const StageArm &a : arms) {
		if (!s.roads.crosswalk_on(n, a.seg)) continue;
		bool walked = false;
		for (size_t k = 0; k < ordinary; k++) {
			bool driven = among(j.approaches[k], a.in);
			for (int ci = g.node_conn[size_t(n)]; ci >= 0 && !driven; ci = g.conns[size_t(ci)].next_at_node) {
				const Conn &c = g.conns[size_t(ci)];
				driven = c.alive && c.turn == Turn::STRAIGHT && among(j.approaches[k], c.from_ds) && g.ds[size_t(c.to_ds)].seg == a.seg;
			}
			if (driven) continue;
			j.walks[k].push_back(a.seg);
			walked = true;
		}
		if (!walked) nowhere.push_back(a.seg);
	}
	if (nowhere.empty()) return;
	j.approaches.push_back({});
	j.turn_of.push_back(NO_STAGE);
	j.walks.push_back(nowhere);
}

// Each stage's greens and intergreen (MBST vol. V 5.7, 5.14, 7.3.3): an ordinary stage's least green is its detection's and its
// walkers' safety green, its most the longest cycle's equal share; a turn stage's are the data's; the walkers' own lets them over.
void time_stages(CityState &s, int n, Junction &j, bool walkers) {
	const TrafficParams &tp = s.catalog.traffic;
	const size_t count = j.approaches.size();
	j.green.assign(count, tp.signal_safety_green_s);
	j.green_max.assign(count, 0.f);
	j.amber.assign(count, 0.f);
	j.all_red.assign(count, 0.f);
	float lost = 0;
	int shares = 0;
	for (size_t k = 0; k < count; k++) {
		for (int ds : j.approaches[k]) {
			float amber = 0, all_red = 0;
			intergreen(s, n, ds, walkers, amber, all_red);
			j.amber[k] = std::max(j.amber[k], amber);
			j.all_red[k] = std::max(j.all_red[k], all_red);
			if (j.turn_of[k] < 0) j.green[k] = std::max(j.green[k], detection(s, ds));
		}
		if (j.approaches[k].empty()) { j.green[k] = 0; j.all_red[k] = tp.ped_all_red_s; }
		for (int seg : j.walks[k]) {
			const float need = tp.ped_safety_green_s + ped_clearance(s, seg) + tp.ped_all_red_s;
			j.green[k] = std::max(j.green[k], need - j.amber[k] - j.all_red[k]);
		}
		if (j.turn_of[k] >= 0) { j.green[k] = tp.turn_stage_green_s; continue; }
		lost += j.amber[k] + j.all_red[k];
		shares++;
	}
	j.cycle = lost;
	for (size_t k = 0; k < count; k++) {
		if (j.turn_of[k] >= 0) { j.green_max[k] = std::max(j.green[k], tp.turn_stage_most_s); continue; }
		j.green_max[k] = std::max(j.green[k], (tp.signal_cycle_max_s - lost) / float(std::max(shares, 1)));
		j.cycle += j.green[k];
	}
	j.least = j.green;
}

} // namespace

void plan_stages(CityState &s, int n, const std::vector<StageArm> &arms, Junction &j) {
	LaneGraph &g = s.traffic.g;
	std::vector<int> stage(arms.size(), NO_STAGE);
	const int count = join_free(g, n, arms, stage, pair_opposite(arms, s.catalog.traffic.stage_opposite_dot, stage));
	j.approaches.assign(size_t(count), {});
	j.turn_of.assign(size_t(count), NO_STAGE);
	for (size_t i = 0; i < arms.size(); i++) {
		if (stage[i] == NO_STAGE) continue;
		g.ds[size_t(arms[i].in)].stage = stage[i];
		j.approaches[size_t(stage[i])].push_back(arms[i].in);
	}
	turn_stages(g, n, j);
	j.walks.assign(j.approaches.size(), {});
	walking_stages(s, n, arms, j, size_t(count));
	time_stages(s, n, j, s.roads.has_crosswalks(n) || s.roads.mid_block(n));
	// the player's fixed plan: each stage its own green, never under its least
	const SignalSettings &set = s.roads.nodes[size_t(n)].signal;
	j.fixed = set.fixed;
	j.offset = set.offset;
	j.buses_first = set.buses_first;
	for (size_t k = 0; set.fixed && k < j.green.size(); k++) {
		if (k < set.greens.size()) {
			j.cycle += std::max(0.f, set.greens[k] - j.green[k]);
			j.green[k] = std::max(j.green[k], set.greens[k]);
		}
		j.green_max[k] = j.green[k];
	}
	j.served.assign(j.green.size(), s.traffic.time);
	j.hold_from = -1;
	j.run = 0;
	j.run_green = -1;
	if (j.green.empty()) return; // nobody arrives and nobody crosses: no stage (signal_timing.cpp update_lights skips it)
	j.run_start = s.traffic.time - double((uint32_t(n) * START_HASH) % START_STEPS) / START_STEPS * j.green[0];
}

} // namespace traffic
} // namespace city
