// A line of poles from one point to another (Roadcraft editor/poles.ts): each end snaps to a pole standing or to the guide
// line behind the kerb, the way goes along the guide lines (sim/street/pole_routes.h), corners get a pole each, the rest
// stand evenly between them, off the crosswalks' landings, and a pole already standing there is used. The preview and the
// command read the same plan.
#include "sim/commands/commands.h"
#include "sim/commands/road_edit.h"

#include "sim/state/city_state.h"
#include "sim/street/street_spots.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace city {

namespace {
constexpr double SAME_PLACE_M = 0.002; // two ends this close are one place
constexpr int MARGIN_GROWTH = 2;
constexpr float CLEAR_STEP_M = 1.f; // how far along the route a pole is nudged each try
constexpr int LAMP_PERIOD = 2; // alternate: every other pole

struct Snap {
	double x = 0, z = 0;
	bool free = false;
	int pole = -1;
};

Snap snap_end(const CityState &s, const RouteGraph &g, double x, double z, double reach) {
	const PoleDef &d = s.catalog.furniture.poles;
	Snap out;
	out.x = x;
	out.z = z;
	const int hit = s.street.pole_near(float(x), float(z), float(reach));
	if (hit >= 0) {
		out.pole = hit;
		out.x = s.street.poles.items[size_t(hit)].x;
		out.z = s.street.poles.items[size_t(hit)].z;
		return out;
	}
	const bool on_walk = footway_at(s.roads, x, z, 0).ok; // anywhere on the pavement takes its line; off it, only just beside it
	const int st = g.nearest(x, z, on_walk ? d.footway_catch_m : d.line_catch_m);
	if (st < 0) {
		out.free = true;
		return out;
	}
	out.x = g.point(st).x;
	out.z = g.point(st).z;
	return out;
}

// Indices Ramer-Douglas-Peucker keeps at `tolerance`.
std::vector<size_t> corners(const std::vector<RoutePoint> &p, double tolerance) {
	std::vector<uint8_t> keep(p.size(), 0);
	keep.front() = keep.back() = 1;
	std::vector<std::pair<size_t, size_t>> stack{ { 0, p.size() - 1 } };
	while (!stack.empty()) {
		const auto [i, j] = stack.back();
		stack.pop_back();
		const double dx = p[j].x - p[i].x, dz = p[j].z - p[i].z, len = std::hypot(dx, dz);
		size_t far = 0;
		double far_d = tolerance;
		for (size_t k = i + 1; k < j; k++) {
			const double d = len > SAME_PLACE_M ? std::fabs((p[k].x - p[i].x) * dz - (p[k].z - p[i].z) * dx) / len : std::hypot(p[k].x - p[i].x, p[k].z - p[i].z);
			if (d > far_d) {
				far_d = d;
				far = k;
			}
		}
		if (far == 0) continue;
		keep[far] = 1;
		stack.push_back({ i, far });
		stack.push_back({ far, j });
	}
	std::vector<size_t> out;
	for (size_t i = 0; i < keep.size(); i++)
		if (keep[i]) out.push_back(i);
	return out;
}

struct Route {
	std::vector<RoutePoint> path;
	std::vector<double> cum;
	RoutePoint at(double s) const {
		size_t i = 1;
		while (i + 1 < cum.size() && cum[i] < s) i++;
		const double s0 = cum[i - 1], s1 = cum[i], t = s1 > s0 ? (s - s0) / (s1 - s0) : 0;
		return { path[i - 1].x + (path[i].x - path[i - 1].x) * t, path[i - 1].z + (path[i].z - path[i - 1].z) * t };
	}
};

// Stands clear of a landing and on a footway.
bool clear_place(const CityState &s, const RoutePoint &p) {
	const PoleDef &d = s.catalog.furniture.poles;
	const FootwayHit h = footway_at(s.roads, p.x, p.z, s.catalog.furniture.items.stand_reach_m);
	return h.ok && !on_landing(s.roads, h, s.catalog.furniture.footprint[int(FurnitureKind::POLE)].x_m * 0.5f, d.landing_clear_m);
}

bool lamp_for(int index, LampMode mode) {
	return mode == LampMode::ALL || (mode == LampMode::ALTERNATE && index % LAMP_PERIOD == 0);
}

// The stations of the poles along a route: its ends, its corners and evenly between them, moved off the landings.
std::vector<double> stations(const CityState &s, const Route &r) {
	const PoleDef &d = s.catalog.furniture.poles;
	const double total = r.cum.back();
	std::vector<double> keys{ 0 };
	const std::vector<size_t> bend = corners(r.path, d.corner_tolerance_m);
	for (size_t k = 1; k + 1 < bend.size(); k++) {
		const double at = r.cum[bend[k]];
		if (at < d.min_gap_m || total - at < d.min_gap_m) continue;
		if (keys.size() > 1 && at - keys.back() < d.min_gap_m) keys.back() = (keys.back() + at) / 2;
		else keys.push_back(at);
	}
	keys.push_back(total);
	const double step = std::clamp(double(s.catalog.furniture.lamps.span_m), double(d.min_spacing_m), double(d.max_spacing_m));
	std::vector<double> even{ 0 };
	for (size_t k = 1; k < keys.size(); k++) {
		const int spans = std::max(1, int(std::lround((keys[k] - keys[k - 1]) / step)));
		for (int i = 1; i <= spans; i++) even.push_back(keys[k - 1] + (keys[k] - keys[k - 1]) * i / spans);
	}
	std::vector<double> kept{ 0 };
	for (size_t i = 1; i + 1 < even.size(); i++) {
		const double lo = kept.back() + d.min_gap_m / 2, hi = total - d.min_gap_m / 2;
		if (clear_place(s, r.at(even[i]))) {
			kept.push_back(even[i]);
			continue;
		}
		for (float off = CLEAR_STEP_M; off <= d.clear_search_m; off += CLEAR_STEP_M) {
			if (even[i] + off < hi && clear_place(s, r.at(even[i] + off))) {
				kept.push_back(even[i] + off);
				break;
			}
			if (even[i] - off > lo && clear_place(s, r.at(even[i] - off))) {
				kept.push_back(even[i] - off);
				break;
			}
		}
	}
	kept.push_back(total);
	return kept;
}

Route route_between(const CityState &s, RouteGraph &g, const Snap &a, const Snap &b, double &margin_end) {
	const PoleDef &d = s.catalog.furniture.poles;
	Route r;
	const double x0 = std::min(a.x, b.x), z0 = std::min(a.z, b.z), x1 = std::max(a.x, b.x), z1 = std::max(a.z, b.z);
	for (double margin = std::max(double(d.route_margin_m), std::hypot(a.x - b.x, a.z - b.z)); margin <= d.route_margin_most_m; margin *= MARGIN_GROWTH) {
		margin_end = margin;
		g.build(s.roads, x0 - margin, z0 - margin, x1 + margin, z1 + margin);
		const double catch_m = a.pole >= 0 || b.pole >= 0 ? d.off_line_m : d.line_catch_m;
		const int from = g.nearest(a.x, a.z, catch_m), to = g.nearest(b.x, b.z, catch_m);
		if (from < 0 || to < 0) return r;
		double cost = 0;
		const std::vector<int> st = g.route(from, to, cost);
		if (st.empty() && from != to) continue;
		r.path.push_back({ a.x, a.z });
		for (int k : st) r.path.push_back(g.point(k));
		r.path.push_back({ b.x, b.z });
		break;
	}
	if (r.path.empty()) return r;
	r.cum.push_back(0);
	for (size_t i = 1; i < r.path.size(); i++) r.cum.push_back(r.cum.back() + std::hypot(r.path[i].x - r.path[i - 1].x, r.path[i].z - r.path[i - 1].z));
	return r;
}
} // namespace

