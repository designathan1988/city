// The far traffic's streets (street_flows.h): each directed segment's tables and its two curves (the vehicles counted in at its
// start and out at its end, back in time), fitted to the lane graph as it stands (its length, lanes, speed, what rules its end),
// seeded with what the demand puts on it, and read. The step that moves them is flow_step.cpp, the nodes flow_nodes.cpp, the
// window's side flow_window.cpp, the demand's flow_rates.cpp.
#include "sim/traffic/flow_parts.h"

#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr int RING_LEAST = 3;

// A lane's most, vehicles a second: the peak of the triangular diagram with free speed `v`, wave speed `w` and a standing room
// of `slot` metres a vehicle.
float lane_peak(float v, float w, float slot) { return v * w / ((v + w) * slot); }

// The traffic at the end of `d` waits its turn there: a signal, a stop or a give-way.
bool held_at_end(const LaneGraph &g, const DirSeg &d) {
	const bool lit = d.to >= 0 && size_t(d.to) < g.junctions.size() && g.junctions[size_t(d.to)].ctl == Control::SIGNAL;
	return lit || d.must_stop || d.rank < 2;
}

int ring_take(StreetFlows &f, int n) {
	std::vector<int> &given = f.spare[size_t(n)];
	if (!given.empty()) {
		const int at = given.back();
		given.pop_back();
		return at;
	}
	const int at = int(f.curve.size());
	f.curve.resize(f.curve.size() + size_t(n), 0.f);
	return at;
}

void ring_give(StreetFlows &f, int &at, uint16_t &n) {
	if (at >= 0) f.spare[size_t(n)].push_back(at);
	at = -1;
	n = 0;
}

// Every table by ds with room for `n` of them.
void size_tables(StreetFlows &f, size_t n) {
	if (f.live.size() >= n) return;
	for (std::vector<uint8_t> *v : { &f.live, &f.inside, &f.told_flags, &f.line_due, &f.in_owing }) v->resize(n, 0);
	for (std::vector<float> *v : { &f.cap, &f.store, &f.start_rate, &f.end_share, &f.origin, &f.send, &f.room, &f.came, &f.left, &f.asked, &f.owed,
				 &f.along, &f.told_count, &f.told_queue, &f.told_speed })
		v->resize(n, 0.f);
	f.detailed.resize(n, 0);
	f.up_at.resize(n, -1);
	f.down_at.resize(n, -1);
	f.up_n.resize(n, 0);
	f.down_n.resize(n, 0);
	f.turn_to.resize(n * FLOW_TURNS, -1);
	f.turn_share.resize(n * FLOW_TURNS, 0.f);
	if (f.spare.size() <= size_t(FLOW_RING_MOST)) f.spare.resize(size_t(FLOW_RING_MOST) + 1);
}

// The steps a run of `len` at `speed` takes, as a ring's length.
int ring_steps(float len, float speed, float dt) { return std::min(FLOW_RING_MOST, std::max(RING_LEAST, int(std::ceil(len / (speed * dt))) + 2)); }

} // namespace

float flow_on(const StreetFlows &f, int ds, uint64_t now) {
	const size_t k = size_t(ds);
	return std::max(0.f, f.curve[size_t(f.up_at[k] + flow_cell(now, f.up_n[k], 0))] - f.curve[size_t(f.down_at[k] + flow_cell(now, f.down_n[k], 0))]);
}

float flow_queue(const CityState &s, int ds, uint64_t now) {
	const StreetFlows &f = s.traffic.flows;
	const DirSeg &d = s.traffic.g.ds[size_t(ds)];
	const float run = d.len / std::max(FLOW_SLOWEST_MS, d.speed) / s.catalog.traffic.flow_dt_s;
	const float out = f.curve[size_t(f.down_at[size_t(ds)] + flow_cell(now, f.down_n[size_t(ds)], 0))];
	return std::min(flow_on(f, ds, now), std::max(0.f, flow_past(f, f.up_at[size_t(ds)], f.up_n[size_t(ds)], now, run) - out));
}

float flow_whole(TrafficState &t, float n) {
	const float whole = std::floor(std::max(0.f, n));
	return whole + (t.rng.f() < n - whole ? 1.f : 0.f);
}

float flow_count(const StreetFlows &f, int ds) {
	const size_t k = size_t(ds);
	return k < f.live.size() && f.live[k] && !f.inside[k] ? flow_on(f, ds, f.steps) : 0.f;
}

