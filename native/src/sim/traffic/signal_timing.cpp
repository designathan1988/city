// The signal plan in time (CONTRAN MBST vol. V, Res. 483/2014, item 5.7): the intergreen after each green (amber, then
// all-red), the stage running now and the light each approach shows, when an approach next turns green, and when
// walkers may still start over a crosswalk (their flashing red and all-red must fit before the stage ends).
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

namespace {
// The stage running at `time` (its green, amber and all-red), and how long since its green began; -1 without a plan.
int stage_at(const Junction &j, double time, float &into) {
	if (j.cycle <= 0) return -1;
	float x = float(std::fmod(time + j.offset, double(j.cycle)));
	for (size_t k = 0; k < j.green.size(); k++) {
		const float span = j.green[k] + j.amber[k] + j.all_red[k];
		if (x < span) {
			into = x;
			return int(k);
		}
		x -= span;
	}
	return -1;
}

Light compute_light(const CityState &s, int ds) {
	const LaneGraph &g = s.traffic.g;
	const DirSeg &d = g.ds[ds];
	const Junction &j = g.junctions[d.to];
	if (j.ctl != Control::SIGNAL || d.stage < 0 || j.cycle <= 0) return Light::GREEN;
	float into = 0;
	const int k = stage_at(j, s.traffic.time, into);
	if (k != d.stage || into >= j.green[size_t(k)] + j.amber[size_t(k)]) return Light::RED;
	return into < j.green[size_t(k)] ? Light::GREEN : Light::AMBER;
}
} // namespace

float ped_clearance(const CityState &s, int arm_seg) {
	const TrafficParams &tp = s.catalog.traffic;
	const Profile &p = s.roads.profile_of(arm_seg);
	return tp.ped_reaction_s + (p.asph_left + p.asph_right) / tp.ped_clear_speed;
}

bool walk_signal(const CityState &s, int node, int arm_seg, int arm_stage) {
	const Junction &j = s.traffic.g.junctions[size_t(node)];
	float into = 0;
	const int k = stage_at(j, s.traffic.time, into);
	if (k < 0 || k == arm_stage) return false;
	const float stage_end = j.green[size_t(k)] + j.amber[size_t(k)] + j.all_red[size_t(k)];
	return into < stage_end - ped_clearance(s, arm_seg) - s.catalog.traffic.ped_all_red_s;
}

void update_lights(CityState &s) {
	TrafficState &t = s.traffic;
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
	if (j.ctl != Control::SIGNAL || d.stage < 0 || d.stage >= int(j.green.size()) || j.cycle <= 0) return now;
	double x = std::fmod(now + j.offset, double(j.cycle));
	if (x < 0) x += double(j.cycle);
	double start = 0;
	for (int k = 0; k < d.stage && k < int(j.green.size()); k++) start += j.green[k] + j.amber[k] + j.all_red[k];
	if (x >= start && x < start + j.green[d.stage]) return now;
	if (x < start) return now + (start - x);
	return now + double(j.cycle) - (x - start);
}


} // namespace traffic
} // namespace city
