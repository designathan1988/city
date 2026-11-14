// The signal plan in time (CONTRAN MBST vol. V, 2022): the intergreen after each green (amber, then all-red,
// item 5.7), the detection that actuates each green (cap. 7: minimum green, extension per vehicle over the detection
// section, maximum green), the stage running now and the light each approach shows, when an approach next turns green.
// An actuated plan follows its demand (signal_plan.h): a green rests while nobody else asks, ends when its own calls stop or at
// its most, never over people on its crossings; the next stage is the one asked for that waited most, a stage nobody asks for
// skipped; buses first where the player set it. A fixed plan follows the clock.
#include "sim/traffic/traffic_system.h"

#include "core/units.h"
#include "sim/traffic/signal_plan.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

// The amber an approach's regulated speed asks (CONTRAN MBST vol. V 5.7.1, Eq. 6.4): t_pr + v/(2 a_ad), at least the
// minimum of that speed. Regulated speeds are whole km/h: 60 km/h comes back from m/s a hair over 60.
float amber_wanted(const TrafficParams &tp, float v) {
	const float kmh = std::round(v * KMH_PER_MS);
	const float least = kmh <= tp.amber_low_kmh ? tp.amber_min_s : (kmh <= tp.amber_mid_kmh ? tp.amber_min_mid_s : tp.amber_min_high_s);
	return std::max(least, tp.signal_reaction_s + v / (2 * tp.signal_decel));
}

// The intergreen after an approach's green (5.7.1, Eq. 6.4-6.5): the amber its regulated speed asks, at most amber_max_s
// (what is left goes to the all-red); the all-red (d2 + c)/v to take a vehicle of length c from the stop line past the
// conflict area (its longest way over the junction and the far crosswalk), plus signal_ped_red_s when walkers cross in
// the next stage.
void intergreen(const CityState &s, int n, int ds, bool walkers, float &amber, float &all_red) {
	const TrafficParams &tp = s.catalog.traffic;
	const LaneGraph &g = s.traffic.g;
	const DirSeg &d = g.ds[size_t(ds)];
	const float v = d.speed;
	if (v <= 0) return;
	const float wanted = amber_wanted(tp, v);
	float across = 0;
	for (int ci = g.node_conn[size_t(n)]; ci >= 0; ci = g.conns[size_t(ci)].next_at_node)
		if (g.conns[size_t(ci)].from_ds == ds) across = std::max(across, g.conns[size_t(ci)].len);
	const float d2 = d.stop_back + across + (walkers ? s.catalog.furniture.crossing.cleared() : 0.f);
	amber = std::min(wanted, tp.amber_max_s);
	all_red = wanted - amber + (d2 + tp.signal_vehicle_m) / v + (walkers ? tp.signal_ped_red_s : 0.f);
}

float detection(CityState &s, int ds) {
	const TrafficParams &tp = s.catalog.traffic;
	DirSeg &d = s.traffic.g.ds[size_t(ds)];
	const int lanes = std::clamp(d.lanes, 1, EXTENSION_LANES);
	d.detector = std::min(tp.signal_extension_s[lanes - 1] * d.speed, std::max(0.f, d.len - d.stop_back));
	d.extension = d.speed > 0 ? d.detector / d.speed : 0.f; // a call lasts its vehicle's drive to the line (Eq. 8.3)
	return tp.signal_lost_start_s + d.detector / tp.signal_queue_spacing_m * float(SECONDS_PER_HOUR / tp.signal_sat_flow_lane_h);
}

void signal_call(CityState &s, int ds, float from, float to) {
	TrafficState &t = s.traffic;
	const DirSeg &d = t.g.ds[size_t(ds)];
	if (d.extension <= 0 || size_t(ds) >= t.green_wanted.size()) return;
	const float at = d.len - d.stop_back - d.detector;
	if (from < at && to >= at) t.green_wanted[size_t(ds)] = std::max(t.green_wanted[size_t(ds)], t.time + double(d.extension));
}

void signal_call_far(CityState &s, int ds, bool held) {
	TrafficState &t = s.traffic;
	if (t.g.ds[size_t(ds)].extension <= 0 || size_t(ds) >= t.green_wanted.size()) return;
	// a far vehicle is no body in the detection reach. Held at a red, its call is kept until its approach's green has run (advance
	// ends it), a cycle at most, so a signal resting on another stage turns for it; going through on green, a discharge headway
	const double headway = SECONDS_PER_HOUR / double(s.catalog.traffic.signal_sat_flow_lane_h);
	const double stands = held ? double(s.catalog.traffic.signal_cycle_max_s) : headway;
	t.green_wanted[size_t(ds)] = std::max(t.green_wanted[size_t(ds)], t.time + stands);
}

