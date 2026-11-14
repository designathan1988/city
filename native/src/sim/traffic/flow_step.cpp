// The far traffic's step (street_flows.h): every flow_dt_s of the city, each street's rates, what it can send on and take in, the
// nodes, the starts from the streets' sides, then the curves moved on and the render told what changed. Its work is spread over
// the ticks by a budget of streets and nodes a tick (flow_links_per_tick), so a tick costs the same on any map; a step owed while
// one is under way waits for it. The phases follow one another by one setter (enter_flow_phase, FLOW_PHASES).
#include "sim/traffic/flow_parts.h"

#include "core/checks.h"

#include <cmath>
#include <mutex>

namespace city {
namespace traffic {

namespace {

constexpr float REBASE_AT = 1024.f; // a street's curves are brought back by their floor past this: a float keeps the counts' differences
constexpr double CLOCK_SLACK_S = 1e-6;
constexpr int PHASES = 6;
constexpr int LINE_POINTS = 3; // a street's line is told as its start, its middle and its end

constexpr uint8_t phase_bit(FlowPhase p) { return uint8_t(1u << int(p)); }

// FLOW_PHASES[from]: the phase the step's work enters from `from`: each follows the one before it, the last gives way to idle.
constexpr uint8_t FLOW_PHASES[PHASES] = {
		phase_bit(FlowPhase::RATES), // IDLE: a step begins
		phase_bit(FlowPhase::OFFERS), // RATES
		phase_bit(FlowPhase::NODES), // OFFERS
		phase_bit(FlowPhase::STARTS), // NODES
		phase_bit(FlowPhase::CURVES), // STARTS
		phase_bit(FlowPhase::IDLE), // CURVES: the step is done
};

bool enter_flow_phase(StreetFlows &f, FlowPhase next) {
	if (!(FLOW_PHASES[int(f.phase)] & phase_bit(next))) {
		if (CHECKS) city_stop("a phase the far traffic's step never enters from the one it is in (flow_step.cpp FLOW_PHASES)");
		return false;
	}
	f.phase = next;
	f.cursor = 0;
	return true;
}

// The street's line for the render: the middle of its lanes at its start, half way and its end, as the three points of one curve.
void line_of(const CityState &s, int ds, FlowMark &m) {
	const DirSeg &d = s.traffic.g.ds[size_t(ds)];
	const int last = std::max(0, d.lanes - 1);
	Pose at[LINE_POINTS];
	for (int k = 0; k < LINE_POINTS; k++) {
		Pose right, left;
		const float along = d.len * float(k) / float(LINE_POINTS - 1);
		lane_pose(s, ds, 0, along, right);
		lane_pose(s, ds, last, along, left);
		at[k].x = (right.x + left.x) / 2;
		at[k].y = (right.y + left.y) / 2;
		at[k].z = (right.z + left.z) / 2;
	}
	const Pose &a = at[0], &mid = at[1], &b = at[LINE_POINTS - 1];
	m.flags |= FLOW_LINE;
	m.ax = a.x, m.ay = a.y, m.az = a.z;
	m.bx = b.x, m.by = b.y, m.bz = b.z;
	// the curve through the three: its control point stands as far past the middle as the middle stands off the chord
	m.cx = mid.x + (mid.x - (a.x + b.x) / 2);
	m.cy = mid.y + (mid.y - (a.y + b.y) / 2);
	m.cz = mid.z + (mid.z - (a.z + b.z) / 2);
	m.len = d.len;
	m.lanes = uint8_t(d.lanes);
	m.lane_step = d.lanes > 1 ? std::fabs(d.u[1] - d.u[0]) : 0.f;
}

// What is drawn of `ds` as of step `now`, told to the render when it changed since it was last told.
void tell(CityState &s, int ds, uint64_t now) {
	StreetFlows &f = s.traffic.flows;
	const TrafficParams &p = s.catalog.traffic;
	const size_t k = size_t(ds);
	FlowMark m;
	m.ds = ds;
	if (f.live[k]) m.flags = uint8_t(FLOW_LIVE | (f.inside[k] ? FLOW_INSIDE : 0));
	if (f.live[k] && !f.inside[k]) {
		const DirSeg &d = s.traffic.g.ds[k];
		const float free = std::max(FLOW_SLOWEST_MS, d.speed), through = f.cap[k] / p.flow_dt_s; // vehicles a second its end lets go
		const float queue = flow_queue(s, ds, now);
		m.count = flow_shown(ds, flow_on(f, ds, now)); // whole cars: what is drawn of it (flow_parts.h)
		m.queued = std::min(m.count, flow_shown(ds, queue));
		m.speed = queue <= FLOW_TINY ? free : through > FLOW_TINY ? d.len / (d.len / free + queue / through) : 0.f;
	} else if (f.live[k]) {
		// a street of the window: the vehicles it holds, those driving on it at the step's start and the whole ones still owed to
		// it, as moving at its free speed. The render shows them where the detailed vehicles are not drawn (far_traffic.h)
		m.count = float(f.detailed[k]) + std::floor(f.owed[k]);
		m.speed = std::max(FLOW_SLOWEST_MS, s.traffic.g.ds[k].speed);
	}
	const bool same = !f.line_due[k] && f.told_flags[k] == m.flags && m.count == f.told_count[k] && m.queued == f.told_queue[k] &&
			std::fabs(m.speed - f.told_speed[k]) <= p.flow_speed_step * std::max(f.told_speed[k], 1.f);
	if (same) return;
	f.told_flags[k] = m.flags;
	f.told_count[k] = m.count;
	f.told_queue[k] = m.queued;
	f.told_speed[k] = m.speed;
	if (f.line_due[k] && f.live[k]) line_of(s, ds, m);
	f.line_due[k] = 0;
	f.marks.push_back(m);
}

// What each street in [from, to) can send on and take in this step, and its origin fed with those that want to start on it.
void street_offers(CityState &s, int from, int to) {
	StreetFlows &f = s.traffic.flows;
	const TrafficParams &p = s.catalog.traffic;
	const LaneGraph &g = s.traffic.g;
	const float dt = p.flow_dt_s, wave = flow_wave_ms(p);
	const uint64_t now = f.steps;
	for (int ds = from; ds < to; ds++) {
		const size_t k = size_t(ds);
		f.came[k] = f.left[k] = f.send[k] = f.room[k] = 0;
		if (!f.live[k]) continue;
		const DirSeg &d = g.ds[k];
		if (f.inside[k]) {
			// its vehicles are driven one by one: it takes what room they and those still to be made leave
			f.room[k] = std::max(0.f, std::min(f.cap[k], f.store[k] - float(f.detailed[k]) - f.owed[k]));
		} else {
			const float in = f.curve[size_t(f.up_at[k] + flow_cell(now, f.up_n[k], 0))], out = f.curve[size_t(f.down_at[k] + flow_cell(now, f.down_n[k], 0))];
			const float free = std::max(FLOW_SLOWEST_MS, d.speed);
			// those in a free run ago have reached its end; the room at its start is what left a wave's run ago plus its storage
			const float arrived = flow_past(f, f.up_at[k], f.up_n[k], now, d.len / free / dt - 1);
			const float cleared = flow_past(f, f.down_at[k], f.down_n[k], now, d.len / wave / dt - 1);
			f.send[k] = std::max(0.f, std::min(std::min(arrived - out, f.cap[k]), in - out));
			f.room[k] = std::max(0.f, std::min(cleared + f.store[k] - in, f.cap[k]));
		}
		const float wanting = f.start_rate[k] * dt;
		if (wanting <= 0) continue;
		const float most = p.flow_origin_queue * float(d.lanes);
		f.origin[k] += wanting;
		if (f.origin[k] > most) {
			f.unmade += double(f.origin[k] - most);
			f.origin[k] = most;
		}
	}
}

// Whether the camera's reach of detailed agents holds the middle of `ds` (a vehicle made there would be seen appearing).
bool middle_seen(const CityState &s, int ds) {
	Pose mid;
	lane_pose(s, ds, 0, s.traffic.g.ds[size_t(ds)].len / 2, mid);
	const V2 eye(s.camera_x.load(), s.camera_z.load());
	return (V2(mid.x, mid.z) - eye).len() <= s.catalog.traffic.publish_radius_m;
}

// Starting from a street's sides gives way to the traffic through it: its origin sends what room is left. In the window a start is
// a vehicle to make, and only where nobody sees it appear.
void side_starts(CityState &s, int from, int to) {
	StreetFlows &f = s.traffic.flows;
	for (int ds = from; ds < to; ds++) {
		const size_t k = size_t(ds);
		if (!f.live[k] || f.origin[k] <= 0) continue;
		const float go = std::min(f.origin[k], f.room[k]);
		if (go <= 0 || (f.inside[k] && middle_seen(s, ds))) continue;
		f.origin[k] -= go;
		f.room[k] -= go;
		f.started += double(go);
		if (f.inside[k]) {
			f.owed[k] += go;
			f.along[k] += go;
			flow_owes(f, ds);
		} else {
			f.came[k] += go;
		}
	}
}

// The streets in [from, to) moved on by the step's flows: their counts in and out, the window's owed what came onto them.
void advance_curves(CityState &s, int from, int to) {
	StreetFlows &f = s.traffic.flows;
	const uint64_t now = f.steps;
	for (int ds = from; ds < to; ds++) {
		const size_t k = size_t(ds);
		if (!f.live[k]) {
			if (f.line_due[k]) tell(s, ds, now + 1); // gone since it was told: told once more, as no street
			continue;
		}
		if (f.inside[k]) {
			if (f.came[k] > 0) {
				f.owed[k] += f.came[k];
				f.handed_out += double(f.came[k]);
				flow_owes(f, ds);
			}
		} else {
			float *up = f.curve.data() + f.up_at[k], *down = f.curve.data() + f.down_at[k];
			const int un = f.up_n[k], dn = f.down_n[k];
			up[flow_cell(now, un, -1)] = up[flow_cell(now, un, 0)] + f.came[k];
			down[flow_cell(now, dn, -1)] = down[flow_cell(now, dn, 0)] + f.left[k];
			if (down[flow_cell(now, dn, -1)] > REBASE_AT) {
				const float floor = std::floor(std::min(*std::min_element(up, up + un), *std::min_element(down, down + dn)));
				for (int i = 0; i < un; i++) up[i] -= floor;
				for (int i = 0; i < dn; i++) down[i] -= floor;
			}
		}
		tell(s, ds, now + 1);
	}
}

// Up to `budget` streets and nodes of the step under way; the budget left.
int work(CityState &s, int budget) {
	StreetFlows &f = s.traffic.flows;
	const int total = f.phase == FlowPhase::NODES ? int(s.roads.nodes.size()) : int(f.live.size());
	const int end = std::min(total, f.cursor + budget);
	switch (f.phase) {
		case FlowPhase::RATES: flow_rates(s, f.cursor, end); break;
		case FlowPhase::OFFERS: street_offers(s, f.cursor, end); break;
		case FlowPhase::NODES:
			for (int n = f.cursor; n < end; n++) flow_node(s, n);
			break;
		case FlowPhase::STARTS: side_starts(s, f.cursor, end); break;
		case FlowPhase::CURVES: advance_curves(s, f.cursor, end); break;
		case FlowPhase::IDLE: return 0;
	}
	budget -= end - f.cursor;
	f.cursor = end;
	if (end < total) return budget;
	const bool last = f.phase == FlowPhase::CURVES;
	enter_flow_phase(f, last ? FlowPhase::IDLE : FlowPhase(int(f.phase) + 1));
	if (!last) return budget;
	// the step is done: the rings' clock moves on, the render is told what changed
	f.steps++;
	if (!f.marks.empty()) {
		std::lock_guard<std::mutex> lk(s.log.mutex);
		s.log.flow_marks.insert(s.log.flow_marks.end(), f.marks.begin(), f.marks.end());
		f.marks.clear();
	}
	return budget;
}

} // namespace

void step_flows(CityState &s, float dt) {
	StreetFlows &f = s.traffic.flows;
	const TrafficParams &p = s.catalog.traffic;
	if (s.flows_untold.exchange(false)) { // the render draws the city anew: every street is told again, with its line
		std::fill(f.told_flags.begin(), f.told_flags.end(), uint8_t(0));
		std::fill(f.told_count.begin(), f.told_count.end(), -1.f);
		f.line_due = f.live;
	}
	// the hour jumped (the clock set, not run): the streets carry what the demand puts on them at the new hour
	if (s.clock.day_shift.load() != f.day_shift) seed_flows(s);
	flow_admit(s);
	f.clock += double(dt);
	if (f.clock + CLOCK_SLACK_S >= double(p.flow_dt_s)) {
		f.clock -= double(p.flow_dt_s);
		// a step owed while one is under way waits for it: the far traffic slows, the tick does not grow
		if (f.due || f.phase != FlowPhase::IDLE) f.late++;
		f.due = true;
	}
	if (f.phase == FlowPhase::IDLE && f.due) {
		f.due = false;
		flow_begin(s);
		enter_flow_phase(f, FlowPhase::RATES);
	}
	int budget = int(p.flow_links_per_tick);
	while (budget > 0 && f.phase != FlowPhase::IDLE) budget = work(s, budget);
}

} // namespace traffic
} // namespace city
