// The ways of a yard (yard.h). The grid is the world's, so nothing is turned back and forth: open are the lot, the carriageways and
// the gates' mouths across the footway; shut, every other lot and this lot's solid parts.
#include "sim/traffic/yard.h"
#include "core/plane_geometry.h"

#include "core/laps.h"
#include "core/math.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {
// Units of work (way_search.h) of the planner's own steps, their mean on the planner's bench: a way found brought onto its goal
// (settled: 4.3 µs), a dock's start (its straight reverse tested: 6.7 µs), a dock's two ways joined (44 µs), and a gate's stop
// tested before its search (8.0 µs, 257 of them).
constexpr int SETTLE_WORK = 90, DOCK_WORK = 140, JOIN_WORK = 917, GATE_WORK = 167; // the means over the unit, 0.048 µs
constexpr double HALF = 0.5, TINY = 1e-9, SETTLE_TANGENT = 0.6, H_DERIV = 6; // the derivative of the Hermite basis: 6 u^2 - 6 u

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
		const double moving = std::hypot(dx, dz) > TINY ? std::atan2(dz, dx) : from.angle + (reverse ? PI_D : 0);
		const double angle = i == 0 ? from.angle : i == n ? to.angle : moving + (reverse ? PI_D : 0);
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
	return inside_ring(ring, x, z, [](const V2 &p) { return std::make_pair(p.x, p.y); });
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

// Only the frames: the grids' cells are laid by advance (Stage::LAY), which pays for them.
YardPlanner::YardPlanner(YardInput in) : in_(std::move(in)) {
	field_.begin(in_.x0, in_.z0, in_.cell, in_.nx, in_.nz);
	lot_.begin(in_.x0, in_.z0, in_.cell, in_.nx, in_.nz);
	plan.gates = in_.gates;
	plan.reads = in_.reads;
}

// A row of each grid: solid everywhere for the yard's, nothing for the lot's.
void YardPlanner::lay_step(int &work) {
	const bool field_whole = field_.lay_row(1);
	const bool lot_whole = lot_.lay_row(0); // both grids a row each, whichever is whole first
	work -= 2 * (1 + field_.nx / CELLS_PER_WORK);
	if (field_whole && lot_whole) stage_ = Stage::OPEN;
}

// The ring of the open stage `k`: the lot in the yard's grid, the lot in the lot's, the other lots shut again, then the carriageways
// and gates' mouths: a neighbouring parcel drawn over a street never closes the road (nothing of it stands on the asphalt), while it
// still closes the lot where the two overlap.
std::unique_ptr<RingFill> YardPlanner::open_ring(size_t k) {
	if (k == 0) return std::make_unique<RingFill>(field_, &in_.lot, 0);
	if (k == 1) return std::make_unique<RingFill>(lot_, &in_.lot, 1);
	const size_t fenced = in_.fenced.size();
	if (k - 2 < fenced) return std::make_unique<RingFill>(field_, &in_.fenced[k - 2], 1);
	if (k - 2 - fenced < in_.open.size()) return std::make_unique<RingFill>(field_, &in_.open[k - 2 - fenced], 0);
	return nullptr;
}

YardPlanner::~YardPlanner() = default;

void YardPlanner::open_step(int &work) {
	if (!fill_) fill_ = open_ring(ring_);
	if (!fill_) {
		stage_ = Stage::STREET;
		return;
	}
	if (fill_->advance(ring_ == 1 ? lot_ : field_, work)) {
		fill_.reset();
		ring_++;
	}
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
	if (!fill_) fill_ = std::make_unique<RingFill>(field_, &in_.shut[ring_], 1);
	if (fill_->advance(field_, work)) {
		fill_.reset();
		ring_++;
	}
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
	work -= DOCK_WORK;
	if (pre_dock(in_, field_, docked_, d.out, pre_)) stage_ = Stage::GATE;
	else close_dock();
}

void YardPlanner::close_dock() {
	plan.docks.push_back(std::move(ways_));
	dock_++;
	stage_ = Stage::DOCK;
}

// A search reads the distance map of its goal's cell, worked out once on this field (Dolgov et al.; the 4.0's `maps`).
void YardPlanner::search_for(const AxlePose &from, const AxlePose &to, bool reversing, WayGrain grain, Stage stage) {
	std::shared_ptr<Holonomic> &map = maps_[holonomic_key(field_, to, grain)];
	if (!map) map = std::make_shared<Holonomic>();
	search_ = std::make_unique<WaySearch>(field_, in_.body, from, WayGoal{ to, reversing, in_.reach, in_.turn }, map, grain, space_);
	stage_ = stage;
}