float flow_queued(const CityState &s, int ds) {
	const StreetFlows &f = s.traffic.flows;
	return size_t(ds) < f.live.size() && f.live[size_t(ds)] && !f.inside[size_t(ds)] ? flow_queue(s, ds, f.steps) : 0.f;
}

float flow_speed(const CityState &s, int ds) {
	const StreetFlows &f = s.traffic.flows;
	if (size_t(ds) >= f.live.size() || !f.live[size_t(ds)]) return 0;
	const DirSeg &d = s.traffic.g.ds[size_t(ds)];
	const float free = std::max(FLOW_SLOWEST_MS, d.speed), queue = flow_queued(s, ds), through = f.cap[size_t(ds)] / s.catalog.traffic.flow_dt_s;
	return queue <= FLOW_TINY ? free : through > FLOW_TINY ? d.len / (d.len / free + queue / through) : 0.f;
}

double flow_vehicles(const StreetFlows &f) {
	double all = 0;
	for (size_t ds = 0; ds < f.live.size(); ds++)
		if (f.live[ds]) all += double(f.origin[ds]) + double(f.inside[ds] ? f.owed[ds] : flow_count(f, int(ds)));
	return all;
}

float flow_owed(const StreetFlows &f) {
	float all = 0;
	for (int ds : f.owing) // the streets with a whole vehicle or more owed: the list the window makes them from
		if (f.inside[size_t(ds)]) all += f.owed[size_t(ds)];
	return all;
}

void lay_flow(CityState &s, int ds, float n) {
	StreetFlows &f = s.traffic.flows;
	const TrafficParams &p = s.catalog.traffic;
	const size_t k = size_t(ds);
	const DirSeg &d = s.traffic.g.ds[k];
	const float free = std::max(FLOW_SLOWEST_MS, d.speed), dt = p.flow_dt_s;
	const int un = ring_steps(d.len, free, dt), dn = ring_steps(d.len, flow_wave_ms(p), dt);
	if (f.up_n[k] != un) {
		ring_give(f, f.up_at[k], f.up_n[k]);
		f.up_at[k] = ring_take(f, un);
		f.up_n[k] = uint16_t(un);
	}
	if (f.down_n[k] != dn) {
		ring_give(f, f.down_at[k], f.down_n[k]);
		f.down_at[k] = ring_take(f, dn);
		f.down_n[k] = uint16_t(dn);
	}
	std::fill_n(f.curve.begin() + f.down_at[k], dn, 0.f);
	// entered evenly over the last free run, none left yet
	for (int back = 0; back < un; back++) f.curve[size_t(f.up_at[k] + flow_cell(f.steps, un, back))] = n * std::max(0.f, 1 - float(back) * dt * free / d.len);
}

void flow_take(StreetFlows &f, int ds) {
	const size_t k = size_t(ds);
	if (k >= f.live.size() || !f.live[k] || f.inside[k]) return;
	f.curve[size_t(f.up_at[k] + flow_cell(f.steps, f.up_n[k], 0))] += 1;
	f.handed_in += 1;
}

void flow_owes(StreetFlows &f, int ds) {
	if (f.in_owing[size_t(ds)] || f.owed[size_t(ds)] < 1) return;
	f.in_owing[size_t(ds)] = 1;
	f.owing.push_back(ds);
}

void fit_flow(CityState &s, int ds) {
	TrafficState &t = s.traffic;
	StreetFlows &f = t.flows;
	const TrafficParams &p = s.catalog.traffic;
	size_tables(f, t.g.ds.size());
	const size_t k = size_t(ds);
	if (k >= t.g.ds.size()) return;
	const DirSeg &d = t.g.ds[k];
	const float had = flow_count(f, ds);
	f.line_due[k] = 1; // the render is told it again, with its line (or that it is gone), at the step's next telling
	if (!d.ok || d.lanes <= 0 || d.len <= 0) { // the street is gone: what was on it and waiting for it leaves with it
		if (!f.live[k]) return;
		f.ended += double(had + f.origin[k] + f.owed[k]);
		ring_give(f, f.up_at[k], f.up_n[k]);
		ring_give(f, f.down_at[k], f.down_n[k]);
		f.live[k] = f.inside[k] = 0;
		f.origin[k] = f.owed[k] = f.along[k] = f.start_rate[k] = f.end_share[k] = 0;
		return;
	}
	const float free = std::max(FLOW_SLOWEST_MS, d.speed);
	f.live[k] = 1;
	f.cap[k] = lane_peak(free, flow_wave_ms(p), p.slot_m) * float(d.lanes) * (held_at_end(t.g, d) ? p.flow_green_share : 1.f) * p.flow_dt_s;
	f.store[k] = d.len * float(d.lanes) / p.slot_m;
	// its length or speed changed (or it is new): its vehicles laid along it again; else its curves stand as they are
	const bool refit = f.up_n[k] != ring_steps(d.len, free, p.flow_dt_s) || f.down_n[k] != ring_steps(d.len, flow_wave_ms(p), p.flow_dt_s);
	if (!f.inside[k] && refit) lay_flow(s, ds, std::min(had, f.store[k]));
}

