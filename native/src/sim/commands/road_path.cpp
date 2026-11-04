// A road as the player's gesture left it (Roadcraft editor/roadPath.ts and roadTool.ts pieces): one straight, one curve
// or the chain of tangent quadratics a freehand stroke becomes, and its plan and building as one edit. The stroke is
// thinned by Ramer-Douglas-Peucker (Ramer 1972; Douglas and Peucker 1973), and each kept point is the control of a
// quadratic between the midpoints of its neighbours: a uniform quadratic B-spline, so the tangent is continuous at
// every joint. Callers build inside one undo entry: a refused piece rolls the whole road back.
#include "sim/commands/commands.h"

#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr double TOLERANCE_GROWTH = 1.5; // the thinning loosens by this until the stroke fits max_controls
constexpr double SAME_POINT_M = 1e-6;
constexpr double NODE_FOUND_M = 0.01; // the node a piece just laid ends on

double dist(const StrokePoint &a, const StrokePoint &b) { return std::hypot(a.x - b.x, a.z - b.z); }

double chord_distance(const StrokePoint &p, const StrokePoint &a, const StrokePoint &b) {
	double dx = b.x - a.x, dz = b.z - a.z, l2 = dx * dx + dz * dz;
	if (l2 < SAME_POINT_M * SAME_POINT_M) return dist(p, a);
	double t = std::clamp(((p.x - a.x) * dx + (p.z - a.z) * dz) / l2, 0.0, 1.0);
	return std::hypot(p.x - a.x - t * dx, p.z - a.z - t * dz);
}

std::vector<StrokePoint> thin(const std::vector<StrokePoint> &pts, double tolerance) {
	std::vector<char> keep(pts.size(), 0);
	keep.front() = keep.back() = 1;
	std::vector<std::pair<size_t, size_t>> todo{ { 0, pts.size() - 1 } };
	while (!todo.empty()) {
		auto [from, to] = todo.back();
		todo.pop_back();
		double worst = tolerance;
		size_t at = 0;
		for (size_t i = from + 1; i < to; i++) {
			double d = chord_distance(pts[i], pts[from], pts[to]);
			if (d > worst) {
				worst = d;
				at = i;
			}
		}
		if (at == 0) continue;
		keep[at] = 1;
		todo.push_back({ from, at });
		todo.push_back({ at, to });
	}
	std::vector<StrokePoint> out;
	for (size_t i = 0; i < pts.size(); i++)
		if (keep[i]) out.push_back(pts[i]);
	return out;
}

std::vector<Quad> free_pieces(const Tuning &tn, const StrokePoint &start, const StrokePoint &end, const std::vector<StrokePoint> &samples) {
	double spacing = tn.road_min_len_m; // no piece shorter than the shortest road the plan accepts
	std::vector<StrokePoint> raw{ start };
	for (const StrokePoint &p : samples)
		if (dist(raw.back(), p) >= spacing) raw.push_back(p);
	if (raw.size() > 1 && dist(raw.back(), end) < spacing) raw.pop_back();
	raw.push_back(end);
	double tolerance = tn.road_free_tolerance_m;
	std::vector<StrokePoint> controls = thin(raw, tolerance);
	while (controls.size() > size_t(tn.road_free_max_controls)) {
		tolerance *= TOLERANCE_GROWTH;
		controls = thin(raw, tolerance);
	}
	std::vector<Quad> out;
	if (controls.size() < 3) {
		out.push_back(Quad::straight(start.x, start.z, end.x, end.z));
		return out;
	}
	StrokePoint from = start;
	for (size_t i = 1; i + 1 < controls.size(); i++) {
		const StrokePoint &c = controls[i];
		bool last = i + 2 == controls.size();
		StrokePoint to = last ? end : StrokePoint{ (c.x + controls[i + 1].x) / 2, (c.z + controls[i + 1].z) / 2 };
		if (!last && dist(from, to) < spacing) continue; // its turn goes to the next piece
		out.push_back({ from.x, from.z, c.x, c.z, to.x, to.z });
		from = to;
	}
	return out;
}

