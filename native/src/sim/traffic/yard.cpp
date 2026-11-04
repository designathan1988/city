// The ways of a yard (yard.h). The grid is the world's, so nothing is turned back and forth: open are the lot, the carriageways and
// the gates' mouths across the footway; shut, every other lot and this lot's solid parts.
#include "sim/traffic/yard.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {
constexpr int FIT_WORK = 64; // a body tested cell by cell along a short leg: units of work
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

// Only the street near a gate stays open: a lorry turns in and out there. One row of the grid.
void close_far_street(const YardInput &in, const Field &lot, Field &f, int j) {
	const float window2 = in.street_window * in.street_window;
	for (int i = 0; i < f.nx; i++) {
		const size_t k = size_t(j) * f.nx + i;
		if (f.blocked[k] || lot.blocked[k]) continue;
		const V2 at(float(f.x0 + (i + HALF) * f.cell), float(f.z0 + (j + HALF) * f.cell));
		bool near = false;
		for (const YardGate &g : in.gates) near = near || (g.outer - at).dot(g.outer - at) <= window2;
		if (!near) f.blocked[k] = 1;
	}
}

// What filling a ring costs: the cells of the box round it.
int ring_work(const Field &f, const std::vector<V2> &ring) {
	float lo_x = ring[0].x, hi_x = lo_x, lo_z = ring[0].y, hi_z = lo_z;
	for (const V2 &p : ring) {
		lo_x = std::min(lo_x, p.x);
		hi_x = std::max(hi_x, p.x);
		lo_z = std::min(lo_z, p.y);
		hi_z = std::max(hi_z, p.y);
	}
	return 1 + int((hi_x - lo_x) * (hi_z - lo_z) / (f.cell * f.cell)) / CELLS_PER_WORK;
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

// A way found, brought exactly onto its goal.
bool settled(const YardInput &in, const Field &f, const std::vector<AxlePose> &found, const AxlePose &to, bool reversing, std::vector<AxlePose> &way) {
	const std::vector<AxlePose> tail = settle(found.back(), to, reversing, in.settle_step);
	if (!all_fit(f, in.body, tail, in.fit_margin)) return false;
	way = found;
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

} // namespace

YardPlanner::YardPlanner(YardInput in) : in_(std::move(in)) {
	field_.resize(in_.x0, in_.z0, in_.cell, in_.nx, in_.nz, 1);
	lot_.resize(in_.x0, in_.z0, in_.cell, in_.nx, in_.nz, 0);
	plan.gates = in_.gates;
}

YardPlanner::~YardPlanner() = default;

void YardPlanner::open_step(int &work) {
	if (ring_ == 0) {
		fill_ring(field_, in_.lot, 0);
		fill_ring(lot_, in_.lot, 1);
		work -= ring_work(field_, in_.lot);
	} else if (ring_ <= in_.open.size()) {
		fill_ring(field_, in_.open[ring_ - 1], 0);
		work -= ring_work(field_, in_.open[ring_ - 1]);
	} else {
		stage_ = Stage::STREET;
		return;
	}
	ring_++;
}

void YardPlanner::street_step(int &work) {
	if (row_ == field_.nz) {
		lot_ = Field();
		ring_ = 0;
		stage_ = Stage::SHUT;
		return;
	}
	close_far_street(in_, lot_, field_, row_++);
	work -= 1 + field_.nx / CELLS_PER_WORK;
}

void YardPlanner::shut_step(int &work) {
	if (ring_ == in_.shut.size()) {
		clearance_ = std::make_unique<ClearanceFill>(field_);
		stage_ = Stage::CLEARANCE;
		return;
	}
	fill_ring(field_, in_.shut[ring_], 1);
	work -= ring_work(field_, in_.shut[ring_]);
	ring_++;
}

void YardPlanner::next_dock(int &work) {
	if (dock_ == in_.docks.size()) {
		stage_ = Stage::DONE;
		return;
	}
	const YardDock &d = in_.docks[dock_];
	ways_ = DockWays();
	ways_.bay = d.bay;
	const double centre = in_.body.length * HALF + in_.dock_gap - in_.body.rear;
	docked_ = { d.door.x + d.out.x * centre, d.door.y + d.out.y * centre, std::atan2(double(d.out.y), double(d.out.x)), true };
	order_.clear();
	for (int g = 0; g < int(in_.gates.size()); g++) order_.push_back(g);
	std::sort(order_.begin(), order_.end(), [&](int a, int b) { return (in_.gates[size_t(a)].inner - d.door).len() < (in_.gates[size_t(b)].inner - d.door).len(); });
	gate_ = 0;
	work -= FIT_WORK;
	if (pre_dock(in_, field_, docked_, d.out, pre_)) stage_ = Stage::GATE;
	else close_dock();
}

void YardPlanner::close_dock() {
	plan.docks.push_back(std::move(ways_));
	dock_++;
	stage_ = Stage::DOCK;
}

void YardPlanner::search_for(const AxlePose &from, const AxlePose &to, bool reversing, bool fine, Stage stage) {
	search_ = std::make_unique<WaySearch>(field_, in_.body, from, WayGoal{ to, reversing, in_.reach, in_.turn }, fine);
	stage_ = stage;
}

void YardPlanner::next_gate(int &work) {
	if (gate_ == order_.size()) {
		close_dock();
		return;
	}
	const YardGate &g = in_.gates[size_t(order_[gate_])];
	work--;
	ways_.reached = std::max(ways_.reached, int(YARD_NO_STOP));
	if (!body_clear(field_, in_.body, g.stop)) {
		gate_++;
		return;
	}
	ways_.reached = std::max(ways_.reached, int(YARD_NO_WAY));
	search_for(g.stop, pre_, true, false, Stage::WAY_IN);
}

// The way in, the way out, and when neither was found the way in again at the finer grain. The search keeps one pose to a cell, so
// it can lose a way that exists: one found holds for both.
void YardPlanner::search_step(int &work) {
	if (!search_->advance(work)) return;
	const YardGate &g = in_.gates[size_t(order_[gate_])];
	const bool found = search_->found();
	work -= FIT_WORK;
	if (stage_ == Stage::WAY_OUT) {
		found_out_ = found && settled(in_, field_, search_->way(), g.join, false, away_);
		if (!found_in_ && !found_out_) {
			search_for(g.stop, pre_, true, true, Stage::WAY_FINE);
			return;
		}
	} else {
		found_in_ = found && settled(in_, field_, search_->way(), pre_, true, onto_);
		if (stage_ == Stage::WAY_IN) {
			search_for(pre_, g.join, false, false, Stage::WAY_OUT);
			return;
		}
	}
	search_.reset();
	if (join_ways(g)) {
		ways_.gate = order_[gate_];
		close_dock();
	} else {
		gate_++;
		stage_ = Stage::GATE;
	}
}

// Both ways of a dock through one gate. The lorry backs in from past the gate, or backs out to the stop, when only the other way was
// found: the street is closed for it all the same (sweep_lanes).
bool YardPlanner::join_ways(const YardGate &g) {
	if (!found_in_ && !found_out_) return false;
	if (!found_in_ || !found_out_) {
		ways_.reached = std::max(ways_.reached, int(YARD_NO_LANE));
		const std::vector<AxlePose> lane = settle(g.stop, g.join, false, in_.settle_step);
		if (!all_fit(field_, in_.body, lane, in_.fit_margin)) return false;
		ways_.reached = std::max(ways_.reached, int(YARD_ONE_WAY));
		if (found_in_) joined(driven_back(onto_), lane, away_);
		else joined(lane, driven_back(away_), onto_);
	} else ways_.reached = std::max(ways_.reached, int(YARD_BOTH_WAYS));
	ways_.into = onto_;
	const std::vector<AxlePose> to_dock = straight(pre_, docked_, true, in_.settle_step);
	ways_.into.insert(ways_.into.end(), to_dock.begin() + 1, to_dock.end());
	ways_.into_street = int(ways_.into.size()) - 1;
	for (size_t i = 0; i < ways_.into.size(); i++)
		if (body_in_lot(in_, ways_.into[i])) { ways_.into_street = int(i); break; }
	ways_.away = straight(docked_, pre_, false, in_.settle_step);
	ways_.away.insert(ways_.away.end(), away_.begin() + 1, away_.end());
	ways_.away_street = int(ways_.away.size()) - 1;
	for (size_t i = 0; i < ways_.away.size(); i++)
		if (!body_in_lot(in_, ways_.away[i])) { ways_.away_street = std::max(0, int(i) - 1); break; }
	return true;
}

// Stops at the step that spends the last of the work: the next tick goes on from there.
bool YardPlanner::advance(int work) {
	while (work > 0 && stage_ != Stage::DONE) {
		switch (stage_) {
			case Stage::OPEN: open_step(work); break;
			case Stage::STREET: street_step(work); break;
			case Stage::SHUT: shut_step(work); break;
			case Stage::CLEARANCE:
				if (clearance_->advance(work)) {
					clearance_.reset();
					stage_ = Stage::DOCK;
				}
				break;
			case Stage::DOCK: next_dock(work); break;
			case Stage::GATE: next_gate(work); break;
			default: search_step(work); break;
		}
	}
	return stage_ == Stage::DONE;
}

} // namespace traffic
} // namespace city