void fit_turns(CityState &s, int node) {
	TrafficState &t = s.traffic;
	StreetFlows &f = t.flows;
	if (size_t(node) >= s.roads.nodes.size()) return;
	for (int seg : s.roads.nodes[size_t(node)].segs) {
		const int ds = in_ds(s.roads, seg, node);
		if (size_t(ds) >= f.live.size() || size_t(ds) >= t.g.ds.size()) continue;
		int *to = f.turn_to.data() + size_t(ds) * FLOW_TURNS;
		float *share = f.turn_share.data() + size_t(ds) * FLOW_TURNS;
		std::fill_n(to, FLOW_TURNS, -1);
		std::fill_n(share, FLOW_TURNS, 0.f);
		if (!t.g.ds[size_t(ds)].ok) continue;
		// the streets its lanes' connectors lead onto, each weighed by how many lead there; back the way it came only when nothing else
		for (const bool back : { false, true }) {
			float all = 0;
			each_conn_from(t.g, ds, [&](int c) {
				const Conn &k = t.g.conns[size_t(c)];
				if ((k.turn == Turn::UTURN) != back) return;
				int at = 0;
				while (at < FLOW_TURNS && to[at] >= 0 && to[at] != k.to_ds) at++;
				if (at == FLOW_TURNS) return;
				to[at] = k.to_ds;
				share[at] += 1;
				all += 1;
			});
			if (all <= 0) continue;
			for (int at = 0; at < FLOW_TURNS; at++) share[at] /= all;
			break;
		}
	}
}

void refit_flows(CityState &s, const std::vector<int> &removed, const std::vector<int> &changed, const std::vector<int> &nodes) {
	// a street laid inside the window is the window's at once, one removed or laid again elsewhere on its id leaves it: the window
	// looks at its own streets again (window.cpp check_window), a walk of the window and never of the map
	s.traffic.flows.relook = true;
	for (const std::vector<int> *segs : { &removed, &changed })
		for (int seg : *segs) {
			fit_flow(s, seg * 2);
			fit_flow(s, seg * 2 + 1);
		}
	for (int n : nodes) {
		if (n < 0 || n >= int(s.roads.nodes.size())) continue;
		for (int seg : s.roads.nodes[size_t(n)].segs) fit_flow(s, in_ds(s.roads, seg, n)); // what rules its end may have changed
		fit_turns(s, n);
	}
}

void seed_flows(CityState &s) {
	TrafficState &t = s.traffic;
	StreetFlows &f = t.flows;
	f.day_shift = s.clock.day_shift.load();
	for (int ds = 0; ds < int(f.live.size()); ds++) {
		const size_t k = size_t(ds);
		if (!f.live[k]) continue;
		const DirSeg &d = t.g.ds[k];
		const float n = std::min(f.store[k], flow_density(s, ds) * d.len * float(d.lanes));
		if (!f.inside[k]) {
			lay_flow(s, ds, n);
			continue;
		}
		// a street of the window: what it lacks of its share is owed to it, made along it
		f.owed[k] = f.along[k] = flow_whole(t, n - float(f.detailed[k]));
		flow_owes(f, ds);
	}
}

void reset_flows(CityState &s) {
	TrafficState &t = s.traffic;
	t.flows = StreetFlows();
	size_tables(t.flows, t.g.ds.size());
	for (int ds = 0; ds < int(t.g.ds.size()); ds++) fit_flow(s, ds);
	for (int n = 0; n < int(s.roads.nodes.size()); n++) fit_turns(s, n);
	seed_flows(s); // a street opens carrying what its sides put on it at this hour, in free flow: the map is full from its first step
}

} // namespace traffic
} // namespace city
