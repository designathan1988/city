// The signal plan in time (CONTRAN MBST vol. V, Res. 483/2014): the intergreen after each green (amber, then all-red,
// item 5.7), the detection that actuates each green (cap. 7: minimum green, extension per vehicle over the detection
// section, maximum green), the stage running now and the light each approach shows, when an approach next turns green,
// and when walkers may still start over a crosswalk (their flashing red and all-red must fit before the stage ends).
#include "sim/traffic/traffic_system.h"

#include "core/units.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

// The intergreen after an approach's green (CONTRAN MBST vol. V 5.7.1, Eq. 6.4-6.5): the amber t_pr + v/(2 a_ad) at its
// regulated speed, at least the minimum that speed asks and at most amber_max_s (what is left goes to the all-red); the
// all-red (d2 + c)/v to take a vehicle of length c from the stop line past the conflict area (its longest way over the
// junction and the far crosswalk), plus signal_ped_red_s when walkers cross in the next stage.
void intergreen(const CityState &s, int n, int ds, bool walkers, float &amber, float &all_red) {
	const TrafficParams &tp = s.catalog.traffic;
	const LaneGraph &g = s.traffic.g;
	const DirSeg &d = g.ds[size_t(ds)];
	const float v = d.speed, kmh = v * KMH_PER_MS;
	if (v <= 0) return;
	const float least = kmh <= tp.amber_low_kmh ? tp.amber_min_s : (kmh <= tp.amber_mid_kmh ? tp.amber_min_mid_s : tp.amber_min_high_s);
	const float wanted = std::max(least, tp.signal_reaction_s + v / (2 * tp.signal_decel));
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

void signal_call_far(CityState &s, int ds) {
	TrafficState &t = s.traffic;
	if (t.g.ds[size_t(ds)].extension <= 0 || size_t(ds) >= t.green_wanted.size()) return;
	const double headway = SECONDS_PER_HOUR / double(s.catalog.traffic.signal_sat_flow_lane_h);
	t.green_wanted[size_t(ds)] = std::max(t.green_wanted[size_t(ds)], t.time + headway);
}

namespace {
// Runs junction `j`'s actuated plan up to now (MBST vol. V cap. 7): a green lasts its minimum, then as long as calls
// want it (each until its vehicle reaches the line), never past its maximum; then the amber, the all-red, the next stage.
void advance(const TrafficState &t, Junction &j) {
	const size_t stages = j.green.size();
	for (size_t guard = 0; guard <= stages; guard++) {
		const size_t k = size_t(j.run);
		const double since = t.time - j.run_start;
		if (j.run_green < 0) {
			double want = j.green[k];
			for (int ds : j.approaches[k])
				if (size_t(ds) < t.green_wanted.size()) want = std::max(want, t.green_wanted[size_t(ds)] - j.run_start);
			want = std::min(want, double(j.green_max[k]));
			if (since < want) return;
			j.run_green = float(want);
		}
		const double span = double(j.run_green) + j.amber[k] + j.all_red[k];
		if (since < span) return;
		j.run_start += span;
		j.run = int((k + 1) % stages);
		j.run_green = -1;
	}
}

Light compute_light(const CityState &s, int ds) {
	const LaneGraph &g = s.traffic.g;
	const DirSeg &d = g.ds[ds];
	const Junction &j = g.junctions[d.to];
	if (j.ctl != Control::SIGNAL || d.stage < 0 || j.green.empty()) return Light::GREEN;
	if (d.stage != j.run) return Light::RED;
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

bool walk_signal(const CityState &s, int node, int arm_seg, int arm_stage) {
	const Junction &j = s.traffic.g.junctions[size_t(node)];
	if (j.green.empty() || j.run == arm_stage) return false;
	// the running stage lasts at least its minimum green and its intergreen: walkers start while their clearance fits
	const size_t k = size_t(j.run);
	const float least = j.green[k] + j.amber[k] + j.all_red[k];
	return s.traffic.time - j.run_start < least - ped_clearance(s, arm_seg) - s.catalog.traffic.ped_all_red_s;
}

void update_lights(CityState &s) {
	TrafficState &t = s.traffic;
	if (t.green_wanted.size() != t.g.ds.size()) t.green_wanted.resize(t.g.ds.size(), 0.0);
	for (Junction &j : t.g.junctions)
		if (j.ctl == Control::SIGNAL && !j.green.empty()) advance(t, j);
	t.light_now.assign(t.g.ds.size(), uint8_t(Light::GREEN));
	for (int ds = 0; ds < int(t.g.ds.size()); ds++)
		if (t.g.ds[ds].ok) t.light_now[size_t(ds)] = uint8_t(compute_light(s, ds));
}

Light light_for(const CityState &s, int ds) {
	const TrafficState &t = s.traffic;
	if (ds >= 0 && size_t(ds) < t.light_now.size()) return Light(t.light_now[size_t(ds)]);
	return compute_light(s, ds);
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
