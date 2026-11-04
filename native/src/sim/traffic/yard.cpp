// The ways of a yard (yard.h). The grid is the world's, so nothing is turned back and forth: open are the lot, the carriageways and
// the gates' mouths across the footway; shut, every other lot and this lot's solid parts.
#include "sim/traffic/yard.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {
constexpr double HALF = 0.5, TINY = 1e-9, PI = 3.14159265358979323846, SETTLE_TANGENT = 0.6, H_DERIV = 6; // the derivative of the Hermite basis: 6 u^2 - 6 u

std::vector<AxlePose> straight(const AxlePose &from, const AxlePose &to, bool reverse, double step) {
	const int n = std::max(1, int(std::ceil(std::hypot(to.x - from.x, to.z - from.z) / step)));
	std::vector<AxlePose> out;
	for (int i = 0; i <= n; i++) out.push_back({ from.x + (to.x - from.x) * i / n, from.z + (to.z - from.z) * i / n, to.angle, reverse });
	return out;
}

// A short leg between two poses, driven one way: a cubic Hermite with the ends' headings.
std::vector<AxlePose> settle(const AxlePose &from, const AxlePose &to, bool reverse, double step) {
	const double len = std::hypot(to.x - from.x, to.z - from.z), dir = reverse ? -1 : 1, k = SETTLE_TANGENT * len;
	const double t0x = std::cos(from.angle) * dir, t0z = std::sin(from.angle) * dir, t1x = std::cos(to.angle) * dir, t1z = std::sin(to.angle) * dir;
	const int n = std::max(1, int(std::ceil(len / step)));
	std::vector<AxlePose> out;
	for (int i = 0; i <= n; i++) {
		const double u = double(i) / n, u2 = u * u, u3 = u2 * u;
		const double h00 = 2 * u3 - 3 * u2 + 1, h10 = u3 - 2 * u2 + u, h01 = -2 * u3 + 3 * u2, h11 = u3 - u2;
		const double d00 = H_DERIV * (u2 - u), d10 = 3 * u2 - 4 * u + 1, d01 = -d00, d11 = 3 * u2 - 2 * u;
		const double dx = d00 * from.x + d10 * k * t0x + d01 * to.x + d11 * k * t1x, dz = d00 * from.z + d10 * k * t0z + d01 * to.z + d11 * k * t1z;
		const double moving = std::hypot(dx, dz) > TINY ? std::atan2(dz, dx) : from.angle + (reverse ? PI : 0);
		const double angle = i == 0 ? from.angle : i == n ? to.angle : moving + (reverse ? PI : 0);
		out.push_back({ h00 * from.x + h10 * k * t0x + h01 * to.x + h11 * k * t1x, h00 * from.z + h10 * k * t0z + h01 * to.z + h11 * k * t1z, angle, reverse });
	}
	return out;
}

bool all_fit(const Field &f, const Steered &b, const std::vector<AxlePose> &way, double margin) {
	for (const AxlePose &p : way)
		if (!body_fits(f, b, p, margin)) return false;
	return true;
}

bool point_in(const std::vector<V2> &ring, double x, double z) {
	bool in = false;
	for (size_t a = 0, b = ring.size() - 1; a < ring.size(); b = a++) {
		const V2 &p = ring[a], &q = ring[b];
		if ((p.y > z) != (q.y > z) && x < p.x + (z - p.y) * (q.x - p.x) / (q.y - p.y)) in = !in;
	}
	return in;
}

bool body_in_lot(const YardInput &in, const AxlePose &p) {
	const Steered &b = in.body;
	const double c = std::cos(p.angle), s = std::sin(p.angle), back = b.length * HALF - b.rear, front = b.length * HALF + b.rear, hw = b.width * HALF;
	for (double u : { -back, front })
		for (double v : { -hw, hw })
			if (!point_in(in.lot, p.x + c * u - s * v, p.z + s * u + c * v)) return false;
	return true;
}

// Only the street near a gate stays open: a lorry turns in and out there.
void close_far_street(const YardInput &in, Field &f) {
	Field lot;
	lot.resize(f.x0, f.z0, f.cell, f.nx, f.nz, 0);
	fill_ring(lot, in.lot, 1);
	const float window2 = in.street_window * in.street_window;
	for (int j = 0; j < f.nz; j++)
		for (int i = 0; i < f.nx; i++) {
			const size_t k = size_t(j) * f.nx + i;
			if (f.blocked[k] || lot.blocked[k]) continue;
			const V2 at(float(f.x0 + (i + HALF) * f.cell), float(f.z0 + (j + HALF) * f.cell));
			bool near = false;
			for (const YardGate &g : in.gates) near = near || (g.outer - at).dot(g.outer - at) <= window2;
			if (!near) f.blocked[k] = 1;
		}
}

// The farthest out its straight reverse onto the dock can start, the body clear there; false when there is none.
bool pre_dock(const YardInput &in, const Field &f, const AxlePose &docked, const V2 &out, AxlePose &pre) {
	for (double d = in.pre_dock; d >= in.pre_step; d -= in.pre_step) {
		const AxlePose at{ docked.x + out.x * d, docked.z + out.y * d, docked.angle, true };
		if (body_clear(f, in.body, at) && all_fit(f, in.body, straight(at, docked, true, in.settle_step), in.fit_margin)) {
			pre = at;
			return true;
		}
	}
	return false;
}