PoleRunPlan plan_pole_run(const CityState &s, double ax, double az, double bx, double bz, double reach, LampMode lamps) {
	PoleRunPlan plan;
	const PoleDef &d = s.catalog.furniture.poles;
	RouteGraph g;
	const double pad = d.footway_catch_m + reach;
	g.build(s.roads, std::min(ax, bx) - pad, std::min(az, bz) - pad, std::max(ax, bx) + pad, std::max(az, bz) + pad);
	const Snap from = snap_end(s, g, ax, az, reach), to = snap_end(s, g, bx, bz, reach);
	plan.from_x = from.x;
	plan.from_z = from.z;
	plan.to_x = to.x;
	plan.to_z = to.z;
	plan.from_free = from.free;
	plan.to_free = to.free;
	if (from.free || to.free) {
		plan.refused = "hint.pole.offFootway";
		return plan;
	}
	if ((from.pole >= 0 && from.pole == to.pole) || std::hypot(from.x - to.x, from.z - to.z) < SAME_PLACE_M) return plan;
	double margin = 0;
	const Route r = route_between(s, g, from, to, margin);
	if (r.path.size() < 2) {
		plan.refused = "hint.pole.noPath";
		return plan;
	}
	const std::vector<double> kept = stations(s, r);
	for (size_t i = 0; i < kept.size(); i++) {
		const bool first = i == 0, last = i + 1 == kept.size();
		const RoutePoint p = first ? RoutePoint{ from.x, from.z } : last ? RoutePoint{ to.x, to.z } : r.at(kept[i]);
		const int existing = first && from.pole >= 0 ? from.pole : last && to.pole >= 0 ? to.pole : s.street.pole_near(float(p.x), float(p.z), d.reuse_m);
		PlannedPole q;
		q.existing = existing;
		q.x = existing >= 0 ? s.street.poles.items[size_t(existing)].x : p.x;
		q.z = existing >= 0 ? s.street.poles.items[size_t(existing)].z : p.z;
		q.lamp = existing >= 0 ? s.street.poles.items[size_t(existing)].lamp : lamp_for(int(i), lamps);
		if (!plan.poles.empty() && existing >= 0 && plan.poles.back().existing == existing) continue; // the same pole twice in a row is one
		plan.poles.push_back(q);
	}
	if (plan.poles.size() < 2) plan.poles.clear();
	return plan;
}

EditResult build_pole_run(CityState &s, double ax, double az, double bx, double bz, double reach, LampMode lamps) {
	EditResult r;
	const PoleRunPlan plan = plan_pole_run(s, ax, az, bx, bz, reach, lamps);
	r.error = plan.refused;
	if (!plan.refused.empty() || plan.poles.size() < 2) return r;
	std::vector<int> ids;
	for (const PlannedPole &p : plan.poles) {
		if (p.existing >= 0) {
			ids.push_back(p.existing);
			continue;
		}
		UtilityPole pole;
		pole.x = float(p.x);
		pole.z = float(p.z);
		pole.lamp = p.lamp;
		ids.push_back(j_add_pole(s, pole));
	}
	for (size_t i = 1; i < ids.size(); i++) {
		if (ids[i] == ids[i - 1] || s.street.span_between(ids[i - 1], ids[i]) >= 0) continue;
		j_add_span(s, ids[i - 1], ids[i]);
		r.ok = true;
	}
	if (r.ok) finish_edit(s, std::min(ax, bx), std::min(az, bz), std::max(ax, bx), std::max(az, bz));
	return r;
}

} // namespace city