void YardPlanner::next_gate(int &work) {
	if (gate_ == order_.size()) {
		close_dock();
		return;
	}
	const YardGate &g = in_.gates[size_t(order_[gate_])];
	work -= GATE_WORK;
	ways_.reached = std::max(ways_.reached, int(YARD_NO_STOP));
	if (!body_clear(field_, in_.body, g.stop)) {
		gate_++;
		return;
	}
	ways_.reached = std::max(ways_.reached, int(YARD_NO_WAY));
	search_for(g.stop, pre_, true, WayGrain::COARSE, Stage::WAY_IN);
}

// The way in, then the way out, at the coarse grain; when neither was found, the way in at the graduated grain, then at the fine one
// (WayGrain: the coarse grain finds most yards' ways fast, and loses a tight yard's, which the graduated one finds with some half the
// poses of the fine one). A search keeps one pose to a cell, so it can lose a way that exists: one found holds for both.
void YardPlanner::search_step(int &work) {
	if (!search_->advance(work)) return;
	const YardGate &g = in_.gates[size_t(order_[gate_])];
	const bool found = search_->found();
	work -= SETTLE_WORK; // the way found brought onto its goal, below
	const int kind = stage_ == Stage::WAY_IN ? YARD_SEARCH_IN : stage_ == Stage::WAY_OUT ? YARD_SEARCH_OUT : stage_ == Stage::WAY_GRADUATED ? YARD_SEARCH_GRADUATED : YARD_SEARCH_FINE;
	plan.cost.searches[kind]++;
	plan.cost.found[kind] += found ? 1 : 0;
	plan.cost.search[kind].add(search_->cost());
	if (stage_ == Stage::WAY_OUT) {
		found_out_ = found && settled(in_, field_, search_->way(), g.join, false, away_);
		if (!found_in_ && !found_out_) {
			search_for(g.stop, pre_, true, WayGrain::GRADUATED, Stage::WAY_GRADUATED);
			return;
		}
	} else {
		found_in_ = found && settled(in_, field_, search_->way(), pre_, true, onto_);
		if (stage_ == Stage::WAY_IN) {
			search_for(pre_, g.join, false, WayGrain::COARSE, Stage::WAY_OUT);
			return;
		}
		if (stage_ == Stage::WAY_GRADUATED && !found_in_) {
			search_for(g.stop, pre_, true, WayGrain::FINE, Stage::WAY_FINE);
			return;
		}
	}
	search_.reset();
	work -= JOIN_WORK;
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
	ways_.in_lot = false;
	for (size_t i = 0; i < ways_.into.size(); i++)
		if (body_in_lot(in_, ways_.into[i])) {
			ways_.into_street = int(i);
			ways_.in_lot = true;
			break;
		}
	ways_.away = straight(docked_, pre_, false, in_.settle_step);
	ways_.away.insert(ways_.away.end(), away_.begin() + 1, away_.end());
	ways_.away_street = int(ways_.away.size()) - 1;
	for (size_t i = 0; i < ways_.away.size(); i++)
		if (!body_in_lot(in_, ways_.away[i])) { ways_.away_street = std::max(0, int(i) - 1); break; }
	return true;
}

// Stops at the step that spends the last of the work: the next tick goes on from there.
bool YardPlanner::advance(int work) {
	Laps took, slice;
	const Stage began = stage_;
	while (work > 0 && stage_ != Stage::DONE) {
		const Stage was = stage_;
		switch (stage_) {
			case Stage::LAY: lay_step(work); break;
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
		const double ms = took.lap();
		plan.cost.total_ms += ms;
		if (was == Stage::DOCK || was == Stage::GATE) plan.cost.dock_ms += ms;
		else if (was >= Stage::WAY_IN) plan.cost.ways_ms += ms;
		else plan.cost.field_ms += ms;
	}
	const double spent = slice.lap();
	if (spent > plan.cost.slice_ms) {
		plan.cost.slice_ms = spent;
		plan.cost.slice_stage = int(began);
	}
	return stage_ == Stage::DONE;
}

} // namespace traffic
} // namespace city