// A way found and brought exactly onto its goal.
bool way_to(const YardInput &in, const Field &f, const AxlePose &from, const AxlePose &to, bool reversing, std::vector<AxlePose> &way, bool fine = false) {
	if (!find_way(f, in.body, from, { to, reversing, in.reach, in.turn }, way, fine)) return false;
	const std::vector<AxlePose> tail = settle(way.back(), to, reversing, in.settle_step);
	if (!all_fit(f, in.body, tail, in.fit_margin)) return false;
	way.insert(way.end(), tail.begin() + 1, tail.end());
	return true;
}

// The same way driven the other way round: a pose is reached the opposite way to how the one after it was.
std::vector<AxlePose> driven_back(const std::vector<AxlePose> &way) {
	std::vector<AxlePose> out(way.rbegin(), way.rend());
	for (size_t i = 1; i < out.size(); i++) out[i].reverse = !way[way.size() - i].reverse;
	if (out.size() > 1) out[0].reverse = out[1].reverse;
	return out;
}

// One way after another, the pose they share once.
void joined(const std::vector<AxlePose> &first, const std::vector<AxlePose> &then, std::vector<AxlePose> &out) {
	out = first;
	out.insert(out.end(), then.begin() + 1, then.end());
}

// Both ways of a dock through one gate. The lorry backs in from past the gate, or backs out to the stop, when only the other way was
// found: the street is closed for it all the same (sweep_lanes).
bool through_gate(const YardInput &in, const Field &f, const YardGate &g, const AxlePose &docked, const AxlePose &pre, DockWays &out) {
	const auto got = [&](int stage) { out.reached = std::max(out.reached, stage); };
	got(YARD_NO_STOP);
	if (!body_clear(f, in.body, g.stop)) return false;
	got(YARD_NO_WAY);
	std::vector<AxlePose> onto, away;
	bool found_in = way_to(in, f, g.stop, pre, true, onto);
	const bool found_out = way_to(in, f, pre, g.join, false, away);
	found_in = found_in || (!found_out && way_to(in, f, g.stop, pre, true, onto, true));
	if (!found_in && !found_out) return false;
	if (!found_in || !found_out) { // the search keeps one pose to a cell, so it can lose a way that exists: one found holds for both
		got(YARD_NO_LANE);
		const std::vector<AxlePose> lane = settle(g.stop, g.join, false, in.settle_step);
		if (!all_fit(f, in.body, lane, in.fit_margin)) return false;
		got(YARD_ONE_WAY);
		if (found_in) joined(driven_back(onto), lane, away);
		else joined(lane, driven_back(away), onto);
	} else got(YARD_BOTH_WAYS);
	out.into = onto;
	const std::vector<AxlePose> to_dock = straight(pre, docked, true, in.settle_step);
	out.into.insert(out.into.end(), to_dock.begin() + 1, to_dock.end());
	out.into_street = int(out.into.size()) - 1;
	for (size_t i = 0; i < out.into.size(); i++)
		if (body_in_lot(in, out.into[i])) { out.into_street = int(i); break; }
	out.away = straight(docked, pre, false, in.settle_step);
	out.away.insert(out.away.end(), away.begin() + 1, away.end());
	out.away_street = int(out.away.size()) - 1;
	for (size_t i = 0; i < out.away.size(); i++)
		if (!body_in_lot(in, out.away[i])) { out.away_street = std::max(0, int(i) - 1); break; }
	return true;
}
} // namespace

void plan_yard(const YardInput &in, YardPlan &out) {
	Field f;
	f.resize(in.x0, in.z0, in.cell, in.nx, in.nz, 1);
	fill_ring(f, in.lot, 0);
	for (const std::vector<V2> &ring : in.open) fill_ring(f, ring, 0);
	close_far_street(in, f);
	for (const std::vector<V2> &ring : in.shut) fill_ring(f, ring, 1);
	fill_clearance(f);
	out.gates = in.gates;
	for (const YardDock &d : in.docks) {
		DockWays ways;
		ways.bay = d.bay;
		const double centre = in.body.length * HALF + in.dock_gap - in.body.rear;
		const AxlePose docked{ d.door.x + d.out.x * centre, d.door.y + d.out.y * centre, std::atan2(double(d.out.y), double(d.out.x)), true };
		AxlePose pre;
		std::vector<int> order;
		for (int g = 0; g < int(in.gates.size()); g++) order.push_back(g);
		std::sort(order.begin(), order.end(), [&](int a, int b) { return (in.gates[size_t(a)].inner - d.door).len() < (in.gates[size_t(b)].inner - d.door).len(); });
		if (pre_dock(in, f, docked, d.out, pre))
			for (int g : order)
				if (through_gate(in, f, in.gates[size_t(g)], docked, pre, ways)) {
					ways.gate = g;
					break;
				}
		out.docks.push_back(std::move(ways));
	}
}

} // namespace traffic
} // namespace city