// A dragged curve: bent as far as the drag went off the chord.
Quad dragged_curve(const Tuning &tn, const RoadStroke &k) {
	Quad straight = Quad::straight(k.start.x, k.start.z, k.end.x, k.end.z);
	double dx = k.end.x - k.start.x, dz = k.end.z - k.start.z, chord = std::hypot(dx, dz);
	double nx = -dz / chord, nz = dx / chord, mx = (k.start.x + k.end.x) / 2, mz = (k.start.z + k.end.z) / 2, side = 0;
	for (const StrokePoint &p : k.samples) {
		double c = (p.x - mx) * nx + (p.z - mz) * nz;
		if (std::fabs(c) > std::fabs(side)) side = c;
	}
	side = std::clamp(side, -chord * tn.road_bend_limit, chord * tn.road_bend_limit);
	if (std::fabs(side) < k.flat_m) return straight;
	return { k.start.x, k.start.z, mx + nx * side * tn.road_bend_gain, mz + nz * side * tn.road_bend_gain, k.end.x, k.end.z };
}

Anchor free_anchor(double x, double z) {
	Anchor a;
	a.x = x;
	a.z = z;
	return a;
}
} // namespace

std::vector<Quad> road_pieces(const CityState &s, const RoadStroke &k) {
	const Tuning &tn = s.catalog.tune;
	std::vector<Quad> out;
	if (dist(k.start, k.end) < SAME_POINT_M) return out;
	if (k.alignment == RoadAlignment::FREE) out = free_pieces(tn, k.start, k.end, k.samples);
	else if (k.alignment == RoadAlignment::STRAIGHT) out.push_back(Quad::straight(k.start.x, k.start.z, k.end.x, k.end.z));
	else if (k.has_control) out.push_back({ k.start.x, k.start.z, k.control.x, k.control.z, k.end.x, k.end.z });
	else out.push_back(dragged_curve(tn, k));
	// a curve's inner edge keeps the design car's tightest turn; a road with no carriageway only must not fold
	const Profile &p = s.catalog.profile(k.profile);
	double radius = std::max(double(p.half()), double(std::max(p.asph_left, p.asph_right)) + tn.road_inner_radius_m);
	for (Quad &q : out) q = fit_quad_to_radius(q, radius);
	return out;
}

RoadPlan plan_path(const CityState &s, const std::vector<Quad> &pieces, const Anchor &a, const Anchor &b, int profile) {
	RoadPlan total;
	total.ok = !pieces.empty();
	for (size_t i = 0; i < pieces.size(); i++) {
		const Quad &q = pieces[i];
		Anchor from = i == 0 ? a : free_anchor(q.x0, q.z0), to = i + 1 == pieces.size() ? b : free_anchor(q.x1, q.z1);
		RoadPlan p = plan_road(s, from, true, q.cx, q.cz, to, profile);
		total.len += p.len;
		total.price += p.price;
		total.crossings.insert(total.crossings.end(), p.crossings.begin(), p.crossings.end());
		if (!p.ok && total.ok) {
			total.ok = false;
			total.reason = p.reason;
		}
	}
	return total;
}

EditResult build_path(CityState &s, const std::vector<Quad> &pieces, const Anchor &a, const Anchor &b, int profile) {
	EditResult total;
	if (pieces.empty()) total.error = "refuse.road.tooShort";
	Anchor from = a;
	for (size_t i = 0; i < pieces.size(); i++) {
		const Quad &q = pieces[i];
		Anchor to = i + 1 == pieces.size() ? b : free_anchor(q.x1, q.z1);
		EditResult r = build_road(s, from, true, q.cx, q.cz, to, profile);
		if (!r.ok) return r; // the undo entry rolls the pieces already laid back
		total.money += r.money;
		total.segments += r.segments;
		total.buildings_removed += r.buildings_removed;
		from = free_anchor(q.x1, q.z1);
		from.node = s.roads.find_node_near(q.x1, q.z1, NODE_FOUND_M);
	}
	total.ok = !pieces.empty();
	return total;
}

} // namespace city