namespace {
// Whether the running stage's green, past its least, ends now (MBST vol. V cap. 7, actuated): only when another stage is asked
// for; then once nobody asks for its own (the gap after the last call) or it reached its most. Never over people still on its
// crossings, for ped_hold_limit_s from the moment it would have ended. Where the player put buses first: a bus inside the reach of
// one of its approaches holds it bus_extension_s past its most, and a bus standing on another stage ends it at once.
bool green_ends(const CityState &s, int n, Junction &j, double since) {
	const TrafficParams &tp = s.catalog.traffic;
	const int k = j.run, stages = int(j.green.size());
	bool others = false, bus_waits = false;
	for (int i = 0; i < stages && !others; i++) others = i != k && stage_asked(s, n, j, i);
	if (!others) return false; // nobody else asks: it rests in green
	const double most = double(j.green_max[size_t(k)]);
	if (j.buses_first) {
		if (j.turn_of[size_t(k)] < 0 && since < most + tp.bus_extension_s && bus_near(s, j, k)) return false;
		for (int i = 0; i < stages && !bus_waits; i++) bus_waits = i != k && j.turn_of[size_t(i)] < 0 && bus_near(s, j, i);
	}
	if (!bus_waits && since < most && stage_asked(s, n, j, k)) return false;
	if (!people_on(s, n, j, k)) return true;
	if (j.hold_from < 0) j.hold_from = s.traffic.time;
	return s.traffic.time - j.hold_from >= tp.ped_hold_limit_s;
}

// The stage that runs next: where buses go first, a bus's; a stage asked for and unserved for signal_starve_cycles cycles; the
// stage whose walkers have waited ped_wait_limit_s, the longest wait first; the stage asked for whose wait, weighted by its queue,
// is the longest. A stage nobody asks for is skipped; with nobody asking at all, the next ordinary stage in order.
int next_stage(const CityState &s, int n, const Junction &j) {
	const TrafficParams &tp = s.catalog.traffic;
	const int stages = int(j.green.size()), k = j.run;
	const double starved = double(j.cycle) * tp.signal_starve_cycles;
	int bus = -1, overdue = -1, asked = -1, walkers = -1, plain = -1;
	double overdue_age = 0, asked_age = 0;
	float longest = tp.ped_wait_limit_s;
	for (int step = 1; step < stages; step++) {
		const int i = (k + step) % stages;
		const bool vehicles = !j.approaches[size_t(i)].empty();
		if (plain < 0 && vehicles && j.turn_of[size_t(i)] < 0) plain = i;
		if (const float wait = walkers_wait(s, n, j, i); wait >= longest) { longest = wait; walkers = i; }
		if (!stage_asked(s, n, j, i)) continue;
		if (bus < 0 && j.buses_first && j.turn_of[size_t(i)] < 0 && bus_near(s, j, i)) bus = i;
		const double waited = s.traffic.time - j.served[size_t(i)];
		const double age = waited * (1 + double(stage_queue(s, j, i)) / tp.signal_queue_weight);
		if (vehicles && waited >= starved && age > overdue_age) { overdue_age = age; overdue = i; }
		if (age > asked_age) { asked_age = age; asked = i; }
	}
	for (int pick : { bus, overdue, walkers, asked, plain })
		if (pick >= 0) return pick;
	return (k + 1) % stages;
}

// Runs junction `n`'s actuated plan up to now: the running stage's least green, its end (green_ends), its amber and all-red, then
// the next stage (next_stage).
void advance(CityState &s, int n, Junction &j) {
	TrafficState &t = s.traffic;
	const size_t k = size_t(j.run);
	const double since = t.time - j.run_start;
	if (j.run_green < 0) {
		if (since < j.green[k] || !green_ends(s, n, j, since)) return;
		j.run_green = float(since);
		j.served[k] = t.time;
		for (int ds : j.approaches[k]) // the calls its green answered are over (a far vehicle's stands a cycle: signal_call_far)
			if (size_t(ds) < t.green_wanted.size()) t.green_wanted[size_t(ds)] = std::min(t.green_wanted[size_t(ds)], t.time);
	}
	const double span = double(j.run_green) + j.amber[k] + j.all_red[k];
	if (since < span) return;
	j.run_start += span;
	j.run = next_stage(s, n, j);
	j.run_green = -1;
	j.hold_from = -1;
}

// Where a fixed plan stands now on the common clock, less the junction's offset: the same stage on every run, so two
// junctions keep their distance in time (MBST vol. V: coordinated plans run off one reference).
void run_fixed(const TrafficState &t, Junction &j) {
	if (t.time < j.settle) return;
	double total = 0;
	for (size_t k = 0; k < j.green.size(); k++) total += double(j.green[k]) + j.amber[k] + j.all_red[k];
	if (total <= 0) return;
	double left = std::fmod(t.time - double(j.offset), total);
	if (left < 0) left += total;
	for (size_t k = 0; k < j.green.size(); k++) {
		const double span = double(j.green[k]) + j.amber[k] + j.all_red[k];
		if (left < span) {
			j.run = int(k);
			j.run_start = t.time - left;
			j.run_green = j.green[k];
			return;
		}
		left -= span;
	}
}

Light compute_light(const CityState &s, int ds) {
	const LaneGraph &g = s.traffic.g;
	const DirSeg &d = g.ds[ds];
	const Junction &j = g.junctions[d.to];
	if (j.ctl != Control::SIGNAL || d.stage < 0 || j.green.empty()) return Light::GREEN;
	if (d.stage != j.run && d.turn_stage != j.run) return Light::RED; // its own stage, or the stage of its turns
	const double since = s.traffic.time - j.run_start;
	if (j.run_green < 0 || since < j.run_green) return Light::GREEN;
	return since < j.run_green + j.amber[size_t(j.run)] ? Light::AMBER : Light::RED;
}
} // namespace

float ped_clearance(const CityState &s, int arm_seg) {
	const TrafficParams &tp = s.catalog.traffic;
	const Profile &p = s.roads.profile_of(arm_seg);
	return tp.ped_reaction_s + (p.asph_left + p.asph_right) / tp.ped_clear_speed;
}

void file_signal(LaneGraph &g, int n) {
	if (n < 0) return;
	if (g.signal_at.size() <= size_t(n)) g.signal_at.resize(std::max(g.junctions.size(), size_t(n) + 1), 0);
	const bool timed = size_t(n) < g.junctions.size() && g.junctions[size_t(n)].ctl == Control::SIGNAL && !g.junctions[size_t(n)].green.empty();
	int &at = g.signal_at[size_t(n)];
	if (timed == (at > 0)) return;
	if (timed) {
		g.signal_nodes.push_back(n);
		at = int(g.signal_nodes.size());
		return;
	}
	const int last = g.signal_nodes.back();
	g.signal_nodes[size_t(at - 1)] = last;
	g.signal_at[size_t(last)] = at;
	g.signal_nodes.pop_back();
	at = 0;
}

// Only a signalled junction's approaches show other than green (compute_light): each one with a stage is on its junction's list of
// approaches, and that junction is on LaneGraph::signal_nodes. Those are timed and lit; what was lit the step before and is not now
// goes back to green.
void update_lights(CityState &s) {
	TrafficState &t = s.traffic;
	LaneGraph &g = t.g;
	if (t.green_wanted.size() != g.ds.size()) t.green_wanted.resize(g.ds.size(), 0.0);
	for (int n : g.signal_nodes) {
		Junction &j = g.junctions[size_t(n)];
		if (j.green.empty()) continue; // nobody arrives and nobody crosses: no stage to run
		if (j.fixed) run_fixed(t, j);
		else advance(s, n, j);
	}
	if (t.light_now.size() != g.ds.size()) t.light_now.resize(g.ds.size(), uint8_t(Light::GREEN));
	for (int ds : t.lit_ds)
		if (size_t(ds) < t.light_now.size()) t.light_now[size_t(ds)] = uint8_t(Light::GREEN);
	t.lit_ds.clear();
	for (int n : g.signal_nodes)
		for (const std::vector<int> &approaches : g.junctions[size_t(n)].approaches)
			for (int ds : approaches) {
				if (size_t(ds) >= g.ds.size() || !g.ds[size_t(ds)].ok) continue;
				t.light_now[size_t(ds)] = uint8_t(compute_light(s, ds));
				t.lit_ds.push_back(ds);
			}
}

Light light_for(const CityState &s, int ds) {
	const TrafficState &t = s.traffic;
	if (ds >= 0 && size_t(ds) < t.light_now.size()) return Light(t.light_now[size_t(ds)]);
	return compute_light(s, ds);
}

float amber_left(const CityState &s, int ds) {
	const TrafficState &t = s.traffic;
	const Junction &j = t.g.junctions[size_t(t.g.ds[size_t(ds)].to)];
	return float(j.run_start + double(j.run_green) + double(j.amber[size_t(j.run)]) - t.time);
}

double next_green(const CityState &s, int ds, double now) {
	const TrafficState &t = s.traffic;
	if (ds < 0 || ds >= int(t.g.ds.size())) return now;
	const DirSeg &d = t.g.ds[size_t(ds)];
	if (d.to < 0 || d.to >= int(t.g.junctions.size())) return now;
	const Junction &j = t.g.junctions[size_t(d.to)];
	if (j.ctl != Control::SIGNAL || d.stage < 0 || d.stage >= int(j.green.size())) return now;
	const size_t stages = j.green.size();
	size_t k = size_t(j.run);
	const double since = now - j.run_start;
	if (int(k) == d.stage && (j.run_green < 0 || since < j.run_green)) return now;
	// the earliest it comes: the running stage's least rest, then each stage before it at its minimum
	double at = j.run_start + (j.run_green < 0 ? std::max(since, double(j.green[k])) : double(j.run_green)) + j.amber[k] + j.all_red[k];
	for (k = (k + 1) % stages; int(k) != d.stage; k = (k + 1) % stages) at += j.green[k] + j.amber[k] + j.all_red[k];
	return std::max(now, at);
}

} // namespace traffic
} // namespace city
